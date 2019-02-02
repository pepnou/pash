#include "liste.h"


void resize();
void end();
size_t prompt();
void moveC(size_t source, size_t dest, size_t prw);
void eraseLine(size_t cur, size_t fin, size_t prw);
void bubbleSort(elem* liste);
void display(historique h, int selected);
historique* autoComp(char* buf, size_t* cur, size_t* fin, size_t* prw);
void execution(char* buf);
void selection(char c, char* buf, size_t* cur, size_t* fin, size_t* size, size_t* prw, historique* h, historique* search);
void handle(char c, char* buf, size_t* cur, size_t* fin, size_t* size, size_t* prw, historique* h, historique* search);
