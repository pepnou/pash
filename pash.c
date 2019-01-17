#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>


#define SAVEC "\033[s"
#define RESTC "\033[u"
#define UPC "\033[A"
#define DOWNC "\033[B"
#define FORWC "\033[C"
#define BACKC "\033[D"
#define DELLI "\033[K"

int width, height;

void resize()
{
	struct winsize w;
	ioctl( 0, TIOCGWINSZ, &w);
	width = w.ws_col;
	height = w.ws_row;
}

size_t prompt()
{
	char pr[] = "\nCC$ ";
	write(STDOUT_FILENO, pr, strlen(pr));

	return strlen(pr) - 1; // -1 pour enlever la longueur de \n
}

void eraseLine(size_t* deb, size_t* cur, size_t* fin, size_t* prw)
{
	int nbL = (*cur + *prw) / width;

	for(int i = 0; i < (*prw + *cur) % width; i++)
		write(STDOUT_FILENO, BACKC, strlen(BACKC));
	for(int i = 0; i < nbL; i++)
	{
		write(STDOUT_FILENO, DELLI, strlen(DELLI));
		write(STDOUT_FILENO, UPC, strlen(UPC));
	}

	//write(STDOUT_FILENO, RESTC, strlen(RESTC));
	for(int i = 0; i < *prw; i++)
		write(STDOUT_FILENO, FORWC, strlen(FORWC));

	write(STDOUT_FILENO, DELLI, strlen(DELLI));
}

void moveC(size_t* source, size_t* dest, size_t* prw)
{
	int lS = (*source + *prw) / width;
	int lD = (*dest + *prw) / width;

	int ldiff = lD - lS;
	
	if(ldiff < 0)
	{
		for(int i = 0; i < -1*ldiff; i++)
			write(STDOUT_FILENO, UPC, strlen(UPC));
	}
	else if(ldiff > 0)
	{
		for(int i = 0; i < ldiff; i++)
			write(STDOUT_FILENO, DOWNC, strlen(DOWNC));
	}

	int cS = (*source + *prw) % width;
	int cD = (*dest + *prw) % width;

	int cdiff = cD - cS;

	if(cdiff < 0)
	{
		for(int i = 0; i < -1*cdiff; i++)
			write(STDOUT_FILENO, BACKC, strlen(BACKC));
	}
	else if(cdiff > 0)
	{
		for(int i = 0; i < cdiff; i++)
			write(STDOUT_FILENO, FORWC, strlen(FORWC));
	}
	//printf("%d %d\n", ldiff, cdiff);
	//fflush(stdout);
}

void handle( char c, char* buf, size_t* deb, size_t* cur, size_t* fin, size_t* size, size_t* prw, int* over)
{
	if(c >= 32 && c <= 126)
	{
		if(*cur == *fin)
		{
			write(STDOUT_FILENO, &c, 1);
			buf[*cur] = c;
		}
		else
		{
			strncpy( &(buf[*cur + 1]), &(buf[*cur]), *fin - *cur + 1);
			buf[*cur] = c;

			eraseLine(deb, cur, fin, prw);

			write(STDOUT_FILENO, buf, *fin + 1);
			moveC(fin, cur, prw);

			/*write(STDOUT_FILENO, RESTC, strlen(RESTC));
			for(int i = 0; i < *cur + 1; i++)
				write(STDOUT_FILENO, FORWC, strlen(FORWC));*/
		}

		(*cur)++;
		(*fin)++;
	}
	else
	{
		switch(c)
		{
			//terminer le shell : ctrl + D
			case 4:
			{
				*over = 1;
				write(STDOUT_FILENO, "\n", 1);
				break;
			}
			//new line : ctrl + J
			case 10:
			{
				*prw = prompt();

				(*cur) = (*fin) = (*deb) = 0;
				write(STDOUT_FILENO, SAVEC, strlen(SAVEC));
				break;
			}
			//effacer tout : ctrl + U
			case 21:
			{
				eraseLine(deb, cur, fin, prw);
				(*cur) = (*fin) = (*deb) = 0;
				break;
			}
			//effacer un caractère
			case 127:
			{
				if(*cur == 0)
					break;

				strncpy( &(buf[*cur - 1]), &(buf[*cur]), *fin - *cur + 1);

				eraseLine(deb, cur, fin, prw);

				write(STDOUT_FILENO, buf, *fin + 1);
				moveC(fin, cur, prw);

				/*write(STDOUT_FILENO, RESTC, strlen(RESTC));
				for(int i = 0; i < (*cur + *prw) % width - 1; i++)
					write(STDOUT_FILENO, FORWC, strlen(FORWC));*/

				(*cur)--;
				(*fin)--;
				break;
			}
			//partie avec les flèches
			case 27:
			{
				read(STDIN_FILENO, &c, 1);

				if(c == '[')
				{
					read(STDIN_FILENO, &c, 1);

					switch(c)
					{
						case 'A':
						{
							if(*cur / width)
							{
								write(STDOUT_FILENO, UPC, strlen(UPC));
								*cur = *cur - width;
							}
							break;
						}
						case 'B':
						{
							if(*fin - *cur >= width)
							{
								write(STDOUT_FILENO, DOWNC, strlen(DOWNC));
								*cur = *cur + width;
							}
							break;
						}
						case 'C':
						{
							if(*cur < *fin)
							{
								write(STDOUT_FILENO, FORWC, strlen(FORWC));
								(*cur)++;
							}
							break;
						}
						case 'D':
						{
							if(*cur > 0)
							{
								write(STDOUT_FILENO, BACKC, strlen(BACKC));
								(*cur)--;
							}
							break;
						}
						default:
						{
							handle(c, buf, deb, cur, fin, size, prw, over);
							break;
						}
					}
				}
				else
				{
					handle(c, buf, deb, cur, fin, size, prw, over);
				}

				break;
			}
			default:
			{
				printf("%d\n", c);
			}
		}
	}
}

int main( int argc, char** argv, char** envp)
{

	signal( SIGWINCH, &resize);

	static struct termios old, new1;
	int echo = 0;

	tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */
    new1.c_lflag &= ~ICANON; /* disable buffered i/o */
    new1.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
    


	char c, *buf;
	size_t deb, cur, fin, size = 1024, prw;
	cur = fin = deb = 0;
	if(!(buf = malloc(size)))
	{
		perror("allocation du buffer : ");
		exit(1);
	}
	
	int over = 0;

	resize();
	prw = prompt();
	write(STDOUT_FILENO, SAVEC, strlen(SAVEC));

	do
	{
		read(STDIN_FILENO, &c, 1);

		handle(c, buf, &deb, &cur, &fin, &size, &prw, &over);

	} while(!over);



	tcsetattr(0, TCSANOW, &old);

	exit(0);
}
