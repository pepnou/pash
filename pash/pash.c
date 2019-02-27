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
#include <regex.h>
#include <time.h>
#include <limits.h>


#include "pash.h"

//couleur normal
#define NOIR "\e[30m"
#define ROUGE "\e[31m"
#define VERT "\e[32m"
#define JAUNE "\e[33m"
#define BLEUE "\e[34m"
#define MAGENTA "\e[35m"
#define CYAN "\e[36m"
#define BLANC "\e[37m"

//couleur bright
#define B_NOIR "\e[90m"
#define B_ROUGE "\e[91m"
#define B_VERT "\e[92m"
#define B_JAUNE "\e[93m"
#define B_BLEUE "\e[94m"
#define B_MAGENTA "\e[95m"
#define B_CYAN "\e[96m"
#define B_BLANC "\e[97m"

//
#define RESET "\e[0m"
#define GROS "\e[1m"
#define PETIT "\e[2m"
#define ITALIC "\e[3m"
#define BLINK "\e[5m"
#define UNBLINK "\e[25m"

//controle curseur
#define SAVEC "\e[s"
#define RESTC "\e[u"
#define UPC "\e[A"
#define DOWNC "\e[B"
#define FORWC "\e[C"
#define BACKC "\e[D"
#define DELLI "\e[K"

//fond blanc police noire
#define BACKG "\e[30;47m"


char *smiley = "<(^_^)>";


//FILE* f;


int width, height;
int over = 0;
elem2* processes = NULL;

//same as strncpy but allow buffer overlap
char *strncpy_buff(char *dest, const char *src, size_t n)
{
	char* tmp = malloc(n*sizeof(char));

	strncpy(tmp, src, n);
	strncpy(dest, tmp, n);
	free(tmp);

	return dest;
}

void beep()
{
	int parent = fork();

	if(!parent)
	{
		int i = execvp("beep", NULL);
		if( i == -1)
			perror("execv");
		exit(1);
	}
	wait(NULL);
}

void intro()
{
	int parent = fork();

	if(!parent)
	{
		int i = execvp("intro", NULL);
		if( i == -1)
			perror("execvp");
		exit(1);
	}
	wait(NULL);
}

//window resize signal handler
void resize()
{
	struct winsize w;
	ioctl( 0, TIOCGWINSZ, &w);
	width = w.ws_col;
	height = w.ws_row;
}

//SIGINT signal handler
void end()
{
	elem2* tmp = processes;
	while(tmp != NULL)
	{
		kill(tmp->val, SIGINT);
		tmp = tmp->suiv;
	}
	supprList2(processes);
	processes = NULL;
}

//display prompt
size_t prompt()
{
	size_t size = 0;
	char *pwd = malloc(200*sizeof(char)), *user = getenv("USER"), *home = getenv("HOME"), *host = malloc(100*sizeof(char));
	getcwd(pwd, 200);
	gethostname(host, 100);
	char* prompt = malloc(500*sizeof(char));

	time_t temps = time(NULL);
	struct tm now = *localtime(&temps);
	char* trunc_pwd = malloc(strlen(pwd)*sizeof(char));

	if(!(strncmp(home, pwd, strlen(home))))
	{
		trunc_pwd[0] = '~';
		strcpy(&(trunc_pwd[1]), &(pwd[strlen(home)]));
		trunc_pwd[strlen(pwd) - strlen(home) + 1] = '/';
		trunc_pwd[strlen(pwd) - strlen(home) + 2] = '\0';
	}
	else
	{
		strcpy(trunc_pwd, pwd);
	}

	sprintf(prompt, GROS BLANC "[" ROUGE "%02d" BLANC BLINK ":" UNBLINK ROUGE "%02d" BLANC "]" PETIT "-" RESET GROS B_VERT "%s@%s" BLANC ":" B_CYAN "%s\n"
			B_JAUNE " %s " BLANC BLINK "| "RESET, 
			now.tm_hour, now.tm_min, user, host, trunc_pwd,
			smiley);

	write(STDOUT_FILENO, prompt, strlen(prompt));

	size +=   5 // heure
			+ 1 // tiret
			+ strlen(user)
			+ 1 // @
			+ strlen(host)
			+ 1 // :
			+ strlen(trunc_pwd);
	size = (size/width)*width + width; // \n

	size +=   1 // espace
			+ strlen(smiley)
			+ 3; // espace | espace

	free(host);
	free(prompt);
	free(trunc_pwd);
	free(pwd);

	return size;
}

//move the cursor
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

//erase the entire line
void eraseLine(size_t cur, size_t fin, size_t prw)
{
	prw = prw % width;

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

//use a bubblesort to sort a queue of string
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

//display the result of the auto-completion
void display(historique h, int selected)
{
	elem* tmp = h.liste->suiv;
	char* nom = h.liste->buf;
	unsigned w = width;

	if(w < (h.max_length + 2)*2) //2 resulat par ligne + 2 espace de separation
	{
		while(tmp != NULL)
		{
			if(strlen(nom) != 0)
				write(STDOUT_FILENO, nom, strlen(nom));
			write(STDOUT_FILENO, tmp->buf, strlen(tmp->buf));
			write(STDOUT_FILENO, "\n", 1);
			
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
			//fprintf(f, "%s\n", ordered[i]);
			if(selected >= 0 && i == selected % nbL)
			{
				unsigned pos = (width/nbpL) * (selected/nbL);

				write(STDOUT_FILENO, ordered[i], pos);
				write(STDOUT_FILENO, BACKG, strlen(BACKG));
				write(STDOUT_FILENO, &(ordered[i][pos]), h.max_length + 2);
				write(STDOUT_FILENO, RESET, strlen(RESET));
				write(STDOUT_FILENO, &(ordered[i][pos + h.max_length + 2]), width - pos - (h.max_length + 2));
				write(STDOUT_FILENO, "\n", 1);
			}
			else
			{
				write(STDOUT_FILENO, ordered[i], width);
				write(STDOUT_FILENO, "\n", 1);
			}
		}

		for(unsigned i = 0; i < nbL; i++)
		{
			free(ordered[i]);
		}
		free(ordered);
	}
}

//the auto-completion
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

				closedir(dir);
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
		//fprintf(f, "%s\n", &(buf[*cur]));
		//fprintf(f, "%s\n", buf);
		//fprintf(f, "%s\n", buf);
		strncpy_buff( &(buf[*cur + strlen(h->liste->buf)]), &(buf[*cur]), *fin - *cur);
		//fprintf(f, "%s\n", buf);
		strncpy(&(buf[*cur]), h->liste->buf, strlen(h->liste->buf));
		//fprintf(f, "%s\n\n", buf);

		*cur = *cur + strlen(h->liste->buf);
		*fin = *fin + strlen(h->liste->buf);

		buf[*fin] = '\0';

		eraseLine(*cur, *fin, *prw);
		write(STDOUT_FILENO, buf, strlen(buf));
		moveC( *fin, *cur, *prw);
		supprList(h->liste);
		free(h);
		h = NULL;
	}
	else
	{
		moveC( *cur, *fin, *prw);
		write(STDOUT_FILENO, "\n", 1);

		bubbleSort(h->liste);
		ajoutDeb(&(h->liste), nom, strlen(nom));
		display(*h, -1);

		*prw = prompt();
		write(STDOUT_FILENO, buf, strlen(buf));
		moveC( *fin, *cur, *prw);
	}

	free(nom);
	return h;
}

//execute a command line
void execution(char* buf)
{
	//fprintf(f, "%s\n", buf);
	//fprintf(f, "\n");

	char *cpy =  NULL, **tmp1 = NULL, ***tmp2 = NULL, ****WOW = NULL;
	size_t tmp = 0, size1 = 0, *size2 = NULL, **size3 = NULL;

	cpy = malloc((strlen(buf) + 1) * sizeof(char));

	{
		size_t j = 0;
		for(size_t i = 0; i < strlen(buf); i++)
		{
			if(!(buf[i] == ' ' && (j == 0 || cpy[j-1] == ' ' || buf[i+1] == ' ' || buf[i+1] == '\0')))
			{
				cpy[j] = buf[i];
				j++;
			}
		}
		cpy[j] = '\0';

		// fprintf(f, "%s\n", cpy);
		// fprintf(f, "\n");

		j = 0;
		for(size_t i = 0; i < strlen(cpy); i++)
		{
			if(!(cpy[i] == ' ' && (cpy[i+1] == '|' || cpy[i+1] == '&' || cpy[i-1] == '|' || cpy[i-1] == '&')))
			{
				cpy[j] = cpy[i];
				j++;
			}
		}
		cpy[j] = '\0';

		// fprintf(f, "%s\n", cpy);
		// fprintf(f, "\n");
	}

	int background = 0;
	if(cpy[strlen(cpy) - 1] == '&')
	{
		background = 1;
		cpy[strlen(cpy) - 1] = '\0';
	}


	int err;
	regex_t preg;

	char mot[200], ensemble[200], str_regex[200];
	sprintf(mot, "[^&\\|\n\t\v ]+");
	sprintf(ensemble, "(%s[ ]{1})*%s", mot, mot);
	sprintf(str_regex, "^(%s([&]{2}|[\\|]{1}))*%s$", ensemble, ensemble);

	err = regcomp (&preg, str_regex, REG_EXTENDED);
	if (err == 0)
	{
		int match;
		regmatch_t matchs[1];

		match = regexec (&preg, cpy, 1, matchs, 0);
		regfree (&preg);

		if (match == REG_NOMATCH/* || matchs[0].rm_so != 0 || matchs[0].rm_eo != strlen(cpy)*/)
		{
			write(STDOUT_FILENO, "\nSyntax error", strlen("\nSyntax error"));
			return;
		}
	}





	

	size1 = 0;
	for(size_t i = 0; i < strlen(cpy) - 1; i++)
		if(!strncmp(&cpy[i], "&&", 2))
			size1++;
	size1++;
	//fprintf(f, "%ld\n", size1);
	//fflush(f);
	
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
		//fprintf(f, "%s\n", tmp1[tmp]);
		//fflush(f);
		
		tmp++;
	} while(tmp1[tmp - 1]);



	//fprintf(f, "\n");
	//fflush(f);


	for(size_t i = 0; i < size1; i++)
	{
		size2[i] = 0;
		for(size_t j = 0; j < strlen(tmp1[i]); j++)
			if(!strncmp(&(tmp1[i][j]), "|", 1))
				size2[i]++;
		size2[i]++;
		//fprintf(f, "%ld\n", size2[i]);
		//fflush(f);

		tmp2[i] = malloc((size2[i] + 1)*sizeof(char*));
		WOW[i] = malloc((size2[i] + 1)*sizeof(char**));

		size3[i] = malloc((size2[i] + 1)*sizeof(size_t*));

		tmp = 0;
		do {
			if(!tmp)
				tmp2[i][tmp] = strtok(tmp1[i], "|");
			else
				tmp2[i][tmp] = strtok(NULL, "|");
			//fprintf(f, "%s\n", tmp2[i][tmp]);
			//fflush(f);
			
			tmp++;
		} while(tmp2[i][tmp - 1]);
	}
	//fprintf(f, "\n");
	//fflush(f);


	for(size_t i = 0; i < size1; i++)
	{
		for(size_t j = 0; j < size2[i]; j++)
		{
			size3[i][j] = 0;
			for(size_t k = 0; k < strlen(tmp2[i][j]); k++)
				if(!strncmp(&tmp2[i][j][k], " ", 1))
					size3[i][j]++;
			size3[i][j]++;
			//fprintf(f, "%ld\n", size3[i][j]);
			//fflush(f);

			WOW[i][j] = malloc((size3[i][j] + 1)*sizeof(char*));

			tmp = 0;
			do {
				if(!tmp)
					WOW[i][j][tmp] = strtok(tmp2[i][j], " ");
				else
					WOW[i][j][tmp] = strtok(NULL, " ");
				//fprintf(f, "%s\n", WOW[i][j][tmp]);
				//fflush(f);
				
				tmp++;
			} while(WOW[i][j][tmp - 1]);
		}
	}
	//fprintf(f, "\n");
	//fflush(f);





	// TRAITEMENT


	/*
	int pp[2];
	pipe(pp);

	pid_t child1 = fork();

	if( child1 == 0) {
		// Replace stdout with the write end of the pipe
		dup2(pp[1], STDOUT_FILENO);
		// Close read end of the pipe
		close(pp[0]);
		close(pp[1]);
		// Run command
		char * argv1[] = {"printf","Salut Tout Le Monde ", NULL};
		execvp( argv1[0], argv);
	}
	else {
		pid_t child2 = fork();
	
		if(child2 == 0) {
			// Replace stdin with the read end of the pipe
			dup2(pp[0], STDIN_FILENO);
			// Close write end of the pipe
			close(pp[1]);
			close(pp[0]);
			// Run command
			char * argv2[] = {"wc","-c", " ", NULL};
			execvp( argv2[0], argv);
		}
		else {
			// Close both end of the pipe
			close(pp[0]);
			close(pp[1]);
			// wait for two child
			wait(NULL);
			wait(NULL);
		}
	}*/

	int nb_builtin;

	for(size_t i = 0; i < size1; i++)
	{
		int pp[size2[i] - 1][2];
		nb_builtin = 0;
		for(size_t j = 0; j < size2[i]; j++)
		{
			pipe(pp[j]);

			if(!specialExec(size3[i][j], WOW[i][j]))
			{
				int parent = fork();
				if(!parent)
				{
					if(j != 0)
					{
						close(pp[j-1][1]);
						dup2(pp[j-1][0], STDIN_FILENO);
					}
					if(j != size2[i] - 1)
					{
						close(pp[j][0]);
						dup2(pp[j][1], STDOUT_FILENO);
					}

					int k = execvp(WOW[i][j][0], WOW[i][j]);

					if(k == -1)
						perror(WOW[i][j][0]);
					exit(1);
				}
                                if(background)
				        ajoutDeb2(&processes, parent, WOW[i][j][0]);
			}
			else
				nb_builtin++;

			if(j > 0)
			{
				close(pp[j-1][0]);
				close(pp[j-1][1]);
			}
		}
		if(!background)
		{
			for(size_t j = 0; j < size2[i] - nb_builtin; j++)
				wait(NULL);
		}
	}





	free(cpy);

	//pas besoin de free le résultat de strtok, comme ca n alloue pas de mem
	for(size_t i = 0; i < size1; i++)
	{
		for(size_t j = 0; j < size2[i]; j++)
		{
			free(WOW[i][j]);
		}
		free(WOW[i]);
		free(tmp2[i]);
	}
        
	free(WOW);
	free(tmp2);
	free(tmp1);

	for(size_t i = 0; i < size1; i++)
	{
		free(size3[i]);
	}
	free(size3);

	free(size2);
}

int specialExec(int argc, char** argv)
{
	if(!strcmp(argv[0], "cd") && argc == 2)
	{
		DIR* dir = opendir(argv[1]);
		if(dir == NULL)
			perror(argv[1]);
		else
		{
			closedir(dir);
			chdir(argv[1]);
		}

		return 1;
	}
	else if(!strcmp(argv[0], "jobs"))
	{
		elem2* tmp = processes;
		char job[200];
		while(tmp != NULL)
		{
			sprintf(job, "%10d: %s\n", tmp->val, tmp->buf);
			write(STDOUT_FILENO, job, strlen(job));
			tmp = tmp->suiv;
		}

		return 1;
	}
        else if(!strcmp(argv[0], "exit"))
        {
            over = 1;
            return 1;
        }
	return 0;
}

//handle navigation into the result of the auto-completion
void selection( char c, char* buf, size_t* cur, size_t* fin, size_t* size, size_t* prw, historique* h, historique* search)
{
	unsigned w = width;
	char* nom = search->liste->buf;

	unsigned nbpL = w / (search->max_length + 2);
	unsigned nbL = ceil((double)search->cur/nbpL);

	size_t Sfin = nbL * width;

	unsigned finished = 0;
	unsigned selected = 0;


	moveC(*cur, *fin, *prw);
	eraseLine( *fin + *prw + 1, *fin + *prw + 1, 0);
	eraseLine( Sfin, Sfin, 0);
	display( *search, selected);
	*prw = prompt();
	write(STDOUT_FILENO, buf, strlen(buf));
	moveC(*fin, *cur, *prw);


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
				//*prw = prompt();
				//write(STDOUT_FILENO, nom, strlen(nom));
				break;
			}
			//new line : ctrl + J : selectionne
			case 10:
			{
				//moveC(*cur + *prw + width, width - 1, 0);
				//prompt();
				moveC(*cur, 0, *prw);

				elem* tmp = search->liste;
				for(unsigned i = 0; i <= selected; i++)
					tmp = tmp->suiv;

				strncpy_buff( &(buf[*cur + strlen(tmp->buf)]), &(buf[*cur]), *fin - *cur);
				strncpy( &(buf[*cur - strlen(nom)]), nom, strlen(nom));
				strncpy( &(buf[*cur]), tmp->buf, strlen(tmp->buf));

				*cur += strlen(tmp->buf);
				*fin += strlen(tmp->buf);

				buf[*fin] = '\0';

				write(STDOUT_FILENO, buf, strlen(buf));

				moveC(*fin, *cur, *prw);

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

							moveC(*cur, *fin, *prw);
							eraseLine( *fin + *prw + 1, *fin, 0);
							eraseLine( Sfin, Sfin, 0);
							display( *search, selected);

							*prw = prompt();
							write(STDOUT_FILENO, buf, strlen(buf));
							moveC(*fin, *cur, *prw);
							break;
						}
						case 'B': //bas
						{
							if((selected + 1) % nbL == 0 || selected + 1 == search->cur)
								break;
							selected++;

							moveC(*cur, *fin, *prw);
							eraseLine( *fin + *prw + 1, *fin, 0);
							eraseLine( Sfin, Sfin, 0);
							display( *search, selected);

							*prw = prompt();
							write(STDOUT_FILENO, buf, strlen(buf));
							moveC(*fin, *cur, *prw);
							break;
						}
						case 'C': //droite
						{
							if(selected + nbL >= search->cur)
								break;
							selected += nbL;

							moveC(*cur, *fin, *prw);
							eraseLine( *fin + *prw + 1, *fin, 0);
							eraseLine( Sfin, Sfin, 0);
							display( *search, selected);

							*prw = prompt();
							write(STDOUT_FILENO, buf, strlen(buf));
							moveC(*fin, *cur, *prw);
							break;
						}
						case 'D': //gauche
						{
							if(selected/nbL == 0)
								break;
							selected -= nbL;

							moveC(*cur, *fin, *prw);
							eraseLine( *fin + *prw + 1, *fin, 0);
							eraseLine( Sfin, Sfin, 0);
							display( *search, selected);

							*prw = prompt();
							write(STDOUT_FILENO, buf, strlen(buf));
							moveC(*fin, *cur, *prw);
							break;
						}
					}
				}
				break;
			}
		}
	}
}

//handle stdin input
void handle( char c, char* buf, size_t* cur, size_t* fin, size_t* size, size_t* prw, historique* h, historique* search)
{
	if(c >= 32 && c <= 126)
	{
		if(*cur == *fin)
		{
			write(STDOUT_FILENO, &c, 1);
			buf[*cur] = c;
			buf[*fin + 1] = '\0';

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
			strncpy_buff( &(buf[*cur + 1]), &(buf[*cur]), *fin - *cur + 1);
			buf[*cur] = c;
			buf[*fin + 1] = '\0';
						
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
						selection(c, buf, cur, fin, size, prw, h, search);

						supprList(search->liste);
						free(search);
						search = NULL;
					}
					else
					{
						supprList(search->liste);
						free(search);
						search = NULL;

						handle(c, buf, cur, fin, size, prw, h, search);
					}
				}
				break;
			}
			//new line : ctrl + J
			case 10:
			{
				write(STDOUT_FILENO, "\n", 1);

				buf[*fin] = '\0';
				h->cur = 0;
				if(*fin != 0)
				{
					ajoutDeb(&(h->liste), buf, *fin);
					execution(buf);
				}
                                
                                if(!over)
                                {
				    *prw = prompt();

				    (*cur) = (*fin) = 0;
				    write(STDOUT_FILENO, SAVEC, strlen(SAVEC));
                                }
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

				strncpy_buff( &(buf[*cur - 1]), &(buf[*cur]), *fin - *cur + 1);

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

							strncpy_buff( &(buf[*cur]), &(buf[*cur + 1]), *fin - *cur);

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

int main()
{
	//f = fopen("./log.txt", "a+");

	char *user = getenv("USER");
	char chemin[strlen(user) + 15];
	strcpy(chemin, "/home/");
	strcat(chemin, user);
	strcat(chemin, "/build-in");
	printf("%s\n", chemin);

	char *mypath = getenv("PATH");
	strcat(mypath, ":");
	strcat(mypath, chemin);

	intro();
	beep();

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

        elem2* tmp = processes;
        while(tmp)
        {
            
        }

        supprList(h.liste);
        supprList2(processes);
        free(buf);
	
        
        //fclose(f);



	tcsetattr(0, TCSANOW, &old);

	exit(0);
}
