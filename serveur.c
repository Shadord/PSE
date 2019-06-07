#include "ligne.h"
#include "pse.h"



int initLog(int oldJournal);
int createPool(int journal, int nbr);
int findWorker(int canal);
void * dialog(void * datas); // Notre fabuleuse fonction de thread Data est un Datathread qui cible le thread
void setAllToFree();


int arretServeur = 1;
sem_t sem;

int createPool(int journal, int nbr) {
  initDataThread(); // Genère une liste de datathread
  int counter = 0;
  for (int worker = 0; worker < nbr; worker++) {

    DataThread* D = ajouterDataThread();// On crée un Statthread qui contient le DataSpec du nouveau thread
    D->spec.libre = VRAI; // Est-ce qu'il est libre ? VRAI par défaut car pas de connections faites
    D->spec.canal = -1; // -1 Car aucune réponse n'a été accepté d'un client mdr
    D->spec.tid = journal; // On link quand meme le journal pour être sur que le client va écrire dedans
    sem_t sem_reveil;
    sem_init(&sem_reveil, 0, 0);
    D->spec.sem = sem_reveil;
    //Creation du thread
    pthread_t idThread; // On crée le thread
    int ret = pthread_create (&idThread, NULL, dialog, D); // On le génère
    if (ret != 0) { // On check si c'est bon ?
      printf("Erreur dans la création du worker\n");
      erreur_IO ("pthread_create");
    }else{
      printf("Création du worker %d\n", ret);
      counter++;
    }
    D->spec.id = idThread; // On place dans D notre id de thread pour pouvoir le suivre
  }
  return counter;

}

int findWorker(int canal) { // Cette fonction parcourt le datathread pour savoir si des cannaux sont libres ou non
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

int initLog(int oldJournal) { // Notre belle fonction d'reset journal
  close(oldJournal);
  int newJournal = open("journal.log", O_WRONLY | O_TRUNC);
  if(oldJournal == -1) {
    erreur_IO("erreur de ouverture");
  }
  return newJournal;
}

void * dialog(void * datas) {
  DataThread* donnees = (DataThread*) datas;
  while(1) { // On boucle tant qu'on attend une reponse
    if(donnees->spec.canal == -1){
      sem_wait(&donnees->spec.sem);
      printf("Worker unlocked\n");
    }




    char buffer[160];
    int result = lireLigne(donnees->spec.canal, buffer);
    if(result != -1 && result != 0) {
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
      }
      if(strcmp(buffer, "init") == 0) { // On demande la fin du serveur
        donnees->spec.tid = initLog(donnees->spec.tid);
        //printf("%d", journal);
        if(donnees->spec.tid != -1) {
          printf("Serveur. Reset journal.log.\n");
        }else{
          donnees->spec.libre = VRAI;
          donnees->spec.canal = -1;
          break;
        }

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

  if(createPool(journal, 5) == 5) {
    printf("Serveur : Création de la pool effectuée !\n");
  }else{
    printf("Serveur : Erreur dans la création de la pool...\n");
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
  if(listen(sock, 10) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }else{
    printf("Serveur : Listen Created !\n");
  }


  printf("Serveur : Mutex create\n");
  if(sem_init(&sem, 0, 5) < 0) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }else{
    printf("Serveur : Mutex Created !\n");
  }

  printf("Serveur : Try to accept...\n");
  while(arretServeur){
    struct sockaddr_in adresse_client; // Creation de la reponse accept
    unsigned int adresse_client_lenght = sizeof(adresse_client); // On prend sa longueur
    int accept_response = accept(sock, (struct sockaddr*) &adresse_client, &adresse_client_lenght); // On attend sa reponse
    if(accept_response < 0) { // Si erreur on quitte
      perror("accept");
      exit(EXIT_FAILURE);
    }

    // La connection est arrivée
    printf("Serveur : Connection entrante %s sur le port %hu\n", stringIP(ntohl(adresse_client.sin_addr.s_addr)), ntohs(adresse_client.sin_port));

    // On l'affecte a un worker si possible
    sem_wait(&sem);
    printf("Worker dispo\n");
    //Worker trouvé
    findWorker(accept_response);

  }
  // Le serveur va être arreter
  setAllToFree();
  close (sock);
  close(journal);
  exit(EXIT_SUCCESS);
}
