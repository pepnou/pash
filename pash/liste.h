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

struct elem2
{
	int val;
	char* buf;

	struct elem2* suiv;
};
typedef struct elem2 elem2;

typedef struct
{
	unsigned cur;
	elem* liste;
	size_t max_length;
} historique;

void ajoutDeb(elem** liste, char* buf, size_t size);
void supprList(elem* liste);
void ajoutDeb2(elem2** liste, int val, char* buf);
void supprList2(elem2* liste);

#endif