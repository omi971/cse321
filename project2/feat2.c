// ------------------ Data Bitmap Checker ------------------ 

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BLOCK_SIZE 4096
#define INODE_SIZE 256
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)
#define INODE_TABLE_BLOCK_START 3
#define INODE_TABLE_BLOCK_END   7
#define TOTAL_INODE_BLOCKS (INODE_TABLE_BLOCK_END - INODE_TABLE_BLOCK_START + 1)
#define MAX_INODES (INODES_PER_BLOCK * TOTAL_INODE_BLOCKS)

#define DATA_BITMAP_BLOCK 2
#define DATA_BLOCK_START 8
#define DATA_BLOCK_COUNT (64 - DATA_BLOCK_START)

#define MAX_DIRECT_BLOCKS 1 // According to your structure, only 1 direct pointer

// Reads one block into buffer
void read_block(int fd, int block_num, void *buffer) {
    lseek(fd, block_num * BLOCK_SIZE, SEEK_SET);
    read(fd, buffer, BLOCK_SIZE);
}

// Get bitmap value at bit i
int get_bitmap_bit(uint8_t *bitmap, int bit_index) {
    int byte_index = bit_index / 8;
    int bit_offset = 7 - (bit_index % 8);
    return (bitmap[byte_index] >> bit_offset) & 1;
}

// Inode structure based on your layout
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <fs_image.img>\n", argv[0]);
        return 1;
    }

    const char *img = argv[1];
    int fd = open(img, O_RDONLY);
    if (fd < 0) {
        perror("Error opening image");
        return 1;
    }

    // === Step 1: Read bitmap ===
    uint8_t bitmap_block[BLOCK_SIZE];
    read_block(fd, DATA_BITMAP_BLOCK, bitmap_block);

    bool block_used_by_inode[DATA_BLOCK_COUNT] = {false};

    // === Step 2: Scan inodes ===
    for (int b = 0; b < TOTAL_INODE_BLOCKS; b++) {
        uint8_t block[BLOCK_SIZE];
        read_block(fd, INODE_TABLE_BLOCK_START + b, block);

        for (int i = 0; i < INODES_PER_BLOCK; i++) {
            Inode *inode = (Inode *)(block + i * INODE_SIZE);
            if (inode->links_count == 0 || inode->dtime != 0)
                continue; // Invalid or deleted inode

            // Check direct block
            if (inode->direct_block >= DATA_BLOCK_START && inode->direct_block < 64) {
                int index = inode->direct_block - DATA_BLOCK_START;
                block_used_by_inode[index] = true;
            }

            // Add support for indirect blocks if needed
        }
    }

    // === Step 3: Compare ===
    printf("\n=== Data Bitmap Consistency Check ===\n");

    bool consistent = true;

    // (a) Bitmap → inode
    for (int i = 0; i < DATA_BLOCK_COUNT; i++) {
        int bitmap_val = get_bitmap_bit(bitmap_block, i);
        if (bitmap_val && !block_used_by_inode[i]) {
            printf("❌ Block %d marked USED in bitmap but not used by any inode\n", i + DATA_BLOCK_START);
            consistent = false;
        }
    }

    // (b) Inode → bitmap
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

    close(fd);
    return 0;
}
