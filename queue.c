#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <pthread.h>

#include "queue.h"

typedef struct queue {
    void **arr;
    int front;
    int back;
    int size;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} queue_t;

queue_t *queue_new(int size) {
    queue_t *que = (queue_t *) calloc(sizeof(queue_t), 1);
    que->arr = (void **) calloc(sizeof(void *), size);
    que->front = 0;
    que->back = 0;
    que->size = size;
    que->count = 0;
    pthread_mutex_init(&que->mutex, NULL);
    pthread_cond_init(&que->not_full, NULL);
    pthread_cond_init(&que->not_empty, NULL);
    return que;
}

void queue_delete(queue_t **q) {
    free((*q)->arr);
    (*q)->arr = NULL;
    pthread_mutex_destroy(&(*q)->mutex);
    pthread_cond_destroy(&(*q)->not_full);
    pthread_cond_destroy(&(*q)->not_empty);
    free(*q);
    *q = NULL;
}

bool queue_push(queue_t *q, void *elem) {
    if (!q) {
        return false;
    }
    pthread_mutex_lock(&q->mutex);
    while (q->count == q->size) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    q->arr[q->back] = elem;
    q->back += 1;
    if (q->back == q->size) {
        q->back = 0;
    }
    q->count += 1;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
    return true;
}

bool queue_pop(queue_t *q, void **elem) {
    if (!q) {
        return false;
    }
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    *elem = q->arr[q->front];
    q->front += 1;
    if (q->front == q->size) {
        q->front = 0;
    }
    q->count -= 1;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    return true;
}
