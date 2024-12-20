#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define NUM_CLIENTS 3

// 뮤텍스와 조건 변수
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// 클라이언트 쓰레드에서 사용할 메시지
char message[256];

// 클라이언트 쓰레드 함수
void* client_thread(void* arg) {
    int id = *((int*)arg);
    
    // 메시지를 기다리기
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    
    // 메시지를 수신
    printf("Client %d received message: %s\n", id, message);
    
    pthread_mutex_unlock(&mutex);
    return NULL;
}

// 서버 쓰레드 함수
void* server_thread(void* arg) {
    // 클라이언트들로부터 메시지 전송 요청을 받으면,
    // 모든 클라이언트에게 메시지를 방송.
    pthread_mutex_lock(&mutex);
    
    printf("Enter a message to broadcast: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = 0;  // 개행 문자 제거
    
    // 조건 변수를 사용해 모든 클라이언트에게 메시지 방송
    pthread_cond_broadcast(&cond);
    
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t clients[NUM_CLIENTS];
    pthread_t server;
    int client_ids[NUM_CLIENTS];
    
    // 클라이언트 쓰레드 생성
    for (int i = 0; i < NUM_CLIENTS; i++) {
        client_ids[i] = i + 1;
        pthread_create(&clients[i], NULL, client_thread, &client_ids[i]);
    }
    
    // 서버 쓰레드 생성
    pthread_create(&server, NULL, server_thread, NULL);
    
    // 서버 쓰레드가 끝날 때까지 기다림
    pthread_join(server, NULL);
    
    // 클라이언트 쓰레드들이 메시지를 받은 후 종료할 때까지 기다림
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }
    
    // 자원 해제
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    
    return 0;
}
