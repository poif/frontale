
#include <openssl/sha.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <iostream>

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
        //hashlist += "\n";
        listnom  += nom; 
        //listnom  += "\n";
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