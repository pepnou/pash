#include <stdio.h>
#include <wordexp.h>
#include <stdlib.h>
#include <time.h>


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
	char *pwd = getenv("PWD"), *user = getenv("USER"), *home = getenv("HOME"), min[2], hour[2], mypwd[100];
	strncpy(mypwd, &pwd[strlen(home)], (strlen(pwd) - strlen(home)));
	sprintf(hour, "%d", now.tm_hour);
	sprintf(min, "%d", now.tm_min);
	/*char zero[2];
	zero[0] = '0';
	if(now.tm_hour < 10)
		strcat(zero, hour);
	if(now.tm_min < 10)
		strcat(zero, min);*/
	//printf(B_CYAN"test\n"CYAN"test\n");
	printf(GROS B_CYAN"~%s\n"B_JAUNE" <(^_^)>"BLANC" ["ROUGE"%d"BLANC BLINK":"UNBLINK ROUGE"%d"BLANC "]"PETIT" -"RESET GROS B_VERT" %s "BLANC "=>\n\n", mypwd, now.tm_hour, now.tm_min, user);

	return 0;
}