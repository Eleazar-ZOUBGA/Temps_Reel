#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>

#define LOAD 1000000

pthread_barrier_t barrier;

void timespec_add(struct timespec* next_activation, struct timespec* task_period) {
  next_activation->tv_sec += task_period->tv_sec;
  next_activation->tv_nsec += task_period->tv_nsec;    
}

void sleep_until_next_activation(struct timespec next_activation) {
  int err;
  do {
    err = clock_nanosleep ( CLOCK_MONOTONIC ,TIMER_ABSTIME, &next_activation, NULL);
  } while ( err != 0 && errno == EINTR);
  assert ( err == 0) ;
}

void* task1(void* arg) {

  struct timespec next_activation, task_period;
  
  task_period.tv_sec = 2.0;
  task_period.tv_nsec = 0.0;
  clock_gettime(CLOCK_MONOTONIC, &next_activation);

  pthread_barrier_wait(&barrier);
  
  while(1) {

    sleep_until_next_activation(next_activation);
    
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    long start_time_sec = tp.tv_sec;
    long start_time_nsec = tp.tv_nsec;
  
    for ( int i = 0; i < LOAD*1000; i ++) {}

    clock_gettime(CLOCK_MONOTONIC, &tp);
    long end_time_sec = tp.tv_sec;
    long end_time_nsec = tp.tv_nsec;

    long delta_sec = end_time_sec - start_time_sec;
    long delta_nsec = end_time_nsec - start_time_nsec;

    if (delta_nsec < 0) {
      delta_sec -= 1;
      delta_nsec += 1000000000;
    }

    long total_time = delta_sec * 1000000000 + delta_nsec;

    printf("task1() response time: %ld ns\n", total_time);

    timespec_add(&next_activation, &task_period);
  }
}

void* task2(void* arg) {

  struct timespec next_activation, task_period;
  
  task_period.tv_sec = 1.0;
  task_period.tv_nsec = 0.0;
  clock_gettime(CLOCK_MONOTONIC, &next_activation);

  pthread_barrier_wait(&barrier);
  
  while(1) {

    sleep_until_next_activation(next_activation);
    
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    long start_time_sec = tp.tv_sec;
    long start_time_nsec = tp.tv_nsec;
  
    for ( int i = 0; i < LOAD*1000; i ++) {}

    clock_gettime(CLOCK_MONOTONIC, &tp);
    long end_time_sec = tp.tv_sec;
    long end_time_nsec = tp.tv_nsec;

    long delta_sec = end_time_sec - start_time_sec;
    long delta_nsec = end_time_nsec - start_time_nsec;

    if (delta_nsec < 0) {
      delta_sec -= 1;
      delta_nsec += 1000000000;
    }

    long total_time = delta_sec * 1000000000 + delta_nsec;

    printf("task2() response time: %ld ns\n", total_time);

    timespec_add(&next_activation, &task_period);
  }
  pthread_barrier_wait(&barrier);
}

int main() {

  pthread_t tid1, tid2;

  pthread_barrier_init(&barrier, NULL, 2);

  pthread_create(&tid1, NULL, task1, NULL);
  pthread_create(&tid2, NULL, task2, NULL);

  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);

  pthread_barrier_destroy(&barrier);
  
  while(1);

  return 0;
  
}
