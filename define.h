#ifndef LINUX_DEFINE_H
#define LINUX_DEFINE_H

#define BLOCK_SIZE 64  // в байтах
#define BLOCKS_COUNT 2048
#define INODES_COUNT 128

#define DIRECT_COUNT_BLOCKS 12  // кол-во адресных ячеек использующих прямую адресацию
#define BLOCKS_PER_INODE 16     // кол-во блоков на одн inode

#define MAX_FILE_NAME 32

#endif //LINUX_DEFINE_H
