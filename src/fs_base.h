#ifndef LINUX_FS_BASE_H
#define LINUX_FS_BASE_H

#include <time.h>

#include "fs_store_functions.h"
#include "base_structures.h"

void updateSuperBlock(struct super_block* superBlock);
void initSuperBlock();
struct super_block* getSuperBlock();

void initBlocksMap();
void updateBlocksMap(bool* blocksMap);
bool* getBlocksMap();
size_t* getFreeBlocks(size_t count);

// ----------------------------- SUPER BLOCK LOGIC ----------------------------------------------

void initSuperBlock() {
    struct super_block* superBlock = (struct super_block*)malloc(sizeof(struct super_block));
    superBlock->blocksCount = BLOCKS_COUNT;
    superBlock->freeBlocksCount = BLOCKS_COUNT;
    superBlock->inodesCount = INODES_COUNT;
    superBlock->inodesFreeCount = INODES_COUNT;
    superBlock->sizeBlock = BLOCK_SIZE;

    superBlock->superBlockOffset = 0;
    superBlock->blocksMapOffset = superBlock->superBlockOffset + (long) sizeof(struct super_block);
    superBlock->inodesMapOffset = superBlock->blocksMapOffset + (long) sizeof(superBlock->blocksCount);
    superBlock->inodesTableOffset = superBlock->inodesMapOffset + (long) sizeof(superBlock->inodesCount);
    superBlock->dataBlocksOffset = superBlock->inodesTableOffset + (long) sizeof(struct inode);
    superBlock->currentInode = ROOT_INODE;
    superBlock->blocksPerInode = BLOCKS_PER_INODE;
    superBlock->maxFileName = MAX_FILE_NAME;
    superBlock->blocksCountWithDirectAddress = DIRECT_COUNT_BLOCKS;
    updateSuperBlock(superBlock);
    free(superBlock);
}

void updateSuperBlock(struct super_block* superBlock) {
    writeToFile((void *)superBlock, sizeof(struct super_block), 1, superBlock->superBlockOffset);
}

struct super_block* getSuperBlock() {
    struct super_block* superBlock = (struct super_block*)malloc(sizeof(struct super_block));
    readFromFile(superBlock, sizeof(struct super_block), 1, 0);
    return superBlock;
}

// --------------------------------- DATA BLOCKS LOGIC ----------------------------

void initBLocksMap() {
    struct super_block* superBlock = getSuperBlock();
    size_t blocksCount = superBlock->blocksCount;
    bool* blocksMap = (bool *)malloc(blocksCount);
    for (size_t i = 0; i < blocksCount; ++i) {
        blocksMap[i] = false; // значит что блоки свободен
    }
    updateBlocksMap(blocksMap);
    free(blocksMap);
}

void updateBlocksMap(bool* blocksMap) {
    struct super_block* superBlock = getSuperBlock();
    writeToFile(blocksMap, superBlock->blocksCount, superBlock->blocksCount, superBlock->blocksMapOffset);
    free(superBlock);
}

bool* getBlocksMap() {
    struct super_block* superBlock = getSuperBlock();
    size_t blocksCount = superBlock->blocksCount;
    bool* blocksMap = (bool *)malloc(blocksCount);
    readFromFile(blocksMap, blocksCount, blocksCount, superBlock->blocksMapOffset);
    free(superBlock);
    return blocksMap;
}

size_t* getFreeBlocks(size_t count) {
    struct super_block* superBlock = getSuperBlock();
    if (superBlock->freeBlocksCount < count) {
        printf("Невозможно выделить %d кол-во блоков, так как сейчас свободно %d",
                (int)count,
                (int)superBlock->freeBlocksCount);
        return NULL;
    }
    bool* blocksMap = getBlocksMap();
    size_t* freeBlocks = (size_t*)malloc(count * sizeof(size_t));
    size_t pointer = 0;
    for (size_t i = 0; i < superBlock->blocksCount; ++i) {
        if (!blocksMap[i]) {
            freeBlocks[pointer++] = i;
            superBlock->freeBlocksCount--;
        }
        if (pointer == count) {
            break;
        }
    }
    updateSuperBlock(superBlock);
    free(superBlock);
    free(blocksMap);
    return freeBlocks;
}

// ------------------------------------- INODES LOGIC ---------------------------------

#endif //LINUX_FS_BASE_H
