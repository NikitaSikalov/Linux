//
// Created by sikalov on 11/20/19.
//

#ifndef FILE_SYSTEM_BLOCKS_H
#define FILE_SYSTEM_BLOCKS_H

#include "define.h"


struct block {
    short currentInodeIdx;
    short parentInodeIdx;
    bool isDirectAddress;
    void* data;
};

struct dir_data {
     short inodeIdx;
     char fileName[MAX_FILE_NAME];
};

struct block* initNewBlock(bool isDir, bool isDirectAddress, short currentInodeIdx, short parentInodeIdx) {
    size_t sizeStructBlock = sizeof(struct block);
    struct block* newBlock = (struct block*)malloc(sizeStructBlock);
    size_t sizeData = 0;
    if (isDir) {
        size_t sizeDirData = sizeof(struct dir_data);
        size_t countDirDataStructures = (size_t)((BLOCK_SIZE - sizeStructBlock) / sizeDirData);
        sizeData = sizeDirData * countDirDataStructures;
    } else {
        sizeData = BLOCK_SIZE - sizeStructBlock;
    }
    newBlock->data = malloc(sizeData);
    newBlock->currentInodeIdx = currentInodeIdx;
    newBlock->parentInodeIdx = parentInodeIdx;
    newBlock->isDirectAddress = isDirectAddress;
    return newBlock;
}

/**
 * Поиск индексов свободных блоков, занятие их и обновление в файле-хранилище fs
 * @param superBlock
 * @param blocksMap
 * @param count - необходимое кол-во блоков
 * @return
 */
short int* getFreeBlocks(short int count) {
    struct super_block* superBlock = (struct super_block*)malloc(SUPER_BLOCK_SIZE);
    readFromFile(superBlock, FS_STORE, SUPER_BLOCK_SIZE, 1, OFFSET_SUPER_BLOCK);
    const size_t blocksCount = superBlock->blocksCount;
    if (superBlock->freeBlocksCount <= count) {
        printf("Все блоки заняты, больше создавать файлы или делать записи в нех нельзя");
        return NULL;
    }

    bool* blocksMap = (bool* )malloc(blocksCount);
    readFromFile(blocksMap, FS_STORE, blocksCount, 1, OFFSET_BLOCKS_MAP);
    short int* freeBlocksIdxes = (short int*)malloc(count * sizeof(short int));
    size_t pointer = 0;
    for (size_t i = 0; i < blocksCount; i++) {
        if (!blocksMap[i]) {
            blocksMap[i] = true;
            freeBlocksIdxes[pointer++] = i;
            superBlock->freeBlocksCount--;
        }
        if (pointer + 1 == count) {
            break;
        }
    }
    writeToFile(blocksMap, FS_STORE, blocksCount, 1, OFFSET_BLOCKS_MAP);
    free(superBlock);
    free(blocksMap);
    return freeBlocksIdxes;
}

#endif //FILE_SYSTEM_BLOCKS_H
