#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define BLOCK_SIZE 4096
#define INODE_SIZE 256
#define BLOCKS 64
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)
#define INODE_TABLE_BLOCK_START 3
#define INODE_TABLE_BLOCKS 5
#define INODE_TABLE_BLOCK_END   7
#define TOTAL_INODE_BLOCKS (INODE_TABLE_BLOCK_END - INODE_TABLE_BLOCK_START + 1)
#define MAX_INODES (INODES_PER_BLOCK * TOTAL_INODE_BLOCKS)
#define INODES         (INODE_TABLE_BLOCKS * BLOCK_SIZE / INODE_SIZE)
#define DATA_BITMAP_BLOCK 2
#define DATA_BLOCK_START 8
#define DATA_BLOCK_END   63
#define DATA_BLOCK_COUNT (64 - DATA_BLOCK_START)


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

typedef struct {
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint32_t file_size;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;
    uint32_t links_count;
    uint32_t block_count;
    uint32_t direct_block;
    uint32_t single_indirect;
    uint32_t double_indirect;
    uint32_t triple_indirect;
    uint8_t  reserved[156];
} __attribute__((packed)) Inode;

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

// Superblock structure (only needed fields)
typedef struct {
    uint16_t magic;          // 0x00
    uint32_t block_size;     // 0x02
    uint32_t total_blocks;   // 0x06
    uint32_t inode_bitmap;   // 0x0A
    uint32_t data_bitmap;    // 0x0E
    uint32_t inode_table_start; // 0x12
    uint32_t data_block_start;// 0x16
    uint32_t inode_size;     // 0x1A
    uint32_t inode_count;    // 0x1E
    // ... reserved
} __attribute__((packed)) superblock_t;


// Inode structure (only relevant fields)
typedef struct {
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint32_t file_size;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;
    uint32_t links_count;
    uint32_t blocks_count;
    uint32_t direct_block;  // We'll just check the direct pointer here.
    uint32_t single_indirect;
    uint32_t double_indirect;
    uint32_t triple_indirect;
    // ... reserved[156]
} __attribute__((packed)) inode_t;




// ------------------------ Task - 2 [Data Bitmap Checker] ------------------------

bool read_block(int fd, int block_num, void *buffer) {
    off_t offset = lseek(fd, block_num * BLOCK_SIZE, SEEK_SET);
    if (offset == (off_t)-1) {
        perror("lseek failed");
        return false;
    }
    ssize_t read_bytes = read(fd, buffer, BLOCK_SIZE);
    if (read_bytes != BLOCK_SIZE) {
        perror("read failed");
        return false;
    }
    return true;
}

int get_bitmap_bit(uint8_t *bitmap, int bit_index) {
    int byte_index = bit_index / 8;
    int bit_offset = 7 - (bit_index % 8);  // Big-endian bit order
    return (bitmap[byte_index] >> bit_offset) & 1;
}

void check_data_bitmap_consistency(int fd) {
    uint8_t bitmap_block[BLOCK_SIZE];
    if (!read_block(fd, DATA_BITMAP_BLOCK, bitmap_block)) {
        fprintf(stderr, "Failed to read data bitmap block.\n");
        return;
    }

    bool block_used_by_inode[DATA_BLOCK_COUNT] = {false};

    for (int b = 0; b < TOTAL_INODE_BLOCKS; b++) {
        uint8_t inode_block[BLOCK_SIZE];
        if (!read_block(fd, INODE_TABLE_BLOCK_START + b, inode_block)) {
            fprintf(stderr, "Failed to read inode block %d.\n", INODE_TABLE_BLOCK_START + b);
            return;
        }

        for (int i = 0; i < INODES_PER_BLOCK; i++) {
            Inode *inode = (Inode *)(inode_block + i * INODE_SIZE);

            if (inode->links_count == 0 || inode->dtime != 0)
                continue;

            uint32_t db = inode->direct_block;
            if (db >= DATA_BLOCK_START && db < 64) {
                int index = db - DATA_BLOCK_START;
                block_used_by_inode[index] = true;
            }
        }
    }

    printf("\n=== Data Bitmap Consistency Check ===\n");

    bool consistent = true;

    for (int i = 0; i < DATA_BLOCK_COUNT; i++) {
        int bitmap_val = get_bitmap_bit(bitmap_block, i);
        if (bitmap_val && !block_used_by_inode[i]) {
            printf("❌ Block %d marked USED in bitmap but not used by any inode\n", i + DATA_BLOCK_START);
            consistent = false;
        }
    }

    for (int i = 0; i < DATA_BLOCK_COUNT; i++) {
        int bitmap_val = get_bitmap_bit(bitmap_block, i);
        if (block_used_by_inode[i] && !bitmap_val) {
            printf("❌ Block %d used by inode but marked FREE in bitmap\n", i + DATA_BLOCK_START);
            consistent = false;
        }
    }

    if (consistent) {
        printf("✅ Data bitmap is consistent with inode references!\n");
    }
}

// ----------------- Task - 4 [Duplicate Block Check] ----------------------------------
void check_duplicate_blocks(int fd) {
    int block_usage_count[DATA_BLOCK_COUNT] = {0};

    // Traverse inode table
    for (int blk = INODE_TABLE_BLOCK_START; blk <= INODE_TABLE_BLOCK_END; blk++) {
        uint8_t block[BLOCK_SIZE];
        if (!read_block(fd, blk, block)) {
            fprintf(stderr, "Failed to read inode block %d.\n", blk);
            return;
        }

        for (int i = 0; i < INODES_PER_BLOCK; i++) {
            Inode *inode = (Inode *)(block + i * INODE_SIZE);

            if (inode->links_count == 0 || inode->dtime != 0)
                continue;

            uint32_t db = inode->direct_block;
            if (db >= DATA_BLOCK_START && db <= DATA_BLOCK_END) {
                int index = db - DATA_BLOCK_START;
                block_usage_count[index]++;
            }
        }
    }

    // Report duplicates
    printf("\n=== Duplicate Block Checker ===\n");
    int duplicates_found = 0;

    for (int i = 0; i < DATA_BLOCK_COUNT; i++) {
        if (block_usage_count[i] > 1) {
            printf("❌ Block %d is referenced by %d inodes (DUPLICATE)\n", i + DATA_BLOCK_START, block_usage_count[i]);
            duplicates_found++;
        }
    }

    if (duplicates_found == 0) {
        printf("✅ No duplicate data block references found.\n");
    }
}

// --------------------------------- Task - 3 ------------------------------

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

// --------------------------------- Task - 1 ------------------------------
int validate_superblock(FILE *fd, superblock_t* sb) {
    fseek(fd, 0, SEEK_SET);
    fread(sb, sizeof(superblock_t), 1, fd);

    int valid = 1;

    printf("Superblock Validation:\n");

    if(sb->magic != 0xd34d) {
        printf("  [ERROR] Magic number invalid (expected 0xD34D, got 0x%04X)\n", sb->magic);
        valid = 0;
    }
    if(sb->block_size != BLOCK_SIZE) {
        printf("  [ERROR] Block size invalid (expected %d, got %u)\n", BLOCK_SIZE, sb->block_size);
        valid = 0;
    }
    if(sb->total_blocks != BLOCKS) {
        printf("  [ERROR] Total blocks invalid (expected %d, got %u)\n", BLOCKS, sb->total_blocks);
        valid = 0;
    }
    if(sb->inode_bitmap != 1) {
        printf("  [ERROR] Inode bitmap block invalid (expected 1, got %u)\n", sb->inode_bitmap);
        valid = 0;
    }
    if(sb->data_bitmap != 2) {
        printf("  [ERROR] Data bitmap block invalid (expected 2, got %u)\n", sb->data_bitmap);
        valid = 0;
    }
    if(sb->inode_table_start != INODE_TABLE_BLOCK_START) {
        printf("  [ERROR] Inode table start block invalid (expected %d, got %u)\n", INODE_TABLE_BLOCK_START, sb->inode_table_start);
        valid = 0;
    }
    if(sb->data_block_start != DATA_BLOCK_START) {
        printf("  [ERROR] First data block invalid (expected %d, got %u)\n", DATA_BLOCK_START, sb->data_block_start);
        valid = 0;
    }
    if(sb->inode_size != INODE_SIZE) {
        printf("  [ERROR] Inode size invalid (expected %d, got %u)\n", INODE_SIZE, sb->inode_size);
        valid = 0;
    }
    if(sb->inode_count > INODES) {
        printf("  [ERROR] Inode count invalid (expected at most %d, got %u)\n", INODES, sb->inode_count);
        valid = 0;
    }

    if(valid) printf("  Superblock is valid!\n");
    return valid;
}

// --------------------------------- Task - 5 ------------------------------

void bad_block_checker(FILE *fd, superblock_t* sb) {
    printf("\nBad Block Checker:\n");

    size_t inode_table_offset = sb->inode_table_start * BLOCK_SIZE;
    fseek(fd, inode_table_offset, SEEK_SET);

    char buffer[INODE_SIZE];
    int bad_blocks_found = 0;

    for(uint32_t i = 0; i < sb->inode_count; ++i) {
        fseek(fd, inode_table_offset + i * INODE_SIZE, SEEK_SET);
        fread(buffer, INODE_SIZE, 1, fd);

        inode_t* inode = (inode_t*) buffer;

        // Check only valid inodes: link count > 0 and dtime == 0
        if (inode->links_count > 0 && inode->dtime == 0) {
            // Check direct block
            uint32_t block = inode->direct_block;
            if(block != 0 && (block < DATA_BLOCK_START || block > DATA_BLOCK_END)) {
                printf("  [ERROR] Inode %u references bad block %u (valid range: %d-%d)\n", i, block, DATA_BLOCK_START, DATA_BLOCK_END);
                bad_blocks_found = 1;
            }
            // TODO: You can check indirect blocks similarly!
        }
    }
    if(!bad_blocks_found) printf("  No bad blocks detected!\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <fs_image.img>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];

    // ✅ Open with fopen to get FILE*
    FILE *fp = fopen(image_path, "r+b");
    if (!fp) {
        perror("Error opening image");
        return EXIT_FAILURE;
    }

    // ✅ Convert FILE* to int fd when needed (e.g., for read())
    int fd = fileno(fp);

    // Task - 1: Superblock
    printf("\n\n---------- Task - 1 [Superblock check]----------------\n");
    superblock_t sb;
    int sb_valid = validate_superblock(fp, &sb);

    // Task - 2: Data Bitmap
    printf("\n\n---------- Task - 2 [Data bitmap consistency check]----------------\n");
    check_data_bitmap_consistency(fd);

    // Task - 3: Inode Bitmap
    printf("\n\n---------- Task - 3 [Inode bitmap consistency check]----------------\n");
    int problems_found = check_inode_bitmap_consistency(image_path, true);

    // Task - 4: Duplicate Block
    printf("\n\n---------- Task - 4 [Check duplicate blocks]----------------\n");
    check_duplicate_blocks(fd);

    // Task - 5: Bad Block Check
    printf("\n\n--------- Task - 5 [Bad Block Check]----------------\n");
    bad_block_checker(fp, &sb);

    fclose(fp);
    return problems_found > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
