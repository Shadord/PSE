#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*Affiche le dessin du pendu en fonction du nombre d'échec*/
void affichage(int nb_echec)
	{
	printf("\nEtat pendu :\n\n");
	switch(nb_echec)
		{
		case 0:
			break;
		case 1:
			printf("	|\n	|\n	|\n	|\n	|\n");
			break;
		case 2:
			printf("	|\n	|\n	|\n	|\n      __|__\n");
			break;
		case 3:
			printf("	__________\n");
			printf("	|\n	|\n	|\n	|\n      __|__\n");
			break;
		case 4:
			printf("	__________\n");
			printf("	|/\n	|\n	|\n	|\n      __|__\n");
			break;
		case 5:
			printf("	__________\n");
			printf("	|/	 |\n	|\n	|\n	|\n      __|__\n");
			break;	
		case 6:
			printf("	__________\n");
			printf("	|/	 |\n	|	 O\n	|\n	|\n      __|__\n");
			break;
		case 7:
			printf("	__________\n");
			printf("	|/	 |\n	|	 O\n	|	 |\n	|\n      __|__\n");
			break;
		case 8:
			printf("	__________\n");
			printf("	|/	 |\n	|	_O\n	|	 |\n	|\n      __|__\n");
			break;
		case 9:
			printf("	__________\n");
			printf("	|/	 |\n	|	_O_\n	|	 |\n	|\n      __|__\n");
			break;
		case 10:
			printf("	__________\n");
			printf("	|/	 |\n	|	_O_\n	|	 |\n	|	 /\n      __|__\n");
			break;
		case 11:
			printf("	__________\n");
			printf("	|/	 |\n	|	_O_\n	|	 |\n	|	 /\\ \n      __|__\n");
			printf("\nPERDU\n");
			break;
		default:
			printf("commande incorrecte\n");
			break;

		}
	printf("\n");
	}

int main(void)
	{
	affichage(1);
	affichage(2);
	affichage(3);
	affichage(4);
	affichage(5);
	affichage(6);
	affichage(7);
	affichage(8);
	affichage(9);
	affichage(10);
	affichage(11);
	return 0;
	}
