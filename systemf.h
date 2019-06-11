#ifndef SYSTEMF_H
#define SYSTEMF_H

#include "pse.h"

void welcome();

void begin();

void listen_(int sock, char side[], int nbr_co);

int socket_(char side[]);

int bindF(int sock, char port[], struct sockaddr_in* adresse, char side[]);



#endif
