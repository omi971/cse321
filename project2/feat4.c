// -------------- Duplicate Block Checker -----------------

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 4096
#define INODE_SIZE 256
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)
#define INODE_TABLE_START_BLOCK 3
#define INODE_TABLE_END_BLOCK   7
#define TOTAL_INODE_BLOCKS (INODE_TABLE_END_BLOCK - INODE_TABLE_START_BLOCK + 1)
#define MAX_INODES (INODES_PER_BLOCK * TOTAL_INODE_BLOCKS)

#define DATA_BLOCK_START 8
#define DATA_BLOCK_END   63
#define DATA_BLOCK_COUNT (DATA_BLOCK_END - DATA_BLOCK_START + 1)

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

// Reads one block from the .img file
void read_block(int fd, int block_num, void *buffer) {
    lseek(fd, block_num * BLOCK_SIZE, SEEK_SET);
    read(fd, buffer, BLOCK_SIZE);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filesystem.img>\n", argv[0]);
        return 1;
    }

    const char *image_file = argv[1];
    int fd = open(image_file, O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return 1;
    }

    // Track how many times each data block is used
    int block_usage_count[DATA_BLOCK_COUNT] = {0};

    // Traverse inode table (blocks 3 to 7)
    for (int blk = INODE_TABLE_START_BLOCK; blk <= INODE_TABLE_END_BLOCK; blk++) {
        uint8_t block[BLOCK_SIZE];
        read_block(fd, blk, block);

        for (int i = 0; i < INODES_PER_BLOCK; i++) {
            Inode *inode = (Inode *)(block + i * INODE_SIZE);

            // Skip deleted or unused inodes
            if (inode->links_count == 0 || inode->dtime != 0) continue;

            // Only checking direct block
            uint32_t db = inode->direct_block;
            if (db >= DATA_BLOCK_START && db <= DATA_BLOCK_END) {
                int index = db - DATA_BLOCK_START;
                block_usage_count[index]++;
            }
        }
    }

    // Report duplicate block usage
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

    close(fd);
    return 0;
}
