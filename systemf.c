#include "systemf.h"

void welcome(){
  system("clear");
  printf("--------------------------------------\n");
  printf("|        WELCOME TO THE GAME         |\n");
  printf("--------------------------------------\n");
  usleep(1000000); // 1 seconde d'attente
}

void begin() {
  system("clear");
  printf("--------------------------------------\n");
  printf("|            LET'S BEGIN             |\n");
  printf("--------------------------------------\n");
  usleep(1000000); // 1 seconde d'attente
}

void listen_(int sock, char side[], int nbr_co){
  printf("%s : Listen connection\n", side);
  if(listen(sock, nbr_co +1) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }else{
    printf("%s : Listen Created !\n", side);
  }
}

int socket_(char side[]) {
  printf("%s : Creating a Socket\n", side);
  int sock = socket(AF_INET, SOCK_STREAM, 0) ;
  if (sock < 0) {
    perror ("socket");
    exit (EXIT_FAILURE);
  }else{
    printf("%s : Socket Created !\n", side);
    return sock;
  }
}

int bindF(int sock, char port[], struct sockaddr_in* adresse, char side[]) {
  int port_ = atoi(port);
  printf("%s : Bind INADDR_ANY\n", side);
  adresse->sin_addr.s_addr = INADDR_ANY;
  adresse->sin_family = AF_INET;

  if(port_ == -1){
    int port = 2345;
    adresse->sin_port = htons((short) port);

    while(bind(sock, (struct sockaddr *) adresse, sizeof(*adresse)) < 0) {
      port++;
      adresse->sin_port = htons((short) port); // port du serveur
    }


  }else{
    adresse->sin_port = htons((short) atoi(port));
    if (bind(sock, (struct sockaddr *) adresse, sizeof(*adresse)) < 0) {
      perror ("bind");
      exit (EXIT_FAILURE);
    }else{
      printf("%s : Bind Created !\n", side);
    }
  }

}
