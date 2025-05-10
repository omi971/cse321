#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define BLOCK_SIZE 4096
#define TOTAL_BLOCKS 64
#define DATA_BLOCK_START 8
#define DATA_BLOCK_END 63
#define DATA_BLOCK_COUNT (DATA_BLOCK_END - DATA_BLOCK_START + 1)

#define SUPERBLOCK_BLOCK 0
#define DATA_BITMAP_BLOCK 2


// fd --> here is vsfs.img file loaded
// block_num --> 
// buffer --> 
void read_block(int fd, int block_num, void *buffer) {
    lseek(fd, block_num * BLOCK_SIZE, SEEK_SET);
    read(fd, buffer, BLOCK_SIZE);
}

int main(int argc, char *argv[]) {
    // if given arguments is not equals to 2
    if (argc != 2) {
        printf("Usage: %s <fs_image.img>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("Error opening file");
        return 1;
    }

    uint8_t bitmap_block[BLOCK_SIZE];
    read_block(fd, DATA_BITMAP_BLOCK, bitmap_block);

    printf("=== Data Bitmap Status (Block %d) ===\n\n", DATA_BITMAP_BLOCK);

    int used_count = 0, free_count = 0;

    printf("Used Data Blocks:\n");

    
    // The loop used to check the data bitmap to 
    // find out how many blocks are used and
    // how many blocks are free

    // here DATA_BLOCK_COUNT is 55
    for (int i = 0; i < DATA_BLOCK_COUNT; i++) {
        int byte_index = i / 8;
        int bit_offset = 7 - (i % 8);
        int used = (bitmap_block[byte_index] >> bit_offset) & 1;
        if (used) {
            printf("  Block %2d\n", DATA_BLOCK_START + i);
            used_count++;
        }
    }

    free_count = DATA_BLOCK_COUNT - used_count;
    printf("\nSummary:\n");

    printf("  Data Block Count : %d\n", DATA_BLOCK_COUNT);
    printf("  Used Blocks : %d\n", used_count);
    printf("  Free Blocks : %d\n", free_count);

    close(fd);
    return 0;
}
