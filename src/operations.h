//
// Created by sikalov on 11/21/19.
//

#ifndef FILE_SYSTEM_OPERATIONS_H
#define FILE_SYSTEM_OPERATIONS_H

void ls();
void printStartString();
void mkdir(char*);

void printStartString();
void addFileToCurrentDir(char* fileName, bool isDir);

//------------------------------------ UTILS -----------------------------------
void printStartString() {
    struct inode* node = getCurrentDirInode();
    printf("myFS:%s$ ", node->fileName);
    free(node);
}

void addFileToCurrentDir(char* fileName, bool isDir) {
    struct super_block* superBlock = getSuperBlock();
    if (fileName == NULL) {
        printf("Пожалуйста введите имя файла\n");
        return;
    }
    if (strlen(fileName) > superBlock->maxFileName) {
        printf("Первышена допустимая длина файла, %lu\n", superBlock->maxFileName);
        return;
    }
    struct inode* node = getCurrentDirInode();
    struct dir_data* dirData = (struct dir_data*)readFromInode(node->id);
    size_t countFiles = getCountOfFiles(node);
    for (size_t i = 0; i < countFiles; ++i) {
        if (strcmp(dirData[i].fileName, fileName) == 0) {
            printf("Файл с таким именем уже существует в данной директории");
            return;
        }
    }
    size_t newDirSize = (countFiles + 1) * sizeof(struct dir_data);
    dirData = realloc(dirData, newDirSize);

    size_t inodeForNewDir = createDirInode(node->id, fileName);
    strcpy(dirData[countFiles].fileName, fileName);
    dirData[countFiles].inodeId = inodeForNewDir;
    writeToInode(node->id, newDirSize, (void* )dirData);

    free(superBlock);
    free(node);
    free(dirData);
}

//----------------------------- COMMANDS -------------------------------

void ls() {
    struct inode* node = getCurrentDirInode();
    struct dir_data* dirData = (struct dir_data*)readFromInode(node->id);
    size_t countFiles = getCountOfFiles(node);
    for (size_t i = 0; i < countFiles; ++i) {
        printf("%s\t", dirData[i].fileName);
    }
    printf("\n");
    free(node);
    free(dirData);
}



void mkdir(char* dirName) {
    addFileToCurrentDir(dirName, true);
}

void touch(char* fileName) {
    addFileToCurrentDir(fileName, false);
}

void cd(char* path, size_t startInode) {
    if (path == NULL || *path == '\0') {
        return;
    }
    struct inode* node = getCurrentDirInode();
    char* nextDir = (char *)malloc(sizeof(char) * strlen(path));
    strcpy(nextDir, path);
    strtok(nextDir, "/");
    struct dir_data* dirData = (struct dir_data*)readFromInode(node->id);
    size_t countFiles = getCountOfFiles(node);
    for (size_t i = 0; i < countFiles; ++i) {
        if(strcmp(dirData[i].fileName, nextDir) == 0) {
            char *nextPath = strchr(path, '/');
            changeCurrentInode(dirData[i].inodeId);
            if (nextPath != NULL) {
                char* newPath = nextPath + 1;
                cd(newPath, startInode);
            }
            free(nextDir);
            free(node);
            free(dirData);
            return;
        }
    }
    changeCurrentInode(startInode);
    printf("Невозможно перейти по такому пути\n");
    free(nextDir);
    free(node);
    free(dirData);
}


#endif //FILE_SYSTEM_OPERATIONS_H
