#ifndef LINUX_DEFINE_H
#define LINUX_DEFINE_H

#define BLOCK_SIZE 64  // в байтах
#define INODES_COUNT 128
#define BLOCKS_PER_INODE 16     // кол-во блоков на один inode
#define BLOCKS_COUNT 2048 // BLOCKS_PER_INODE * INODES_COUNT

#define DIRECT_COUNT_BLOCKS 12  // кол-во адресных ячеек использующих прямую адресацию

#define ROOT_INODE 0    // корневой i-node для пути /

#define MAX_FILE_NAME 32

#define ANSI_COLOR_RED "\x1b[31m"

#define ANSI_COLOR_GREEN "\x1b[32m"

#define ANSI_COLOR_YELLOW "\x1b[33m"

#define ANSI_COLOR_BLUE "\x1b[34m"

#define ANSI_COLOR_MAGENTA "\x1b[35m"

#define ANSI_COLOR_CYAN "\x1b[36m"

#define ANSI_COLOR_RESET "\x1b[0m"

#endif //LINUX_DEFINE_H
