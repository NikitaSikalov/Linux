#ifndef LINUX_FS_BASE_H
#define LINUX_FS_BASE_H

#include <time.h>

#include "fs_store_functions.h"
#include "base_structures.h"

void updateSuperBlock(struct super_block* superBlock);
void initSuperBlock();
struct super_block* getSuperBlock();
void changeCurrentInode(size_t id);

void initBlocksMap();
void updateBlocksMap(bool* blocksMap);
bool* getBlocksMap();
size_t* getFreeBlocks(size_t count);
void writeToBlock(size_t id, void* data);
void readFromBlock(size_t id, void *data);
void makeFreeBlocks(const size_t* blocks, size_t count);

void initNodesMap();
void updateInodesMap(bool*);
bool* getInodesMap();
struct inode* createInodeByIdx(size_t id, size_t parentId, bool isDir, char* fileName);
size_t createInode(size_t parentId, bool isDir, char* fileName);
size_t createDirInode(size_t parentId, char* dirName);
void updateInode(struct inode* node);
struct inode* getInodeById(size_t id);
void writeToInode(size_t id, size_t size, void* data);
void* readFromInode(size_t id);
struct inode* getCurrentDirInode();
size_t getCountOfFiles(struct inode* node);

// ----------------------------- SUPER BLOCK LOGIC ----------------------------------------------

void initSuperBlock() {
    struct super_block* superBlock = (struct super_block*)malloc(sizeof(struct super_block));
    superBlock->blocksCount = BLOCKS_COUNT;
    superBlock->freeBlocksCount = BLOCKS_COUNT;
    superBlock->inodesCount = INODES_COUNT;
    superBlock->inodesFreeCount = INODES_COUNT;
    superBlock->blockSize = BLOCK_SIZE;

    superBlock->superBlockOffset = 0;
    superBlock->blocksMapOffset = superBlock->superBlockOffset + (long) sizeof(struct super_block);
    superBlock->inodesMapOffset = superBlock->blocksMapOffset + (long) superBlock->blocksCount;
    superBlock->inodesTableOffset = superBlock->inodesMapOffset + (long) superBlock->inodesCount;
    superBlock->dataBlocksOffset = superBlock->inodesTableOffset + (long)(sizeof(struct inode) * superBlock->inodesCount);
    superBlock->currentInode = ROOT_INODE;
    superBlock->rootInode = ROOT_INODE;
    superBlock->blocksPerInode = BLOCKS_PER_INODE;
    superBlock->maxFileName = MAX_FILE_NAME;
    superBlock->maxFileSize = (BLOCKS_PER_INODE - DIRECT_COUNT_BLOCKS) * BLOCKS_PER_INODE * BLOCK_SIZE;
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

void changeCurrentInode(size_t id) {
    struct super_block* superBlock = getSuperBlock();
    superBlock->currentInode = id;
    updateSuperBlock(superBlock);
    free(superBlock);
}

// --------------------------------- DATA BLOCKS LOGIC ----------------------------
void initBlocksMap() {
    struct super_block* superBlock = getSuperBlock();
    size_t blocksCount = superBlock->blocksCount;
    bool* blocksMap = (bool *)malloc(blocksCount);
    for (size_t i = 0; i < blocksCount; ++i) {
        blocksMap[i] = false; // значит что блок свободен
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
            blocksMap[i] = true;
        }
        if (pointer == count) {
            break;
        }
    }
    updateSuperBlock(superBlock);
    updateBlocksMap(blocksMap);
    free(superBlock);
    free(blocksMap);
    return freeBlocks;
}

void writeToBlock(size_t id, void* data) {
    struct super_block* superBlock = getSuperBlock();
    long int offset = superBlock->dataBlocksOffset + (long)(id * superBlock->blockSize);
    writeToFile(data, superBlock->blockSize, 1, offset);
    free(superBlock);
}

void readFromBlock(size_t id, void* data) {
    struct super_block* superBlock = getSuperBlock();
    long int offset = superBlock->dataBlocksOffset + (long)(id * superBlock->blockSize);
    readFromFile(data, superBlock->blockSize, 1, offset);
    //free(superBlock);
}

void makeFreeBlocks(const size_t* blocks, size_t count) {
    bool* blocksMap = getBlocksMap();
    struct super_block* superBlock = getSuperBlock();
    for(size_t i = 0; i < count; ++i) {
        blocksMap[blocks[i]] = false;
        superBlock->freeBlocksCount++;
    }

    updateSuperBlock(superBlock);
    updateBlocksMap(blocksMap);
    free(blocksMap);
    free(superBlock);
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
        node->blocksInfo[i].isEmpty = true;
    }
    updateInode(node);
    free(superBlock);
    free(inodesMap);
    return node;
}

size_t createInode(size_t parentId, bool isDir, char* fileName) {
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
    free(node);
    free(inodesMap);
    free(superBlock);
    return idx;
}

size_t createDirInode(size_t parentId, char* dirName) {
    size_t newInode = createInode(parentId, true, dirName);
    struct dir_data rootData[2];
    strcpy(rootData[0].fileName, ".");
    rootData[0].inodeId = newInode;
    strcpy(rootData[1].fileName, "..");
    rootData[1].inodeId = parentId;
    writeToInode(newInode, sizeof(struct dir_data) * 2, (void*)rootData);
    return newInode;
}

void updateInode(struct inode* node) {
    struct super_block* superBlock = getSuperBlock();
    long int offset = superBlock->inodesTableOffset + (long)(node->id * sizeof(struct inode));
    writeToFile(node, sizeof(struct inode), 1, offset);
    free(superBlock);
}

struct inode* getInodeById(size_t id) {
    struct super_block* superBlock = getSuperBlock();
    long int offset = superBlock->inodesTableOffset + (long)(id * sizeof(struct inode));
    struct inode* node = (struct inode*)malloc(sizeof(struct inode));
    readFromFile(node, sizeof(struct inode), 1, offset);
    free(superBlock);
    return node;
}

void writeToInode(size_t id, size_t size, void* data) {
    struct inode* node = getInodeById(id);
    struct super_block* superBlock = getSuperBlock();
    if (size > superBlock->maxFileSize) {
        printf("Превышен максимально допустимый размер файла, %d", (int)superBlock->maxFileSize);
        return;
    }
    size_t writePointer = 0;
    node->fileSize = size;
    for (size_t i = 0; i < superBlock->blocksPerInode && writePointer < size; ++i) {
        struct block_info* block = &(node->blocksInfo[i]);
        // дальше происходит следующее: достаем очередной блок из узла и смотрим
        // если блок использует прямую адресацию, то сразу в него записываем данные и сдвигаем пишущий указатель дальше
        // если блок использует косвенную адресацию, то сначала получаем эти блоки косвенной адресации, а потом
        if (block->isDirect) {
            // блок использует прямую адресацию, сразу в него записываем информацию
            size_t sizeToWrite = (size - writePointer) > superBlock->blockSize
                                 ? superBlock->blockSize
                                 : size - writePointer;
            void* tempData = malloc(sizeToWrite);
            memcpy(tempData, data + writePointer, sizeToWrite);
            writeToBlock(block->id, tempData);
            writePointer += sizeToWrite;
            block->isEmpty = false;
            free(tempData);
        } else {
            size_t addBlocksCount = superBlock->blocksPerInode;
            size_t* additionalBlocks;
            // проверям, выделины ли вообще дополнительные блоки косвенной адресации
            if (!block->isEmpty) {
                additionalBlocks = (size_t*)malloc(addBlocksCount * sizeof(size_t));
                readFromBlock(block->id, additionalBlocks);
            } else {
                // если не выделины, выделяем кол-во доп косвенных блоков в таком же кол-ве,
                // как кол-во блоков на i-node
                additionalBlocks = getFreeBlocks(superBlock->blocksPerInode);
                block->isEmpty = false;
                writeToBlock(block->id, additionalBlocks);
            }
            // далее записываем в косвенные блоки информацию
            size_t j = 0;
            while (size - writePointer != 0 && j < addBlocksCount) {
                size_t sizeToWrite = (size - writePointer) > superBlock->blockSize
                        ? superBlock->blockSize
                        : size - writePointer;

                void* tempData = malloc(sizeToWrite);
                memcpy(tempData, data + writePointer, sizeToWrite);
                writeToBlock(additionalBlocks[j], tempData);
                writePointer += sizeToWrite;
                j++;
                free(tempData);
            }
            free(additionalBlocks);
        }
    }

    updateInode(node);
    free(node);
    free(superBlock);
}

void* readFromInode(size_t id) {
    struct super_block* superBlock = getSuperBlock();
    struct inode* node = getInodeById(id);
    size_t readerPointer = 0;
    size_t size = node->fileSize;
    void* data = malloc(size);
    for (size_t i = 0; i < superBlock->blocksPerInode && readerPointer < size; ++i) {
        struct block_info* block = &(node->blocksInfo[i]);
        if (block->isDirect) {
            size_t sizeToRead = (size - readerPointer) > superBlock->blockSize
                                 ? superBlock->blockSize
                                 : size - readerPointer;
            void* tempData = malloc(sizeToRead);
            readFromBlock(block->id, tempData);
            memcpy(data + readerPointer, tempData, sizeToRead);
            readerPointer += sizeToRead;
            free(tempData);
        } else {
            size_t addBlocksCount = superBlock->blocksPerInode;
            size_t* additionalBlocks = (size_t*)malloc(sizeof(size_t) * addBlocksCount);
            for (size_t j = 0; j < addBlocksCount && readerPointer < size; ++j) {
                size_t sizeToRead = (size - readerPointer) > superBlock->blockSize
                                    ? superBlock->blockSize
                                    : size - readerPointer;
                void* tempData = malloc(sizeToRead);
                readFromBlock(additionalBlocks[j], tempData);
                memcpy(data + readerPointer, tempData, sizeToRead);
                readerPointer += sizeToRead;
                free(tempData);
            }
            free(additionalBlocks);
        }
    }
    free(superBlock);
    free(node);
    return data;
}

struct inode* getCurrentDirInode() {
    struct super_block* superBlock = getSuperBlock();
    struct inode* node = getInodeById(superBlock->currentInode);
    free(superBlock);
    return node;
}

size_t getCountOfFiles(struct inode* node) {
    return (size_t)(node->fileSize / sizeof(struct dir_data));
}


#endif //LINUX_FS_BASE_H
