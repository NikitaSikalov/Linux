//
// Created by sikalov on 11/21/19.
//

#include "fs_base.h"

int main() {
    // инициализурем супер-блок и записываем его в файл
    initSuperBlock();

    // инициализируем карту блоков
    initBlocksMap();
    return 0;
}