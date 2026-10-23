// Copyright David O'Neill
// Producer/consumer using do/while loops guarded by Peterson's algorithm.
#define _GNU_SOURCE
#include <stdio.h>

#include "common/threading_common.h"

// buffer size
#define BUFFER_SIZE 2

// shared global variables
item buffer[BUFFER_SIZE];
int in;
int out;
int count;

int flag[2];
int turn;

// producer thread
int producer(void *argument) {
  int i = 0;
  int items_to_produce = 10;
  do {
    flag[0] = 1;
    turn = 1;

    while (flag[1] && turn == 1) {
    }

    /* enter critical section */

    // race condition (count)
    if (count < BUFFER_SIZE) {
      item nextProduced;
      nextProduced.data = i;
      // NIK i dont think we should actually create an
      // item till we need it ( if (count < BUFFER_SIZE) )
      // having it outside the if statement would create an item for each
      // iteration

      // race condition
      buffer[in] = nextProduced;
      in = (in + 1) % BUFFER_SIZE;

      int j;
      for (j = 0; j < 10; j++) {
        // race condition
        count--;
        // race condition
        count++;
      }

      printf("\n%d has been produced", nextProduced.data);

      // race condition
      count++;
      i++;
    }

    /* exit critical section  */

    flag[0] = 0;
  } while (i < items_to_produce);

  return 0;
}

// consumer thread
int consumer(void *argument) {
  int i = 0;
  int items_to_consume = 10;
  do {
    flag[1] = 1;
    turn = 0;

    while (flag[0] && turn == 0) {
    }

    /* enter critical section */

    // race condition (count)
    if (count > 0) {
      // consume the item in nextConsumed
      item nextConsumed;
      // NIK i dont think we should actually create an
      // item till we need it ( if (count < BUFFER_SIZE) )
      // having it outside the if statement would create an item for each
      // iteration

      // race condition
      nextConsumed = buffer[out];
      out = (out + 1) % BUFFER_SIZE;

      int j;
      for (j = 0; j < 1000000; j++) {
        // race condition (count)
        count--;
        // race condition (count)
        count++;
      }

      // print a message
      printf("\n%d has been consumed", nextConsumed.data);

      // race condition (count)
      count--;

      i++;
    }

    /* exit critical section */

    flag[1] = 0;
  } while (i < items_to_consume);

  return 0;
}

int main() {
  // initialize flag and turn
  // flag 1 means a program wants to enter
  // turn holds the id of the thread whos turn it is
  turn = 0;
  flag[0] = 0;
  flag[1] = 0;

  // initialise shared global variables
  in = 0;
  out = 0;
  count = 0;

  run_producer_consumer(&producer, &consumer, 0);
  return 0;
}
