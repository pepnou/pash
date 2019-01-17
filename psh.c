#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define SAVEC "\033[s"
#define RESTC "\033[u"
#define BACKC "\033[D"
#define FORWC "\033[C"
#define DELLI "\033[K"


void prompt()
{
	char pr[] = "\nCC : ";
	write(STDOUT_FILENO, pr, strlen(pr));
}

int main(int argc, char** argv, char** envp)
{
	static struct termios old, new1;
	int echo = 0;

	tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */
    new1.c_lflag &= ~ICANON; /* disable buffered i/o */
    new1.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
    


	char c, *buf;
	size_t deb, cur, size = 128;
	cur = deb = 0;
	if(!(buf = malloc(size)))
	{
		perror("allocation du buffer : ");
		exit(1);
	}
	
	int over = 0;


	prompt();
	do
	{
		read(STDIN_FILENO, &c, 1);

		if(c >= 32 && c <= 126)
		{
			write(STDOUT_FILENO, &c, 1);

			buf[cur] = c;
			cur++;
		}
		else
		{
			switch(c)
			{
				case 4:
				{
					over = 1;
					break;
				}
				case 10:
				{
					//write(STDOUT_FILENO, &c, 1);
					prompt();

					cur = deb = 0;
					write(STDOUT_FILENO, SAVEC, strlen(SAVEC));
					break;
				}
				case 127:
				{
					write(STDOUT_FILENO, RESTC, strlen(RESTC));
					break;
				}
				default:
				{
					printf("%d\n", c);
				}
			}
		}

	} while(!over);



	tcsetattr(0, TCSANOW, &old);

	exit(0);
}
