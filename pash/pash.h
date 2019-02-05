#include "liste.h"

char *strncpy_buff(char *dest, const char *src, size_t n);

void resize();
void end();
size_t prompt();
void moveC(size_t source, size_t dest, size_t prw);
void eraseLine(size_t cur, size_t fin, size_t prw);
void bubbleSort(elem* liste);
void display(historique h, int selected);
historique* autoComp(char* buf, size_t* cur, size_t* fin, size_t* prw);
void execution(char* buf);
int specialExec(int argc, char** argv);
void selection(char c, char* buf, size_t* cur, size_t* fin, size_t* size, size_t* prw, historique* h, historique* search);
void handle(char c, char* buf, size_t* cur, size_t* fin, size_t* size, size_t* prw, historique* h, historique* search);
