
#include <openssl/sha.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include "message.h"
#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <client.h>
#include <reception.h>

#define BUF_SIZE 100

using namespace std;

	string* traitement_look(string& affectation) {
//pas nécessairement dans la classe message (messageatraiter, faut voir...)

/*THEORIQUEMENT LAFFECTATION EST RECUPEREE DANS LE MESSAGE ET PAS PASSEE EN PARAMETRE*/

//il vaudrait mieux faire une méthode par requete et message::traitement appellerait ces méthodes
//il vaudrait mieux diviser le code en méthode


		//recherche nom
			//reception_frontale)->affectation,statut
			//string affectation=message reçu affectation;
        //A decommenter apres le test
        // TODO : il faudrait insérer un en-tête avant d'ajouter l'affectation demandée
        string nouveau = affectation;
        Message msg(QString(nouveau.data()), 'S', '*');
        msg.entete();
        const unsigned char key[]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF}; 
        msg.chiffrement(key);
        client cli;

        cli.socBind();
        cli.emission(msg.getMsg());

        /*
        Creer une fonction pour la reception du message client adaptée à la requete de demande de ressource
        reception ser;
        ser.ecoute();
        //attendre connexion

        */
			//reception_client(nom) si client.affectation==affectation
			char * nom = (char*)malloc(200*sizeof(char));
                 char * statut = (char*)malloc(200*sizeof(char));
			unsigned char hash[SHA_DIGEST_LENGTH];
			char hashString[SHA_DIGEST_LENGTH*2+1];
			char * token=(char*)malloc(200*sizeof(char));
			char * hashlist=(char*)malloc(200*sizeof(char));
                 char * listnom =(char*)malloc(200*sizeof(char));
			string fichier = "test.txt";
			/**remplacement pour test**/
			/*************************************/

/* TODO : Gestion BDD à insérer */


/*lecture dans le fichier (IE le message reçu)*/
			ifstream file(fichier.c_str(), ios::in); 
        		if(file){ 
        			size_t buffsize=100;
        			char *line=(char*)malloc(buffsize*sizeof(char));
        			string sline=line;
        			string tokStr;
        			while (getline(file,sline)){
        				line = &sline[0];			
						nom = strtok(line, " ");
						statut = strtok(NULL," ");
						//next token
						token =strtok(NULL," ");
						tokStr = token;
						//token = Affectation
						if (affectation == tokStr){	//line : nom statut affectation

			/**Recuperation du nom des clients et hashage***/
							SHA_CTX ctx;
    						      SHA1_Init(&ctx);
    						      SHA1_Update(&ctx, statut, strlen(statut));
   						 	SHA1_Final(hash, &ctx);

   						 	
  							//for (int j = 0; j < SHA_DIGEST_LENGTH; j++)
      					 	//	 sprintf(&hashString[j*2], "%02x", (unsigned int)hash[j]);
      					 	//(possibilité d'affichage du SHA1 en hexa)
 							//strcat(hashlist," ");
 							strcat(hashlist,(const char*)hash);
 							strcat(hashlist,"\n");
                                          strcat(listnom,(const char*)nom);
                                          strcat(listnom,"\n");
 						} 
 					}
 					file.close();		
  					//free(token);  
 					//free(line);
 /*probleme de libération mémoire : interaction bizarre depuis line = &line[0] qui fait que je dois d'abord libérer
 slin mais que je ne peux pas parce que c'est un string? à voir.*/

                    //envoi_frontale(nom)
 	           		string listStr = string(hashlist, sizeof(hashlist));
                       string nomStr = string(listnom, sizeof(hashlist)); 
                       string *finalList = new string[2];
                       finalList[0] = listStr;
                       finalList[1] = nomStr;
 	           		return finalList;

    	    	}
        		else {
        			printf("error : ouverture impossible!\n");
        			exit(EXIT_FAILURE);
           			
           		}
        		
			

	}


  string traitement_exist(string& status) {
//retourne juste le nom hashÃ©
      string readStatus;
      string hashlist;
      string hashS;
    char*nom=(char*)malloc(sizeof(char)*30);
      string fichier = "test.txt";
        char *hash=(char*)malloc(sizeof(char)*SHA_DIGEST_LENGTH);
      /**remplacement pour test**/
      /*************************************/
         ifstream file(fichier.c_str(), ios::in);
         if(file){
                 char* line= (char*)malloc(BUF_SIZE * sizeof(char));
                 string sline=line;
                 while (getline(file,sline)){
                       line=(char*)sline.c_str();
                         nom = strtok(line, " ");
                         readStatus = string(strtok(NULL," "));
                         if (status == readStatus){    //line : nom statut affectation
                         /**Recuperation du nom des clients et hashage***/
                                SHA_CTX ctx;
                                SHA1_Init(&ctx);
                                SHA1_Update(&ctx, nom, strlen(nom));
                                SHA1_Final((unsigned char*)hash, &ctx);
                                string hashS= hash;
                                hashlist += hashS;
                                hashlist += "\n";
                          }
                  }
                  file.close();
             //     free(hash);
            //      free(nom);
            //      free(line);
                  //envoi_frontale(nom) 
                  return hashlist;
          }
          else {
                  printf("error : ouverture impossible!\n");
                  exit(EXIT_FAILURE);
       }
}

string* traitement_lookrec(string& datatype, string& status) {
//rÃ©cupÃ©ration des rÃ©fÃ©rences et du hash des noms
    string *finalList = new string[2];
      string readStatus;
      char*nom=(char*)malloc(sizeof(char)*30);
      char* hash=(char*)malloc(sizeof(char)*SHA_DIGEST_LENGTH);
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
                 char* line= (char*)malloc(BUF_SIZE * sizeof(char));
                 string sline=line;
                 while (getline(file,sline)){
    //on reÃ§oit nom statut reference
                     line=(char*)sline.c_str();
                         nom = strtok(line, " ");
                         readStatus = string(strtok(NULL," "));
                         readDataType=string(strtok(NULL," "));
             reference=string(strtok(NULL," "));
                         if (status == readStatus && datatype==readDataType){    //line : nom status datatype rÃ©fÃ©rence
                         /**Recuperation du nom des clients et hashage***/
                                SHA_CTX ctx;
                                SHA1_Init(&ctx);
                                SHA1_Update(&ctx, nom, strlen(nom));
                                SHA1_Final((unsigned char*)hash, &ctx);
                                hashS=hash;
                listRef += reference;
                listRef += "\n";
                                hashlist+= hashS;
                                hashlist+= "\n";
                          }
                  }
                  file.close();
                  //free(line);
            //free(nom);
             free(hash);
                  //envoi_frontale(nom) 
                  finalList[0] = listRef;
            finalList[1] = hashlist;
                  return finalList;
          }
          else {
                  printf("error : ouverture impossible!\n");
                  exit(EXIT_FAILURE);
       }
}

string traitement_pull(string& reference, vector<string>& groupes_client ) {
//rÃ©cupÃ©ration du document
   string readReference;
//readGroups : token du fichier contenant la liste des groupes séparés par /
//readGroup : token de readGroups
   char* readGroups=(char*)malloc(sizeof(char)*100);
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
                char* line= (char*)malloc(BUF_SIZE * sizeof(char));
                string sline=line;
                while (getline(file,sline)){  //line ref document groupes
                     line=(char*)sline.c_str();
                         readReference = string(strtok(line, " "));
                         document = string(strtok(NULL," "));
                         readGroups = strtok(NULL," ");
                         //init groupe
                         readGroup = string(strtok(readGroups,"/"));
                         if (reference==readReference){
               while (!readGroup.empty()) {
                for (int j=0;j<groupes_client.size();j++){
                  //cout <<"comparaison de : "<<readGroup << " avec " <<groupes_client[j]<<" pour la référence " << reference << " vs " << readReference<<endl;
                  if (readGroup==groupes_client[j]){  
                          file.close();
                    //        free(line);
                    //        free(readGroups);
                    return document;
                  }
                            }
                            readGroup = string(strtok(NULL,"/"));
                          }
                        }
                        //inc groupe
                 }
                 file.close();
                 return NULL;
    //           free(line);
   //            free(readGroups);
                  //envoi_frontale(nom) 
          }

          else {
                  printf("error : ouverture impossible!\n");
                  exit(EXIT_FAILURE);
          }
}