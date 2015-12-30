#include "envoi.h"

/*======================================================================
**  Nom          : envoi_requete
**  Description  : Permet l'envoi d'une requête vers la frontale
** ---------------------------------------------------------------------
**  Parametres   :
**      char * requete : requete à envoyer
**======================================================================*/

int envoi_requete(char *requete, int type)
{
  int sockfd;
  struct sockaddr_in serveur; 
  fd_set readfds;
  printf("Longueur du paquet -> %i\n", strlen(requete));
  sockfd = socket(AF_INET,/*SOCK_STREAM*/SOCK_DGRAM,/*IPPROTO_TCP*/IPPROTO_UDP);

  serveur.sin_family = AF_INET;
 
  inet_aton(recup_valeur("ip_frontale"), &(serveur.sin_addr));

  printf("ADRESSE -> %s\n", recup_valeur("ip_frontale"));

  if(type == WANT){
      printf("PORT -> %i\n", recup_valeur_entier ( "port_frontale_want" ));

      serveur.sin_port = htons(recup_valeur_entier ( "port_frontale_want" ));
  }
  else{
      printf("PORT -> %i\n", recup_valeur_entier ( "port_frontale_give" ));

      serveur.sin_port = htons(recup_valeur_entier ( "port_frontale_give" ));
  }

 
  if(connect(sockfd,(struct sockaddr*)&serveur,sizeof(serveur)) == -1)
  {
    perror("Erreur de connexion -> ");
    exit(2);
  }

  puts("AFTER CONNECT");
    
 /* if(select(sockfd,&readfds,NULL,NULL,NULL) == -1)
  {
        perror("Erreur lors de l'appel a select -> ");
        exit(1);
  }*/

  printf("requete -> %s\n", requete);
  /*unsigned short taille_requete = ((unsigned short) strlen(requete));

  unsigned char num[3];

  num[0] = taille_requete >> 8;
  num[1] = taille_requete & 0xFF;
  num[2] = '\0';

 printf("taille requete : %d\n", taille_requete);
 int random = sizeof(taille_requete);
 printf("taill taill %d\n", random);

 unsigned char a_envoyer[sizeof (char *) * 1024];
 a_envoyer[0] = num[0];
 a_envoyer[1] = num[1];
 strcpy(&a_envoyer[2],requete);
 //printf("requete -> %s\n", a_envoyer);

 taille_requete = ((unsigned short) strlen(a_envoyer));

 printf("taille requete : %d\n", taille_requete);*/

  if(send(sockfd,requete,strlen(requete),0) == -1)
  {
    perror("Erreur lors de l'appel a send -> ");
    exit(1);
  }

  close(sockfd);

  return 0;             
}
