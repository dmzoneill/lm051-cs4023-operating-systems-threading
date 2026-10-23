// Copyright David O'Neill
// Shared setup for the clone()-based producer/consumer examples.

#pragma once

#include <malloc.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

// 64kB stack
#define FIBER_STACK (1024 * 64)

// item type
typedef struct {
  int data;
} item;

// Allocate a thread stack or exit(1) with the given message.
static char *alloc_stack(const char *message) {
  char *stack = malloc(FIBER_STACK);
  if (stack == 0) {
    perror(message);
    exit(1);
  }
  return stack;
}

// Create a child thread with clone() or exit(2) with the given message.
static pid_t start_thread(int (*fn)(void *), char *stack, const char *message) {
  pid_t pid =
      clone(fn, stack + FIBER_STACK,
            SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0);
  if (pid == -1) {
    perror(message);
    exit(2);
  }
  return pid;
}

// Run a producer and a consumer thread and wait for both.
// If consumer_first is non-zero the consumer is waited for first.
static void run_producer_consumer(int (*producer)(void *),
                                  int (*consumer)(void *), int consumer_first) {
  char *stack_producer;
  char *stack_consumer;
  pid_t pid_producer, pid_consumer;

  // allocate producer's stack
  stack_producer = alloc_stack("\nmalloc: could not allocate producer's stack");
  // allocate consumer's stack
  stack_consumer = alloc_stack("\nmalloc: could not allocate stack\n");

  printf("\ncreating the producer thread");
  pid_producer = start_thread(producer, stack_producer,
                              "\nclone failed (producer) \nexiting...\n");

  printf("\ncreating the consumer thread");
  pid_consumer =
      start_thread(consumer, stack_consumer, "\nclone failed\nexiting...\n");

  if (consumer_first) {
    waitpid(pid_consumer, 0, 0);
    waitpid(pid_producer, 0, 0);
  } else {
    waitpid(pid_producer, 0, 0);
    waitpid(pid_consumer, 0, 0);
  }

  // free the stacks
  free(stack_producer);
  free(stack_consumer);

  // print a message
  printf("\nproducer and consumer completed successfully.\n");
}
