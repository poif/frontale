
#include <openssl/sha.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <iostream>

#define BUF_SIZE 100

using namespace std;

string* traitement_look(string& affectation) {
  //reception_frontale)->affectation,statut
  //string affectation=message reÃ§u affectation;
        //A decommenter apres le test
        /*string nouveau = affectation;
        Message msg(QString(nouveau.data()), 'S', '*');
        msg.entete();
        //msg.chiffrement();
        client cli;
        cli.socBind();
        cli.emission(msg.getMsg());

        reception ser;
        ser.ecoute();*/
        //attendre connexion

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
          hash[SHA_DIGEST_LENGTH] = '\0';
          hashS=string(hash,SHA_DIGEST_LENGTH);
        hashlist += hashS; 
        hashlist += "*";
        listnom  += nom; 
        listnom  += "*";
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
                                hash[SHA_DIGEST_LENGTH] = '\0';
                                hashS=string(hash,SHA_DIGEST_LENGTH);
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
                listRef += "*";
                                hashlist+= hashS;
                                hashlist+= "*";
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