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
    free(node);
    free(superBlock);
    return 0;
}