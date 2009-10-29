#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024  

int main(int argc, char **argv)
{
    // file input descriptor 
    int fdRead;  
    // file output descriptor 
    int fdWrite;  
    // Read/Write buffer.
    char buffer[BUFFER_SIZE]; 
    // pointer to the write buffer
    char *writeBufferPointer; 
    // number of bytes remaining to be written
    int bufferChars; 
    // bytes written on last right     
    int writtenChars; 
    
    // allocate memory for the file input name
    char* fileInput = (char *) malloc(256); 
    // allocate memory for the file output name
    char* fileOutput = (char *) malloc(256); 

    if(argc!=3)
    {
        // arguments weren't specified correctly 
        printf ("%s","Please specify the input file : ");
        scanf ("%s",fileInput); 
        printf ("%s", "Please specify the output file name : ");
        scanf ("%s",fileOutput); 
    }
    else
    {
        // use arguments
        fileInput = argv[1]; 
        fileOutput = argv[2]; 
    }

    // open file to be copied
    if ((fdRead = open(fileInput, O_RDONLY, 0)) < 0) 
    {
        perror("Open source file failed");
        exit(1);
    }

    // open file to be created
    if ((fdWrite = open(fileOutput, O_WRONLY | O_CREAT | O_TRUNC | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
    {
        perror("Open destination file failed");
        exit(1);
    }

    int doneWriting = 0;    
    
    while (!doneWriting)
    {        
        // read some bytes (could be prempted, context switch)
        if ((bufferChars = read(fdRead, buffer, BUFFER_SIZE)) > 0)
        {
            // next byte to write
            writeBufferPointer = buffer;  

            // cant gurantee the it will be written all at once
            // we have to aloow it to be written in chunks
            while (bufferChars > 0)
            {
                if ((writtenChars = write(fdWrite, writeBufferPointer, bufferChars)) < 0)
                {
                    perror("Write failed");
                    exit(1);
                }

                // number of chars left in the buffer
                bufferChars -= writtenChars;  
                // update the pointer to the next position 
                writeBufferPointer += writtenChars; 
            }
        }
        // EOF reached
        else if (bufferChars == 0) 
        {
            doneWriting = 1;
        }
        // read failure
        else  
        {
            perror("Read failed");
            exit(1);
        }
    }

    // close off the input file
    if(close(fdRead) < 0)
    {
        perror("Error Closing Input file");
        exit(1);
    }
    // close off the output file
    if(close(fdWrite) < 0)
    {
        perror("Error Closing Output file");
        exit(1);
    }

    // finally everything went smooth
    printf("%s \n","Copy completed sucessfully");
    
    // return succcess
    return 0;
}

