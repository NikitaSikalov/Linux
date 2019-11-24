//
// Created by sikalov on 11/21/19.
//

#include "fs_base.h"

int main() {
    // инициализурем супер-блок и записываем его в файл
    initSuperBlock();

    // инициализируем карту блоков
    initBlocksMap();

    // инициализируем карту i-nodes
    initInodesMap();

    struct super_block* superBlock = getSuperBlock();
    // Инициадизируем корневой каталог /
    struct inode* node = createInodeByIdx(superBlock->rootInode, superBlock->rootInode, true, "/");
    size_t sizeRootData = sizeof(struct dir_data) * 2;
    struct dir_data rootData[2];
    strcpy(rootData[0].fileName, ".");
    rootData[0].inodeId = node->id;
    strcpy(rootData[1].fileName, "..");
    rootData[1].inodeId = node->id;
    writeToInode(node->id, sizeRootData, (void*)rootData);
    free(node);
    free(superBlock);
    printf("Инициализация файловой системы прошла успешно!");
    return 0;
}