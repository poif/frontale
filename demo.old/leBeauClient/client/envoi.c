#include "envoi.h"


/*======================================================================
**  Nom          : envoi_requete
**  Description  : Permet l'envoi d'une requête vers la frontale
** ---------------------------------------------------------------------
**  Parametres   :
**      char * requete : requete à envoyer
**======================================================================*/

int envoi_requete(char *requete)
{
  int sockfd;
  struct sockaddr_in serveur; 
  fd_set readfds;

  sockfd = socket(AF_INET,/*SOCK_STREAM*/SOCK_DGRAM,/*IPPROTO_TCP*/IPPROTO_UDP);

  serveur.sin_family = AF_INET;
 
  inet_aton(recup_valeur("ip_frontale"), &(serveur.sin_addr));

  printf("ADRESSE -> %s\n", recup_valeur("ip_frontale"));

  printf("PORT -> %i\n", recup_valeur_entier ( "port_frontale" ));

  serveur.sin_port = htons(recup_valeur_entier ( "port_frontale" ));
 
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
                                           
  if(send(sockfd,requete,strlen(requete),0) == -1)
  {
    perror("Erreur lors de l'appel a send -> ");
    exit(1);
  }

  close(sockfd);

  return 0;             
}
