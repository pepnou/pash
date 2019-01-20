#include <stdio.h>

struct elem
{
	char* buf;
	size_t size;

	struct elem* suiv;
};
typedef struct elem elem;

void ajoutDeb(elem** liste, char* buf, size_t size);
void supprList(elem* liste);
