#ifndef LINUX_FS_BASE_H
#define LINUX_FS_BASE_H

#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "src/define.h"

/**
 * Структура супер-блока файловой системы
 */
struct super_block {
    size_t blocksCount;
    size_t freeBlocksCount;
    size_t inodesCount;
    size_t inodesFreeCount;

    unsigned int rootInode;
    size_t blockSize;
};

/**
 * Инициализация супер блока
 * @return
 */
struct super_block* initSuperBlock() {
    struct super_block* superBlock = (struct super_block*)malloc(sizeof(struct super_block));
    superBlock->blocksCount = BLOCKS_COUNT;
    superBlock->freeBlocksCount = BLOCKS_COUNT;
    superBlock->inodesCount = INODES_COUNT;
    superBlock->inodesFreeCount = INODES_COUNT;
    return superBlock;
}

/**
 * Для упрощения в нашей файловой системе будет только прямая и косвенная адресация
 */
struct inode {
    unsigned int id;
    bool isDir;  // считаем что в нашей ФС будет всего два типа файлов -> dir - true и file - false
    size_t fileSize;    // реальный размер файла
    time_t MTime;   // время последний модифкации файла
    time_t CTime;   // время создания файла

    char fileName[MAX_FILE_NAME] ;

    size_t blocksDirectDataCount;     // это число адресных ячеек использующие прямую адресацию
    unsigned short blocksIdxes[BLOCKS_PER_INODE];      // индексы блоков, который использует данный i-узел
};

struct inode* createInode(short int id, char* fileName, bool isDir, short int freeBlockIdxes[BLOCKS_PER_INODE]) {
    struct inode* newInode = (struct inode*)malloc(sizeof(struct inode));
    newInode->isDir = isDir;
    const size_t fileNameLength = strlen(fileName);
    if (fileNameLength > MAX_FILE_NAME) {
        return NULL;
    }
    strcpy(newInode->fileName, fileName);
    time(&newInode->CTime);
    time(&newInode->MTime);
    newInode->blocksDirectDataCount = DIRECT_COUNT_BLOCKS;
    memcpy(newInode->blocksIdxes, freeBlockIdxes, 16 * sizeof(short));
    return newInode;
}

/**
 * Считывает с файла с определенного места указанное кол-во байт
 * @param data - указатель, куда будут записываться данные
 * @param fileName - имя файла, откуда будем считывать инфу
 * @param size - размер считываемого элемента
 * @param count - кол-во считываемых элементов
 * @param offset - смещение с начала файла для считывания данных в байтах
 */
void readFromFile(void* data, const char* fileName, size_t size, size_t count, const long int offset) {
    FILE* filePtr = fopen(fileName, "r+");
    if (filePtr == NULL) {
        printf("Не удалось открыть файл %s", fileName);
        exit(1);
    }
    fseek(filePtr, offset, SEEK_SET);
    fread(data, size, count, filePtr);
    fclose(filePtr);
}

/**
 * Записыввает информацию в файл
 * @param data - указатель, куда будут записываться данные
 * @param fileName - имя файла, куда будем записывать данные
 * @param size - размер записываемого элемента
 * @param count - кол-во считываемых элементов
 * @param offset - смещение с начала файла для записи данных в байтах
 */
void writeToFile(void* data, const char* fileName, size_t size, size_t count, const long int offset) {
    FILE* filePtr = fopen(fileName, "r+");
    if (filePtr == NULL) {
        printf("Не удалось открыть файл %s", fileName);
        exit(1);
    }
    fseek(filePtr, offset, SEEK_SET);
    fwrite(data, size, count, filePtr);
    fclose(filePtr);
}

/**
 * Инициализирует карту свободные блоков - глобально это будет просто массив bool,
 * причем если true на индексе блока -> значит он свободен (считаем блок занятым, если он используется каким-то inode)
 * false -> занят
 * @return
 */
bool* initBlocksMap() {
    bool* freeBlocksMap = (bool*)malloc(BLOCKS_COUNT);
    for (size_t i = 0; i < BLOCKS_COUNT; ++i) {
        // все блоки изначально не заняты
        freeBlocksMap[i] = true;
    }
    return freeBlocksMap;
}

/**
 * Аналогичен initBlocksMap, но для inodes
 * @return
 */
bool* initInodesMap() {
    bool* freeInodesMap = (bool*)malloc(INODES_COUNT);
    for (size_t i = 0; i < INODES_COUNT; ++i) {
        // все блоки изначально не заняты
        freeInodesMap[i] = true;
    }
    return freeInodesMap;
}

const long int OFFSET_SUPER_BLOCK = 0;
const long int OFFSET_BLOCKS_MAP = OFFSET_SUPER_BLOCK + (long)sizeof(struct super_block);
const long int OFFSET_INODES_MAP = OFFSET_BLOCKS_MAP + (long)sizeof(bool) * BLOCKS_COUNT;
const long int OFFSET_INODES_TABLE = OFFSET_INODES_MAP + (long)sizeof(bool) * INODES_COUNT;
const long int OFFSET_DATA_BLOCKS = OFFSET_INODES_TABLE + (long)sizeof(struct inode) * INODES_COUNT;
const char FS_STORE[] = "tmpFile";
const size_t SUPER_BLOCK_SIZE = sizeof(struct super_block);
const size_t INODE_SIZE = sizeof(struct inode);


#endif //LINUX_FS_BASE_H
