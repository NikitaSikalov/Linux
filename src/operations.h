//
// Created by sikalov on 11/21/19.
//

#ifndef FILE_SYSTEM_OPERATIONS_H
#define FILE_SYSTEM_OPERATIONS_H

void ls();
void mkdir(char*);
void echo(char*, char*);
void cd(char* path, size_t startInode);
void cat(char*);
void rm(char*);
void help();

void printStartString();
void addFileToCurrentDir(char* fileName, bool isDir);
char* getCurrentPath();
struct inode* getInodeByFileName(char* fileName);
void help();

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
        //free(node);
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
    if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0) {
        printf(ANSI_COLOR_RED"Недопустимое имя файла\n"ANSI_COLOR_RESET);
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
            printf(ANSI_COLOR_RED"Файл с таким именем уже существует в данной директории\n"ANSI_COLOR_RED);
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

size_t getMaxFileSize() {
    struct super_block* superBlock = getSuperBlock();
    size_t size =superBlock->maxFileSize;
    free(superBlock);
    return size;
}

struct inode* getInodeByFileName(char* fileName) {
    struct inode* node = getCurrentDirInode();
    struct dir_data* dirData = (struct dir_data*)readFromInode(node->id);
    size_t countFiles = getCountOfFiles(node);
    for (size_t i = 0; i < countFiles; ++i) {
        if(strcmp(fileName, dirData[i].fileName) == 0) {
            struct inode* foundNode = getInodeById(dirData[i].inodeId);
            free(node);
            free(dirData);
            return foundNode;
        }
    }
    free(node);
    free(dirData);
    return NULL;
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

void echo(char* fileName, char* info) {
    if (fileName == NULL || *fileName == '\0') {
        printf(ANSI_COLOR_YELLOW"Введите имя файла\n"ANSI_COLOR_RESET);
        return;
    }
    struct inode* node = getCurrentDirInode();
    struct dir_data* dirData = (struct dir_data*)readFromInode(node->id);
    size_t countFiles = getCountOfFiles(node);
    for (size_t i = 0; i < countFiles; ++i) {
        if(strcmp(fileName, dirData[i].fileName) == 0) {
            struct inode* foundNode = getInodeById(dirData[i].inodeId);
            if (foundNode->isDir) {
                printf(ANSI_COLOR_RED"echo нельзя применть к директории %s\n"ANSI_COLOR_RESET, fileName);
                return;
            }
            writeToInode(foundNode->id, (strlen(info) + 1) * sizeof(char), info);
            free(foundNode);
            return;
        }
    }
    printf(ANSI_COLOR_RED"Файл %s не найден\n"ANSI_COLOR_RESET, fileName);
    free(node);
    free(dirData);
}

void cat(char* fileName) {
    if (fileName == NULL || *fileName == '\0') {
        printf(ANSI_COLOR_YELLOW"Введите имя файла\n"ANSI_COLOR_RESET);
        return;
    }
    struct inode* node = getCurrentDirInode();
    struct dir_data* dirData = (struct dir_data*)readFromInode(node->id);
    size_t countFiles = getCountOfFiles(node);
    for (size_t i = 0; i < countFiles; ++i) {
        if(strcmp(fileName, dirData[i].fileName) == 0) {
            struct inode* foundNode = getInodeById(dirData[i].inodeId);
            if (foundNode->isDir) {
                printf(ANSI_COLOR_RED"cat нельзя применть к директории %s\n"ANSI_COLOR_RESET, fileName);
                return;
            }
            char* info = readFromInode(foundNode->id);
            free(foundNode);
            printf("%s\n", info);
            free(info);
            return;
        }
    }
    printf(ANSI_COLOR_RED"Файл %s не найден\n"ANSI_COLOR_RESET, fileName);
    free(node);
    free(dirData);
}

void rm(char* fileName) {
    struct inode* node = getInodeByFileName(fileName);
    if (strcmp(fileName, ".") == 0 || strcmp(fileName, "..") == 0) {
        return;
    }
    if (node == NULL) {
        printf(ANSI_COLOR_RED"Файл не найден\n"ANSI_COLOR_RESET);
        return;
    }
    struct inode* currentNode = getCurrentDirInode();
    // Если удаляемый файл является директорий, то перед ее удалением необходимо почистить ее содержимое
    if (node->isDir) {
        // переходим сначала в удаляемую папку
        cd(fileName, currentNode->id);
        // считываем какие в ней есть файлы
        struct dir_data* dirData = readFromInode(node->id);
        size_t countOfFiles = getCountOfFiles(node);
        // удаляем каждый файл
        for (size_t i = 0; i < countOfFiles; ++i) {
            rm(dirData[i].fileName);
        }
        // вовзвращаемся обратно и далее эту узел к этой директории можно спокойно удалять
        cd("..", currentNode->id);
    }

    // подчищаем записи об этой файле в текущей директории
    struct dir_data* currentDirData = readFromInode(currentNode->id);
    size_t count = getCountOfFiles(currentNode);
    size_t newDirDataSize = (count - 1) * sizeof(struct dir_data);
    struct dir_data* newDirData = (struct dir_data*)malloc(newDirDataSize);
    for (size_t i = 0, pointer = 0; i < count; ++i) {
        if (strcmp(currentDirData[i].fileName, fileName) != 0) {
            strcpy(newDirData[pointer].fileName, currentDirData[i].fileName);
            newDirData[pointer].inodeId = currentDirData[i].inodeId;
            pointer++;
        }
    }
    writeToInode(currentNode->id, newDirDataSize, newDirData);
    makeFreeInode(node);
    free(node);
    free(currentNode);
}

void help() {
    printf(ANSI_COLOR_MAGENTA"⍰ "ANSI_COLOR_RESET);
    printf("Помощь по командам");
    printf(ANSI_COLOR_MAGENTA" ⍰ \n"ANSI_COLOR_RESET);

    printf(ANSI_COLOR_BLUE"• "ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW"ls"ANSI_COLOR_RESET);
    printf(" —— выводит список файлов в текущей директории\n");

    printf(ANSI_COLOR_BLUE"• "ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW"mkdir <name of dir>"ANSI_COLOR_RESET);
    printf(" —— создает директорию в текущей директории\n");

    printf(ANSI_COLOR_BLUE"• "ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW"touch <name of file>"ANSI_COLOR_RESET);
    printf(" —— создает файл в текущей директории\n");

    printf(ANSI_COLOR_BLUE"• "ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW"echo <string> > <file name>"ANSI_COLOR_RESET);
    printf(" —— делает запись в файл\n");

    printf(ANSI_COLOR_BLUE"• "ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW"cat <file name>"ANSI_COLOR_RESET);
    printf(" —— выводит строковые данные из файла\n");

    printf(ANSI_COLOR_BLUE"• "ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW"cd <path>"ANSI_COLOR_RESET);
    printf(" —— переходит в выбранную директорию по заданному (можно задать путь абсолютно)\n");

    printf(ANSI_COLOR_BLUE"• "ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW"rm <file/dir name>"ANSI_COLOR_RESET);
    printf(" —— удаляет файл в текущей директории. Если файл директория, то удаляет ее и рекурсвино все подфайлы в ней\n");

    printf(ANSI_COLOR_BLUE"• "ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW"help"ANSI_COLOR_RESET);
    printf(" —— выводит справку по командам\n");

    printf(ANSI_COLOR_BLUE"• "ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW"exit"ANSI_COLOR_RESET);
    printf(" —— выходит из программы\n");
}

#endif //FILE_SYSTEM_OPERATIONS_H
