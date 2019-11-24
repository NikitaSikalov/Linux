//
// Created by sikalov on 11/21/19.
//

#ifndef FILE_SYSTEM_OPERATIONS_H
#define FILE_SYSTEM_OPERATIONS_H

void ls();
void printStartString();
void mkdir(char*);

//------------------------------------
void printStartString() {
    struct inode* node = getCurrentDirInode();
    printf("myFS:%s$ ", node->fileName);
    free(node);
}

void ls() {
    struct inode* node = getCurrentDirInode();
    struct dir_data* dirData = (struct dir_data*)readFromInode(node->id);
    size_t countFiles = (int)(node->fileSize / sizeof(struct dir_data));
    for (size_t i = 0; i < countFiles; ++i) {
        printf("%s\t", dirData[i].fileName);
    }
    printf("\n");
    free(node);
    free(dirData);
}

void mkdir(char* dirName) {
    struct super_block* superBlock = getSuperBlock();
    if (dirName == NULL) {
        printf("Пожалуйста введите имя директории\n");
        return;
    }
    if (strlen(dirName) > superBlock->maxFileName) {
        printf("Первышена допустимая длина файла, %lu\n", superBlock->maxFileName);
        return;
    }
    struct inode* node = getCurrentDirInode();
    struct dir_data* dirData = (struct dir_data*)readFromInode(node->id);
    size_t countFiles = (int)(node->fileSize / sizeof(struct dir_data));
    for (size_t i = 0; i < countFiles; ++i) {
        if (strcmp(dirData[i].fileName, dirName) == 0) {
            printf("Папка с таким именем уже существует в данной директории");
            return;
        }
    }
    size_t newDirSize = (countFiles + 1) * sizeof(struct dir_data);
    dirData = realloc(dirData, newDirSize);
    size_t inodeForNewDir = createInode(node->id, true, dirName);
    strcpy(dirData[countFiles].fileName, dirName);
    dirData[countFiles].inodeId = inodeForNewDir;
    writeToInode(node->id, newDirSize, (void* )dirData);
    free(superBlock);
    free(node);
    free(dirData);
}

#endif //FILE_SYSTEM_OPERATIONS_H
