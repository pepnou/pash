#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>


int main(int argc, char** argv, char** envp)
{
	static struct termios old, new1;
	int echo = 0;

	tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */
    new1.c_lflag &= ~ICANON; /* disable buffered i/o */
    new1.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
    


	char c;

	while(read(STDIN_FILENO, &c, 1))
	{
		if(c == 4)
			break;

		printf("%c", c);
		
		fflush(stdout);
	}



	tcsetattr(0, TCSANOW, &old);

	exit(0);
}