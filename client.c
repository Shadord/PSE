#include "ligne.h"
#include "pse.h"

void* internal_serveur(void* datas);

struct {
  int inside; // Un message est arrivé
  int outside; // Un message doit être envoyé
  char inside_c[50]; // Le message qui est arrivé est stocké ici
  char outside_c[50]; // Le message qui doit être envoyé est stocké la
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
    printf("Création du worker %d\n", ret);
  }


  freeResolv();

  while(running) {
    char buffer[160];
    printf("ligne> ");
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
  int_S* donnees = (int_S*) datas; // Cast en un int_S*
  int sock = socket(AF_INET, SOCK_STREAM, 0) ;
  if (sock < 0) {
    perror ("socket");
    exit (EXIT_FAILURE);
  }else{
    printf("Internal Serveur : Socket Created !\n");
  }

  printf("Internal erveur : Bind INADDR_ANY\n");
  int port = 2345;
  donnees->int_adresse.sin_port = htons((short) atoi(port)); // port du serveur

  while(bind(sock, (struct sockaddr *)&donnees->int_adresse, sizeof(donnees->int_adresse)) < 0) {
    port++;
    donnees->int_adresse.sin_port = htons((short) atoi(port)); // port du serveur
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
      if(strcmp(buffer, "fin") == 0) { // On demande la fin du serveur
        printf("External Serveur. Arret demandé.\n");
        int_serveur_run = 0;
      }
      printf("External Serveur. %d octets -> %s\n", result, buffer);
    }
  }
}
