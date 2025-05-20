// ✅ client/main_client.c (SIGINT 핸들러 포함)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUF_SIZE 256

volatile sig_atomic_t keep_running = 1;
void int_handler(int signo) {
    keep_running = 0;
    printf("\n[클라이언트 종료: Ctrl+C]\n");
}

int main(int argc, char* argv[]) {
    signal(SIGINT, int_handler);

    if (argc != 3) {
        fprintf(stderr, "사용법: %s <서버IP> <포트번호>\n", argv[0]);
        exit(1);
    }

    const char* server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket() 실패");
        exit(1);
    }

    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("connect() 실패");
        close(sock);
        exit(1);
    }

    printf("서버에 연결됨 (%s:%d)\n", server_ip, server_port);

    char message[BUF_SIZE];
    char response[BUF_SIZE];

    while (keep_running) {
        printf("명령 입력 (예: LED ON, BUZZER OFF): ");
        fgets(message, BUF_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "exit") == 0) {
        printf("클라이언트 종료 요청\n");
        break;
        }

        if (write(sock, message, strlen(message)) <= 0) {
        perror("서버 전송 실패");
        break;
        }

        // 서버로부터 응답 수신
        int len = read(sock, response, BUF_SIZE - 1);
        if (len <= 0) {
            perror("read() 실패");  // 추가
            printf("서버 응답 없음 또는 연결 종료\n");
            break;
        }
        response[len] = '\0';
        printf("서버 응답: %s\n", response);
    }

    close(sock);
    return 0;
}