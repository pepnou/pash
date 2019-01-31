#include <stdio.h>
#include <wordexp.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>


//couleur normal
#define NOIR "\e[30m"
#define ROUGE "\e[31m"
#define VERT "\e[32m"
#define JAUNE "\e[33m"
#define BLEUE "\e[34m"
#define MAGENTA "\e[35m"
#define CYAN "\e[36m"
#define BLANC "\e[37m"

//couleur bright
#define B_NOIR "\e[90m"
#define B_ROUGE "\e[91m"
#define B_VERT "\e[92m"
#define B_JAUNE "\e[93m"
#define B_BLEUE "\e[94m"
#define B_MAGENTA "\e[95m"
#define B_CYAN "\e[96m"
#define B_BLANC "\e[97m"

#define RESET "\e[0m"
#define GROS "\e[1m"
#define PETIT "\e[2m"
#define ITALIC "\e[3m"
#define BLINK "\e[5m"
#define UNBLINK "\e[25m"

char *smiley = "<(^_^)>";
//http://ezprompt.net/

int main(int argc, char *argv[])
{
		/*wordexp_t ptr;
	wordexp("PATH", &ptr, WRDE_UNDEF);
	printf("%s\n", ptr);*/
	size_t size = 0;
	char *pwd = getenv("PWD"), *user = getenv("USER"), *home = getenv("HOME");
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	/*char *base_color, *time_color, *user_color, *pwd_color, *smiley_color, *smiley_face;
	size_t size = 0;
	ssize_t lu;
	FILE *file = fopen("./built-in/build/prompt_param.txt", "r");
	for (int i = 0; i < 6; ++i)
	{
		switch (i)
		{
			case 0 :
				lu = getline(&base_color, &size, file);
				base_color[-1] = '\0';
				break;

			case 1 :
				lu = getline(&time_color, &size, file);
				break;

			case 2 :
				lu = getline(&user_color, &size, file);
				break;

			case 3 :
				lu = getline(&pwd_color, &size, file);
				break;

			case 4 :
				lu = getline(&smiley_color, &size, file);
				break;

			case 5 :
				lu = getline(&smiley_face, &size, file);
				break;
		}
	}
	printf("%s bonsoir", base_color);*/

	time_t temps = time(NULL);
	struct tm now = *localtime(&temps);
	char shortpwd[strlen(pwd) - strlen(home)];

	//debut prompt
	printf("\n" GROS);
	//time
	printf(BLANC"["ROUGE);
	if(now.tm_hour<10)
		printf("0");
	printf("%d"BLANC BLINK":"UNBLINK ROUGE, now.tm_hour);
	if(now.tm_min<10)
		printf("0");
	printf("%d"BLANC "]", now.tm_min);
	size +=8;

	//username : - user =>
	printf(PETIT"-"RESET GROS B_VERT"%s" BLANC ":", user);
	size += 2+strlen(user);

	//chemin absolut avec raccourci ~
	printf(GROS B_CYAN);
	if(strlen(pwd)>strlen(home))
	{
		strncpy(shortpwd, &pwd[strlen(home)], (strlen(pwd) - strlen(home)));
		printf("~%s\n", shortpwd);	
		size += 2+strlen(pwd);
	}
	else
	{
		if (strlen(pwd) == strlen(home))
		{
			printf("~\n");
			size += 2;
		}
		else
		{
			printf("%s\n", pwd);
			size += 1 + strlen(pwd);
		}
	}
	if(size <= w.ws_col)
		size += w.ws_col - size;
	else
		size += (w.ws_col - (size%w.ws_col));

	printf(RESET);

	//smiley
	printf( GROS B_JAUNE" %s " BLANC BLINK"| "RESET, smiley);
	size += strlen(smiley) + 4;
	size -=2;

	return 0;
}