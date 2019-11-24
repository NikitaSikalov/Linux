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

void initNodesMap();
void updateInodesMap(bool*);
bool* getInodesMap();
struct inode* createInodeByIdx(size_t id, size_t parentId, bool isDir, char* fileName);
void createInode(size_t parentId, bool isDir, char* fileName);
void updateInode(struct inode* node, size_t id);

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
    superBlock->rootInode = ROOT_INODE;
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
    writeToFile(blocksMap, 1, superBlock->blocksCount, superBlock->blocksMapOffset);
    free(superBlock);
}

bool* getBlocksMap() {
    struct super_block* superBlock = getSuperBlock();
    size_t blocksCount = superBlock->blocksCount;
    bool* blocksMap = (bool *)malloc(blocksCount);
    readFromFile(blocksMap, 1, blocksCount, superBlock->blocksMapOffset);
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
void initInodesMap() {
    struct super_block* superBlock = getSuperBlock();
    size_t inodesCount = superBlock->inodesCount;
    bool* inodesMap = (bool*)malloc(inodesCount);
    for (size_t i = 0; i < inodesCount; ++i) {
        inodesMap[i] = false;
    }
    updateInodesMap(inodesMap);
    free(inodesMap);
    free(superBlock);
}

void updateInodesMap(bool* inodesMap) {
    struct super_block* superBlock = getSuperBlock();
    writeToFile(inodesMap, 1, superBlock->inodesCount, superBlock->inodesMapOffset);
    free(superBlock);
}

bool* getInodesMap() {
    struct super_block* superBlock = getSuperBlock();
    bool* inodesMap = (bool*)malloc(superBlock->inodesCount);
    readFromFile(inodesMap, 1, superBlock->inodesCount, superBlock->inodesMapOffset);
    free(superBlock);
    return inodesMap;
}

struct inode* createInodeByIdx(size_t id, size_t parentId, bool isDir, char* fileName) {
    struct inode* node = (struct inode*)malloc(sizeof(struct inode));
    struct super_block* superBlock = getSuperBlock();
    bool* inodesMap = getInodesMap();
    if (id >= superBlock->inodesCount) {
        printf("Невозможно создать i-node с таким id=%d", (int)id);
        return NULL;
    }
    if (inodesMap[id]) {
        printf("Блок с такм id=%d уже занят", (int)id);
        return NULL;
    }
    inodesMap[id] = true;
    superBlock->inodesFreeCount--;
    node->id = id;
    node->parentId = parentId;
    node->isDir = isDir;
    node->fileSize = 0;
    updateSuperBlock(superBlock);
    updateInodesMap(inodesMap);

    memcpy(node->fileName, fileName, sizeof(char) * superBlock->maxFileName);

    size_t* freeBlocks = getFreeBlocks(superBlock->blocksPerInode);
    if (freeBlocks == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < superBlock->blocksPerInode; i++) {
        node->blocksInfo[i].id = freeBlocks[i];
        node->blocksInfo[i].isDirect = i < superBlock->blocksCountWithDirectAddress;
        node->blocksInfo[i].realSize = 0;
    }
    updateInode(node, id);
    free(superBlock);
    free(inodesMap);
    return node;
}

void createInode(size_t parentId, bool isDir, char* fileName) {
    struct super_block* superBlock = getSuperBlock();
    if (superBlock->inodesFreeCount <= 0) {
        printf("Больше создать файл невозможно, так как все i-nodes заняты");
    }
    struct inode* node;
    bool* inodesMap = getInodesMap();
    size_t idx;
    // Ищем в цикле свободный i-node
    for (idx = 0; idx < superBlock->inodesCount && inodesMap[idx]; ++idx) {}
    if (idx >= superBlock->inodesCount) {
        exit(1);
    }
    node = createInodeByIdx(idx, parentId, isDir, fileName);
    free(inodesMap);
    free(superBlock);
}

void updateInode(struct inode* node, size_t id) {
    struct super_block* superBlock = getSuperBlock();
    long int offset = superBlock->inodesTableOffset + (long)(id * sizeof(struct inode));
    writeToFile(node, sizeof(struct inode), 1, offset);
    free(superBlock);
}

#endif //LINUX_FS_BASE_H
