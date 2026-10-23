// Copyright David O'Neill
// Producer/consumer using do/while loops and a racy shared counter.
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

// producer thread
int producer(void *argument) {
  int i = 0;
  int items_to_produce = 10;
  do {
    // produce an item and put in nextProduced
    item nextProduced;
    nextProduced.data = i;

    // store in the buffer
    if (count < BUFFER_SIZE) {
      buffer[in] = nextProduced;
      in = (in + 1) % BUFFER_SIZE;

      int j;
      for (j = 0; j < 1000000; j++) {
        count--;
        count++;
      }

      // print a message
      printf("\n%d has been produced", nextProduced.data);

      count++;
      i++;
    }
  } while (i < items_to_produce);
  return 0;
}

// consumer thread
int consumer(void *argument) {
  int i = 0;
  int items_to_consume = 10;
  do {
    // consume the item in nextConsumed
    item nextConsumed;

    if (count > 0) {
      nextConsumed = buffer[out];
      out = (out + 1) % BUFFER_SIZE;

      int j;
      for (j = 0; j < 1000000; j++) {
        count--;
        count++;
      }

      // print a message
      printf("\n%d has been consumed", nextConsumed.data);

      count--;
      i++;
    }
  } while (i < items_to_consume);
  return 0;
}

int main() {
  // initialise shared global variables
  in = 0;
  out = 0;
  count = 0;

  run_producer_consumer(&producer, &consumer, 0);
  return 0;
}
