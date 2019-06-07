




Carte* draw_card(Paquet* P) {
  int set = 1;
  while(set)
    double nombre  = (rand()/(double)RAND_MAX)*NB_CARTES; // Entre 0 et 1
    int card_value = round(nombre);
    set = isset_CinP(P, card_value, card_familly)

}
