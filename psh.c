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



int prompt()
{
	char pr[] = "\nCC : ";
	write(STDOUT_FILENO, pr, strlen(pr));

	return strlen(pr) - 1; // -1 pour enlever la longueur de \n
}

void handle( char c, char* buf, char* tmp, size_t* deb, size_t* cur, size_t* fin, size_t* size, int* over)
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
			//strncpy( &(buf[*cur + 1]), &(buf[*cur]), *fin - *cur);

			strncpy( tmp, &(buf[*cur]), *fin - *cur);
			strncpy( &(buf[*cur + 1]), tmp, *fin - *cur);


			buf[*cur] = c;

			write(STDOUT_FILENO, RESTC, strlen(RESTC));
			write(STDOUT_FILENO, DELLI, strlen(DELLI));

			write(STDOUT_FILENO, buf, *fin + 1);
		}

		(*cur)++;
		(*fin)++;
	}
	else
	{
		switch(c)
		{
			case 4:
			{
				*over = 1;
				write(STDOUT_FILENO, "\n", 1);
				break;
			}
			case 10:
			{
				//write(STDOUT_FILENO, &c, 1);
				prompt();

				(*cur) = (*fin) = (*deb) = 0;
				write(STDOUT_FILENO, SAVEC, strlen(SAVEC));
				break;
			}
			case 21:
			{
				write(STDOUT_FILENO, RESTC, strlen(RESTC));
				write(STDOUT_FILENO, DELLI, strlen(DELLI));
				(*cur) = (*fin) = (*deb) = 0;
				break;
			}
			case 127:
			{
				
				break;
			}
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

							break;
						}
						case 'B':
						{

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
							handle(c, buf, tmp, deb, cur, fin, size, over);
							break;
						}
					}
				}
				else
				{
					handle(c, buf, tmp, deb, cur, fin, size, over);
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
	static struct termios old, new1;
	int echo = 0;

	tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */
    new1.c_lflag &= ~ICANON; /* disable buffered i/o */
    new1.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
    


	char c, *buf, *tmp;
	size_t deb, cur, fin, size = 128;
	cur = fin = deb = 0;
	if(!(buf = malloc(size)))
	{
		perror("allocation du buffer : ");
		exit(1);
	}
	if(!(tmp = malloc(size)))
	{
		perror("allocation du buffer : ");
		exit(1);
	}
	
	int over = 0;


	prompt();
	write(STDOUT_FILENO, SAVEC, strlen(SAVEC));

	do
	{
		read(STDIN_FILENO, &c, 1);

		handle(c, buf, tmp, &deb, &cur, &fin, &size, &over);

	} while(!over);



	tcsetattr(0, TCSANOW, &old);

	exit(0);
}
