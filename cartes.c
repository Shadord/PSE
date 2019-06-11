#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define NB_TAB 220

/*NE PAS OUBLIER DE RAJOUTER -lm POUR COMPILER !!*/

/*Structures (partie à retirer ensuite car déjà dans le programme de Nathan*/
typedef struct _Carte
	{
	int status; // 1 si la carte est prise, 0 sinon
	int value; // numero de la carte, de 11 à 13 pour les figures
	int famille; // 0 pic, 1 trèfle, 2 carreau, 3 coeur
	struct _Carte *next;
	} Carte;

typedef struct
	{
	int size; // taille du paquet de cartes
	Carte *first;
	Carte *alast;
	Carte *last;
	} Paquet;

typedef struct
	{
	int size;
	Carte **liste_cartes;
	} Main;

typedef struct
	{
	int nb_points;
	Main *Main;
	} Joueur;

/*Fonctions*/

/*empile une carte C dans un paquet de cartes P*/
void empiler(Paquet *P, Carte *C)
	{
	if (P->first==NULL)
		{
		P->first = C;
		P->last = C;
		}
	else
		{
		P->alast = P->last;
		P->last->next=C;
		P->last=C;
		}
	}


/*Creation d'un paquet de cartes bien mélangé
La fonction retourne le paquet bien mélangé*/
void paquet_bien_range(Paquet *P)
	{
	P->size = 52;
	for (int i=0; i<P->size; i++)
		{
		Carte *C = malloc(sizeof(Carte));
		C->status=0;
		if (i<13) // Pique
			{
			C->value = i+1;
			C->famille=0;
			}
		else if (i>=13 && i<26) // Trèfle
			{
			C->value = i-12;
			C->famille=1;
			}
		else if (i>=26 && i<39) // Carreau
			{
			C->value = i-25;
			C->famille=2;
			}
		else // Coeur
			{
			C->value = i-38;
			C->famille=3;
			}
		empiler(P, C);
		}
	}

/*Affiche le paquet actuel*/
void afficher_paquet(Paquet *P)
	{
	printf("Jeu :\n");
	Carte *C = P->first;
	while (C!=NULL)
		{
		printf("famille %d n°%d\n", C->famille, C->value);
		C=C->next;
		}
	printf("\n\n");
	}

/*Mélanger le paquet de cartes
Il faut mettre en argument le paquet de carte bien trié P et le nouveau paquet de cartes Q*/
Paquet* melange_paquet(Paquet *P, Paquet *Q)
	{
	//Paquet *Q = malloc(sizeof(Paquet*)); // Q correspond au paquet P mélangé
	Q->size=P->size;
	double alea;
	int nb_alea;
	Carte *Cartei;
	Carte *Carteprec;
	for (int i=0; i<Q->size; i++)
		{
		alea=(P->size - i -1)*(double)rand()/RAND_MAX; // nombre aléatoire entre 0 et 52 cartes - i pour retirer une carte de P au hasard
		nb_alea = (int)round(alea);// position de la carte tirée au hasard dans P
		Cartei=P->first;
		//afficher_paquet(Q);
		for (int j=0; j<nb_alea+1; j++)
			{
			if (j==nb_alea)
				{
				if (nb_alea==0) // première carte à retirer de P
					{
					empiler(Q,Cartei);
					P->first=Cartei->next;
					Cartei->next=NULL;
					}
				else if (nb_alea == 52 - i) // dernière carte à retirer de P
					{
					P->last=P->alast;
					empiler(Q, Cartei);
					}
				else
					{
					Carteprec->next=Cartei->next;// retirer la carte du paquet P
					Cartei->next=NULL;
					empiler(Q, Cartei);
					}
				 // On met la carte tirée aléatoirement dans le paquet Q
				}
			else
				{
				Carteprec=Cartei;
				Cartei=Cartei->next;
				}
			}
		}
	//afficher_paquet(Q);
	}

/*Séparation du paquet de cartes en 2 et par suite création de deux mains*/
void couper_paquet_en_deux(Paquet *P, Main *Main1, Main *Main2)
	{
	Main1->size = ((P->size)/2);
	Main2->size = P->size - ((P->size) /2);
	Main1->liste_cartes=(Carte**)malloc((P->size)*sizeof(Carte*));
	Main2->liste_cartes=(Carte**)malloc((P->size)*sizeof(Carte*));

	Carte *courant = P->first;
	for (int i=0; i<P->size; i++)
		{
		if (i<Main1->size)
			{
			Main1->liste_cartes[i]=courant;
			}
		else
			{
			Main2->liste_cartes[i-(Main1->size)]=courant;
			}
		courant = courant ->next;
		}
	}

void afficher_main(Main *M)
	{
	printf("Affichage Main :\n");
	for (int i=0; i<52; i++)
		{
		Carte *C = M->liste_cartes[i];
		if(i < 10)
			printf("0%d ", i);
		else
			printf("%d ", i);
		switch(C->famille){
			case 0 :
				if(C->value < 10)
					printf("\033[30;47m0%d ♠\033[0m\n", C->value);
				else
					printf("\033[30;47m%d ♠\033[0m\n", C->value);
				break;

			case 1 :
				if(C->value < 10)
					printf("\033[30;47m0%d ♣\033[0m\n", C->value);
				else
					printf("\033[30;47m%d ♣\033[0m\n", C->value);
				break;

			case 2 :
				if(C->value < 10)
					printf("\033[31;47m0%d ♦\033[0m\n", C->value);
				else
					printf("\033[31;47m%d ♦\033[0m\n", C->value);
				break;

			case 3 :
				if(C->value < 10)
					printf("\033[31;47m0%d ♥\033[0m\n", C->value);
				else
					printf("\033[31;47m%d ♥\033[0m\n", C->value);
				break;
		}
		}
	printf("\n\n");
	}

int decaler(Main *M)
	{
	Carte **liste_cartes=(Carte**)malloc(52*sizeof(Carte*));
	int indice = 0;
	for (int j=0; j<52; j++) // Le tableau liste_cartes est identique à la main à la différence près qu'il ne comprend pas de "trou".
		{
		if (M->liste_cartes[j]!=NULL)
			{
			liste_cartes[indice]=M->liste_cartes[j];
			indice++;
			}
		} 
	free(M->liste_cartes);
	M->liste_cartes = liste_cartes;
	return indice;
	}

void retirer_carte_main(Carte *C, Main *M)
	{
	int i = 0;
	int trouve = 0;
	while(M->liste_cartes[i] == NULL){
		i++;
	}
	Carte *courant = M->liste_cartes[i];
	while (trouve == 0 && i<52)
		{
		if (courant->value == C->value)
			{
			trouve = 1;
			break;
			}
		else
			{
			i++;
			while(M->liste_cartes[i] == NULL){
				i++;
			}

			courant = M->liste_cartes[i];
			}
		}
	if (trouve == 1) // La carte que l'on souhaite retirer se trouve bien dans la main
		{
		M->liste_cartes[i]=NULL; // On retire C de la main
		M->size--;
		//decaler(M);
		}
	if (i>=52)
		{
		printf("La carte que vous souhaitez retirer ne se trouve pas dans la main.\n");
		}
	}

void ajouter_carte_main(Carte *C, Main *M)
	{
	int indice;
	indice = decaler(M);
	M->liste_cartes[indice]=C;
	M->size++;
	}


void comparer_cartes(int indice_carte_1, int indice_carte_2, Joueur *J1, Joueur *J2)	
	{
	Carte *C1 = J1->Main->liste_cartes[indice_carte_1];
	Carte *C2 = J2->Main->liste_cartes[indice_carte_2];
	retirer_carte_main(C1, J1->Main); // On retire les deux cartes choisies des paquets respectifs des joueurs
	retirer_carte_main(C2, J2->Main);
	printf("1\n");
	if (C1->value==C2->value) // Les deux cartes jouées ont même valeur
		{
		printf("6\n");
		printf("Les cartes ont même valeur.\n");
		ajouter_carte_main(C1, J1->Main); // On les remet chacune dans les paquets des joueurs
		printf("9\n");
		ajouter_carte_main(C2, J2->Main);
		printf("2\n");
		}
	else if (C1->value>C2->value || C1->value == 1) // J1 gagne
		{
		printf("10\n");
		decaler(J2->Main);
		printf("Joueur 1 gagne.\n");
		J1->nb_points++; // J1 gagne 1 point
		printf("11\n");
		ajouter_carte_main(C1, J1->Main); // J1 récupère sa carte
		printf("12\n");
		ajouter_carte_main(C2, J1->Main); // J1 récupère la carte de J2 également
		printf("3\n");
		}
	else if (C1->value<C2->value || C2->value ==1)
		{
		printf("13\n");
		decaler(J1->Main);
		printf("Joueur 2 gagne.\n");
		J2->nb_points++; // J2 gagne 1 point
		printf("14\n");
		ajouter_carte_main(C1, J2->Main); // J2 récupère la carte de J1
		printf("15\n");
		ajouter_carte_main(C2, J2->Main); // J2 récupère sa carte
		printf("4\n");
		}
	printf("Taille Main 1 : %d\n", J1->Main->size);
	printf("Taille Main 2 : %d\n", J2->Main->size);
	printf("5\n");
	}

void coder_main(Main *M, char code[])
	{
	char nb_cartes[2];
	char carte[5];

	code[0]='%';
	code[1]='C';

	sprintf(nb_cartes, "%d", M->size);
	strcat(code, nb_cartes);

	Carte *courant;

	for (int i=0; i<M->size; i++)
		{
		courant = M->liste_cartes[i];
		if (courant->value<10)
			{
			sprintf(carte, "0%d%d", courant->value, courant->famille);
			strcat(code, carte);
			}
		else
			{
			sprintf(carte, "%d%d", courant->value, courant->famille);
			strcat(code, carte);
			}
		}
	printf("%s\n", code);
	}




/*Main*/

int main(void)
	{
	srand(time(NULL));
	Paquet Paquet_Cartes = {0, NULL, NULL};
	Paquet nouvPaquet_Cartes = {0, NULL, NULL};
	Main Main1 = {0, NULL};
	Main Main2 = {0, NULL};
	Joueur Joueur1 = {0, &Main1};
	Joueur Joueur2 = {0, &Main2};

	paquet_bien_range(&Paquet_Cartes);
	printf("Affichage paquet de cartes bien rangé : \n");
	afficher_paquet(&Paquet_Cartes);

	printf("\n\nAffichage paquet de cartes mélangé : \n");
	melange_paquet(&Paquet_Cartes, &nouvPaquet_Cartes);
	Paquet_Cartes = nouvPaquet_Cartes;
	afficher_paquet(&Paquet_Cartes);

	couper_paquet_en_deux(&Paquet_Cartes, &Main1, &Main2);
	printf("Main 1 :\n");
	afficher_main(&Main1);
	printf("Main 2 :\n");
	afficher_main(&Main2);
	for(int i = 0; i<4; i++){
		printf("\n\nTOUR %d\n", i);
		comparer_cartes(0, 0, &Joueur1, &Joueur2);
		printf("Main 1 %d:\n", Joueur1.nb_points);
		afficher_main(&Main1);
		printf("Main 2 %d:\n", Joueur2.nb_points);
		afficher_main(&Main2);
	}

	char code[NB_TAB];
	//coder_main(&Main1, code);
	
	return 0;
	}
