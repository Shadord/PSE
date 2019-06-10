#include "ligne.h"
#include "pse.h"
#define NBR_JOUEURS 2

int createPool(int nbr);
int findJoueur(int canal);
void * dialog(void * datas); // Notre fabuleuse fonction de thread Data est un Datathread qui cible le thread
void setAllToFree();
void* internal_client(void* datas);

typedef struct {
  sem_t ecriture;
  sem_t lecture;
  struct sockaddr_in int_adresse;
} int_C;

int arretServeur = 1;
sem_t sem;

int createPool(int nbr) {
  initDataThread(); // Genère une liste de datathread
  int counter = 0;
  for (int joueur = 0; joueur < nbr; joueur++) {

    DataThread* D = ajouterDataThread();// On crée un Statthread qui contient le DataSpec du nouveau thread
    D->spec.libre = VRAI; // Est-ce qu'il est libre ? VRAI par défaut car pas de connections faites
    D->spec.canal = -1; // -1 Car aucune réponse n'a été accepté d'un client mdr
    sem_t sem_reveil;
    sem_init(&sem_reveil, 0, 0);
    D->spec.sem = sem_reveil;
    //Creation du thread
    pthread_t idThread; // On crée le thread
    int ret = pthread_create (&idThread, NULL, dialog, D); // On le génère
    if (ret != 0) { // On check si c'est bon ?
      printf("Erreur dans la création du joueur\n");
      erreur_IO ("pthread_create");
    }else{
      printf("Création du joueur %d\n", ret);
      counter++;
    }
    D->spec.id = idThread; // On place dans D notre id de thread pour pouvoir le suivre
  }
  return counter;

}

int findJoueur(int canal) { // Cette fonction parcourt le datathread pour savoir si des cannaux sont libres ou non
  //printf("Recherche de worker en cours... \n");
  DataThread* D = listeDataThread;
  while(D != NULL) {
    if(D->spec.canal == -1) { // Il est libre !! donc on lui affecte un client ;)
      D->spec.canal = canal;
      D->spec.libre = FAUX;
      sem_post(&D->spec.sem);
      return D->spec.id; // Thread de libre donc on renvoit son id
    }
    D = D->next; // Libre donc on passe au suivant
  }
  //printf("    Pas de worker dispo... \n");
  return -1; // Aucun thread de libre trouvé
}


void * dialog(void * datas) {
  DataThread* donnees = (DataThread*) datas;
  while(1) { // On boucle tant qu'on attend une reponse
    if(donnees->spec.canal == -1){
      sem_wait(&donnees->spec.sem);
      printf("Joueur connecté\n");
    }

    char buffer[160];
    int result = lireLigne(donnees->spec.canal, buffer); // On attend les instructions
    if(result != -1 && result != 0) {
      if(buffer[0] == '%'){ // il s'agit d'une commande client, on recoit l'ip et le port
        if(buffer[1] == 'S') { // S pour start IP - PORT serveur interne client
          int indice_buff = 2;
          char IP[15];
          char PORT[5];
          int indice_IP = 0;
          while(buffer[indice_buff] != '%') {
            IP[indice_IP] = buffer[indice_buff];
            indice_IP++;
            indice_buff++;
          }
          indice_buff++;
          indice_IP = 0;
          while(buffer[indice_buff]) {
            PORT[indice_IP] = buffer[indice_buff];
            indice_IP++;
            indice_buff++;
          }

          int_C C;
          sem_init(&C.ecriture, 0,0);
          sem_init(&C.lecture, 0,0);
          struct sockaddr_in* int_adresse = resolv(IP, PORT);
          C.int_adresse = *int_adresse;
          pthread_t idThread; // On crée le thread
          int ret = pthread_create (&idThread, NULL, internal_client, &C); // On le génère
          if(ret != 0) { // On check si c'est bon ?
            printf("Erreur dans la création du joueur\n");
            erreur_IO ("pthread_create");
          }else{
            printf("Création du client interne pour le joueur\n");
          }
        }
      }
      if(strcmp(buffer, "fin") == 0) { // On demande la fin du serveur
        printf("Client. Arret demande.\n");
        donnees->spec.libre = VRAI;
        donnees->spec.canal = -1;
        sem_post(&sem);
      }
      if(strcmp(buffer, "exit") == 0) { // On demande la fin du serveur
        printf("Serveur. Arret demande.\n");
        donnees->spec.libre = VRAI;
        donnees->spec.canal = -1;
        arretServeur = 0;
      }else{
        printf("Serveur. %d octets -> journal.log : %s\n", result, buffer);
        ecrireLigne(donnees->spec.tid, buffer);
      }
    }

  }
  close(donnees->spec.canal);
  pthread_exit(NULL);
}

void setAllToFree(){
  DataThread* D = listeDataThread;
  while(D != NULL) {
    D->spec.libre = VRAI;
    D->spec.canal = -1;
  }
  int ret = joinDataThread();
  if(ret != 0) {
    printf("All Workers Closed\n");
  }
}

int main(int argc, char const *argv[]) {
  int journal = open("journal.log", O_WRONLY); //Création du journal !
  lseek(journal, 0, SEEK_END);


  printf("--------------------------------------\n");
  printf("|        WELCOME TO THE GAME         |\n");
  printf("--------------------------------------\n");
  usleep(1000000); // 1 seconde d'attente

  if(createPool(NBR_JOUEURS) == NBR_JOUEURS) {
    printf("GAME : Création du jeu effectué !\n");
  }else{
    printf("GAME : Erreur dans la création du jeu...\n");
    exit(EXIT_FAILURE);
  }

  printf("Serveur : Creating a Socket\n");
  int sock = socket(AF_INET, SOCK_STREAM, 0) ;
  if (sock < 0) {
    perror ("socket");
    exit (EXIT_FAILURE);
  }else{
    printf("Serveur : Socket Created !\n");
  }

  printf("Serveur : Bind INADDR_ANY\n");
  struct sockaddr_in adresse;
  adresse.sin_addr.s_addr = INADDR_ANY;
  adresse.sin_family = AF_INET;
  adresse.sin_port = htons((short) atoi(argv[1]));

  if (bind(sock, (struct sockaddr *)&adresse, sizeof(adresse)) < 0) {
    perror ("bind");
    exit (EXIT_FAILURE);
  }else{
    printf("Serveur : Bind Created !\n");
  }

  printf("Serveur : Listen connection\n");
  if(listen(sock, NBR_JOUEURS +1) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }else{
    printf("Serveur : Listen Created !\n");
  }


  printf("Serveur : Mutex create\n");
  if(sem_init(&sem, 0, NBR_JOUEURS) < 0) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }else{
    printf("Serveur : Mutex Created !\n");
  }

  printf("Serveur : Attente de connections...\n");
  int wait_player = NBR_JOUEURS;
  while(wait_player){

    struct sockaddr_in adresse_client; // Creation de la reponse accept
    unsigned int adresse_client_lenght = sizeof(adresse_client); // On prend sa longueur
    int accept_response = accept(sock, (struct sockaddr*) &adresse_client, &adresse_client_lenght); // On attend sa reponse
    if(accept_response < 0) { // Si erreur on quitte
      perror("accept");
      exit(EXIT_FAILURE);
    }

    // La connection est arrivée
    printf("Serveur : Nouveau joueur IP: %s -- PORT: %hu\n", stringIP(ntohl(adresse_client.sin_addr.s_addr)), ntohs(adresse_client.sin_port));

    // On l'affecte a un thread si possible
    sem_wait(&sem);
    printf("Serveur : Thread disponible\n");
    //thread trouvé
    findJoueur(accept_response);
    wait_player--;

  }
  printf("Serveur : Tous les joueurs sont connectés, le jeux va commencer !\n");
  usleep(1000000); // 1 seconde d'attente
  system("clear");
  printf("--------------------------------------\n");
  printf("|            LET'S BEGIN             |\n");
  printf("--------------------------------------\n");
  usleep(1000000); // 1 seconde d'attente

  // On a tous les joueurs
  int game = 1;
  while(game) {

  }
  // Le serveur va être arreter
  setAllToFree();
  close (sock);
  close(journal);
  exit(EXIT_SUCCESS);
}



void* internal_client(void* datas){
  int_C* donnees = (int_C*) datas; // Cast en un int_S*
  printf("Internal Client : Creating a Socket\n");
  int sock = socket(AF_INET, SOCK_STREAM, 0) ;
  if (sock < 0) {
    perror ("socket");
    exit (EXIT_FAILURE);
  }
  printf("Internal Client : Socket Created\n");

  struct sockaddr_in* adresse_serveur = &donnees->int_adresse;

  int connection = connect(sock, (struct sockaddr*) adresse_serveur, sizeof(struct sockaddr_in));
  int running = 0;
  if(connection < 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  running = 1;
  // Création d'un thread serveur qui attendrai 1 connection : celle du client du serveur


  freeResolv();

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
