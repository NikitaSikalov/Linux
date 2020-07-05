//
// Created by sikalov on 11/21/19.
//

#ifndef FILE_SYSTEM_BASE_STRUCTURES_H
#define FILE_SYSTEM_BASE_STRUCTURES_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "define.h"

/**
 * Структура супер-блока файловой системы
 */
struct super_block {
    // Блоки
    size_t blocksCount;
    size_t freeBlocksCount;
    size_t blockSize;
    // i-nodes
    size_t inodesCount;
    size_t inodesFreeCount;
    // offsets для считывания и записи в FS_STORE
    long int superBlockOffset;
    long int blocksMapOffset;
    long int inodesMapOffset;
    long int inodesTableOffset;
    long int dataBlocksOffset;

    size_t currentInode;    // указывает на i-node текущей папки
    size_t rootInode;       // указывает на корневой каталог
    size_t blocksPerInode;  // кол-во блоков на один i-узел
    size_t maxFileName;     // максимальное имя файла
    size_t maxFileSize;     // максимальный размер файла
    size_t blocksCountWithDirectAddress;       // кол-во блоков в i-node с прямой адресацией
};

struct block_info {
    size_t id;          // индекс i-node
    bool isEmpty;    // флаг занятости блока
    bool isDirect;     // прямая адресация или косвенная
};

/**
 * Для упрощения в нашей файловой системе будет только прямая и косвенная адресация
 */
struct inode {
    size_t id;
    size_t parentId;

    bool isDir;     // считаем что в нашей ФС будет всего два типа файлов -> dir - true и file - false
    size_t fileSize;    // реальный размер файла

    char fileName[MAX_FILE_NAME];

    struct block_info blocksInfo[BLOCKS_PER_INODE];      // индексы блоков, который использует данный i-узел
};

struct dir_data {
    size_t inodeId;
    char fileName[MAX_FILE_NAME];
};



#endif //FILE_SYSTEM_BASE_STRUCTURES_H
