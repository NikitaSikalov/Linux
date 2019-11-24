//
// Created by sikalov on 11/18/19.
//

#include "fs_base.h"
#include "operations.h"

int main() {
    struct inode* node = getCurrentDirInode();
    printStartString();
    free(node);
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
        } else if (strcmp(cmd, "exit") == 0) {
            return 0;
        }
        printStartString();
    }
}
