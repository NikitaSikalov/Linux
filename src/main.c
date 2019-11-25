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
        } else if (strcmp(cmd, "exit") == 0) {
            return 0;
        }
        printStartString();
    }
}
