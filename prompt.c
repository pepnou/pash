#include <stdio.h>
#include <wordexp.h>

//http://ezprompt.net/

int main(int argc, char const *argv[])
{
	wordexp_t ptr;
	wordexp("PATH", &ptr, WRDE_UNDEF);
	printf("%s\n", ptr);
	return 0;
}