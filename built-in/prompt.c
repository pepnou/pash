#include <stdio.h>
#include <wordexp.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


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




//http://ezprompt.net/

int main(int argc, char const *argv[])
{
	/*wordexp_t ptr;
	wordexp("PATH", &ptr, WRDE_UNDEF);
	printf("%s\n", ptr);*/

	time_t temps = time(NULL);
	struct tm now = *localtime(&temps);
	char *pwd = getenv("PWD"), *user = getenv("USER"), *home = getenv("HOME");
	char shortpwd[strlen(pwd) - strlen(home)];

	//debut prompt
	printf("\n" GROS);
	//time
	printf(BLANC" ["ROUGE);
	if(now.tm_hour<10)
		printf("0");
	printf("%d"BLANC BLINK":"UNBLINK ROUGE, now.tm_hour);
	if(now.tm_min<10)
		printf("0");
	printf("%d"BLANC "]", now.tm_min);

	//username : - user =>
	printf(PETIT"-"RESET GROS B_VERT"%s" BLANC ":", user);

	//chemin absolut avec raccourci ~
	printf(GROS B_CYAN);
	if(strlen(pwd)>strlen(home))
	{
		strncpy(shortpwd, &pwd[strlen(home)], (strlen(pwd) - strlen(home)));
		printf("~%s\n", shortpwd);	
	}
	else
	{
		if (strlen(pwd) == strlen(home))
			printf("~\n");
		else
			printf("%s\n", pwd);
	}
	printf(RESET);

	//smiley
	printf( GROS B_JAUNE" <(^_^)> " BLANC BLINK"| "RESET);

	return 0;
}