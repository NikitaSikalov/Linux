//
// Created by sikalov on 11/18/19.
//

#include <netinet/in.h>
#include <zconf.h>
#include <syslog.h>
#include "fs_base.h"
#include "operations.h"
#include "demonize.h"

void processCommand(char cmd[], char buf[]) {
    cmd = strtok(cmd, " \n\t");
    if (cmd == NULL) {
        strcpy(buf, "\0");
        return;
    }
    if (strcmp(cmd, "ls") == 0) {
        ls(buf);
        return;
    }
    if (strcmp(cmd, "mkdir") == 0) {
        cmd = strtok(NULL, " \n\t");
        mkdir(cmd, buf);
        return;
    }
    if (strcmp(cmd, "touch") == 0) {
        cmd = strtok(NULL, " \n\t");
        touch(cmd, buf);
        return;
    }
    if (strcmp(cmd, "cd") == 0) {
        cmd = strtok(NULL, " \n\t");
        struct super_block* superBlock = getSuperBlock();
        cd(cmd, superBlock->currentInode, buf);
        free(superBlock);
        return;
    }
    if (strcmp(cmd, "echo") == 0) {
        cmd = strtok(NULL, "\"");
        if (cmd == NULL) {
            strcpy(buf, ANSI_COLOR_RED"Введите сообщение\n"ANSI_COLOR_RESET);
            return;
        }

        size_t sizeOfInput = sizeof(char) * strlen(cmd);
        size_t maxFileSize = getMaxFileSize();
        if (sizeOfInput > maxFileSize) {
            strcpy(buf, ANSI_COLOR_RED"Превышен максимальный размер файла\n"ANSI_COLOR_RESET);
            return;
        }
        char* text = (char*)malloc(sizeOfInput);
        strcpy(text, cmd);

        cmd = strtok(NULL, " >\n\t");
        if (cmd == NULL) {
            strcpy(buf, text);
            strcat(buf, "\n");
            return;
        }
        echo(cmd, text, buf);
        free(text);
        return;
    }
    if (strcmp(cmd, "cat") == 0) {
        cmd = strtok(NULL, " \n\t");
        if (cmd == NULL) {
            strcpy(buf, ANSI_COLOR_RED"Введите имя файла\n"ANSI_COLOR_RESET);
            return;
        }
        cat(cmd, buf);
        return;
    }
    if (strcmp(cmd, "rm") == 0) {
        cmd = strtok(NULL, " \n\t");
        if (cmd == NULL) {
            strcpy(buf, ANSI_COLOR_RED"Введите имя файла\n"ANSI_COLOR_RESET);
            return;
        }
        rm(cmd, buf);
        return;
    }
    if (strcmp(cmd, "help") == 0) {
        help(buf);
        return;
    }
    if (strcmp(cmd, "exit") == 0) {
        exit(0);
    }
}

int main() {
    demonize();
    struct sockaddr_in addr;
    char buf[MAX_BUF_SIZE];
    int bytes_read, sock;
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listener, (struct sockaddr*)&addr, sizeof(addr));
    listen(listener, 1);


    struct super_block* superBlock1 = getSuperBlock();
    changeCurrentInode(superBlock1->rootInode);
    free(superBlock1);

    while (1) {
        sock = accept(listener, NULL, NULL);
        pid_t pid = fork();
        // родительский процесс создал дочерний процесс для обработки запроса, чтобы не блокировать остальных
        if (pid > 0) {
            continue;
        }
        syslog(LOG_DEBUG, "Подсоединился новый пользователь, sock = %d. Обрабатывает процесс pid = %d", sock, getpid());

        help(buf);
        send(sock, buf, MAX_BUF_SIZE, 0);
        while(1) {
            printStartString(buf);
            send(sock, buf, MAX_BUF_SIZE, 0);

            bytes_read = recv(sock, buf, MAX_BUF_SIZE, 0);
            if (bytes_read <= 0) break;

            char command[MAX_BUF_SIZE];
            strcpy(command, buf);
            strcpy(buf, "\0");
            processCommand(command, buf);
            send(sock, buf, MAX_BUF_SIZE, 0);
        }
        syslog(LOG_DEBUG, "Сессия с sock = %d окончена. Процесс с pid = %d завершился", sock, getpid());
        close(sock);
        return 0;
    }
}
