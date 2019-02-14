#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define RESET "\e[0m"
#define BLINK "\e[5m"

/*
links :
https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
https://stackoverflow.com/questions/3219393/stdlib-and-colored-output-in-c
http://web.theurbanpenguin.com/adding-color-to-your-output-from-c/

generator :
http://patorjk.com/software/taag/#p=display&f=Colossal&t=Welcome%0Ain%20pash%20!%20!
*/

void beep()
{
	int parent = fork();

	if(!parent)
	{
		char ** argv = malloc(sizeof(char*));
		argv[0] = "/home/enderswype/master_M2/aise/projet/psh/built-in/build/beep";
		int i = execv("/home/enderswype/master_M2/aise/projet/psh/built-in/build/beep", argv);
		if( i == -1)
			perror("execv");
		exit(1);
	}
}

int main()
{
	//c'etait mon premier essaie j'avais tout fais a la main xD
	//printf("#       #      ###\n #       #      # #\n #       #      # #\n #       #      # #\n #       #      # #\n #       # #### ### #### ##### ##### ####\n #   #   # #  # #   #    #   # # # # #  #\n oo ooo oo oooo o   o    o   o o o o oooo\n  ooo ooo  o    o   o    o   o o   o o\n   o   o   oooo ooo oooo ooooo o   o oooo\n ");

	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);

	beep();

	for (int i = 0; i < w.ws_col; ++i)
	{
		printf("#");
	}

	if(w.ws_col >= 63 /*&& w.ws_col <= 70*/)
		printf("\n\n ██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗\n ██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝\n ██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗  \n ██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝  \n ╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗\n  ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝\n                                                               \n ██╗███╗   ██╗    ██████╗  █████╗ ███████╗██╗  ██╗    ██╗██╗██╗\n ██║████╗  ██║    ██╔══██╗██╔══██╗██╔════╝██║  ██║    ██║██║██║\n ██║██╔██╗ ██║    ██████╔╝███████║███████╗███████║    ██║██║██║\n ██║██║╚██╗██║    ██╔═══╝ ██╔══██║╚════██║██╔══██║    ╚═╝╚═╝╚═╝\n ██║██║ ╚████║    ██║     ██║  ██║███████║██║  ██║    ██╗██╗██╗\n ╚═╝╚═╝  ╚═══╝    ╚═╝     ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝    ╚═╝╚═╝╚═╝\n\n");

	if(w.ws_col < 63 && w.ws_col >=30)
		printf(" __        __   _                          \n \\ \\      / /__| | ___ ___  _ __ ___   ___ \n  \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\\n   \\ V  V /  __/ | (_| (_) | | | | | |  __/\n  _ \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|\n (_)_ __    _ __   __ _ ___| |__   | | | | \n | | '_ \\  | '_ \\ / _` / __| '_ \\  | | | | \n | | | | | | |_) | (_| \\__ \\ | | | |_| |_| \n |_|_| |_| | .__/ \\__,_|___/_| |_| (_) (_) \n           |_|                             \n\n");
	if(w.ws_col < 30)
		printf("WELCOME\nIN PASH\n\n");

	for (int i = 0; i < w.ws_col; ++i)
	{
		printf("#");
	}

	char *user = getenv("USER");
	printf("\n\nBienvenue %s dans ce PASH qualitatif crée par :\n\nPEPIN Thibaut\n     "BLINK"&"RESET"\nEMERY Antoine\n\nN'hésitez pas à utiliser la commande help si vous etes perdu o_<", user);
	fflush(stdout);
	wait(NULL);
	return 0;
}