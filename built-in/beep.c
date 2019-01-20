#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

void nsleep(long us)
{
    struct timespec wait;
    
    wait.tv_sec = us / (1000 * 1000);
    wait.tv_nsec = (us % (1000 * 1000)) * 1000;
    nanosleep(&wait, NULL);
}

int main()
{
    struct timeval t1, t2;
    long long t;
    int size = 8;
    int song[] = {1, 1, 0, 1, 1, 0, 1, 1};
	
    long microseconds = 500000;
	
	for (int i = 0; i < size; ++i)
	{
		if(song[i] == 0)
		    nsleep(microseconds);
		else
		{
			printf("\a\n");
			nsleep(microseconds);
		}
	}

}