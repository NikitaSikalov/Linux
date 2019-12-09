//
// Created by sikalov on 12/9/19.
//

#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <zconf.h>

#define MAX_LENGTH_COMMAND 100
#define BUF_SIZE 1024

int sock;
struct sockaddr_in addr;

void execCommand(char* command, char* buf) {
    send(sock, command, sizeof(char) * MAX_LENGTH_COMMAND, 0);
    recv(sock, buf, sizeof(char) * BUF_SIZE, 0);
}

int main() {
    size_t maxLengthCommand = MAX_LENGTH_COMMAND;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    char buf[BUF_SIZE];
    char* cmd = NULL;
    while(1) {
        getline(&cmd, &maxLengthCommand, stdin);
        cmd = strtok(cmd, " \n\t");
        if (strcmp(cmd, "exit") == 0) {
            break;
        }
        execCommand(cmd, buf);
        printf("%s", buf);
    }
    close(sock);
}