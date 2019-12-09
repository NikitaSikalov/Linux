//
// Created by sikalov on 12/8/19.
//

#include <netinet/in.h>
#include <zconf.h>
#include <syslog.h>
#include "daemon/demonize.h"

int main() {
    demonize();
    struct sockaddr_in addr;
    char buf[1024];
    int bytes_read, sock;
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listener, (struct sockaddr*)&addr, sizeof(addr));
    listen(listener, 1);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        sock = accept(listener, NULL, NULL);
        pid_t pid = fork();
        // родительсвикй процесс создал дочерний процесс для обработки запроса, чтобы не блокировать остальных
        if (pid > 0) {
            continue;
        }
        syslog(LOG_DEBUG, "Подсоединился %d новый пользователь. Обрабатывает процесс %d", sock, getpid());
        while(1) {
            bytes_read = recv(sock, buf, 1024, 0);
            if (bytes_read <= 0) break;
            send(sock, buf, bytes_read, 0);
        }
        close(sock);
        return 0;
    }
#pragma clang diagnostic pop
}
