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

static bool inspect_inode_validity(const struct vsfs_inode *inodeValidity) {
    return (inodeValidity->links > 0 && inodeValidity->dtime == 0);
}

static bool is_bit_located(const uint8_t *bitMAP, uint32_t bit_num) {
    uint32_t byte_Index = bit_num / 8;
    uint32_t bit_indx = bit_num % 8;
    return (bitMAP[byte_Index] & (1 << bit_indx)) != 0;
}

static void locateBit(uint8_t *bitMAP, uint32_t bit_num, bool Values) {
    uint32_t byte_Index = bit_num / 8;
    uint32_t bit_indx = bit_num % 8;
    if (Values) {
        bitMAP[byte_Index] |= (1 << bit_indx);
    } else {
        bitMAP[byte_Index] &= ~(1 << bit_indx);
    }
}

static void Show_superblock_info(const struct vsfs_super_block *sb) {
    printf("Filesystem Details:\n");
    printf("Magic: 0x%08x\n", sb->magic);
    printf("Block Size: %u bytes\n", sb->block_size);
    printf("Total Blocks: %u\n", sb->total_blocks);
    printf("Inode Count: %u\n", sb->inode_count);
    printf("Inode Size: %u bytes\n", sb->inode_size);
    printf("\n");
}

static int investigate_inodes(const struct vsfs_super_block *sb, uint8_t *bitMAP,
                              uint32_t *nodes_to_mark, uint32_t *nodes_to_unmark,
                              int *counters, uint8_t *filesystem_data) {
    uint32_t mark_count = 0, unmark_count = 0;
    uint32_t inode_ID_number = 0;

    while (inode_ID_number < sb->inode_count) {
        uint32_t node_location = (sb->inode_table_block * sb->block_size) +
                                 (inode_ID_number * sb->inode_size);
        struct vsfs_inode current_iNode;
        memcpy(&current_iNode, filesystem_data + node_location, sizeof(struct vsfs_inode));

        bool is_valid = inspect_inode_validity(&current_iNode);
        bool is_marked = is_bit_located(bitMAP, inode_ID_number);

        if (is_valid && is_marked) {
            counters[0]++;
        } else if (is_valid && !is_marked) {
            counters[1]++;
            printf("Problem: Valid node %u not marked\n", inode_ID_number);
            nodes_to_mark[mark_count++] = inode_ID_number;
        } else if (!is_valid && is_marked) {
            counters[2]++;
            printf("Problem: Invalid node %u is marked\n", inode_ID_number);
            nodes_to_unmark[unmark_count++] = inode_ID_number;
        } else {
            counters[3]++;
        }

        inode_ID_number++;
    }

    return mark_count + unmark_count;
}

static int request_fixes(const char *image_path, const struct vsfs_super_block *sb,
                         uint8_t *bitMAP, uint32_t *nodes_to_mark, uint32_t mark_count,
                         uint32_t *nodes_to_unmark, uint32_t unmark_count) {
    int fixes_applied = mark_count + unmark_count;

    uint32_t i = 0;
    while (i < mark_count) {
        locateBit(bitMAP, nodes_to_mark[i], true);
        i++;
    }

    i = 0;
    while (i < unmark_count) {
        locateBit(bitMAP, nodes_to_unmark[i], false);
        i++;
    }

    if (fixes_applied > 0) {
        FILE *disk_image = fopen(image_path, "r+b");
        if (!disk_image) {
            fprintf(stderr, "Can't open for writing: %s\n", strerror(errno));
            return -1;
        }

        off_t bitmap_position = sb->inode_bitmap_block * sb->block_size;
        fseek(disk_image, bitmap_position, SEEK_SET);
        if (fwrite(bitMAP, 1, sb->block_size, disk_image) != sb->block_size) {
            fclose(disk_image);
            return -1;
        }

        fclose(disk_image);
    }

    return fixes_applied;
}

static void expose_results(const int *counters, int total_problems, int fixes_applied) {
    printf("\nScan Results:\n");
    printf("Valid nodes properly marked: %d\n", counters[0]);
    printf("Valid nodes needing marking: %d\n", counters[1]);
    printf("Invalid nodes incorrectly marked: %d\n", counters[2]);
    printf("Invalid nodes properly unmarked: %d\n", counters[3]);
    printf("Total problems found: %d\n", total_problems);

    if (fixes_applied >= 0) {
        printf("Problems fixed: %d\n", fixes_applied);
        if (fixes_applied > 0) {
            printf("Filesystem fixed successfully.\n");
        } else {
            printf("No repairs needed.\n");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <image_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];
    bool enabling_Auto_Repair = true;
    int counters[4] = {0};

    FILE *disk_image = fopen(image_path, "rb");
    if (!disk_image) {
        fprintf(stderr, "Can't open %s: %s\n", image_path, strerror(errno));
        return EXIT_FAILURE;
    }

    fseek(disk_image, 0, SEEK_END);
    long image_size = ftell(disk_image);
    fseek(disk_image, 0, SEEK_SET);

    uint8_t *filesystem_data = malloc(image_size);
    if (!filesystem_data || fread(filesystem_data, 1, image_size, disk_image) != image_size) {
        fprintf(stderr, "File operation failed: %s\n", strerror(errno));
        fclose(disk_image);
        free(filesystem_data);
        return EXIT_FAILURE;
    }
    fclose(disk_image);

    struct vsfs_super_block superblock;
    memcpy(&superblock, filesystem_data, sizeof(superblock));
    Show_superblock_info(&superblock);

    uint8_t *bitMAP = filesystem_data + (superblock.inode_bitmap_block * superblock.block_size);
    uint32_t *nodes_to_mark = malloc(superblock.inode_count * sizeof(uint32_t));
    uint32_t *nodes_to_unmark = malloc(superblock.inode_count * sizeof(uint32_t));

    if (!nodes_to_mark || !nodes_to_unmark) {
        fprintf(stderr, "Memory allocation failed\n");
        free(filesystem_data);
        return EXIT_FAILURE;
    }

    int total_problems = investigate_inodes(&superblock, bitMAP, nodes_to_mark, nodes_to_unmark, counters, filesystem_data);

    int fixes_applied = 0;
    if (enabling_Auto_Repair) {
        fixes_applied = request_fixes(image_path, &superblock, bitMAP,
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
    return total_problems > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
