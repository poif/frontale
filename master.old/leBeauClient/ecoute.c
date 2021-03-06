#include "ecoute.h"

/*======================================================================
**  Nom          : ecoute_thread_client
**  Description  : Initialisation de l'écoute du client
**======================================================================*/

void * ecoute_thread_client()
{

  int sockfd_ecoute, sockfd_distant;       
  fd_set readfds;              

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

 /* if(listen(sockfd_ecoute,1) != 0)
  {
    perror("Erreur lors de l'appel a listen -> ");
    exit(2);
  }*/
  while(1)
  {
    memset(buf, '\0', sizeof(buf));
    puts("YOLO");

    if (recvfrom(sockfd_ecoute, buf, sizeof(buf), 0, (struct sockaddr*)&client,&sin_size) == -1)
    {
      perror("Error recvfrom -> ");
      close(sockfd_ecoute);
      exit(2);
    }
    printf("BUF -> %s\n", buf);
    traiter_recu(buf);
    puts("AFTER");
/*
	puts("HIHA");
  	FD_ZERO(&readfds);
	puts("NIANIA");
  	if(sockfd_ecoute != 0 || sockfd_distant != 0)
	{
		FD_SET(sockfd_ecoute, &readfds);
		FD_SET(sockfd_distant, &readfds);
	}

  	*if( select(sockfd_ecoute, &readfds, NULL, NULL, NULL) == -1 )
  	{
  		perror("Erreur lors de l'appel a select -> ");
        exit(1);
  	}

  	if(FD_ISSET(sockfd_ecoute,&readfds))
  	{
          if((sockfd_distant = accept(sockfd_ecoute,(struct sockaddr*)&client,&sin_size)) == -1)
          {
              perror("Erreur lors de accept -> ");
              exit(3);
          }
	  printf("socket -> %i\n", sockfd_distant);
	  puts("YOLO");
	  yolo=1;
        }
if(yolo){
    if(FD_ISSET(sockfd_distant, &readfds))
    {
	puts("ARG");
    	if(recv_from(sockfd_distant,&buf,1024,0) == -1)
    	{
    		perror("Erreur lors du recv ->");
    		exit(4);
    	}
	puts("j'ai recu un truc");
	traiter_recu(buf);
      //appel a la fonction de decryptage
    }
  }}*/

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
