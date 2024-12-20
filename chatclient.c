/* chat_client.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUF_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char buf[BUF_SIZE];
    int nbytes;

    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 서버의 IP 주소 입력

    // 서버 연결
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected to server. Start chatting!\n");

    while (1) {
        printf("You: ");
        fgets(buf, BUF_SIZE, stdin); // 사용자 입력 받기
        send(sock, buf, strlen(buf), 0); // 서버로 메시지 전송

        memset(buf, 0, BUF_SIZE); // 버퍼 초기화
        nbytes = recv(sock, buf, BUF_SIZE, 0); // 서버로부터 메시지 수신
        if (nbytes <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        printf("Server: %s", buf); // 서버 메시지 출력
    }

    close(sock);
    return 0;
}
