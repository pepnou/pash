#ifndef __LISTE__
#define __LISTE__

#include <stdio.h>

struct elem
{
	char* buf;
	size_t size;

	struct elem* suiv;
};
typedef struct elem elem;

typedef struct
{
	unsigned cur;
	elem* liste;
	size_t max_length;
} historique;

void ajoutDeb(elem** liste, char* buf, size_t size);
void supprList(elem* liste);

#endif