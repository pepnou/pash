#include "liste.h"

#define SAVEC "\033[s"
#define RESTC "\033[u"
#define UPC "\033[A"
#define DOWNC "\033[B"
#define FORWC "\033[C"
#define BACKC "\033[D"
#define DELLI "\033[K"


void resize();
void end();
size_t prompt();
void moveC(size_t* source, size_t* dest, size_t* prw);
void eraseLine(size_t* cur, size_t* fin, size_t* prw);
void handle( char c, char* buf, size_t* cur, size_t* fin, size_t* size, size_t* prw, historique* h);
int autoComp(char* buf, size_t* cur, size_t* fin, size_t* prw);