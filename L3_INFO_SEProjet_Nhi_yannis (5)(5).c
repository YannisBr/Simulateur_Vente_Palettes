#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sys/wait.h>

typedef struct {


  int fd1[2];
  int fd0[2];
  int fd2[2];
  int fd3[2];
  int fd4[2];
  char *nom_article;
  char *nom_acheteur;
  char *nom_transporteur;
  char *nom_serveur;
  int quantite_stock;
  int surface_m2;
  int prix_pallet;
} pro_data_t;

int rand_count = 0;
int random_between(int a, int b) {
  srand(time(NULL) + rand_count);
  rand_count++;
  return a + (rand() % (b + 1 - a));
}

void initilize_pro_data(pro_data_t *data) {
  pipe(data->fd1);
  pipe(data->fd0);
  pipe(data->fd2);
  pipe(data->fd3);
  pipe(data->fd4);
  data->nom_article = random_between(0, 1) ? "creux" : "plein";
  data->nom_serveur = random_between(0, 1) ? "sw1" : "sw2";
  data->nom_acheteur = random_between(0, 1) ? "Antoine" : "Francoise";
  data->nom_transporteur = random_between(0, 1) ? "Jule" : "Anne";
  data->quantite_stock = random_between(0, 300);
  data->surface_m2 = random_between(0, data->quantite_stock);
  data->prix_pallet = random_between(10, 20);
}

void acheteur(pro_data_t *pro_data) {
  int nbytes;
  char buf[100];
  char *nom_acheteur = pro_data->nom_acheteur;
  int *fd1 = pro_data->fd1;
  int *fd0= pro_data->fd0;
  int *fd3= pro_data->fd3;
  int *fd4= pro_data->fd4;
  close(fd1[1]);
  close(fd0[0]);
  close(fd4[0]);
  close(fd3[1]);


  // 1. l'acheteur saisit le nom d'un article.
  char *nom_article = pro_data->nom_article;
  write(fd0[1], nom_article, (strlen(nom_article)+1));

  // 2.2 L'acheteur reçoit  la surface en m2 disponible en stock.
  nbytes = read(fd1[0], buf, sizeof(buf)); buf[nbytes] = 0;
  int quantite_dispo = strtol(buf, NULL, 10);
  printf("Acheteur %s recoit (quantite) : %d\n", nom_acheteur, quantite_dispo);

  // 3. L'acheteur saisit la surface souhaitée.
  sprintf(buf, "%d", pro_data->surface_m2);
  write(fd0[1], buf, (strlen(buf)+1));

  // L'Acheteur reçoit le nombre de palettes correspondant à la surface demandée et le prix total.
  nbytes = read(fd1[0], buf, sizeof(buf)); buf[nbytes] = 0;
  int nb_palettes = strtol(strtok(buf, " "), NULL, 10);
  int prix_total = strtol(strtok(NULL, " "), NULL, 10);
  printf("Acheteur %s recoit : nb_palettes = %d ; prix_total = %d€\n", nom_acheteur, nb_palettes, prix_total);

  // 5.l'acheteur saisit ses informations de paiement.
  printf("Acheteur effectue un paiement securise.\n");
  sprintf(buf, "%s %s %s", pro_data->nom_acheteur, "1234-5678-9876-5432", "157");
  write(fd0[1], buf, (strlen(buf)+1));



  // l'acheteur reçoit un accusé de reception du paiement.
  nbytes = read(fd1[0], buf, sizeof(buf)); buf[nbytes] = 0;
  printf("Acheteur %s recoit : %s€\n", nom_acheteur, buf);

  //9. L'acheteur reçoit  et signe le bon de livraison.
  nbytes = read(fd3[0], buf, sizeof(buf)); buf[nbytes] = 0;
  printf("Acheteur  %s recoit et signe: %s\n", pro_data->nom_acheteur, buf);

  // L'Acheteur remet le bon de livraison au transporteur.
  sprintf(buf, "bon_livraison: %s (x%d)", nom_article, nb_palettes);
  write(fd4[1], buf, (strlen(buf)+1));


}



void livreur(pro_data_t *pro_data) {
  int nbytes;
  char buf[100];
  char *nom_article = pro_data->nom_article;
  int *fd2 = pro_data->fd2;
  int *fd3 = pro_data->fd3;
  int *fd4 = pro_data->fd4;
  close(fd2[1]);
  close(fd3[0]);
  close(fd4[1]);

  // Le transporteur reçoit  le bon de livraison.
  nbytes = read(fd2[0], buf, sizeof(buf)); buf[nbytes] = 0;
  printf("Livreur %s recoit : %s\n", pro_data->nom_transporteur,  buf);

  //8. Le transporteur envoie le bon de livraison à l'acheteur.

  write(fd3[1], buf, (strlen(buf)+1));
  sprintf(buf, "bon_livraison: %s (x%d)", nom_article, buf);



  // Le transporteur reçoit  le bon de livraison signé par l'acheteur.

  nbytes = read(fd4[0], buf, sizeof(buf)); buf[nbytes] = 0;
  strcat(buf, " (signé)");
  printf("Livreur %s recoit : %s\n", pro_data->nom_transporteur, buf);
  printf("Transaction terminée.\n");

}




int main(int argc, char *argv[]) {
  pro_data_t pro_data;
  pid_t pid_client, pid_livreur;

  initilize_pro_data(&pro_data);

  if((pid_client = fork()) == -1) {
    perror("fork");
    exit(1);
  }

  if(pid_client == 0) {
    acheteur(&pro_data);
  } else {
    if((pid_livreur = fork()) == -1) {
      perror("fork");
      exit(1);
    }

    if (pid_livreur == 0) {
      livreur(&pro_data);
    } else {
      int nbytes;
      char buf[100];
      char nom_article[100];
      char *nom_serveur = pro_data.nom_serveur;
      int *fd1 = pro_data.fd1;
      int *fd0 = pro_data.fd0;
      int *fd2 = pro_data.fd2;
      int *fd3 = pro_data.fd3;
      close(fd1[0]);
      close(fd0[1]);
      close(fd2[0]);
      close(fd3[1]);

      // Le serveur reçoit le nom de l'article souhaité par l'acheteur
      nbytes = read(fd0[0], buf, sizeof(buf)); buf[nbytes] = 0;
      strcpy(nom_article, buf);
      printf("Serveur %s recoit (nom d\'article) : %s\n", nom_serveur, nom_article);

      // 2. Le serveur transmet la suface en m2 disponible en stock à l'acheteur.
      sprintf(buf, "%d", pro_data.quantite_stock);
      write(fd1[1], buf, (strlen(buf)+1));

      // Le serveur reçoit la surface en m2 souhaitée par l'acheteur.
      nbytes = read(fd0[0], buf, sizeof(buf)); buf[nbytes] = 0;
      int surface_m2 = strtol(buf, NULL, 10);
      printf("Serveur %s recoit (surface en m2) : %d\n", nom_serveur, surface_m2);

      // Le serveur transmet le nombre de palettes de parpaings et le montant total de la facture.
      int nb_palettes = ceil(surface_m2 / 50);
      int prix_total = nb_palettes * pro_data.prix_pallet;
      sprintf(buf, "%d %d", nb_palettes, prix_total);
      write(fd1[1], buf, (strlen(buf)+1));

      // Le serveur reçoit  les informations de paiement de l'acheteur.
      nbytes = read(fd0[0], buf, sizeof(buf)); buf[nbytes] = 0;
      char *nom_client = strtok(buf, " ");
      char *carte_bancaire_nombre = strtok(NULL, " ");
      char *carte_bancaire_cvv = strtok(NULL, " ");
      printf("Serveur %s recoit : nom = %s ; carte_bancaire = %s ; cvv = %s\n", nom_serveur, nom_client, carte_bancaire_nombre, carte_bancaire_cvv);

      //6. Le serveur envoie un accusé de reception à l'acheteur.
      sprintf(buf, "accuse_reception: %d", prix_total);
      write(fd1[1], buf, (strlen(buf)+1));

      //7. Le serveur transmet le bon de livraison au transporteur.
      sprintf(buf, "bon_livraison: %s (x%d)", nom_article, nb_palettes);
      write(fd2[1], buf, (strlen(buf)+1));

      wait(NULL);
    }
  }

  return 0;
}
