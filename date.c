#include <time.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
	
	time_t temps = time(NULL);

	struct tm now = *localtime(&temps);

	printf("Nous sommes le %d-%d-%d et il est %dh%dmin%dsec\n", now.tm_mday, now.tm_mon + 1, now.tm_year + 1900, now.tm_hour, now.tm_min, now.tm_sec);

	printf("et\nil s'est écoulé %d secondes depuis le 1er janvier 1970 à minuit\n", temps);
	
	return 0;
}