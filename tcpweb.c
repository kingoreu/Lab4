#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080
#define MAXBUF 1024

void handle_client(int client_sock);
void handle_get(int client_sock, const char *request);
void handle_post(int client_sock, const char *request);

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // 서버 소켓 생성
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // 소켓 바인딩
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // 소켓 대기열 생성
    if (listen(server_sock, 10) == -1) {
        perror("listen");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // 클라이언트 연결 처리 루프
    while (1) {
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1) {
            perror("accept");
            continue;
        }

        printf("New connection: %s\n", inet_ntoa(client_addr.sin_addr));
        handle_client(client_sock);
        close(client_sock);
    }

    close(server_sock);
    return 0;
}

void handle_client(int client_sock) {
    char buffer[MAXBUF];
    memset(buffer, 0, MAXBUF);

    // 클라이언트 요청 읽기
    if (recv(client_sock, buffer, MAXBUF, 0) <= 0) {
        perror("recv");
        return;
    }

    printf("Request received:\n%s\n", buffer);

    // 요청 유형 확인
    if (strncmp(buffer, "GET", 3) == 0) {
        handle_get(client_sock, buffer);
    } else if (strncmp(buffer, "POST", 4) == 0) {
        handle_post(client_sock, buffer);
    } else {
        const char *response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        send(client_sock, response, strlen(response), 0);
    }
}

void handle_get(int client_sock) {
    const char *html_content = 
        "<!DOCTYPE html>"
        "<html>"
        "<head><title>Simple Web Server</title></head>"
        "<body><h1>Welcome to Lab4 Test Web Server</h1></body>"
        "</html>";

    char response[MAXBUF];
    snprintf(response, MAXBUF,
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "Content-Length: %lu\r\n"
             "Connection: close\r\n"
             "\r\n%s",
             strlen(html_content), html_content);

    send(client_sock, response, strlen(response), 0);
}

void handle_post(int client_sock, const char *request) {
    // POST 요청의 데이터 추출
    char *body = strstr(request, "\r\n\r\n");
    if (body) {
        body += 4;
        printf("POST Data:\n%s\n", body);

        int pid = fork();
        if (pid == 0) {
            dup2(client_sock, STDOUT_FILENO);
            execl("./cgiprogram", "./cgiprogram", NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            wait(NULL);
        }
    } else {
        const char *response = "HTTP/1.1 400 Bad Request\r\n\r\n";
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n"  // 명확한 연결 종료 지시
            "\r\n";
        send(client_sock, response, strlen(response), 0);
    }
}
