#include <stddef.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "work.h"
#include "game.h"

struct WorkItem *queue_head = NULL;
pthread_mutex_t queue_lock;
pthread_cond_t work_available = PTHREAD_COND_INITIALIZER;

void init_workers() {
  pthread_t tid[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++) {
    pthread_create(&tid[i], NULL, work, NULL);
  }
}

void *work() {
  while (1) {
    pthread_mutex_lock(&queue_lock);
    while (queue_head == NULL) {
      pthread_cond_wait(&work_available, &queue_lock);
    }
    struct WorkItem *work_item = queue_head;
    queue_head = queue_head->next;
    // printf("queue_head = %d", *((int *)queue_head->work_args));
    pthread_mutex_unlock(&queue_lock);

    work_item->work(work_item->work_args);

    pthread_mutex_lock(work_item->batch_counter_lock);

    int is_last_batch_item = --(*(work_item->batch_counter)) == 0;
    pthread_mutex_unlock(work_item->batch_counter_lock);

    if (is_last_batch_item) {
      pthread_cond_signal(work_item->batch_complete_cond);
    }
  }
}

void push_work(struct WorkItem *work_item) {
  pthread_mutex_lock(&queue_lock);
  work_item->next = queue_head;
  queue_head = work_item;
  pthread_mutex_unlock(&queue_lock);
  pthread_cond_broadcast(&work_available);
}

void print_number(void *work_args) {
  int *num = (int *)work_args;
  printf("num: %d, tid: %ld \n", *num, pthread_self());
}

void do_batch(int num_work_items, void (*work)(void *work_args), void *work_args_arr_p) {
    void** work_args_arr = work_args_arr_p;

    struct WorkItem work_items[num_work_items];

    int *batch_counter = malloc(sizeof(int));
    *batch_counter = num_work_items;

    pthread_mutex_t *batch_counter_lock = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(batch_counter_lock, NULL);

    pthread_cond_t *batch_complete_cond = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(batch_complete_cond, NULL);

    for (int i = 0; i < num_work_items; i++) {
      work_items[i].work = work;
      work_items[i].work_args = work_args_arr[i];
      work_items[i].batch_counter = batch_counter;
      work_items[i].batch_counter_lock = batch_counter_lock;
      work_items[i].batch_complete_cond = batch_complete_cond;
      push_work(&work_items[i]);
    }

    pthread_mutex_lock(batch_counter_lock);
    if (*batch_counter != 0) {
      pthread_cond_wait(batch_complete_cond, batch_counter_lock);
    }
    pthread_mutex_unlock(batch_counter_lock);

    pthread_mutex_destroy(batch_counter_lock);
    pthread_cond_destroy(batch_complete_cond);
    free(batch_counter_lock);
    free(batch_complete_cond);
}

const struct WorkLibSpec WorkLib = {
    .init = init_workers,
    .do_batch = do_batch
};
