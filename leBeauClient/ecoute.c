#include "ecoute.h"

/*======================================================================
**  Nom          : ecoute_thread_client
**  Description  : Initialisation de l'écoute du client
**======================================================================*/

void * ecoute_thread_client()
{

  int sockfd_ecoute;           

  char buf[1024];               

  memset(buf,'\0',1024);        

  struct sockaddr_in my_addr;  
  struct sockaddr_in client;    

  socklen_t sin_size = sizeof(struct sockaddr_in);

  sockfd_ecoute = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

  my_addr.sin_family = AF_INET;

  my_addr.sin_port = ntohs(recup_valeur_entier ( "uport" ));

  my_addr.sin_addr.s_addr=htonl(INADDR_ANY);

  if(bind(sockfd_ecoute,(struct sockaddr*)&my_addr,sizeof(my_addr)) != 0)
  {
    perror("Erreur lors de l'appel a bind -> ");
    exit(1);
  }

  while(1)
  {
    memset(buf, '\0', sizeof(buf));

    if (recvfrom(sockfd_ecoute, buf, sizeof(buf), 0, (struct sockaddr*)&client,&sin_size) == -1)
    {
      perror("Error recvfrom -> ");
      close(sockfd_ecoute);
      exit(2);
    }
    printf("BUF -> %s\n", buf);
    char *save_ptr;
    
    if(buf[0] != '5')
      traiter_recu(buf);
    else{
      //char *temp ;
      char *result = malloc(sizeof(char)*1024);
      //temp = strtok(buf,"5*");

      int k;
      for(k=0;k<1023; k++) result[k] = buf[k+2];

      recup_aes(result);
    }

    puts("AFTER");
  }
}

/*======================================================================
**  Nom          : ecoute_client
**  Description  : Permet la création du thread d'écoute
**======================================================================*/

int ecoute_client()
{
	return pthread_create(&thread_ecoute, NULL, &ecoute_thread_client, NULL);
}

/*======================================================================
**  Nom          : ecoute_kill_thread
**  Description  : Permet la terminaison du thread
**======================================================================*/

void ecoute_kill_thread()
{
	pthread_cancel (&thread_ecoute);
}
