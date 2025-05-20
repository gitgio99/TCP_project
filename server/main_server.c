#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>

#include "mod_loader.h"
#include "logger.h"

#define PORT 5200
#define BUF_SIZE 256

volatile sig_atomic_t running = 1;

// SIGINT: 서버 종료 요청
void sigint_handler(int signo) {
    running = 0;
    log_info("서버 종료 요청: SIGINT");
}

// SIGCHLD: 좀비 프로세스 방지
void sigchld_handler(int signo) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// 데몬화 함수
void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0); // 부모 종료

    setsid();
    umask(0);
    chdir("/");

    // 표준 입출력 제거
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_RDWR);
}

int main() {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_size = sizeof(clnt_addr);

    // 로그 초기화
    init_log();
    log_info("서버 시작됨 (포트 %d)", PORT);

    // 시그널 처리
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);

    daemonize(); // 데몬 프로세스 전환

    // 소켓 생성
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        log_error("socket() 실패");
        exit(1);
    }

    // 소켓 옵션 설정
    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 주소 구조체 설정
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);

    // 바인딩
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        log_error("bind() 실패: %s", strerror(errno));
        exit(1);
    }

    // 리슨 시작
    if (listen(serv_sock, 5) < 0) {
        log_error("listen() 실패");
        exit(1);
    }

    log_info("클라이언트 접속 대기 중...");

    // 메인 서버 루프
    while (running) {
        clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_size);
        if (clnt_sock < 0) continue;

        pid_t pid = fork();
        if (pid == 0) {
            // 자식 프로세스
            close(serv_sock);
            log_info("자식 %d: 클라이언트 접속", getpid());

            char buffer[BUF_SIZE];
            const char* response = NULL;

            while (1) {
                int len = read(clnt_sock, buffer, BUF_SIZE - 1);

                if (len > 0) {
                    buffer[len] = '\0';
                    log_info("자식 %d: 수신 명령: %s", getpid(), buffer);

                    void* handle = load_module("/home/iam/project/modules/libled_mod.so");
                    if (handle) {
                        run_device_command(handle, buffer);
                        dlclose(handle);
                        response = "OK\n";
                        log_info("명령 처리 완료: %s", buffer);
                    } else {
                        response = "FAIL\n";
                        log_error("모듈 로딩 실패");
                    }

                    // ✅ 응답 무조건 전송
                    if (response) {
                        log_info("응답 준비됨: %s", response ? response : "NULL");
                        ssize_t sent = write(clnt_sock, response, strlen(response));
                        if (sent <= 0) {
                            log_error("응답 전송 실패: %s", strerror(errno));
                            break;
                        } else {
                            log_info("응답 전송 완료: %s", response);
                        }
                    }
                    if (response != NULL) {
                        log_info("응답 문자열 확인: %s", response);  // ✅ 로그 추가
                        ssize_t sent = write(clnt_sock, response, strlen(response));
                    }

                }
                else if (len == 0) {
                    log_info("자식 %d: 클라이언트가 연결을 정상 종료함 (EOF)", getpid());
                    break;
                }

                else {
                    log_error("자식 %d: read() 오류 발생: %s", getpid(), strerror(errno));
                    break;
                }
            }

            close(clnt_sock);
            log_info("자식 %d 종료", getpid());
            exit(0);
        }

        // 부모는 클라이언트 소켓만 닫고 계속 루프
        close(clnt_sock);
    }

    // 루프 종료 시 처리
    close(serv_sock);
    log_info("서버 정상 종료");
    close_log();
    return 0;
}