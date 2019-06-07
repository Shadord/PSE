#ifndef POKER_H
#define POKER_H


#define NB_CARTES = 56;

struct {
  int status; // 0 -> Non prise; 1 -> Prise
  int value;
  // 0 -> Joker
  // 11 -> Valet
  // 12 -> Dame
  // 13 -> roi
  int familly;
  // 0 -> Pique
  // 1 -> Trèfle
  // 2 -> Carreau
  // 3 -> Coeur
} Carte;

struct {
  int size; // Nombre de carte restantes
  Carte** liste_cartes;// Tableau des cartes
} Paquet;

struct {
  int size;
  Carte** liste_cartes; // Tableau qui contient les cartes du joueur
} Main;


void isset_CinP(Paquet* P, Carte* C);
Carte* draw_card(Paquet* P);






#endif
