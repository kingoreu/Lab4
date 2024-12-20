#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5  // 버퍼 크기
#define NUM_PRODUCERS 2 // 생산자 수
#define NUM_CONSUMERS 2 // 소비자 수

// 버퍼
int buffer[BUFFER_SIZE];
int in = 0;  // 생산자가 아이템을 넣을 인덱스
int out = 0; // 소비자가 아이템을 꺼낼 인덱스

// 뮤텍스와 조건 변수
pthread_mutex_t mutex;
pthread_cond_t not_full, not_empty;

// 생산자 쓰레드 함수
void *producer(void *arg) {
    int item;
    while (1) {
        item = rand() % 100;  // 랜덤 아이템 생성
        pthread_mutex_lock(&mutex);
        
        // 버퍼가 가득 차면 대기
        while ((in + 1) % BUFFER_SIZE == out) {
            pthread_cond_wait(&not_full, &mutex);
        }
        
        // 아이템을 버퍼에 넣음
        buffer[in] = item;
        printf("Producer %ld produced: %d\n", (long)arg, item);
        in = (in + 1) % BUFFER_SIZE;
        
        // 소비자가 아이템을 소비할 수 있도록 신호
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
        
        sleep(1);  // 생산 간 시간 지연
    }
}

// 소비자 쓰레드 함수
void *consumer(void *arg) {
    int item;
    while (1) {
        pthread_mutex_lock(&mutex);
        
        // 버퍼가 비면 대기
        while (in == out) {
            pthread_cond_wait(&not_empty, &mutex);
        }
        
        // 아이템을 버퍼에서 꺼냄
        item = buffer[out];
        printf("Consumer %ld consumed: %d\n", (long)arg, item);
        out = (out + 1) % BUFFER_SIZE;
        
        // 생산자가 아이템을 넣을 수 있도록 신호
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
        
        sleep(1);  // 소비 간 시간 지연
    }
}

int main() {
    pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS];
    long i;

    // 뮤텍스와 조건 변수 초기화
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    // 생산자 쓰레드 생성
    for (i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producers[i], NULL, producer, (void *)i);
    }

    // 소비자 쓰레드 생성
    for (i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumers[i], NULL, consumer, (void *)i);
    }

    // 쓰레드 종료 대기
    for (i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    for (i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    // 뮤텍스와 조건 변수 자원 해제
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}
