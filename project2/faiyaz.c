
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE     4096
#define BLOCKS         64
#define INODE_SIZE     256
#define INODE_TABLE_START 3
#define INODE_TABLE_BLOCKS 5
#define INODES         (INODE_TABLE_BLOCKS * BLOCK_SIZE / INODE_SIZE)
#define DATA_BLOCK_START 8
#define DATA_BLOCK_END   (BLOCKS - 1) // 63

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


int validate_superblock(FILE *fp, superblock_t* sb) {
    fseek(fp, 0, SEEK_SET);
    fread(sb, sizeof(superblock_t), 1, fp);

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
    if(sb->inode_table_start != INODE_TABLE_START) {
        printf("  [ERROR] Inode table start block invalid (expected %d, got %u)\n", INODE_TABLE_START, sb->inode_table_start);
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

void bad_block_checker(FILE *fp, superblock_t* sb) {
    printf("\nBad Block Checker:\n");

    size_t inode_table_offset = sb->inode_table_start * BLOCK_SIZE;
    fseek(fp, inode_table_offset, SEEK_SET);

    char buffer[INODE_SIZE];
    int bad_blocks_found = 0;

    for(uint32_t i = 0; i < sb->inode_count; ++i) {
        fseek(fp, inode_table_offset + i * INODE_SIZE, SEEK_SET);
        fread(buffer, INODE_SIZE, 1, fp);

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

int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Usage: %s vsfs.img\n", argv[0]);
        return 1;
    }

    FILE* fp = fopen(argv[1], "rb");
    if(!fp) {
        perror("Error opening image");
        return 1;
    }
   
    superblock_t sb;
    int sb_valid = validate_superblock(fp, &sb);

    printf("--------------------------------------------------\n");
    // if(sb_valid) {
    bad_block_checker(fp, &sb);
    // } else {
        // printf("Superblock invalid. Skipping bad block check.\n");
    // }

    fclose(fp);
    return 0;
}