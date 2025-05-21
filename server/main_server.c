#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <errno.h>
#include <dlfcn.h>

#include "../include/mod_loader.h"
#include "../include/logger.h"


#define PORT 5200
#define BUF_SIZE 256
#define MODULE_PATH "/home/iam/project/modules/libled_mod.so"

int server_fd = -1;  // 전역으로 선언해서 SIGINT에서 닫을 수 있게

// SIGINT 핸들러
void sigint_handler(int signo) {
    log_info("서버 종료 요청: SIGINT");
    if (server_fd != -1) close(server_fd);
    close_log();
    exit(0);
}

// 표준적인 데몬화 함수 (2단 fork)
void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_RDWR);
}

// 클라이언트 스레드 함수
void* client_handler(void* arg) {
    int clnt_sock = *(int*)arg;
    free(arg);

    char buffer[BUF_SIZE] = {0};
    log_info("클라이언트 스레드 시작 (TID: %lu)", pthread_self());

    void* handle = load_module(MODULE_PATH);
    if (!handle) {
        log_error("모듈 로딩 실패");
        write(clnt_sock, "FAIL: 모듈 로딩 실패\n", 22);
        close(clnt_sock);
        return NULL;
    }

    while (1) {
        memset(buffer, 0, BUF_SIZE);
        int len = read(clnt_sock, buffer, BUF_SIZE - 1);
        if (len <= 0) {
            log_info(len == 0 ? "클라이언트 연결 종료 (EOF)" : "read() 오류: %s", strerror(errno));
            break;
        }

        buffer[strcspn(buffer, "\r\n")] = 0; // 개행 제거
        log_info("수신 명령: %s", buffer);

        if (strcmp(buffer, "exit") == 0) {
            log_info("클라이언트 종료 요청 수신");
            break;
        }

        run_device_command(handle, buffer);
        write(clnt_sock, "OK\n", 3);
    }

    dlclose(handle);
    close(clnt_sock);
    log_info("클라이언트 스레드 종료 (TID: %lu)", pthread_self());
    return NULL;
}

int main() {
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_size = sizeof(clnt_addr);

    signal(SIGINT, sigint_handler);
    init_log("/tmp/server_log.txt");
    // daemonize();

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        log_error("socket() 실패: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        log_error("bind() 실패: %s", strerror(errno));
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        log_error("listen() 실패: %s", strerror(errno));
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    log_info("서버 시작됨 (포트 %d)", PORT);

    while (1) {
        int* clnt_sock = malloc(sizeof(int));
        *clnt_sock = accept(server_fd, (struct sockaddr*)&clnt_addr, &clnt_size);
        if (*clnt_sock < 0) {
            free(clnt_sock);
            continue;
        }

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clnt_addr.sin_addr, ip, sizeof(ip));
        log_info("클라이언트 연결 수락: %s:%d", ip, ntohs(clnt_addr.sin_port));

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, clnt_sock) != 0) {
            log_error("pthread_create() 실패");
            close(*clnt_sock);
            free(clnt_sock);
        } else {
            pthread_detach(tid);
        }
    }

    // 종료 루프는 sigint_handler가 처리하므로 여긴 도달 안 함
    return 0;
}
