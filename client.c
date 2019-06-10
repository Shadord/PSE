#include "ligne.h"
#include "pse.h"

void* internal_serveur(void* datas);

typedef struct {
  sem_t attente_serveur;
  struct sockaddr_in int_adresse;
} int_S;

int main(int argc, char const *argv[]) {
  printf("Client : Creating a Socket\n");
  int sock = socket(AF_INET, SOCK_STREAM, 0) ;
  if (sock < 0) {
    perror ("socket");
    exit (EXIT_FAILURE);
  }
  printf("Client : Socket Created\n");


  struct sockaddr_in* adresse_serveur = resolv(argv[1], argv[2]);

  int connection = connect(sock, (struct sockaddr*) adresse_serveur, sizeof(struct sockaddr_in));
  int running = 0;
  if(connection < 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  running = 1;
  // Création d'un thread serveur qui attendrai 1 connection : celle du client du serveur
  int_S S;
  sem_init(&S.attente_serveur, 0, 0);
  struct sockaddr_in adresse;
  adresse.sin_addr.s_addr = INADDR_ANY;
  adresse.sin_family = AF_INET;
  S.int_adresse = adresse;
  pthread_t idThread; // On crée le thread
  int ret = pthread_create (&idThread, NULL, internal_serveur, &S); // On le génère
  if(ret != 0) { // On check si c'est bon ?
    printf("Erreur dans la création du worker\n");
    erreur_IO ("pthread_create");
  }else{
    printf("Création du Serveur Interne\n");
  }

  freeResolv();
  printf("Client : Wait For Line Sent\n");
  sem_wait(&S.attente_serveur); // On bloque, des que le serveur interne sera pret on envois la commande de connection

  char co_line[100];
  strcat(co_line, "%S");
  strcat(co_line, stringIP(ntohl(S.int_adresse.sin_addr.s_addr)));
  strcat(co_line, "%");
  int someInt = ntohs(S.int_adresse.sin_port);
  char str[7];
  sprintf(str, "%d", someInt);
  strcat(co_line, str);
  printf("%s",co_line);
  if(ecrireLigne(sock, co_line)<0) {
    perror("ecrireLigne");
    exit(EXIT_FAILURE);
  }

  printf("Client : Connection Line Sent\n");
  printf("Game : Attente d'autres joueurs...\n");
  sem_wait(&S.attente_serveur); // On attend que le client_serveur ai envoyé un msg
  printf("Game : Tous les joueurs sont connectés !\n");
  printf("Game : Le jeu va démarrer !\n");
  usleep(1000000);
  system("clear");
  printf("--------------------------------------\n");
  printf("|            LET'S BEGIN             |\n");
  printf("--------------------------------------\n");
  printf("Game : Tri des cartes par le serveur...\n");
  sem_wait(&S.attente_serveur);

  while(running) {
    char buffer[160];
    if (fgets(buffer, LIGNE_MAX, stdin) == NULL) {
      perror("fgets");
      exit(EXIT_FAILURE);
    }

    if(ecrireLigne(sock, buffer)<0) {
      perror("ecrireLigne");
      exit(EXIT_FAILURE);
    }
    printf("\n");
  }

  close(sock);
  close(connection);

  exit(EXIT_SUCCESS);
}

void* internal_serveur(void* datas){
  int int_serveur_run = 0;
  int_S* S = (int_S*) datas; // Cast en un int_S*
  int sock = socket(AF_INET, SOCK_STREAM, 0) ;
  if (sock < 0) {
    perror ("socket");
    exit (EXIT_FAILURE);
  }else{
    printf("Internal Serveur : Socket Created !\n");
  }

  printf("Internal Serveur : Bind INADDR_ANY\n");
  int port = 2345;
  S->int_adresse.sin_port = htons((short)port); // port du serveur

  while(bind(sock, (struct sockaddr *) &S->int_adresse, sizeof(S->int_adresse)) < 0) {
    port++;
    S->int_adresse.sin_port = htons((short) atoi(port)); // port du serveur
  }
  printf("Internal Serveur : Bind Created on port %d!\n", port);


  printf("Internal Serveur : Listen connection\n");
  if(listen(sock, 1) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }else{
    printf("Internal Serveur : Listen Created !\n");
  }

  struct sockaddr_in adresse_client; // Creation de la reponse accept
  unsigned int adresse_client_lenght = sizeof(adresse_client); // On prend sa longueur
  sem_post(&S->attente_serveur);
  int accept_response = accept(sock, (struct sockaddr*) &adresse_client, &adresse_client_lenght); // On attend sa reponse
  if(accept_response < 0) { // Si erreur on quitte
    perror("accept");
    exit(EXIT_FAILURE);
  }else{
    int_serveur_run = 1;
  }
  // La connection est arrivée
  printf("Internal Serveur : Connection entrante %s sur le port %hu\n", stringIP(ntohl(adresse_client.sin_addr.s_addr)), ntohs(adresse_client.sin_port));

  while(int_serveur_run) {
    char buffer[160];
    int result = lireLigne(accept_response, buffer);
    if(result != -1 && result != 0) {
      if(buffer[0] == '%'){ // il s'agit d'une commande client, on recoit l'ip et le port
        if(buffer[1] == 'J') { // Tout les joueurs sont prets
          sem_post(&S->attente_serveur);
        }
        if(buffer[1] == 'T') { // Toutes les cartes sont pretes
          sem_post(&S->attente_serveur);
        }
        if(buffer[1] == 'C') { // On recoit son paquet
          int nbr_cartes;
          sem_post(&S->attente_serveur);
        }

      }
      if(strcmp(buffer, "fin") == 0) { // On demande la fin du serveur
        printf("External Serveur. Arret demandé.\n");
        int_serveur_run = 0;
      }
      printf("External Serveur. %d octets -> %s\n", result, buffer);
    }
  }
}
