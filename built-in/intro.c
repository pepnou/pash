#include <stdio.h>
#include <sys/ioctl.h>

/*
links :
https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
https://stackoverflow.com/questions/3219393/stdlib-and-colored-output-in-c
http://web.theurbanpenguin.com/adding-color-to-your-output-from-c/

generator :
http://patorjk.com/software/taag/#p=display&f=Colossal&t=Welcome%0Ain%20pash%20!%20!
*/

int main()
{
	//c'etait mon premier essaie j'avais tout fais a la main xD
	//printf("#       #      ###\n #       #      # #\n #       #      # #\n #       #      # #\n #       #      # #\n #       # #### ### #### ##### ##### ####\n #   #   # #  # #   #    #   # # # # #  #\n oo ooo oo oooo o   o    o   o o o o oooo\n  ooo ooo  o    o   o    o   o o   o o\n   o   o   oooo ooo oooo ooooo o   o oooo\n ");

	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);

	if(w.ws_col >= 63 /*&& w.ws_col <= 70*/)
		printf("\n ██╗    ██╗███████╗██╗      ██████╗ ██████╗ ███╗   ███╗███████╗\n ██║    ██║██╔════╝██║     ██╔════╝██╔═══██╗████╗ ████║██╔════╝\n ██║ █╗ ██║█████╗  ██║     ██║     ██║   ██║██╔████╔██║█████╗  \n ██║███╗██║██╔══╝  ██║     ██║     ██║   ██║██║╚██╔╝██║██╔══╝  \n ╚███╔███╔╝███████╗███████╗╚██████╗╚██████╔╝██║ ╚═╝ ██║███████╗\n  ╚══╝╚══╝ ╚══════╝╚══════╝ ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝\n                                                               \n ██╗███╗   ██╗    ██████╗  █████╗ ███████╗██╗  ██╗    ██╗██╗██╗\n ██║████╗  ██║    ██╔══██╗██╔══██╗██╔════╝██║  ██║    ██║██║██║\n ██║██╔██╗ ██║    ██████╔╝███████║███████╗███████║    ██║██║██║\n ██║██║╚██╗██║    ██╔═══╝ ██╔══██║╚════██║██╔══██║    ╚═╝╚═╝╚═╝\n ██║██║ ╚████║    ██║     ██║  ██║███████║██║  ██║    ██╗██╗██╗\n ╚═╝╚═╝  ╚═══╝    ╚═╝     ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝    ╚═╝╚═╝╚═╝\n\n");

	if(w.ws_col < 63 && w.ws_col >=30)
		printf(" __        __   _                          \n \\ \\      / /__| | ___ ___  _ __ ___   ___ \n  \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\\n   \\ V  V /  __/ | (_| (_) | | | | | |  __/\n  _ \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|\n (_)_ __    _ __   __ _ ___| |__   | | | | \n | | '_ \\  | '_ \\ / _` / __| '_ \\  | | | | \n | | | | | | |_) | (_| \\__ \\ | | | |_| |_| \n |_|_| |_| | .__/ \\__,_|___/_| |_| (_) (_) \n           |_|                             \n\n");
	if(w.ws_col < 30)
		printf("WELCOME\nIN PASH\n\n");
	return 0;
}