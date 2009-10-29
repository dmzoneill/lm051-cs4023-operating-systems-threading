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
#define BUFFER_SIZE 2

//item type
typedef struct { int data; } item;

//shared global variables
item buffer[BUFFER_SIZE];
int in;
int out;
int count;

int flag[2];
int turn;

// producer thread
int producer( void* argument )
{
    int i = 0;
    int items_to_produce = 10;
    do 
    {
	flag[0] = 1;
        turn = 1;

        while (flag[1] && turn==1);

        /* enter critical section */

        if (count < BUFFER_SIZE) // race condition (count)
        {
            item nextProduced;
            nextProduced.data = i;
	    // NIK i dont think we should actually create an 
            // item till we need it ( if (count < BUFFER_SIZE) )
            // having it outside the if statement would create an item for each iteration
   
            buffer[in] = nextProduced; // race condition
            in = (in + 1) % BUFFER_SIZE; 
      
            int j;
            for(j=0; j<10; j++) 
            {
                count--; // race condition
                count++; // race condition
            }

            printf("\n%d has been produced", nextProduced.data);
      
            count++; // race condition
            i++;
        }
        
        /* exit critical section  */	

        flag[0] = 0;
        
    } 
    while (i < items_to_produce);  

    return 0;
}

//consumer thread
int consumer( void* argument )
{
    int i = 0;
    int items_to_consume = 10;
    do 
    {    
        flag[1] = 1;
        turn = 0;
        
        while (flag[0] && turn==0);

        /* enter critical section */

        if (count > 0)  // race condition (count)
        {
            // consume the item in nextConsumed
            item nextConsumed; 
	    // NIK i dont think we should actually create an 
            // item till we need it ( if (count < BUFFER_SIZE) )
            // having it outside the if statement would create an item for each iteration

            nextConsumed =  buffer[out]; // race condition
            out = (out + 1) % BUFFER_SIZE; 

            int j;
            for(j=0; j<1000000; j++) 
            {
                count--; // race condition (count)
                count++; // race condition (count)
            }

            // print a message
            printf("\n%d has been consumed", nextConsumed.data);

            count--; // race condition (count)

            i++;	    	
        }
        
        /* exit critical section */	
            
        flag[1] = 0;
        
    } 
    while (i < items_to_consume);

    return 0;
}

int main()
{
    void *stack_producer;
    void *stack_consumer;
    pid_t pid_producer, pid_consumer;

    // initialize flag and turn
    // flag 1 means a program wants to enter
    // turn holds the id of the thread whos turn it is
    turn = 0;
    flag[0] = 0;
    flag[1] = 0;
   

    //initialise shared global variables
    in = 0;
    out = 0;
    count = 0;   
     
    //allocate producer's stack
    stack_producer = malloc( FIBER_STACK );
    if ( stack_producer == 0 ) 
    {
        perror( "\nmalloc: could not allocate producer's stack" );
        exit( 1 );
    }

    //allocate consumer's stack
    stack_consumer = malloc( FIBER_STACK );
    if ( stack_consumer == 0 ) 
    {
        perror( "\nmalloc: could not allocate stack\n" );
        exit( 1 );
    }
  
    printf( "\ncreating the producer thread" );
        
    //call the clone system call to create a child thread
    pid_producer = clone( &producer, (char*) stack_producer + FIBER_STACK, SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0 );

    if ( pid_producer == -1 ) 
    {
        perror( "\nclone failed (producer) \nexiting...\n" );
        exit( 2 );
    }

    printf( "\ncreating the consumer thread" );
        
    //call the clone system call to create a child thread
    pid_consumer = clone( &consumer, (char*) stack_consumer + FIBER_STACK, SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0 );

    if ( pid_consumer == -1) 
    {
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
