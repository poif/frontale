#include "traitement_recu.h"

static int flag = 0;

extern int groupe_rep;

int traiter_recu (char * requete_recu)
{
    unsigned char requete_decrypt[1024];
    memset(requete_decrypt, '\0', 1024);
    puts("YIHI");
    decrypt(requete_recu, requete_decrypt, 512);
    printf("JAI RECU -> %s\n", requete_recu);
    printf("DECRYPT -> %s\n", requete_decrypt);

    char *type = malloc (sizeof (char) * 256);
    char *donnee = malloc (sizeof (char) * 256);
    char * save_ptr;
    char *numero_requete = malloc (sizeof (char) * 1024);

    char * copie = malloc(sizeof (char) * 1024);

    strncpy(copie,requete_decrypt, 1024);
    copie[1023] = '\0';

    numero_requete = strtok_r(copie, "*", &save_ptr);


    if(numero_requete[0] == 'R')
    {
    	type = 'z';
    	traitement_R(requete_decrypt);
    	return 0;
    }

    else
	    type = strtok_r(NULL, "*", &save_ptr);

	int j = 0;


	switch(type[0])
	{
				
		case '1' :				
		{
				char *status_requete = malloc (sizeof (char) * 1024);
				
				char *affectation_requete = malloc (sizeof (char) * 1024);
				
				char *nom = recup_valeur("nom");
				

				status_requete = strtok_r(NULL, "*", &save_ptr);
				affectation_requete = strtok_r(NULL, "*", &save_ptr);
				
				

				usleep(100);

				if( strcmp(recup_valeur("affectation"), affectation_requete) == 0)
				{
					puts("Correspond");
					unsigned char a_envoyer[sizeof (char) * 1024];
					sprintf(a_envoyer, "chall*%s*1*%s*%s*EOF", numero_requete, nom, recup_valeur("statut"));
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					memset(a_envoyer_crypt, '\0', sizeof(a_envoyer));
					
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt,strlen(a_envoyer)+16-(strlen(a_envoyer)%16),GIVE);
				}
				else
				{
					puts("NOP");
					unsigned char a_envoyer[sizeof (char) * 1024];
					strcat(a_envoyer,numero_requete);
					strcat(a_envoyer, "chall*1*none*EOF");
					
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					memset(a_envoyer_crypt, '\0', sizeof(a_envoyer));
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt,strlen(a_envoyer)+16-(strlen(a_envoyer)%16),GIVE);	
				}

				break;
		}

		case '2' :
		{
				char *status_requete = malloc (sizeof (char) * 1024);
				char *affectation_requete = malloc (sizeof (char) * 1024);
				char *test = malloc (sizeof (char) * 1024);
				char *nom = recup_valeur("nom");
				
				
				
				
				status_requete = strtok_r(NULL, "*", &save_ptr);
				affectation_requete = strtok_r(NULL, "*", &save_ptr);
				
				
				
				

				if( strcmp(recup_valeur("affectation"), affectation_requete) == 0)
				{
					puts("Correspond");
					unsigned char a_envoyer[sizeof (char) * 1024];
					sprintf(a_envoyer, "chall*%s*2*%s*%s*EOF",numero_requete, nom, recup_valeur("statut"));
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					memset(a_envoyer_crypt, '\0', sizeof(a_envoyer));
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt,strlen(a_envoyer)+16-(strlen(a_envoyer)%16),GIVE);
				}
				else
				{
					puts("NOP");
					unsigned char a_envoyer[sizeof (char) * 1024];
					sprintf(a_envoyer, "chall*%s*2*none*EOF", numero_requete);
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					memset(a_envoyer_crypt, '\0', sizeof(a_envoyer));
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt,strlen(a_envoyer)+16-(strlen(a_envoyer)%16),GIVE);	
				}
				break;
		}

		case '3' :
		{
				char *status_requete = malloc (sizeof (char) * 1024);//doit etre none
				char *affectation_requete = malloc (sizeof (char) * 1024);
				char *groupe_requete = malloc (sizeof (char) * 1024); //doit etre none
				char *datatype_requete = malloc (sizeof (char) * 1024); 
				char *test = malloc (sizeof (char) * 1024);
				char *nom = recup_valeur("nom");
				printf("NOM -> %s\n", nom);
				
				
				
				status_requete = strtok_r(NULL, "*", &save_ptr);
				affectation_requete = strtok_r(NULL, "*", &save_ptr);
				groupe_requete = strtok_r(NULL, "*", &save_ptr);
				datatype_requete = strtok_r(NULL, "*", &save_ptr);
				
				
				
				int ret = 1;

				if( strcmp(recup_valeur("affectation"), affectation_requete) == 0 &&  ret== 1)
				{
					puts("Correspond");
					unsigned char a_envoyer[sizeof (char) * 1024];
					
					char *ref_amoi = malloc (sizeof (char) * 256);
					
					
					sprintf(a_envoyer, "chall*%s*3*fichier2.txt;%s*EOF",numero_requete/*, ref_amoi*/, nom);
					
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt,strlen(a_envoyer)+16-(strlen(a_envoyer)%16),GIVE);
				}
				else
				{
					puts("NOP");
					unsigned char a_envoyer[sizeof (char) * 1024];
					sprintf(a_envoyer, "chall*%s*3*none*EOF", numero_requete);
					unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
					crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
					envoi_requete(a_envoyer_crypt,strlen(a_envoyer)+16-(strlen(a_envoyer)%16),GIVE);	
				}
				break;
			
		}

				case '4' :
				{
					unsigned char a_envoyer[sizeof (char) * 1024];
					char *groupe_demande = malloc (sizeof (char) * 1024);
					char *ref = malloc (sizeof (char) * 1024);
					
					groupe_demande = strtok_r(NULL, "*", &save_ptr);
					groupe_demande = strtok_r(NULL, "*", &save_ptr);
					groupe_demande = strtok_r(NULL, "*", &save_ptr);
					ref = strtok_r(NULL, "*", &save_ptr);
					ref = strtok_r(NULL, "*", &save_ptr);


					if (1==1/* || groupe_correspondance(groupe_demande)*/)
					{
						envoi_fichier(retour_path(ref), numero_requete);
					}
					else
					{
						unsigned char a_envoyer[sizeof (char) * 1024];
						sprintf(a_envoyer, "chall*%s*4*none*EOF", numero_requete);
						unsigned char a_envoyer_crypt[sizeof(a_envoyer)];
						crypt(a_envoyer, a_envoyer_crypt, strlen(a_envoyer));
						envoi_requete(a_envoyer_crypt,strlen(a_envoyer)+16-(strlen(a_envoyer)%16),GIVE);	

					}

					break;
			
				}

				case '6' :
				{
					char *groupe_demande = malloc (sizeof (char) * 1024);
					unsigned char a_envoyer[sizeof (char) * 1024];
					groupe_demande = strtok_r(NULL, "*", &save_ptr);
					if (groupe_correspondance(groupe_demande) == 1)
					{
						sprintf(a_envoyer, "chall*%s*6*1*EOF", numero_requete);
					}
					else
					{
						sprintf(a_envoyer, "chall*%s*6*0*EOF", numero_requete);
					}
				}

	}
	return 0;

}

void traitement_R(char *recu)
{
	char *save_ptr;
	char *numero = malloc (sizeof (char) * 1024);
	char *temp = malloc (sizeof (char) * 1024);
	strncpy(temp, (const char *) recu, 1000);
	
	char * garb = strtok_r(temp, "*", &save_ptr);
	numero = strtok_r(NULL, "*", &save_ptr);
	
	

	switch(numero[0])
	{
		case '1':
		{
			;
			int i = 0;
			
			while(recu[i] != '\0')
			{
				if(recu[i] == 'R');
								i++;
			}
			break;
		}

		case '2':
		{	
			printf("recu : %s\n", recu);
			if(recu[4] == '1')
				puts("OUI");
			else
				puts("NON");
			break;
		}

		case '4':
		{
			char *fichier= malloc (sizeof (char) * 1024);
			fichier = strtok(recu, "R*4*");

			if (strcmp(fichier, "none") == 0) puts("Aucun fichier");
			else recevoir_fichier(fichier);
			break;
		}

		case '6':
		{
			if(recu[4] == 1)
				puts("Groupe déjà existant !");
			else
			{
				puts("Groupe");
				groupe_rep = 1;
			}

			flag = 1;
			break;
		}

	}

}

void recevoir_fichier(char *fichier)
{
	 FILE * recvFILE;
	 recvFILE = fopen ( "newdl","w+" );
	 fwrite (fichier, sizeof(fichier) , 1024, recvFILE );
	 fclose(recvFILE);
}

void *wait_answer()
{
	while(flag ==0 );
	
	flag = 0;
}
