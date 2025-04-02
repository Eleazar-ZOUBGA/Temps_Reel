#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>

#define LOAD 1000000

void task1() {

  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  long start_time_sec = tp.tv_sec;
  long start_time_nsec = tp.tv_nsec;
  
  for ( int i = 0; i < LOAD*1000; i ++) {}

  clock_gettime(CLOCK_REALTIME, &tp);
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
}

void sleep_until_next_activation(struct timespec next_activation) {
  int err;
  do {
    err = clock_nanosleep ( CLOCK_MONOTONIC ,TIMER_ABSTIME, &next_activation, NULL);
  } while ( err != 0 && errno == EINTR);
  assert ( err == 0) ;
}

void timespec_add(struct timespec* next_activation, struct timespec* task_period) {
  next_activation->tv_sec += task_period->tv_sec;
  next_activation->tv_nsec += task_period->tv_nsec;    
}

int main() {

  struct timespec next_activation, task_period;

  task_period.tv_sec = 5.0;
  clock_gettime(CLOCK_MONOTONIC, &next_activation);

  while(1) {

    sleep_until_next_activation(next_activation);

    task1();

    timespec_add(&next_activation, &task_period);
  }

  return 0;
  
}
