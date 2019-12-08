//
// Created by sikalov on 12/8/19.
//

#include "daemon/demonize.h"

int main() {
    demonize();
    while (1) {}
    return 0;
}
