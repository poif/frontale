#include <openssl/sha.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include "traitement.h"
using namespace std;

string hashString(char * to_hash){
	string sHash;
	char* hash=(char*)malloc(SHA_DIGEST_LENGTH*sizeof(char));
	SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx, to_hash, strlen(to_hash));
    SHA1_Final((unsigned char*)hash, &ctx);
    sHash = hash;
    free(hash);
    return sHash;
}

//////////////////////////
/******///client///******/
//////////////////////////

/////////////////////////
/*******REQUETES********/
/////////////////////////

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
              hashlist += "\n";
              listnom  += nom; 
              listnom  += "\n";
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
         string sline=line;
         while (getline(file,sline)){
             line=(char*)sline.c_str();
             nom = strtok(line, " ");
             readStatus = string(strtok(NULL," "));
             if (status == readStatus){    //line : nom statut affectation
        /**Recuperation du nom des clients et hashage***/
                hashS=hashString(nom);
                hashlist += hashS;
                hashlist += "\n";
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
                 string sline=line;
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
                                listRef += "\n";
                                hashlist+= hashS;
                                hashlist+= "\n";
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
   int found=0;
   int i=0;
   int k=0;
         /**remplacement pour test**/
         /*************************************/
   ifstream file(fichier.c_str(), ios::in);
        if(file){
                char* line;
                string sline=line;
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
                           for (int j=0;j<groupes_client.size();j++){
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
          }

          else {
                  printf("error : ouverture impossible!\n");
                  exit(EXIT_FAILURE);
          }
}

//fonction de formatage de requete
string traitement_req_client(string& action, string& statut, string& affectation, vector<string>& groupes_client, string& typeData, string& ref, string& user){
//pas besoin de traiter par requete a première vue c'est mieux comme ca	
//fonction de test ou le client/bdd le fait?	
  string to_send = action + "*";
  if (statut == "none")
    to_send +="NULL*";
  else 
    to_send += statut + "*";

  if (affectation == "none")
    to_send += "NULL*";
  else 
    to_send += affectation + "*"; 

  if (groupes_client[0] == "none")
    to_send += "NULL*";
  else {
    for (int i = 0; i < groupes_client.size(); i++){
      if (i==groupes_client.size()-1)
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

/////////////////////////
/*******REPONSES********/
/////////////////////////

//retransmission client->frontale1
string traitement_rep_client(string a_traiter){
	//on garde la convention ...*EOF
	char *ca_traiter = (char*)a_traiter.c_str();
	string action = string(strtok(ca_traiter,"*"));
	string to_send;
	int testerror=0;
	int compteur_error=0;  //juste pour vérifier que la requete document est valide
	char * token;
	// // // // // // 
	if (action == "1"){
	//RECHERCHE REFERENCE +HASH
		while ((token = strtok(NULL,"*"))!=NULL && string(token)!="EOF") {
			//token contient un couple reference;hash
			if (testerror==0){
				testerror=1;
				if (token=="ERROR"){
					to_send = "ERROR*EOF";
					//envoi
					return to_send;
					exit(EXIT_FAILURE);
				}
			}
			to_send += string(token) + "*";
		}
	}
	// // // // // //
	else if (action == "2"){
	//RECUPERATION DOCUMENT
		while ((token = strtok(NULL,"*"))!="EOF"){
		//si pas d'erreur, on récupère juste le document en un token
			if (testerror==0){
				testerror=1;
				if (token=="ERROR"){
					to_send = "ERROR*EOF";
					//envoi
					return to_send;
					exit(EXIT_FAILURE);
				}
			}
			if (compteur_error>0){ //ie on a déja bouclé
				to_send = "ERROR*EOF";
				//envoi
				return to_send;
				exit(EXIT_FAILURE);
			}
			compteur_error++;
			to_send += string(token);
		}
	}
	else 
		to_send ="ERROR*";
	to_send += "EOF";
	return to_send;
}



/***************************************************/
/////////////////////////
/*         BDD         */
/////////////////////////

//formatage requete bdd
string traitement_req_bdd(string& action, string& statut, string& affectation, vector<string>& groupes_client, string& typeData, string& ref, string& user){
  string to_send = action + "*";
  if (statut == "none")
    to_send +="NULL*";
  else 
    to_send += statut + "*";

  if (affectation == "none")
    to_send += "NULL*";
  else 
    to_send += affectation + "*"; 

  if (groupes_client[0] == "none")
    to_send += "NULL*";
  else {
    for (int i = 0; i < groupes_client.size(); i++){
      if (i==groupes_client.size()-1)
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
	//on garde la convention ...*EOF
	char *ca_traiter = (char*)a_traiter.c_str();
	string action = string(strtok(ca_traiter,"*"));
	string to_send;
	int testerror=0;
	int compteur_error=0;  //juste pour vérifier que la requete document est valide
	char * token;
	// // // // // // 
	if (action == "300"){
	//RECHERCHE REFERENCE +HASH
		while ((token = strtok(NULL,"*"))!=NULL && string(token)!="EOF") {
			//token contient un couple reference;hash
			if (testerror==0){
				testerror=1;
				if (token=="ERROR"){
					to_send = "ERROR*EOF";
					//envoi
					return to_send;
					exit(EXIT_FAILURE);
				}
			}
			to_send += string(token) + "*";
		}
	}
	// // // // // //
	else if (action == "301"){
	//RECUPERATION DOCUMENT
		while ((token = strtok(NULL,"*"))!="EOF"){
		//si pas d'erreur, on récupère juste le document en un token
			if (testerror==0){
				testerror=1;
				if (token=="ERROR"){
					to_send = "ERROR*EOF";
					//envoi
					return to_send;
					exit(EXIT_FAILURE);
				}
			}
			if (compteur_error>0){ //ie on a déja bouclé
				to_send = "ERROR*EOF";
				//envoi
				return to_send;
				exit(EXIT_FAILURE);
			}
			compteur_error++;
			to_send += string(token);
		}
	}
	else
		to_send = "ERROR*";
	to_send += "EOF";
	return to_send;
}

