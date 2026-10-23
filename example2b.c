// Copyright David O'Neill
// Producer/consumer guarded by Peterson's algorithm.
#define _GNU_SOURCE
#include <stdio.h>

#include "common/threading_common.h"

// buffer size
#define BUFFER_SIZE 5

// shared global variables
item buffer[BUFFER_SIZE];
int in;
int out;
int count;

int flag[2];
int turn;

// producer thread
int producer(void *argument) {
  int i;
  for (i = 0; i < 10; i++) {
    flag[0] = 1;
    turn = 1;

    while (flag[1] && turn == 1) {
    }

    /* enter critical section */
    if (count != BUFFER_SIZE) {
      // produce an item and put in nextProduced
      item nextProduced;
      nextProduced.data = i;

      // store in the buffer
      buffer[in] = nextProduced;
      in = (in + 1) % BUFFER_SIZE;

      count++;

      // print a message
      printf("\n%d has been produced", nextProduced.data);
    } else {
      i--;
    }

    /* exit critical section  */
    flag[0] = 0;
  }
  return 0;
}

// consumer thread
int consumer(void *argument) {
  int i;
  for (i = 0; i < 10; i++) {
    flag[1] = 1;
    turn = 0;

    while (flag[0] && turn == 0) {
    }

    /* enter critical section */
    if (count != 0) {
      // consume the item in nextConsumed
      item nextConsumed;

      nextConsumed = buffer[out];
      out = (out + 1) % BUFFER_SIZE;

      count--;

      // print a message
      printf("\n%d has been consumed", nextConsumed.data);
    } else {
      i--;
    }
    /* exit critical section */
    flag[1] = 0;
  }
  return 0;
}

int main() {
  // initialise shared global variables
  in = 0;
  out = 0;
  count = 0;

  // initialize flag and turn
  // flag 1 means a program wants to enter
  // turn holds the id of the thread whos turn it is
  turn = 0;
  flag[0] = 0;
  flag[1] = 0;

  // the consumer is waited for before the producer
  run_producer_consumer(&producer, &consumer, 1);
  return 0;
}
