#ifndef LINUX_DEFINE_H
#define LINUX_DEFINE_H

#define BLOCK_SIZE 64  // в байтах
#define INODES_COUNT 128
#define BLOCKS_PER_INODE 16     // кол-во блоков на один inode
#define BLOCKS_COUNT 2048 // BLOCKS_PER_INODE * INODES_COUNT

#define DIRECT_COUNT_BLOCKS 12  // кол-во адресных ячеек использующих прямую адресацию

#define MAX_FILE_NAME 32

#endif //LINUX_DEFINE_H
