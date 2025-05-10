#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>

struct vsfs_super_block {
    uint32_t magic;
    uint16_t block_size;
    uint32_t total_blocks;
    uint32_t inode_bitmap_block;
    uint32_t data_bitmap_block;
    uint32_t inode_table_block;
    uint32_t first_data_block;
    uint32_t inode_size;
    uint32_t inode_count;
};

struct vsfs_inode {
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint32_t size;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;
    uint32_t links;
    uint32_t blocks;
    uint32_t direct[10];
    uint32_t indirect;
};

static bool inspect_inode_validity(const struct vsfs_inode *inode) {
    return (inode->links > 0 && inode->dtime == 0);
}

static bool is_bit_located(const uint8_t *bitmap, uint32_t bit_num) {
    uint32_t byte_index = bit_num / 8;
    uint32_t bit_index = bit_num % 8;
    return (bitmap[byte_index] & (1 << bit_index)) != 0;
}

static void locateBit(uint8_t *bitmap, uint32_t bit_num, bool value) {
    uint32_t byte_index = bit_num / 8;
    uint32_t bit_index = bit_num % 8;
    if (value) {
        bitmap[byte_index] |= (1 << bit_index);
    } else {
        bitmap[byte_index] &= ~(1 << bit_index);
    }
}

static void Show_superblock_info(const struct vsfs_super_block *sb) {
    printf("Filesystem Details:\n");
    printf("Magic: 0x%08x\n", sb->magic);
    printf("Block Size: %u bytes\n", sb->block_size);
    printf("Total Blocks: %u\n", sb->total_blocks);
    printf("Inode Count: %u\n", sb->inode_count);
    printf("Inode Size: %u bytes\n\n", sb->inode_size);
}

static int investigate_inodes(const struct vsfs_super_block *sb, uint8_t *bitmap,
                              uint32_t *nodes_to_mark, uint32_t *nodes_to_unmark,
                              int *counters, uint8_t *filesystem_data) {
    uint32_t mark_count = 0, unmark_count = 0;
    for (uint32_t inode_id = 0; inode_id < sb->inode_count; inode_id++) {
        uint32_t inode_offset = (sb->inode_table_block * sb->block_size) +
                                (inode_id * sb->inode_size);
        struct vsfs_inode inode;
        memcpy(&inode, filesystem_data + inode_offset, sizeof(struct vsfs_inode));

        bool valid = inspect_inode_validity(&inode);
        bool marked = is_bit_located(bitmap, inode_id);

        if (valid && marked) {
            counters[0]++;
        } else if (valid && !marked) {
            counters[1]++;
            printf("Problem: Valid inode %u not marked\n", inode_id);
            nodes_to_mark[mark_count++] = inode_id;
        } else if (!valid && marked) {
            counters[2]++;
            printf("Problem: Invalid inode %u is marked\n", inode_id);
            nodes_to_unmark[unmark_count++] = inode_id;
        } else {
            counters[3]++;
        }
    }
    return mark_count + unmark_count;
}

static int request_fixes(const char *image_path, const struct vsfs_super_block *sb,
                         uint8_t *bitmap, uint32_t *nodes_to_mark, uint32_t mark_count,
                         uint32_t *nodes_to_unmark, uint32_t unmark_count) {
    for (uint32_t i = 0; i < mark_count; i++) {
        locateBit(bitmap, nodes_to_mark[i], true);
    }

    for (uint32_t i = 0; i < unmark_count; i++) {
        locateBit(bitmap, nodes_to_unmark[i], false);
    }

    int fixes_applied = mark_count + unmark_count;
    if (fixes_applied > 0) {
        FILE *disk_image = fopen(image_path, "r+b");
        if (!disk_image) {
            fprintf(stderr, "Can't open for writing: %s\n", strerror(errno));
            return -1;
        }

        off_t bitmap_offset = sb->inode_bitmap_block * sb->block_size;
        fseek(disk_image, bitmap_offset, SEEK_SET);
        if (fwrite(bitmap, 1, sb->block_size, disk_image) != sb->block_size) {
            fclose(disk_image);
            return -1;
        }

        fclose(disk_image);
    }

    return fixes_applied;
}

static void expose_results(const int *counters, int total_problems, int fixes_applied) {
    printf("\nScan Results:\n");
    printf("Valid inodes properly marked     : %d\n", counters[0]);
    printf("Valid inodes needing marking     : %d\n", counters[1]);
    printf("Invalid inodes incorrectly marked: %d\n", counters[2]);
    printf("Invalid inodes properly unmarked : %d\n", counters[3]);
    printf("Total inconsistencies found      : %d\n", total_problems);

    if (fixes_applied >= 0) {
        printf("Fixes applied                   : %d\n", fixes_applied);
        printf("%s\n", fixes_applied > 0 ? "Filesystem fixed successfully." : "No repairs needed.");
    }
}

int check_inode_bitmap_consistency(const char *image_path, bool auto_fix) {
    int counters[4] = {0};

    FILE *disk_image = fopen(image_path, "rb");
    if (!disk_image) {
        fprintf(stderr, "Can't open %s: %s\n", image_path, strerror(errno));
        return -1;
    }

    fseek(disk_image, 0, SEEK_END);
    long image_size = ftell(disk_image);
    fseek(disk_image, 0, SEEK_SET);

    uint8_t *filesystem_data = malloc(image_size);
    if (!filesystem_data || fread(filesystem_data, 1, image_size, disk_image) != image_size) {
        fprintf(stderr, "File operation failed: %s\n", strerror(errno));
        fclose(disk_image);
        free(filesystem_data);
        return -1;
    }
    fclose(disk_image);

    struct vsfs_super_block sb;
    memcpy(&sb, filesystem_data, sizeof(sb));
    Show_superblock_info(&sb);

    uint8_t *bitmap = filesystem_data + (sb.inode_bitmap_block * sb.block_size);
    uint32_t *nodes_to_mark = malloc(sb.inode_count * sizeof(uint32_t));
    uint32_t *nodes_to_unmark = malloc(sb.inode_count * sizeof(uint32_t));

    if (!nodes_to_mark || !nodes_to_unmark) {
        fprintf(stderr, "Memory allocation failed\n");
        free(filesystem_data);
        return -1;
    }

    int total_problems = investigate_inodes(&sb, bitmap, nodes_to_mark, nodes_to_unmark, counters, filesystem_data);

    int fixes_applied = 0;
    if (auto_fix) {
        fixes_applied = request_fixes(image_path, &sb, bitmap,
                                      nodes_to_mark, counters[1],
                                      nodes_to_unmark, counters[2]);
        if (fixes_applied < 0) {
            fprintf(stderr, "Repair operation failed\n");
        }
    }

    expose_results(counters, total_problems, fixes_applied);

    free(filesystem_data);
    free(nodes_to_mark);
    free(nodes_to_unmark);
    return total_problems;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <image_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int problems_found = check_inode_bitmap_consistency(argv[1], true);
    return problems_found > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
