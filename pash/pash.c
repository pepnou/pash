#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <dirent.h>
#include <math.h>


#include "pash.h"


FILE* f;


int width, height;
int over = 0;

void resize()
{
	struct winsize w;
	ioctl( 0, TIOCGWINSZ, &w);
	width = w.ws_col;
	height = w.ws_row;
}

//mettre fin au processus enfants
void end()
{
	//regarder ptrace
}

size_t prompt()
{
	// http://ezprompt.net/
	char pr[] = "\nCC$ ";
	write(STDOUT_FILENO, pr, strlen(pr));

	return strlen(pr) - 1; // -1 pour enlever la longueur de \n
	
	/*char* argmnts[] = {NULL}; 
	execv("./home/enderswype/master_M2/AISE/Projet_shell/psh/buil", argmnts);*/
}

void moveC(size_t source, size_t dest, size_t prw)
{
	int lS = (source + prw) / width;
	int lD = (dest + prw) / width;

	int ldiff = lD - lS;
	
	if(ldiff < 0)
	{
		for(int i = 0; i < -1*ldiff; i++)
			write(STDOUT_FILENO, UPC, strlen(UPC));
	}
	else if(ldiff > 0)
	{
		for(int i = 0; i < ldiff; i++)
			write(STDOUT_FILENO, DOWNC, strlen(DOWNC));
	}

	int cS = (source + prw) % width;
	int cD = (dest + prw) % width;

	int cdiff = cD - cS;

	if(cdiff < 0)
	{
		for(int i = 0; i < -1*cdiff; i++)
			write(STDOUT_FILENO, BACKC, strlen(BACKC));
	}
	else if(cdiff > 0)
	{
		for(int i = 0; i < cdiff; i++)
			write(STDOUT_FILENO, FORWC, strlen(FORWC));
	}
}

void eraseLine(size_t cur, size_t fin, size_t prw)
{

	moveC(cur, fin, prw);

	unsigned nbL = (fin + prw) / width;
	unsigned nbC = (prw + fin) % width;
	
	for(unsigned i = 0; i < nbC; i++)
		write(STDOUT_FILENO, BACKC, strlen(BACKC));

	for(unsigned i = 0; i < nbL; i++)
	{
		write(STDOUT_FILENO, DELLI, strlen(DELLI));
		write(STDOUT_FILENO, UPC, strlen(UPC));
	}

	for(unsigned i = 0; i < prw; i++)
		write(STDOUT_FILENO, FORWC, strlen(FORWC));

	write(STDOUT_FILENO, DELLI, strlen(DELLI));
}

void  bubbleSort(elem* liste)
{
	elem* tmp = liste;
	char* swap;
	int done;

	do {
		done = 1;

		while(tmp && tmp->suiv)
		{
			if(strcmp(tmp->buf, tmp->suiv->buf) > 0)
			{
				done = 0;

				swap = tmp->buf;
				tmp->buf = tmp->suiv->buf;
				tmp->suiv->buf = swap;
			}

			tmp = tmp->suiv;
		}
		tmp = liste;
	} while(!done);
}

void display(historique h, int selected)
{
	elem* tmp = h.liste->suiv;
	char* nom = h.liste->buf;
	unsigned w = width;

	if(w < (h.max_length + 2)*2) //2 resulat par ligne + 2 espace de separation
	{
		while(tmp != NULL)
		{
			write(STDOUT_FILENO, "\n", 1);
			if(strlen(nom) != 0)
				write(STDOUT_FILENO, nom, strlen(nom));
			write(STDOUT_FILENO, tmp->buf, strlen(tmp->buf));
			
			tmp = tmp->suiv;
		}
	}
	else
	{
		unsigned nbpL = w / (h.max_length + 2);
		unsigned nbL = ceil((double)h.cur/nbpL);

		char** ordered = malloc(nbL*sizeof(char*));
		for(unsigned i = 0; i < nbL; i++)
		{
			ordered[i] = malloc(w + 1);
			for(unsigned j = 0; j < w + 1; j++)
			{
				ordered[i][j] = ' ';
			}
			ordered[i][w] = '\0';
		}

		unsigned cur = 0;
		while(tmp != NULL)
		{
			unsigned pos = (width/nbpL) * (cur/nbL);

			strncpy( &(ordered[cur%nbL][pos]), nom, strlen(nom));
			strncpy( &(ordered[cur%nbL][pos+strlen(nom)]), tmp->buf, strlen(tmp->buf));
			
			tmp = tmp->suiv;
			cur++;
		}

		for(unsigned i = 0; i < nbL; i++)
		{
			if(selected >= 0 && i == selected % nbL)
			{
				unsigned pos = (width/nbpL) * (selected/nbL);

				write(STDOUT_FILENO, "\n", 1);
				write(STDOUT_FILENO, ordered[i], pos);
				write(STDOUT_FILENO, BACKG, strlen(BACKG));
				write(STDOUT_FILENO, &(ordered[i][pos]), h.max_length + 2);
				write(STDOUT_FILENO, RESET, strlen(RESET));
				write(STDOUT_FILENO, &(ordered[i][pos + h.max_length + 2]), width - pos - h.max_length + 2);
			}
			else
			{
				write(STDOUT_FILENO, "\n", 1);
				write(STDOUT_FILENO, ordered[i], width);
			}
		}
	}
}

historique* autoComp(char* buf, size_t* cur, size_t* fin, size_t* prw)
{
	if(*cur == 0)
		return 0;

	int deb = 0, path = -1;
	for(deb = *cur - 1; deb >= 0; deb--)
	{
		if(buf[deb] == '|')
			break;

		if(buf[deb] == ' ' && (deb == 0 || buf[deb - 1] != '\\'))
			break;

		if(path == -1 && buf[deb] == '/')
			path = deb;
	}
	deb++;
	if(path == -1)
		path = deb;

	char *chemin, *nom;

	

	historique* h = malloc(sizeof(historique));
	h->max_length = 0;
	h->cur = 0;
	h->liste = NULL;

	struct dirent* file;
	DIR* dir;

	if( buf[deb] == '/' || buf[deb] == '.' || buf[deb] == '~') //chemin absolu ou relatif
	{
		chemin = malloc(path - deb + 1);
		if(!chemin)
		{
			perror("alloc: ");
			exit(1);
		}
		strncpy(chemin, &(buf[deb]), path - deb + 1);
		chemin[path - deb] = '\0';

		nom = malloc(*cur - path + 1);
		if(!nom)
		{
			perror("alloc: ");
			exit(1);
		}
		strncpy(nom, &(buf[path + 1]), *cur - path);
		nom[*cur - path] = '\0';
		//fprintf(f, "%s\n", nom);

		dir = opendir(chemin);

		if(!dir)
		{
			supprList(h->liste);
			free(h);
			return NULL;
		}

		while((file = readdir(dir)))
		{
			if((strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") !=0) && (strlen(nom) == 0 || strncmp(nom, file->d_name, strlen(nom)) == 0))
			{
				h->cur++;
				if(file->d_type == DT_DIR)
				{
					ajoutDeb(&(h->liste), &(file->d_name[strlen(nom)]), strlen(file->d_name) - strlen(nom) + 1);
					h->liste->buf[strlen(h->liste->buf) + 1] = '\0';
					h->liste->buf[strlen(h->liste->buf)] = '/';

					if(strlen(file->d_name) - strlen(nom) + 1 > h->max_length)
						h->max_length = strlen(file->d_name) + 1;
				}
				else
				{
					ajoutDeb(&(h->liste), &(file->d_name[strlen(nom)]), strlen(file->d_name) - strlen(nom));

					if(strlen(file->d_name) - strlen(nom) > h->max_length)
						h->max_length = strlen(file->d_name);
				}
			}
		}

		closedir(dir);
		free(chemin);
	}
	else //$PATH
	{
		nom = malloc(*cur - path + 1);
		if(!nom)
		{
			perror("alloc: ");
			exit(1);
		}
		strncpy( nom, &(buf[path]), *cur - path);
		nom[*cur - path] = '\0';
		//fprintf(f, "%s\n", nom);

		char* PATH = getenv("PATH");
		unsigned Pdeb = 0, Pfin;

		for(Pfin = 0; Pfin <= strlen(PATH); Pfin++)
		{
			if(PATH[Pfin] == ':' || PATH[Pfin] == '\0')
			{
				chemin = malloc(Pfin - Pdeb + 3); // +1 pour le dernier /, +1 pour \0, +1 car on indice a partir de 0
				if(!chemin)
				{
					perror("alloc: ");
					exit(1);
				}
				strncpy(chemin, &(PATH[Pdeb]), Pfin - Pdeb);
				chemin[Pfin - Pdeb + 1] = '\0';
				chemin[Pfin - Pdeb] = '/';


				dir = opendir(chemin);

				if(!dir)
				{
					Pdeb = Pfin + 1;
					free(chemin);
					continue;
				}

				while((file = readdir(dir)))
				{
					if(!strncmp(nom, file->d_name, strlen(nom)) && strcmp(file->d_name, ".") && strcmp(file->d_name, ".."))
					{
						h->cur++;
						if(file->d_type == DT_DIR)
						{
							ajoutDeb(&(h->liste), &(file->d_name[strlen(nom)]), strlen(file->d_name) - strlen(nom) + 1);
							h->liste->buf[strlen(h->liste->buf) + 1] = '\0';
							h->liste->buf[strlen(h->liste->buf)] = '/';

							if(strlen(file->d_name) - strlen(nom) + 1 > h->max_length)
								h->max_length = strlen(file->d_name) + 1;
						}
						else
						{
							ajoutDeb(&(h->liste), &(file->d_name[strlen(nom)]), strlen(file->d_name) - strlen(nom));

							if(strlen(file->d_name) - strlen(nom) > h->max_length)
								h->max_length = strlen(file->d_name);
						}
					}
				}


				Pdeb = Pfin + 1;
				free(chemin);
			}
		}
	}

	if(h->cur == 0)
	{
		free(h);
		h = NULL;
	}
	else if(h->cur == 1)
	{
		strncpy( &(buf[*cur + strlen(h->liste->buf)]), &(buf[*cur]), *fin - *cur);
		strncpy(&(buf[*cur]), h->liste->buf, strlen(h->liste->buf));

		eraseLine(*cur, *fin, *prw);

		write(STDOUT_FILENO, buf, strlen(buf));

		*cur = *cur + strlen(h->liste->buf);
		*fin = *fin + strlen(h->liste->buf);

		moveC( *fin, *cur, *prw);

		supprList(h->liste);
		free(h);
		h = NULL;
	}
	else
	{
		moveC( *cur, *fin, *prw);

		bubbleSort(h->liste);
		ajoutDeb(&(h->liste), nom, strlen(nom));
		display(*h, -1);

		prompt();
		write(STDOUT_FILENO, buf, strlen(buf));
		moveC( *fin, *cur, *prw);
	}

	free(nom);
	return h;
}

void execution(char* buf)
{
	/*int parent = fork();
	if(!parent)
	{
		char** argv = malloc(sizeof(char*));
		argv[0] = "/bin/ls";
		int i = execv("/bin/ls", argv);
		if(i == -1)
			perror("execv");
		exit(1);
	}
	wait(NULL);*/

	fprintf(f, "%s\n", buf);
	fprintf(f, "\n");

	char *cpy, **tmp1, ***tmp2, ****WOW;
	size_t tmp, size1, *size2, **size3;

	cpy = malloc((strlen(buf) + 1) * sizeof(char));
	//strcpy( cpy, buf);


	/*int done;
	do {
		done = 1;
		for(size_t i = 0; i < strlen(cpy); i++)
		{
			if(cpy[i] == ' ')
			{
				if(i == 0 || cpy[i+1] == ' ' || cpy[i+1] == '&' || cpy[i+1] == '|' || cpy[i+1] == '\0')
				{
					strncpy( &cpy[i+1], &cpy[i], strlen(cpy) - i - 1);
					done = 0;
					//i--;
				}
			}
			else if(cpy[i] == '&')
			{
				if(i == 0 || (cpy[i-1] == '&' && (cpy[i+1] == '&')) || cpy[i+1] == '\0')
				{
					strncpy( &cpy[i+1], &cpy[i], strlen(cpy) - i - 1);
					done = 0;
					//i--;
				}
			}
			else if(cpy[i] == '|')
			{
				if(i == 0 || cpy[i+1] == '|' || cpy[i+1] == '\0')
				{
					strncpy( &cpy[i+1], &cpy[i], strlen(cpy) - i - 1);
					done = 0;
					//i--;
				}
			}
		}
	} while(!done);*/

	int j = 0;
	for(size_t i = 0; i < strlen(buf); i++)
	{
		if(!(buf[i] == ' ' && (j == 0 || cpy[j-1] == ' ' || cpy[j-1] == '&' || cpy[j-1] == '|' || buf[i+1] == '&'|| buf[i+1] == '|'|| buf[i+1] == '\0')))
			if(!(buf[i] == '&' && (j == 0 || (cpy[j-1] == '&' && (buf[i+1] == '&' || buf[i+1] == '\0')))))
				if(!(buf[i] == '|' && (j == 0 || buf[i+1] == '|' || buf[i+1] == '\0')))
				{
					cpy[j] = buf[i];
					j++;
				}
	}
	cpy[j] = '\0';

	fprintf(f, "%s\n", cpy);
	fprintf(f, "\n");

	size1 = 0;
	for(size_t i = 0; i < strlen(cpy) - 1; i++)
		if(!strncmp(&cpy[i], "&&", 2))
			size1++;
	size1++;
	fprintf(f, "%ld\n", size1);
	fflush(f);
	
	tmp1 = malloc((size1 + 1)*sizeof(char*));
	tmp2 = malloc((size1 + 1)*sizeof(char**));
	WOW = malloc((size1 + 1)*sizeof(char***));

	size2 = malloc((size1 + 1)*sizeof(size_t));
	size3 = malloc((size1 + 1)*sizeof(size_t*));

	tmp = 0;
	do {
		if(!tmp)
			tmp1[tmp] = strtok(cpy, "&&");
		else
			tmp1[tmp] = strtok(NULL, "&&");
		fprintf(f, "%s\n", tmp1[tmp]);
		fflush(f);
		
		tmp++;
	//} while(tmp <= size1 + 1);
	} while(tmp1[tmp - 1]);
	fprintf(f, "\n");
	fflush(f);


	for(size_t i = 0; i < size1; i++)
	{
		size2[i] = 0;
		for(size_t j = 0; j < strlen(tmp1[i]); j++)
			if(!strncmp(&tmp1[i][j], "|", 1))
				size2[i]++;
		size2[i]++;
		fprintf(f, "%ld\n", size2[i]);
		fflush(f);

		tmp2[i] = malloc((size2[i] + 1)*sizeof(char*));
		WOW[i] = malloc((size2[i] + 1)*sizeof(char**));

		size3[i] = malloc((size1 + 1)*sizeof(size_t*));

		tmp = 0;
		do {
			if(!tmp)
				tmp2[i][tmp] = strtok(tmp1[i], "|");
			else
				tmp2[i][tmp] = strtok(NULL, "|");
			fprintf(f, "%s\n", tmp2[i][tmp]);
			fflush(f);
			
			tmp++;
		//} while(tmp <= size2[i] + 1);
		} while(tmp2[i][tmp - 1]);
	}
	fprintf(f, "\n");
	fflush(f);


	for(size_t i = 0; i < size1; i++)
	{
		for(size_t j = 0; j < size2[i]; j++)
		{
			size3[i][j] = 0;
			for(size_t k = 0; k < strlen(tmp2[i][j]); k++)
				if(!strncmp(&tmp2[i][j][k], " ", 1))
					size3[i][j]++;
			size3[i][j]++;
			fprintf(f, "%ld\n", size3[i][j]);
			fflush(f);

			WOW[i][j] = malloc((size3[i][j] + 1)*sizeof(char*));

			tmp = 0;
			do {
				if(!tmp)
					WOW[i][j][tmp] = strtok(tmp2[i][j], " ");
				else
					WOW[i][j][tmp] = strtok(NULL, " ");
				fprintf(f, "%s\n", WOW[i][j][tmp]);
				fflush(f);
				
				tmp++;
			//} while(tmp <= size3[i][j] + 1);
			} while(WOW[i][j][tmp - 1]);
		}
	}
	fprintf(f, "\n");
	fflush(f);





	// TRAITEMENT



	free(cpy);

	for(size_t i = 0; i < size1; i++)
	{
		for(size_t j = 0; j < size2[i]; j++)
		{
			for(size_t k = 0; k < size3[i][j]; j++)
			{
				free(WOW[i][j][k]);
			}
			free(WOW[i][j]);
			free(tmp2[i][j]);
		}
		free(WOW[i]);
		free(tmp2[i]);
		free(tmp1[i]);
	}
	free(WOW);
	free(tmp2);
	free(tmp1);

	fprintf(f, "yolo\n");
	fflush(f);

	for(size_t i = 0; i < size1; i++)
	{
		free(size3[i]);
	}
	free(size3);

	free(size2);
}

void selection( char c, char* buf, size_t* cur, size_t* fin, size_t* size, size_t* prw, historique* h, historique* search)
{
	unsigned w = width;
	char* nom = search->liste->buf;

	unsigned nbpL = w / (search->max_length + 2);
	unsigned nbL = ceil((double)search->cur/nbpL);

	size_t Sfin = nbL * width;

	unsigned finished = 0;
	unsigned selected = 0;

	eraseLine( *cur + *prw + 1, *fin, 0);
	eraseLine( Sfin, Sfin, 0);
	moveC( width, width - 1, 0);
	display( *search, selected);
	prompt();
	write(STDOUT_FILENO, buf, strlen(buf));


	//fprintf(f, "%d, %d, %d, %d\n", w, nbpL, nbL, Sfin);

	while(!over && !finished)
	{
		read(STDIN_FILENO, &c, 1);

		switch(c)
		{
			//terminer le shell : ctrl + D
			case 4:
			{
				over = 1;
				write(STDOUT_FILENO, "\n", 1);
				break;
			}
			//tab : exit selection
			case 9:
			{
				finished = 1;
				prompt();
				write(STDOUT_FILENO, nom, strlen(nom));
				break;
			}
			//new line : ctrl + J : selectionne
			case 10:
			{
				moveC(*cur + *prw + width, width - 1, 0);
				prompt();

				elem* tmp = search->liste;
				for(unsigned i = 0; i <= selected; i++)
					tmp = tmp->suiv;

				strncpy( &(buf[*cur + strlen(tmp->buf)]), &(buf[*cur]), *fin - *cur);
				strncpy( &(buf[*cur - strlen(nom)]), nom, strlen(nom));
				strncpy( &(buf[*cur]), tmp->buf, strlen(tmp->buf));

				*cur += strlen(tmp->buf);
				*fin += strlen(tmp->buf);

				//fprintf(f, "%ld, %ld\n", *cur, *fin);

				moveC(*cur, *fin, *prw);

				buf[*fin + 1] = '\0';

				write(STDOUT_FILENO, buf, strlen(buf));

				finished = 1;
				break;
			}
			//partie avec les flèches
			case 27:
			{
				read(STDIN_FILENO, &c, 1);

				if(c == '[')
				{
					read(STDIN_FILENO, &c, 1);

					switch(c)
					{
						case 'A': //haut
						{
							if(selected % nbL == 0)
								break;
							selected--;

							/*w = width;

							nbpL = w / (search->max_length + 2);
							nbL = ceil((double)search->cur/nbpL);

							Sfin = nbL * width;*/

							eraseLine( *cur + *prw + 1, *fin, 0);
							eraseLine( Sfin, Sfin, 0);
							moveC( width, width - 1, 0);
							display( *search, selected);
							prompt();
							write(STDOUT_FILENO, buf, strlen(buf));
							break;
						}
						case 'B': //bas
						{
							if((selected + 1) % nbL == 0 || selected + 1 == search->cur)
								break;
							selected++;

							/*w = width;

							nbpL = w / (search->max_length + 2);
							nbL = ceil((double)search->cur/nbpL);

							Sfin = nbL * width;*/

							eraseLine( *cur + *prw + 1, *fin, 0);
							eraseLine( Sfin, Sfin, 0);
							moveC( width, width - 1, 0);
							display( *search, selected);
							prompt();
							write(STDOUT_FILENO, buf, strlen(buf));
							break;
						}
						case 'C': //droite
						{
							if(selected + nbL >= search->cur)
								break;
							selected += nbL;

							/*w = width;

							nbpL = w / (search->max_length + 2);
							nbL = ceil((double)search->cur/nbpL);

							Sfin = nbL * width;*/

							//fprintf(f, "%d, %d, %d, %ld\n", w, nbpL, nbL, Sfin);

							eraseLine( *cur + *prw + 1, *fin, 0);
							eraseLine( Sfin, Sfin, 0);
							moveC( width, width - 1, 0);
							display( *search, selected);
							prompt();
							write(STDOUT_FILENO, buf, strlen(buf));
							break;
						}
						case 'D': //gauche
						{
							if(selected/nbL == 0)
								break;
							selected -= nbL;

							/*w = width;

							nbpL = w / (search->max_length + 2);
							nbL = ceil((double)search->cur/nbpL);

							Sfin = nbL * width;*/

							eraseLine( *cur + *prw + 1, *fin, 0);
							eraseLine( Sfin, Sfin, 0);
							moveC( width, width - 1, 0);
							display( *search, selected);
							prompt();
							write(STDOUT_FILENO, buf, strlen(buf));
							break;
						}
					}
				}
				break;
			}
		}
	}
}

void handle( char c, char* buf, size_t* cur, size_t* fin, size_t* size, size_t* prw, historique* h, historique* search)
{
	if(c >= 32 && c <= 126)
	{
		if(*cur == *fin)
		{
			write(STDOUT_FILENO, &c, 1);
			buf[*cur] = c;

			if((*cur + *prw + 1) % width == 0)
			{
				write(STDOUT_FILENO, " ", 1);
				write(STDOUT_FILENO, BACKC, strlen(BACKC));
			}
			(*cur)++;
			(*fin)++;
		}
		else
		{
			strncpy( &(buf[*cur + 1]), &(buf[*cur]), *fin - *cur + 1);
			buf[*cur] = c;
						
			write(STDOUT_FILENO, &(buf[*cur]), *fin - *cur + 1);
			
			if((*fin + *prw + 1) % width == 0)
			{
				write(STDOUT_FILENO, " ", 1);
				write(STDOUT_FILENO, BACKC, strlen(BACKC));
			}

			(*cur)++;
			(*fin)++;

			moveC( *fin, *cur, *prw);
		}
	}
	else
	{
		switch(c)
		{
			//terminer le shell : ctrl + D
			case 4:
			{
				over = 1;
				write(STDOUT_FILENO, "\n", 1);
				break;
			}
			//tab
			case 9:
			{
				search = autoComp( buf, cur, fin, prw);

				if(search) //selectio parmi les resultats de la recherche : ctrl + q
				{
					read(STDIN_FILENO, &c, 1);
					if(c == 9)
					{
						selection(c, buf, cur, fin, size, prw, h, search);;
					}
					else
					{
						handle(c, buf, cur, fin, size, prw, h, search);
					}
				}
				break;
			}
			//new line : ctrl + J
			case 10:
			{
				h->cur = 0;
				if(*fin != 0)
					ajoutDeb(&(h->liste), buf, *fin);

				execution(buf);

				*prw = prompt();

				(*cur) = (*fin) = 0;
				write(STDOUT_FILENO, SAVEC, strlen(SAVEC));
				break;
			}
			//effacer tout : ctrl + U
			case 21:
			{
				eraseLine(*cur, *fin, *prw);
				(*cur) = (*fin) = 0;
				break;
			}
			//effacer un caractère
			case 127:
			{
				if(*cur == 0)
					break;

				strncpy( &(buf[*cur - 1]), &(buf[*cur]), *fin - *cur + 1);

				eraseLine( *cur, *fin, *prw);

				write(STDOUT_FILENO, buf, *fin - 1);

				if((*fin + *prw - 1) % width == 0)
				{
					write(STDOUT_FILENO, " ", 1);
					write(STDOUT_FILENO, BACKC, strlen(BACKC));
				}

				(*cur)--;
				(*fin)--;

				moveC( *fin, *cur, *prw);

				break;
			}
			//partie avec les flèches
			case 27:
			{
				read(STDIN_FILENO, &c, 1);

				if(c == '[')
				{
					read(STDIN_FILENO, &c, 1);

					switch(c)
					{
						case 'A': //haut
						{
							elem* tmp = h->liste;
							unsigned i;
							for(i = 0; i < h->cur; i++)
							{
								if(!tmp)
									break;
								tmp = tmp->suiv;
							}
							if(tmp)
							{
								eraseLine( *cur, *fin, *prw);

								(h->cur)++;
								//*cur = 0;
								//*fin = tmp->size - 1;
								*cur = *fin = tmp->size;

								strncpy(buf, tmp->buf, tmp->size);
								buf[tmp->size] = '\0';
								write(STDOUT_FILENO, buf, tmp->size);

								//moveC( *fin, *cur, *prw);
							}
							break;
						}
						case 'B': //bas
						{
							if(h->cur == 0)
								break;

							if(h->cur == 1)
							{
								eraseLine( *cur, *fin, *prw);

								(h->cur)--;
								*cur = *fin = 0;

								break;
							}

							elem* tmp = h->liste;
							unsigned i;
							for(i = 0; i < h->cur - 2; i++)
								tmp = tmp->suiv;

							eraseLine( *cur, *fin, *prw);

							(h->cur)--;
							//*cur = 0;
							//*fin = tmp->size - 1;
							*cur = *fin = tmp->size;

							strncpy(buf, tmp->buf, tmp->size);
							buf[tmp->size] = '\0';
							write(STDOUT_FILENO, buf, tmp->size);

							//moveC( *fin, *cur, *prw);
							break;
						}
						case 'C': //droite
						{
							if(*cur < *fin)
							{
								moveC( *cur, *fin, *prw);
								(*cur)++;
								moveC( *fin, *cur, *prw);
							}
							break;
						}
						case 'D': //gauche
						{
							if(*cur > 0)
							{
								moveC( *cur, *fin, *prw);
								(*cur)--;
								moveC( *fin, *cur, *prw);
							}
							break;
						}
						case '3': //suppr
						{
							read(STDIN_FILENO, &c, 1);
							if(c != '~')
							{
								handle(c, buf, cur, fin, size, prw, h, search);
								break;
							}

							if(*cur == *fin)
								break;

							strncpy( &(buf[*cur]), &(buf[*cur + 1]), *fin - *cur);

							eraseLine( *cur, *fin, *prw);

							write(STDOUT_FILENO, buf, *fin - 1);

							if((*fin + *prw - 1) % width == 0)
							{
								write(STDOUT_FILENO, " ", 1);
								write(STDOUT_FILENO, BACKC, strlen(BACKC));
							}

							(*fin)--;

							moveC( *fin, *cur, *prw);

							break;
						}


						default:
						{
							handle(c, buf, cur, fin, size, prw, h, search);
							break;
						}
					}
				}
				else
				{
					handle(c, buf, cur, fin, size, prw, h, search);
				}

				break;
			}
			default:
			{
				printf("%d\n", c);
			}
		}
	}
}







int main( int argc, char** argv, char** envp)
{
	f = fopen("./log.txt", "a+");



	signal( SIGWINCH, &resize);
	signal( SIGINT, &end);

	static struct termios old, new1;
	int echo = 0;

	tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */
    new1.c_lflag &= ~ICANON; /* disable buffered i/o */
    new1.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
    


	char c, *buf;
	size_t cur, fin, size = 1024, prw;
	cur = fin = 0;
	if(!(buf = malloc(size)))
	{
		perror("allocation du buffer : ");
		exit(1);
	}
	
	resize();
	prw = prompt();
	write(STDOUT_FILENO, SAVEC, strlen(SAVEC));

	historique h;
	h.cur = 0;
	h.liste = NULL;

	do
	{
		read(STDIN_FILENO, &c, 1);

		handle(c, buf, &cur, &fin, &size, &prw, &h, NULL);

	} while(!over);



	tcsetattr(0, TCSANOW, &old);
	supprList(h.liste);


	fclose(f);

	exit(0);
}
