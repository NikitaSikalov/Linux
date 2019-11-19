//
// Created by sikalov on 11/18/19.
//

#include <zconf.h>
#include "define.h"
#include "fs_base.h"

char fileName[] = "tmpFile";
const __uint64_t offsetSuperBlock = 0;
const __uint64_t offsetBlocksMap = offsetSuperBlock + sizeof(struct super_block);
const __uint64_t offsetInodesMap = offsetBlocksMap + sizeof(bool) * BLOCKS_COUNT;
const __uint64_t offsetTableInodes = offsetInodesMap + sizeof(bool) * INODES_COUNT;
const __uint64_t offsetDataBlocks = offsetTableInodes + sizeof(struct inode) * INODES_COUNT;

int main() {
    // инициализурем супер-блок и записываем его в начало в файла
    struct super_block* superBlock = initSuperBlock();
    writeToFile(superBlock, fileName, sizeof(struct super_block), 1, offsetSuperBlock);

    // инициализируем карту блоков памяти и записываем эту карту также в исходный файл fs
    bool* blocksMap = initBlocksMap();
    writeToFile(blocksMap, fileName, sizeof(bool), BLOCKS_COUNT, offsetBlocksMap);

    bool* inodesMap = initInodesMap();
    writeToFile(blocksMap, fileName, sizeof(bool), INODES_COUNT, offsetInodesMap);

    free(inodesMap);
    free(blocksMap);
    free(superBlock);
    return 0;
}
