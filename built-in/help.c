#include <stdio.h>

int main(int argc, char const *argv[])
{
	printf("Bonsoir,\nsi vous optenez ce message sur votre écran, c'est que vous étes perdu.\n\nRassurez vous, ");
	printf("je suis la pour vous !\n\nRassurez ce magnifique et formidable shell, nommé PASH, est très sembable à ");
	printf("votre bien aimé BASH !\n\nCependant, quelques features toutes plus délicieuses les unes que les autres ");
	printf("vous attendent et je vais, ici et mainenant, vous les présenter :\n\n");
	printf("-> Auto complétion avec <Tab> s'il n'y a qu'une seule possibilité, sinon affichage des possibilités\n");
	printf("-> Appuyer une 2ème fois sur <Tab> vous permet de naviguer au travers des différentes possibilités avec les flèches\n");
	printf("-> Quelques petites commandes bonus :\n   - highfive\n   - Date\n   - Ping\n");
	printf("-> Vous pouvez rejouer la musique d'introduction a tout moment avec <beep> et même le message d'introduction avec <intro>\n");
	printf("-> Un <make bigclean> supprimera tout les executables créés par par le <make>\n\n");
	printf("WARNING :\n");
	printf("La navigation intuitive requière les caractères suivant :\n    </>\n    <~/>\n    <./>\nAutrement, Pash se chargera ");
	printf("de rechercher dans les dossiers énoncés par la variable : <PATH>\n\n");
	printf("-> La gestion des arguments d'entrée est basée sur les caractères <espace>, par exemple un 'grep \"hello\"' recherchera ");
	printf("littéralement <\"hello\"> et un 'grep hello world' recherchera <hello> et rencontrera une erreur sur l'argument <world> inconnu au bataillon\n\n");
	
	return 0;
}