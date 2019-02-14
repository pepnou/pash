#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("HI FIVE !\n(._.)/      \\(._.)\a");
	fflush(stdout);
	sleep(1);
	printf("\e[27D\e[K(o_o)/    \\(o_o)\a");
	fflush(stdout);
	sleep(1);
	printf("\e[25D\e[K(0_0)/  \\(0_0)\a");
	fflush(stdout);
	sleep(1);
	printf("\e[23D\e[K(^_^)/\\(^_^)\a\n");
	fflush(stdout);

	return 0;
}