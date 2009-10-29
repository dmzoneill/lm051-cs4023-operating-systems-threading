#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>

// 64kB stack
#define FIBER_STACK 1024*64

//buffer size
#define BUFFER_SIZE 5

//item type
typedef struct { int data; } item;

//shared global variables
item buffer[BUFFER_SIZE];
int in;
int out;
int count;

// producer thread
int producer( void* argument )
{
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
      for(j=0; j<1000000; j++) {
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

//consumer thread
int consumer( void* argument )
{
  int i = 0;
  int items_to_consume = 10;
  do {
    // consume the item in nextConsumed
    item nextConsumed;

    if (count > 0) {
      nextConsumed =  buffer[out];
      out = (out + 1) % BUFFER_SIZE;

      int j;
      for(j=0; j<1000000; j++) {
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

int main()
{
   void *stack_producer;
   void *stack_consumer;
   pid_t pid_producer, pid_consumer;

   //initialise shared global variables
   in = 0;
   out = 0;
   count = 0;   
     
   //allocate producer's stack
   stack_producer = malloc( FIBER_STACK );
   if ( stack_producer == 0 ) {
     perror( "\nmalloc: could not allocate producer's stack" );
     exit( 1 );
   }

   //allocate consumer's stack
   stack_consumer = malloc( FIBER_STACK );
   if ( stack_consumer == 0 ) {
     perror( "\nmalloc: could not allocate stack\n" );
     exit( 1 );
   }
  
   printf( "\ncreating the producer thread" );
        
   //call the clone system call to create a child thread
   pid_producer = clone( &producer, (char*) stack_producer + FIBER_STACK,
                 SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0 );

   if ( pid_producer == -1 ) {
      perror( "\nclone failed (producer) \nexiting...\n" );
      exit( 2 );
   }

   printf( "\ncreating the consumer thread" );
        
   //call the clone system call to create a child thread
   pid_consumer = clone( &consumer, (char*) stack_consumer + FIBER_STACK,
                 SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0 );

   if ( pid_consumer == -1) {
      perror( "\nclone failed\nexiting...\n" );
      exit( 2 );
   }

   //wait for the producer to exit
   waitpid( pid_producer, 0, 0 );

   //wait for the consumer to exit
   waitpid( pid_consumer, 0, 0 );
        
   //free the stacks
   free( stack_producer );
   free( stack_consumer );

   //print a message
   printf( "\nproducer and consumer completed successfully.\n" );     
   return 0;
}
