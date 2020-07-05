//
// Created by sikalov on 11/21/19.
//

#ifndef FILE_SYSTEM_FS_STORE_FUNCTIONS_H
#define FILE_SYSTEM_FS_STORE_FUNCTIONS_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Считывает с файла с определенного места указанное кол-во байт
 * @param data - указатель, куда будут записываться данные
 * @param fileName - имя файла, откуда будем считывать инфу
 * @param size - размер считываемого элемента
 * @param count - кол-во считываемых элементов
 * @param offset - смещение с начала файла для считывания данных в байтах
 */
void readFromFile(void* data, size_t size, size_t count, const long int offset) {
    char fileNameStore[100];
    strcpy(fileNameStore, getenv("FILE_STORE"));
    FILE* filePtr = fopen(fileNameStore, "r+");
    if (filePtr == NULL) {
        printf("Не удалось открыть файл %s", fileNameStore);
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
void writeToFile(void* data, size_t size, size_t count, const long int offset) {
    char fileNameStore[100];
    strcpy(fileNameStore, getenv("FILE_STORE"));
    FILE* filePtr = fopen(fileNameStore, "r+");
    if (filePtr == NULL) {
        printf("Не удалось открыть файл %s", fileNameStore);
        exit(1);
    }
    fseek(filePtr, offset, SEEK_SET);
    fwrite(data, size, count, filePtr);
    fclose(filePtr);
}

#endif //FILE_SYSTEM_FS_STORE_FUNCTIONS_H
