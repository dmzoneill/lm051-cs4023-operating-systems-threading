// Copyright David O'Neill
// Producer/consumer with an unsynchronised shared counter.
#define _GNU_SOURCE
#include <stdio.h>

#include "common/threading_common.h"

// buffer size
#define BUFFER_SIZE 5
// 20

// shared global variables
item buffer[BUFFER_SIZE];
int in;
int out;
int count;

// producer thread
int producer(void *argument) {
  int i;
  for (i = 0; i < 10; i++) {
    // produce an item and put in nextProduced
    item nextProduced;
    nextProduced.data = i;

    // store in the buffer
    while (count == BUFFER_SIZE) {
      // wait
    }
    buffer[in] = nextProduced;
    in = (in + 1) % BUFFER_SIZE;

    int j;
    for (j = 0; j < 1000000; j++) {
      count--;
      count++;
    }
    count++;

    // print a message
    printf("\n%d has been produced", nextProduced.data);
  }
  return 0;
}

// consumer thread
int consumer(void *argument) {
  int i;
  for (i = 0; i < 10; i++) {
    // consume the item in nextConsumed
    item nextConsumed;
    while (count == 0) {
      // wait
    }
    nextConsumed = buffer[out];
    out = (out + 1) % BUFFER_SIZE;

    int j;
    for (j = 0; j < 1000000; j++) {
      count--;
      count++;
    }
    count--;

    // print a message
    printf("\n%d has been consumed", nextConsumed.data);
  }
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
