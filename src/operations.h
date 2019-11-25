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
char* getCurrentPath();

//------------------------------------ UTILS -----------------------------------
char* getCurrentPath() {
    struct inode* node = getCurrentDirInode();
    struct super_block* superBlock = getSuperBlock();
    size_t MAX_PATH_SIZE = superBlock->maxFileName * 100;
    char* currentPath = (char*)malloc(sizeof(char) * MAX_PATH_SIZE);
    if (node->id == superBlock->rootInode) {
        return "/";
    }
    while (node->id != superBlock->rootInode) {
        char tmp[MAX_PATH_SIZE];
        strcpy(tmp, "/");
        strcat(tmp, node->fileName);
        strcat(tmp, currentPath);
        currentPath = strcpy(currentPath, tmp);
        size_t parentIdBlock = node->parentId;
        free(node);
        node = getInodeById(parentIdBlock);
    }
    free(node);
    free(superBlock);
    return currentPath;
}

void printStartString() {
    struct inode* node = getCurrentDirInode();
    char* currentPath = getCurrentPath();
    printf(ANSI_COLOR_GREEN"myFS"ANSI_COLOR_RESET);
    printf(":");
    printf(ANSI_COLOR_BLUE"%s"ANSI_COLOR_RESET, currentPath);
    printf("$ ");
    free(node);
    //free(currentPath);
}

void addFileToCurrentDir(char* fileName, bool isDir) {
    struct super_block* superBlock = getSuperBlock();
    if (fileName == NULL) {
        printf(ANSI_COLOR_YELLOW"Пожалуйста введите имя файла\n"ANSI_COLOR_RESET);
        return;
    }
    if (strlen(fileName) > superBlock->maxFileName) {
        printf(ANSI_COLOR_RED"Первышена допустимая длина файла, %lu\n"ANSI_COLOR_RESET, superBlock->maxFileName);
        return;
    }
    struct inode* node = getCurrentDirInode();
    struct dir_data* dirData = (struct dir_data*)readFromInode(node->id);
    size_t countFiles = getCountOfFiles(node);
    for (size_t i = 0; i < countFiles; ++i) {
        if (strcmp(dirData[i].fileName, fileName) == 0) {
            printf(ANSI_COLOR_RED"Файл с таким именем уже существует в данной директории"ANSI_COLOR_RED);
            return;
        }
    }
    size_t newDirSize = (countFiles + 1) * sizeof(struct dir_data);
    dirData = realloc(dirData, newDirSize);

    size_t inodeForNewFile;
    if (isDir) {
        inodeForNewFile = createDirInode(node->id, fileName);
    } else {
        inodeForNewFile = createInode(node->id, false, fileName);
    }
    strcpy(dirData[countFiles].fileName, fileName);
    dirData[countFiles].inodeId = inodeForNewFile;
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
        struct inode* nodeOfFile = getInodeById(dirData[i].inodeId);
        if (nodeOfFile->isDir) {
            printf(ANSI_COLOR_BLUE"%s\t"ANSI_COLOR_RESET, dirData[i].fileName);
        } else {
            printf("%s\t", dirData[i].fileName);
        }
        free(nodeOfFile);
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
    if (*path == '/') {
        struct super_block* superBlock = getSuperBlock();
        changeCurrentInode(superBlock->rootInode);
        free(superBlock);
        path = path + 1;
    }
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
            if (!changeCurrentInode(dirData[i].inodeId)) {
                // при неудачной попытки перейти в директорию, выходим из цикла
                break;
            }
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
    printf(ANSI_COLOR_RED"Невозможно перейти по такому пути\n"ANSI_COLOR_RESET);
    free(nextDir);
    free(node);
    free(dirData);
}


#endif //FILE_SYSTEM_OPERATIONS_H
