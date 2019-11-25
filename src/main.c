//
// Created by sikalov on 11/18/19.
//

#include "fs_base.h"
#include "operations.h"

int main() {
    struct super_block* superBlock1 = getSuperBlock();
    changeCurrentInode(superBlock1->rootInode);
    free(superBlock1);
    printStartString();
    size_t maxLengthCommand = 100;
    char* cmd = NULL;
    while (true) {
        getline(&cmd, &maxLengthCommand, stdin);
        cmd = strtok(cmd, " \n\t");
        if (cmd == NULL) {
            printStartString();
            continue;
        }
        if (strcmp(cmd, "ls") == 0) {
            ls();
        } else if (strcmp(cmd, "mkdir") == 0) {
            cmd = strtok(NULL, " \n\t");
            mkdir(cmd);
        } else if (strcmp(cmd, "touch") == 0) {
            cmd = strtok(NULL, " \n\t");
            touch(cmd);
        } else if (strcmp(cmd, "cd") == 0) {
            cmd = strtok(NULL, " \n\t");
            struct super_block* superBlock = getSuperBlock();
            cd(cmd, superBlock->currentInode);
            free(superBlock);
        } else if (strcmp(cmd, "echo") == 0) {
            cmd = strtok(NULL, "\"");
            if (cmd == NULL) {
                printf(ANSI_COLOR_RED"Введите сообщение\n"ANSI_COLOR_RESET);
                printStartString();
                continue;
            }

            size_t sizeOfInput = sizeof(char) * strlen(cmd);
            size_t maxFileSize = getMaxFileSize();
            if (sizeOfInput > maxFileSize) {
                printf(ANSI_COLOR_RED"Превышен максимальный размер файла\n"ANSI_COLOR_RESET);
                printStartString();
                continue;
            }
            char* text = (char*)malloc(sizeOfInput);
            strcpy(text, cmd);

            cmd = strtok(NULL, " >\n\t");
            if (cmd == NULL) {
                printf("%s\n", text);
                printStartString();
                continue;
            }
            echo(cmd, text);
            free(text);
        } else if (strcmp(cmd, "cat") == 0) {
            cmd = strtok(NULL, " \n\t");
            if (cmd == NULL) {
                printf(ANSI_COLOR_RED"Введите имя файла\n"ANSI_COLOR_RESET);
                printStartString();
                continue;
            }
            cat(cmd);
        } else if (strcmp(cmd, "exit") == 0) {
            return 0;
        }
        printStartString();
    }
}
