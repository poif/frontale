#include <openssl/sha.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include "traitement.h"

#define CLIENT_MAX_CHAMP 20 	//nombre de champ max qu'un client peut envoyer dans une réponse de récupération de références
#define BDD_MAX_CHAMP    100    //pareil pour la bdd

using namespace std;

string hashString(const char * to_hash){
	string sHash;
	char* hash=(char*)malloc(SHA_DIGEST_LENGTH*sizeof(char));
	SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, to_hash, strlen(to_hash));
    SHA1_Final((unsigned char*)hash, &ctx);
    sHash = HexFormate((const unsigned char*) hash, SHA_DIGEST_LENGTH);
    free(hash);
    return sHash;
}

string HexFormate(const unsigned char * hash, size_t length){
	//init OSS
    ostringstream os;
    os.fill('0');
    os << hex;
    //parcours de la chaine et copie de 2 caractères hexa
    for(const unsigned char * ptr = hash ; ptr < hash + length ; ptr++)
        os << setw(2) << (unsigned int) *ptr;

    return os.str();
}

//////////////////////////
/******///client///******/
//////////////////////////

/////////////////////////////////////////////////////
/***TESTS CONCEPTION : REQUETES DANS DES FICHIERS***/
/////////////////////////////////////////////////////

string *traitement_look(string& affectation) {
  //envoi affectation, récupération hash statut + nom
  string nom;
  char* statut;
  string readAffectation;
  string hashlist;
  string hashS;
  string *finalList = new string[2];
  string listnom;
  string fichier = "test.txt";
  //envoyer to_send
  /**remplacement pour test**/
  /*************************************/
  ifstream file(fichier.c_str(), ios::in); 
    if(file){
        char* line; 
        string sline;
        while (getline(file,sline)){    
            line=(char*)sline.c_str();  
            nom = string(strtok(line, " "));
            statut = strtok(NULL," ");
            readAffectation =string(strtok(NULL," "));
            if (affectation == readAffectation){  //line : nom statut affectation
      /**Recuperation des statuts des clients et hashage***/
              hashS=hashString(statut);
              hashlist += hashS; 
              hashlist += "*";
              listnom  += nom; 
              listnom  += "*";
            }
       } 
       file.close();
       finalList[0] = hashlist;
       finalList[1] = listnom;
       return finalList;
    }
    else {
       printf("error : ouverture impossible!\n");
       exit(EXIT_FAILURE);  
    }               
}

/***********************************************************/

string traitement_exist(string& status) {
//envoi statut, retourne juste le nom hashe
      string readStatus;
      string hashlist;
      string hashS;
      char *nom;
      string fichier = "test.txt";
      /**remplacement pour test**/
      /*************************************/
      ifstream file(fichier.c_str(), ios::in);
      if(file){
         char *line;
         string sline;
         while (getline(file,sline)){
             line=(char*)sline.c_str();
             nom = strtok(line, " ");
             readStatus = string(strtok(NULL," "));
             if (status == readStatus){    //line : nom statut affectation
        /**Recuperation du nom des clients et hashage***/
                hashS=hashString(nom);
                hashlist += hashS;
                hashlist += "*";
            }
         }
         file.close();
         return hashlist;
     }
     else {
         printf("error : ouverture impossible!\n");
         exit(EXIT_FAILURE);
     }
}


/***********************************************************/

string* traitement_lookrec(string& datatype, string& status) {
//rÃ©cupÃ©ration des rÃ©fÃ©rences et du hash des noms
      string *finalList = new string[2];
      string readStatus;
      char*nom;
      string fichier = "test2.txt";
      string readDataType;
      string listRef;
      string reference;
      string hashlist;
      string hashS;
         /**remplacement pour test**/
         /*************************************/
         ifstream file(fichier.c_str(), ios::in);
         if(file){
                 char* line;
                 string sline;
                 while (getline(file,sline)){
    //on reÃ§oit nom statut reference
                         line=(char*)sline.c_str();
                         nom = strtok(line, " ");
                         readStatus = string(strtok(NULL," "));
                         readDataType=string(strtok(NULL," "));
                         reference=string(strtok(NULL," "));
                         if (status == readStatus && datatype==readDataType){    
                                hashS=hashString(nom);
                                listRef += reference;
                                listRef += "*";
                                hashlist+= hashS;
                                hashlist+= "*";
                          }
                  }
                  file.close();
                  finalList[0] = listRef;
                  finalList[1] = hashlist;
                  return finalList;
          }
          else {
                  printf("error : ouverture impossible!\n");
                  exit(EXIT_FAILURE);
       }
}


/***********************************************************/

string traitement_pull(string& reference, vector<string>& groupes_client ) {
//rÃ©cupÃ©ration du document
   string readReference;
//readGroups : token du fichier contenant la liste des groupes séparés par /
//readGroup : token de readGroups
   char* readGroups;
   string readGroup;
   string document;
   string fichier = "test3.txt";

         /**remplacement pour test**/
         /*************************************/
   ifstream file(fichier.c_str(), ios::in);
        if(file){
                char* line;
                string sline;
                while (getline(file,sline)){  //line ref document groupes
                     int stop=0;
                     line=(char*)sline.c_str();
                     readReference = string(strtok(line, " "));
                     document = string(strtok(NULL," "));
                     readGroups = strtok(NULL," ");
                     //init groupe
                     readGroup = string(strtok(readGroups,"/"));
                     if (reference==readReference){
                        while (!readGroup.empty() && stop==0) {
                           for (unsigned int j=0;j<groupes_client.size();j++){
                              if (readGroup==groupes_client[j]){  
                                 file.close();
                                 return document;
                              }
                           }
                           try{
                              readGroup = string(strtok(NULL,"/"));
                           }
                           catch (logic_error){
                              stop=1;
                           }
                        } 
                     }
                        //inc groupe
                }
                file.close();
                return NULL;
          }

          else {
                  printf("error : ouverture impossible!\n");
                  exit(EXIT_FAILURE);
          }
}


/******************************************/
/******************************************/

	/*****************/
	/**TRVE REQVESTS**/
	/*****************/

//fonction de formatage de requete
//ex : tring testreq = traitement_req_client(action,"none",affectation,vector<string>(),datatype,reference,user);
string traitement_req_client(	string numero,
								string action, 
								string statut, 
								string affectation, 
								vector<string> groupes_client,
								string typeData,
								string ref, 
								string user){
  string to_send = numero + "*";	
  to_send = action + "*";
  if (statut == "none")
    to_send +="none*";
  else 
    to_send += statut + "*";

  if (affectation == "none")
    to_send += "none*";
  else 
    to_send += affectation + "*"; 

  if (groupes_client.size()==0)
    to_send += "none*";
  else {
    for (unsigned int i = 0; i < groupes_client.size(); i++){
      if (i==(groupes_client.size()-1))
      	to_send += groupes_client[i];
      else 
      	to_send += groupes_client[i] + ";";
    to_send += "*";
	}
  }

  if (typeData == "none")
    to_send += "none*";
  else 
    to_send += typeData + "*";

  if (ref == "none")
    to_send += "none*";
  else 
    to_send += ref + "*";

  if (user== "none")
    to_send += "none*";
  else
    to_send += user + "*";

  to_send += "EOF";
  return to_send;
}

/////////////////////////
/*******REPONSES********/
/////////////////////////

//retransmission client->frontale1
//il faut hasher le statut si action1, il faut hasher le nom si action2
/*
string traitement_rep_client(string a_traiter){
	char *ca_traiter = (char*)a_traiter.c_str();
	string action = string(strtok(ca_traiter,"*"));
	string to_send;
	int iterator=0;		   //teste le numéro de champ et les sorties de boucle
	int parity=1;		   //permet de repérer les références et les noms, tester : considérer à 1 car action l'incrémente implicitement
	char * token;


*/
/*	if (action=="1") {
	//RECHERCHE UTILISATEUR : RECUPERATION DU NOM ET DU HASH STATUT
		char * status = (char*)malloc(SHA_DIGEST_LENGTH * sizeof(char));
		while ((token = strtok(NULL,"*;")) && strcmp(token, "EOF") != 0) {
*/
		/*TEST D'ERREUR DE LA REPONSE : REPONSE ERREUR OU TROP DE CHAMP*/
/*			if (strcmp(token,"none") == 0 || iterator == 2){
			//ie 3e champ qui n'est pas EOF
					to_send = "ERROR*";
					free(status);
					return to_send;
			}
		
			//nom de l'utilisateur -> premiere boucle
			if (iterator==0){
				to_send += string(token) + "*" ;
			} 
		    if (iterator==1){
			//statut : il faut hasher le statut
				strncpy(status, token, strlen(token));
				to_send += hashString(status) + "*";
			}
			iterator++;	//indique le champ
		}
		free(status);
	}


	else if (action=="2"){
	//RECHERCHE EXISTENCE DUNE PERSONNE : RECUPERATION HASH DU NOM
		char * username = (char*)malloc(SHA_DIGEST_LENGTH * sizeof(char));
		while ((token = strtok(NULL,"*;")) && strcmp(token, "EOF") != 0) {
*/
		/*TEST D'ERREUR DE LA REPONSE : REPONSE ERREUR OU TROP DE CHAMP*/
/*			if (strcmp(token,"none") == 0 || iterator == 1){
					to_send = "ERROR*";
					free(username);
					return to_send;
			}
		
			if (iterator==0)	{ //nom
				strncpy(username, token, strlen(token));
				to_send = hashString(username) + "*";
			}
			free(username);
		}
	}


	else if (action == "3"){
	//RECHERCHE REFERENCE +HASH USERNAME
		char * username = (char*)malloc(SHA_DIGEST_LENGTH * sizeof(char));
		while ((token = strtok(NULL,"*;")) && strcmp(token, "EOF") != 0) {
			//token contient une reference ou un nom du couple ref;nom
*/
		/*TEST D'ERREUR DE LA REPONSE : REPONSE ERREUR OU TROP DE CHAMP*/
/*			if (strcmp(token,"none") == 0 || iterator == CLIENT_MAX_CHAMP){
					to_send = "ERROR*";
					free(username);
					return to_send;
			}
*/
/*A PRIORI IMPOSSIBLE QU'UN NOM OU UNE REFERENCE SOIT SEUL -> testé avec parity*/
/*			if (parity%2==1){
			//on est sur une référence
				to_send += string(token);
			}
			else {
			//on est sur un nom : il faut le hasher
				strncpy(username, token, strlen(token));
				to_send += ";" + hashString(username) + "*";
			}
			iterator++;	//empêche requête du type "1,*;;;;;;;;;;;;;;;;;" trololo
			parity++;	//teste si une ref est associée obligatoirement a un nom
		}
		free(username);
*/
	/*TEST D'ERREUR DE LA REPONSE : NOMBRE DE CHAMPS INVALIDE*/
/*		if (parity % 2 == 0){
		//EOF NE COMPTE PAS
     		to_send = "ERROR*";
			return to_send;
		}

	}



	else if (action == "4"){
	//RECUPERATION DOCUMENT
		while ((token = strtok(NULL,"*")) && strcmp(token, "EOF") != 0){
		//si pas d'erreur, on récupère juste le document en un token
			if ((strcmp(token,"none") == 0)){
					to_send = "ERROR*";
					return to_send;
			}
*/
	/*TEST D'ERREUR DE LA REPONSE : NOMBRE DE CHAMPS INVALIDES*/
/*			if (iterator==1){ //ie 2e champ different de EOF
				to_send = "ERROR*";
				return to_send;
			}


			iterator++;
			to_send += string(token)+"*";
		}
	}
*/
	/*TEST D'ERREUR DE LA REPONSE : CHAMP ACTION INVALIDE*/
/*	else {
		to_send ="ERROR*";
	}
	

	return to_send;
}


*/
/***************************************************/



//VERSION AVEC TOKEN EN PARAM ET RECUPERATION DES REQUETES 
string traitement_rep_client(vector<string> a_traiter){
//	vector<string> a_traiter = vector<string>();
	//fonction pour recuperer les requetes depuis le réseau
	/*BLOC DE TEST EN ATTENDANT*/
	/*REPONSES 1/2*/
//	a_traiter.push_back("123*1*Eichler*prof*EOF");
//	a_traiter.push_back("123*1*Castelain*directeur*EOF");
//	a_traiter.push_back("123*1*Megy*tyran*EOF");
//	a_traiter.push_back("123*1*Hernance*general anti-cafe*EOF");
	/*REPONSES 3*/


	/*REPONSES 4*/
//	a_traiter.push_back("123*4*Ta_mere.mp4*EOF");
//	a_traiter.push_back("123*4*EddyMalou.tar.gz*EOF");
//	a_traiter.push_back("123*4*windows10.exe*EOF");
//	a_traiter.push_back("123*4*traitement.cpp*EOF");

	/****************************/

	vector<string> vectToSend=vector<string>();
	/*Il vaut mieux décomposer comme ca : Si on se rend compte qu'une requete est invalide
	par exemple un nom sans référence, on pourra simplement annuler cette réponse par error
	On pourra également négliger les envois d'erreur si une réponse est valide*/
	string to_send;
	int firstRep = 0;		//c'est votre premiere bafouille?


/***********************************************************/

	for (unsigned int i = 0; i < a_traiter.size(); i++) {
		//chaque réponse est dans une case du tableau
		//on renvoie toutes les réponses concaténées
		char *ca_traiter = (char*)a_traiter[i].c_str();

		string numero = string(strtok(ca_traiter,"*"));
		if (firstRep==0){
			to_send += numero + "*";
			firstRep=1;
		}

		string action= strtok(NULL,"*");
		int iterator=0;		 		    //teste le numéro de champ et les sorties de boucle
		int parity=1;				    //permet de repérer les références et les noms, tester : considérer à 1 car action l'incrémente implicitement
		char * token;
		int breaker=0;					//indique qu'il faut passer a la boucle suivante en cas d'erreur
		string stringInTheVector;		//chaine qu'on va push dans le vector

	
	/*LE NUMERO NE SERA RENVOYEE QU'UNE SEULE FOIS*/

	/*Le numero empêche les problèmes avec des actions différentes*/

	/****************************************/

		if (action=="1") {
		//RECHERCHE UTILISATEUR : RECUPERATION DU NOM ET DU HASH STATUT
			while ((token = strtok(NULL,"*")) && strcmp(token, "EOF") != 0 && breaker==0) {
			breaker=0;
			/*TEST D'ERREUR DE LA REPONSE : REPONSE ERREUR OU TROP DE CHAMP*/
				if (strcmp(token,"none") == 0 || iterator == 2){
				//ie 3e champ qui n'est pas EOF
					stringInTheVector ="ERROR*";
					breaker=1;
				}
			/***************************************************************/
				//nom de l'utilisateur -> premiere boucle
				if (breaker==0){
					if (iterator==0){
						stringInTheVector += string(token) + "*" ;
					} 
			   		if (iterator==1){
						//c'est un statut : il faut hasher le statut
						stringInTheVector += hashString(token) + "*";
					}
					iterator++;	//indique le champ
				}
			}
		}

	/****************************************/

		else if (action=="2"){
		//RECHERCHE EXISTENCE DUNE PERSONNE : RECUPERATION DU NOM ET DU STATUT
			while ((token = strtok(NULL,"*")) && strcmp(token, "EOF") != 0 && breaker==0) {
			/*TEST D'ERREUR DE LA REPONSE : REPONSE ERREUR OU TROP DE CHAMP*/
				breaker=0;
				if (strcmp(token,"none") == 0 || iterator >= 2){
					stringInTheVector = "ERROR*";
					breaker=1;
				}
			/***************************************************************/
				if (breaker==0){		//EVITE DOUBLONS *ERROR*
					if (iterator==0)	{ //nom
						string username = string(token);
						string status = string(strtok(NULL,"*")); //récupération statut
						if (status == "none" || status == "EOF"){
							stringInTheVector = "ERROR*";
							breaker=1;
						}
						string to_hash = username + status;
						char *hash = (char*)to_hash.c_str();
						iterator++;
						stringInTheVector += hashString(hash) + "*";
					}
				}
			}
		}


	/****************************************/

	/****************************************/
 
		else if (action == "3"){
		//RECHERCHE REFERENCE +HASH USERNAME
			while ((token = strtok(NULL,"*;")) && strcmp(token, "EOF") != 0 && breaker==0) {
				//token contient une reference ou un nom du couple ref;nom
			/*TEST D'ERREUR DE LA REPONSE : REPONSE ERREUR OU TROP DE CHAMP*/
				if (strcmp(token,"none") == 0 || iterator == CLIENT_MAX_CHAMP){
					stringInTheVector = "ERROR*";
					breaker=1;
				}
			/***************************************************************/	

		    	if (breaker==0){
	/*A PRIORI IMPOSSIBLE QU'UN NOM OU UNE REFERENCE SOIT SEUL -> testé avec parity*/
					if (parity%2==1)
						stringInTheVector += string(token);
					else {
					//on est sur un couple nom +statut : il faut le hasher
                                    string username = string(token);
                                    string status = string(strtok(NULL,"*")); //récupération statut
                                    if (status == "none" || status == "EOF"){
                                      stringInTheVector = "ERROR*";
                                      breaker=1;
                                    }
                                    string to_hash = username + status;
                                    char *hash = (char*)to_hash.c_str();
                                    stringInTheVector += ";" + hashString(hash) + "*";
					}
					iterator++;	//empêche requête du type "1,*;;;;;;;;;;;;;;;;;" trololo
					parity++;	//teste si une ref est associée obligatoirement a un nom
				}
			}		//SORTIE WHILE

		/*TEST D'ERREUR DE LA REPONSE : NOMBRE DE CHAMPS INVALIDE*/
			if (parity%2==0) {		//IE ON A PAS FINI SUR UNE REFERENCE
			//EOF NE COMPTE PAS
				stringInTheVector = "ERROR*";
				breaker = 1;
			}

		/*********************************************************/
		}

	/*************************************************/

		else if (action == "4"){
		//RECUPERATION DOCUMENT
			while ((token = strtok(NULL,"*")) && strcmp(token, "EOF") != 0 && breaker==0){
			//si pas d'erreur, on récupère juste le document en un token
				if (strcmp(token,"none") == 0){
					stringInTheVector = "ERROR*";
					breaker = 1;
				}

		/*TEST D'ERREUR DE LA REPONSE : NOMBRE DE CHAMPS INVALIDES*/
				if (iterator==1 || breaker!=0){ //ie 2e champ different de EOF
					stringInTheVector = "ERROR*";
				}

		/**********************************************************/

				iterator++;
				stringInTheVector += string(token)+"*";
			}
		}

		/*TEST D'ERREUR DE LA REPONSE : CHAMP ACTION INVALIDE*/
		else {
			stringInTheVector ="ERROR*";
		}

		/*****************************************************/

		vectToSend.push_back(stringInTheVector);
		//on ajoute une réponse traitée dans le vector
	}

/**********************************************************************/
//ON BOUCLE POUR TESTER SIL EXISTE UNE REPONSE QUI NEST PAS UNE ERREUR//
/**********************************************************************/
	int valid_existence=0;
	unsigned int i=0;
	while (i<vectToSend.size() && valid_existence==0){
		if (vectToSend[i] !="ERROR*"){
			valid_existence = 1;
		 }
		 i++;
	}

//On remplit to_send avec les réponses valides, ou avec ERROR* si aucune n'est valide 

	if (valid_existence==0)
		to_send += "ERROR*";
	else {
		for (i=0;i<vectToSend.size();i++){
			if (vectToSend[i] != "ERROR*")
				to_send += vectToSend[i];
		}
	}

	return to_send;
}


/////////////////////////
/*         BDD         */
/////////////////////////

//formatage requete bdd
string traitement_req_bdd(string numero,
						  string action, 
						  string statut, 
						  string affectation, 
						  vector<string> groupes_client, 
						  string typeData, 
						  string ref, 
						  string user){
  string to_send = numero + "*";
  to_send = action + "*";
  if (statut == "none")
    to_send +="NULL*";
  else 
    to_send += statut + "*";

  if (affectation == "none")
    to_send += "NULL*";
  else 
    to_send += affectation + "*"; 

  if (groupes_client.size() == 0)
    to_send += "NULL*";
  else {
    for (unsigned int i = 0; i < groupes_client.size(); i++){
      if (i==(groupes_client.size()-1))
      	to_send += groupes_client[i];
      else 
      	to_send += groupes_client[i] + ";";
    to_send += "*";
	}
  }

  if (typeData == "none")
    to_send += "NULL*";
  else 
    to_send += typeData + "*";

  if (ref == "none")
    to_send += "NULL*";
  else 
    to_send += ref + "*";

  if (user== "none")
    to_send += "NULL*";
  else
    to_send += user + "*";

  to_send += "EOF";

  return to_send;

}



/***********************************/
//retransmission bdd -> frontale 1
string traitement_rep_bdd(string a_traiter){

	char *ca_traiter = (char*)a_traiter.c_str();
	string action = string(strtok(ca_traiter,"*"));
	string to_send;
	int iterator=0;  //juste pour vérifier que la requete document est valide
	char * token;

	/****************************/

	if (action == "300"){
	//RECHERCHE REFERENCE +HASH
	//pas besoin de parity ni de hash ici, les noms étant déjà hashés par la BDD
		while ((token = strtok(NULL,"*"))!=NULL && string(token)!="EOF") {
			//token contient un couple reference;hash
			if (strcmp(token,"ERROR") == 0 || iterator == BDD_MAX_CHAMP){
	//iterator empêche un trop grand nombre de champ
				to_send = "ERROR*";
				return to_send;
			}
			to_send += string(token) + "*";
			iterator++;
		}
	}

	/****************************/

	else if (action == "301"){
	//RECUPERATION DOCUMENT
		while ((token = strtok(NULL,"*")) && strcmp(token, "EOF") != 0){
		//si pas d'erreur, on récupère juste le document en un token
			
			if (strcmp(token,"ERROR") == 0 || iterator == 1){
				to_send = "ERROR*";
				return to_send;
			}

	/*TEST D'ERREUR DE LA REPONSE : NOMBRE DE CHAMPS INVALIDE */
			if (iterator==0){ //ie on a déja bouclé
				to_send = "ERROR*";
				return to_send;
			}
	/**********************************************************/

			iterator++;
			to_send += string(token);
		}
	}

	/*TEST D'ERREUR DE LA REPONSE : L'ACTION EST INVALIDE*/
	else {
		to_send = "ERROR*";
	}
	/*****************************************************/

	to_send += "EOF";
	return to_send;
}

