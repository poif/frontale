#include <openssl/sha.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include "traitement.h"
using namespace std;

string * traitement_look(string& affectation) {
  string nom;
  char* statut;
  char* hash=(char*)malloc(SHA_DIGEST_LENGTH*sizeof(char));
  string readAffectation;
  string hashlist;
  string hashS;
  string *finalList = new string[2];
  string listnom;
  string fichier = "test.txt";
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
              SHA_CTX ctx;
              SHA1_Init(&ctx);
              SHA1_Update(&ctx, statut, strlen(statut));
              SHA1_Final((unsigned char*)hash, &ctx);
              hashS=string(hash,strlen(hash));
              hashlist += hashS; 
              hashlist += "\n";
              listnom  += nom; 
              listnom  += "\n";
            }
       } 
       file.close();
       free(hash);
        //envoi_frontale(nom) 
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
//retourne juste le nom hashÃ©
      string readStatus;
      string hashlist;
      string hashS;
      char *nom;
      string fichier = "test.txt";
      char *hash=(char*)malloc(sizeof(char)*SHA_DIGEST_LENGTH);
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
                SHA_CTX ctx;
                SHA1_Init(&ctx);
                SHA1_Update(&ctx, nom, strlen(nom));
                SHA1_Final((unsigned char*)hash, &ctx);                      hashS=hash;
                hashlist += hashS;
                hashlist += "\n";
            }
         }
         file.close();
         free(hash);
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
                 char* line;
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
                  //envoi_frontale(nom) 
          }

          else {
                  printf("error : ouverture impossible!\n");
                  exit(EXIT_FAILURE);
          }
}

string req_bdd(string& action, string& statut, string& affectation, vector<string>& groupes_client, string& typeData, string& ref, string& user){
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
    for (int i = 0; i < groupes_client.size(); i++)
      to_send += groupes_client[i] + ";";
    to_send += "*";
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

  cout << "chaine   " << to_send<< endl;
  //envoi
  return to_send;
  //void?
}


