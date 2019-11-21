//
// Created by sikalov on 11/18/19.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main() {
    char FS_STORE[100];
    strcpy(FS_STORE, getenv("FILE_STORE"));
    printf("%s\n", FS_STORE);
    return 0;
}
