#include "envoi.h"

/*======================================================================
**  Nom          : envoi_requete
**  Description  : Permet l'envoi d'une requête vers la frontale
** ---------------------------------------------------------------------
**  Parametres   :
**      char * requete : requete à envoyer
**======================================================================*/

int envoi_requete(char *requete, int size, int type)
{
  int sockfd;
  struct sockaddr_in serveur; 

  printf("Longueur du paquet -> %i\n", size);
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

  printf("requete -> %s\n", requete);

  if(send(sockfd,requete,size,0) == -1)
  {
    perror("Erreur lors de l'appel a send -> ");
    exit(1);
  }
  puts("AFTER SEND");
  close(sockfd);

  return 0;             
}

int envoi_fichier(char *fichier, char *num)
{
    char temp[1024];
    FILE * fichier_envoi;
    unsigned char c[sizeof (char *) * 1024];
    unsigned char contenu[sizeof (char *) * 1024];
    unsigned char capart[sizeof (char *) * 2048];
    char save[1];
    char remoteFILE[4096];
    long fichierSIZE;
    int compt1=1,compt2=1, pourcentage;

    fichier_envoi = fopen (fichier,"r");

    if(!fichier_envoi)
    {
        printf("Erreur ouverture fichier\n");
    }

    else
    {

    fseek (fichier_envoi, 0, SEEK_END);
    fichierSIZE = ftell (fichier_envoi);
    rewind(fichier_envoi);//retour debut fichier
    
    int i = 0;

    sprintf(c, "chall*%s*4*", num);
    printf("machin -> %s\n", c);
    while(i<fichierSIZE && ( contenu[i] = fgetc(fichier_envoi)) !=EOF)
    {
      i++;
    }
    sprintf(c+strlen(c), "%s", contenu);

    crypt(c, capart, strlen(c));
    envoi_requete(capart,strlen(c)+16,GIVE);
    }

}

char *retour_chaine_fic(char *fichier)
{
    char temp[1024];
    FILE * fichier_envoi;
    unsigned char c[1024];
    char save[1];
    char remoteFILE[4096];
    long fichierSIZE;
    int compt1=1,compt2=1, pourcentage;
    memset(c, '\0', 1024);
    fichier_envoi = fopen (fichier,"r");

	puts("retour chaine");

    if(!fichier_envoi)
    {
        printf("Erreur ouverture fichier\n");
    }

    else
    {

    fseek (fichier_envoi, 0, SEEK_END);
    fichierSIZE = ftell(fichier_envoi);
   
    rewind(fichier_envoi);//retour debut fichier

    int i = 0;

    while( ( i < fichierSIZE ) && (( c[i] = fgetc(fichier_envoi) ) != EOF))
    {
	i++;
    }
    return c;
    }

return "haha";
}
