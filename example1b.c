#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sched.h>
#include <stdio.h>

// 64kB stack
#define FIBER_STACK 1024*64

int x;

// The child thread will execute this function
int threadFunction( void* argument )
{
         x++;
         printf( "x = %d\n", x );
         printf( "child thread exiting\n" );
         return 0;
}

int main()
{
         x = 0;
         
         void* stack;
         void* stack2;
         pid_t pid;
         pid_t pid2;
        
         // Allocate the stack
         stack = malloc( FIBER_STACK );
         stack2 = malloc( FIBER_STACK );
         if ( stack == 0 || stack2 == 0 )
         {
                 perror( "malloc: could not allocate stack" );
                 exit( 1 );
         }
        
         printf( "Creating child threads\n" );
        
         // Call the clone system call to create the child thread
         pid = clone( &threadFunction, (char*) stack + FIBER_STACK, SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0 );
         pid2 = clone( &threadFunction, (char*) stack2 + FIBER_STACK, SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0 );
         if ( pid == -1 || pid2 == -1)
         {
                 perror( "clone" );
                 exit( 2 );
         }
        
         // Wait for the child thread to exit
         pid = waitpid( pid, 0, 0 );
         pid2 = waitpid( pid2, 0, 0 );
         if ( pid == -1 || pid2 == -1 )
         {
                 perror( "waitpid" );
                 exit( 3 );
         }
        
         // Free the stack
         free( stack );
         free( stack2 );

         printf( "Child threads returned and stacks freed.\n" );
        
         return 0;
}


