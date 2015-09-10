#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
 

static int end;
static void endme(int dummy) { end=1; }
 

int main (void)
{
  int fifo, counter, k;
    if ((fifo = open("/dev/rtf0", O_RDONLY)) < 0) {
        fprintf(stderr, "Error opening /dev/rtf0\n");
        return 1;
    }

    signal(SIGINT, endme);

    for ( k=0; k<20 && ! end; k++ ) {
        read(fifo, &counter, sizeof(counter));
        printf("  counter=%d\n", counter);
    }

    return 0;
}

 

 
