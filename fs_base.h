#ifndef LINUX_FS_BASE_H
#define LINUX_FS_BASE_H

#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "define.h"

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
    unsigned short blocksIdxes[16];      // индексы блоков, который использует данный i-узел
};

struct inode* createInode(unsigned int id, char* fileName, bool isDir, unsigned short freeBlockIdxes[16]) {
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
    memcpy(newInode->blocksIdxes, freeBlockIdxes, 16 * sizeof(unsigned short));
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
void readFromFile(void* data, char* fileName, size_t size, size_t count, long int offset) {
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
void writeToFile(void* data, char* fileName, size_t size, size_t count, long int offset) {
    FILE* filePtr = fopen(fileName, "r+");
    if (filePtr == NULL) {
        printf("Не удалось открыть файл %s", fileName);
        exit(1);
    }
    fseek(filePtr, offset, SEEK_SET);
    fwrite(data, size, count, filePtr);
    fclose(filePtr);
}

#endif //LINUX_FS_BASE_H
