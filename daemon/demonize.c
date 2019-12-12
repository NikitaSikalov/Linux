//
// Created by sikalov on 12/8/19.
//

#include <fcntl.h>
#include <zconf.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <syslog.h>

int demonize() {
    pid_t pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // родительский процесс, завершаем его
        exit(EXIT_SUCCESS);
    }

    // создаем новую сессию
    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    // второй fork (нафига он нужен??)
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);
    chdir("/");
    openlog("mydaemon", LOG_PID, LOG_DAEMON);
    syslog(LOG_DEBUG, "Запущен демон, pid = %d", getpid());
    return 0;
}