#include <openssl/sha.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <list>
#include <string>
#include <iostream>
#include "traitement.h"

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

string traitement_req_client(	string numero,
								string action, 
								string statut, 
								string statut_cible,
								string affectation,
								string affectation_cible, 
								string groupes_client,
								string groupes_cible,
								string typeData,
								string ref, 
								string user){

  string to_send = numero + "*" + action + "*";		

	if (action=="3"){
    to_send += statut_cible + "*";          //statut cible
    to_send += affectation_cible + "*";     //affectation cible

    //on recupere none s'il n'y a rien, sinon il faut tokenizer
    //on récuperera les groupes dans un tableau, car il est compliqué de gérer le dernier groupe
    //(ou il faut mettre * et non ';', avec un simple getline)

  	if (groupes_cible == "none")           //groupes cible
    	to_send += "none*";
  	else {
      istringstream ss(groupes_cible);
      vector<string> groups;
      string readGroup;
      while(getline(ss, readGroup, ';')){
        groups.push_back(readGroup);    
      }
//on peut ainsi récupérer la taille et traiter spécialement le dernier élément
    	for (unsigned int i = 0; i < groups.size(); i++){
      	if (i==(groups.size()-1))
      		to_send += groups[i];
     		else 
      		to_send += groups[i] + ";";
      }
    	to_send += "*";
		}

    to_send += typeData + "*";        //typedata
    to_send += "none*";               //reference
    to_send += "none*";                //nom
    to_send += "EOF";
 	}


 	else if (action=="4"){   
    to_send += statut + "*";       //statut client
    to_send += affectation + "*";  //affectation client

  	if (groupes_client == "none")    //groupes
    	to_send += "none*";
  	else {
      istringstream ss(groupes_client);
      vector<string> groups;
      string readGroup;

      while(getline(ss, readGroup, ';')){
        groups.push_back(readGroup);    
      }

    	for (unsigned int i = 0; i < groups.size(); i++){
      	if (i==(groups.size()-1))
      	  to_send += groups[i];
     		else 
      		to_send += groups[i] + ";";
      }
    	to_send += "*";
		}

    to_send += "none*";             //typeData
    to_send += ref + "*";                 //ref
    to_send += "none*";             //user
    to_send += "EOF";
 	}

 	else if (action=="1" || action=="2"){
    	to_send +="none*";             //status
   		to_send += affectation + "*";  //affectation
    	to_send += "none*";            //groupes
    	to_send += "none*";            //typeData
    	to_send += "none*";            //ref
    	to_send += "none*";            //user
      to_send += "EOF";
	}

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
string traitement_rep_client(list<string>& a_traiter){

	vector<string> vectToSend=vector<string>();
	/*Il vaut mieux décomposer comme ca : Si on se rend compte qu'une requete est invalide
	par exemple un nom sans référence, on pourra simplement annuler cette réponse par error
	On pourra également négliger les envois d'erreur si une réponse est valide*/
	string to_send;
	int firstRep = 0;		//c'est votre premiere bafouille?

//	for (unsigned int i = 0; i < a_traiter.size(); i++) {
  for (list<string>::iterator it = a_traiter.begin(); it != a_traiter.end(); it++) { 
		//chaque réponse est dans une case du tableau
		//on renvoie toutes les réponses concaténées
    istringstream ss(*it);
		string numero; getline(ss, numero, '*');

		if (firstRep==0){
			 to_send += numero + "*";
			 firstRep=1;
		}
		string action; getline(ss, action, '*');

		string stringInTheVector;		//chaine qu'on va push dans le vector
    string nom;
    string Hnomstat;
    string statut;
    string reference;
    string token;   
    string document;
    string tokenEOF;        //hey are you EOF?
    int testEOF = 0;        //avez-vous déjà croisé EOF?
	
	/*LE NUMERO NE SERA RENVOYEE QU'UNE SEULE FOIS*/

	/*Le numero empêche les problèmes avec des actions différentes*/


		if (action=="1") {
		//RECHERCHE UTILISATEUR : RECUPERATION DU NOM ET DU HASH STATUT
			if (!getline(ss, nom, '*') || !getline(ss, statut, '*') || !getline(ss,tokenEOF,'*') ||
          nom == "none" || statut == "none" ||
          nom == "EOF"  || statut == "EOF" || tokenEOF != "EOF") {

					stringInTheVector ="ERROR*";
			}

			else {
			   stringInTheVector += nom + "*" + hashString((char*)statut.c_str())+"*";
			}
		}

	/****************************************/

		else if (action=="2"){
		//RECHERCHE EXISTENCE DUNE PERSONNE : RECUPERATION DU NOM ET DU STATUT
			if (!getline(ss, nom, '*') || !getline(ss, statut, '*') || !getline(ss, tokenEOF, '*')|| 
          nom == "EOF"  || statut == "EOF" ||
          nom == "none" || statut == "none"||
          tokenEOF != "EOF") {

					stringInTheVector = "ERROR*";
			}

			/***************************************************************/
			else { //valide -> renvoi hash(nom+statut)
         string to_hash = nom + statut;
         char *hash = (char*)to_hash.c_str();
				 stringInTheVector += hashString(hash) + "*";
      }
		}

	/****************************************/
 
		else if (action == "3"){
	     //RECHERCHE STATUT*REFERENCE*NOM
       while (getline(ss , token , '*')){
          if (token == "none"){
             stringInTheVector = "ERROR*";
             break;
          }

          else if (token =="EOF" || testEOF == 1){
    /*ON NE PEUT RENTRER QUUNE FOIS SAUF SI LA REPONSE EST INVALIDE*/
             if (testEOF == 0)
                testEOF=1;
             else  
                stringInTheVector = "ERROR*";
          }

          else {
             istringstream ssToken(token);
             if ( !getline(ssToken,statut,';') || !getline(ssToken, reference, ';') || !getline(ssToken, nom, ';') ||
                reference == "EOF" || reference == "none" || nom == "EOF" || nom == "none" || 
                statut == "none"||statut == "EOF"){
                stringInTheVector = "ERROR*";
                continue;
             }
             else {
                string to_hash = nom + statut;
                char *hash = (char*)to_hash.c_str();
                stringInTheVector += reference + "*" + hashString(hash) + "*";
             }
          }
       }
    }

	/*************************************************/

	  else if (action == "4"){
		//RECUPERATION DOCUMENT
       if (!getline(ss, document, '*') || !getline(ss, tokenEOF, '*')|| 
          document == "EOF" || document == "none" || tokenEOF != "EOF"){

          stringInTheVector = "ERROR*";
       }

       else {
          stringInTheVector = document + "*";
       }
    }


		/**********************************************************/

    else if (action == "300"){
    //déja hashé!
       while (getline(ss , token , '*')){
          if (token == "NULL"){
             stringInTheVector = "ERROR*";
             break;
          }

          else if (token =="EOF" || testEOF == 1){
    /*ON NE PEUT RENTRER QUUNE FOIS SAUF SI LA REPONSE EST INVALIDE*/
             if (testEOF == 0)
                testEOF=1;
             else 
                stringInTheVector = "ERROR*";
          }

          else {
             istringstream ssToken(token);
             if (!getline(ssToken, reference, ';') || !getline(ssToken, Hnomstat, ';') ||
                reference == "EOF" || reference == "NULL" || nom == "EOF" || nom == "NULL"){
                stringInTheVector = "ERROR*";
                continue;
             }
             else {
             //BDD : pas besoin de hasher
                stringInTheVector += reference + "*" + Hnomstat + "*";
             }
          }
       }
    }

    else if (action == "301"){
      //RECUPERATION DOCUMENT
       if (!getline(ss, document, '*') || !getline(ss, tokenEOF, '*')|| 
          document == "EOF" || document == "none" || tokenEOF != "EOF"){

          stringInTheVector = "ERROR*";
       }

       else {
          stringInTheVector = document + "*";
       }
    }

		/*TEST D'ERREUR DE LA REPONSE : CHAMP ACTION INVALIDE*/
		else {
			stringInTheVector = "ERROR*";
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
						  string statut_cible,
						  string affectation, 
						  string affectation_cible,
						  string groupes_client, 
						  string groupes_cible,
						  string typeData, 
						  string ref, 
						  string user){

  string to_send = numero + "*" + action + "*";

  if (action=="300"){
 	  if (statut_cible == "none")                            //statut cible facultatif
   		to_send +="NULL*";
 	  else 
   		to_send += statut_cible + "*";

 	  if (affectation_cible == "none")                        //affectation cible facultative
  		to_send += "NULL*";
  	else 
    	to_send += affectation_cible + "*"; 
    if (groupes_cible == "none")                          //groupes cible facultatifs
    	to_send += "NULL*";

  	else {
      istringstream ss(groupes_cible);
      vector<string> groups;
      string readGroup;
      while(getline(ss, readGroup, ';')){
        groups.push_back(readGroup);    
      }

   	  for (unsigned int i = 0; i < groups.size(); i++){
        if (i==(groups.size()-1))
      		to_send += groups[i];
     		else 
      		to_send += groups[i] + ";";
      }
        to_send += "*";
	  }

    to_send += typeData + "*";                              //typeData     

  }

  if (action=="301"){
   	to_send += statut + "*";                                //statut
    to_send += affectation + "*";                           //affectation

    if (groupes_client == "none")
    	to_send += "NULL*";

  	else {                                                   //groupes client
      istringstream ss(groupes_client);
      vector<string> groups;
      string readGroup;
      while(getline(ss, readGroup, ';')){
        groups.push_back(readGroup);    
      }

   	 	for (unsigned int i = 0; i < groups.size(); i++){
      	if (i==(groupes_client.size()-1))
      		to_send += groups[i];
     		else 
      		to_send += groups[i] + ";";
      }
      to_send += "*";
    }

	  if (ref == "none")
     	to_send += "NULL*";
  	else 
    	to_send += ref + "*";
	}
 	to_send += "EOF*";
 	return to_send;
}



/***********************************/
/*
//retransmission bdd -> frontale 1
string traitement_rep_bdd(string a_traiter){

	char *ca_traiter = (char*)a_traiter.c_str();
	string action = string(strtok(ca_traiter,"*"));
	string to_send;
	int iterator=0;  //juste pour vérifier que la requete document est valide
	char * token;
*/
	/****************************/
/*
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
*/
	/****************************/
/*
	else if (action == "301"){
	//RECUPERATION DOCUMENT
		while ((token = strtok(NULL,"*")) && strcmp(token, "EOF") != 0){
		//si pas d'erreur, on récupère juste le document en un token
			
			if (strcmp(token,"ERROR") == 0 || iterator == 1){
				to_send = "ERROR*";
				return to_send;
			}
*/
	/*TEST D'ERREUR DE LA REPONSE : NOMBRE DE CHAMPS INVALIDE */
/*			if (iterator==0){ //ie on a déja bouclé
				to_send = "ERROR*";
				return to_send;
			}*/
	/**********************************************************/
/*
			iterator++;
			to_send += string(token);
		}
	}
*/
	/*TEST D'ERREUR DE LA REPONSE : L'ACTION EST INVALIDE*/
/*		to_send = "ERROR*";
	}*/
	/*****************************************************/
/*
	to_send += "EOF";
	return to_send;
}
*/
