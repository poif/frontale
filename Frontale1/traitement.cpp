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
    to_send += ref + "*";           //ref
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

  else if (action=="6"){             //recherche d'un groupe
      to_send += groupes_cible +"*EOF";
  }               

  return to_send;
}

 
/////////////////////////
/*******REPONSES********/
/////////////////////////

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
    string statut;
    string reference;
    string token;   
    string Hnom;
    string document;
    string tokenEOF;        //hey are you EOF?
	
	/*LE NUMERO NE SERA RENVOYEE QU'UNE SEULE FOIS*/

	/*Le numero empêche les problèmes avec des actions différentes*/


		if (action=="1") {
		//RECHERCHE UTILISATEUR : RECUPERATION DU NOM ET DU HASH STATUT
			if (getline(ss, nom, '*') && getline(ss, statut, '*') && getline(ss,tokenEOF,'*') &&
        nom != "none" && statut != "none" &&
        nom != "EOF"  && statut != "EOF" && tokenEOF == "EOF") {
			  stringInTheVector += nom + "*" + hashString((char*)statut.c_str())+"*";
			}
		}

	/****************************************/

		else if (action=="2"){
		//RECHERCHE EXISTENCE DUNE PERSONNE : RECUPERATION DU NOM ET DU STATUT
			if (getline(ss, nom, '*') && getline(ss, statut, '*') && getline(ss, tokenEOF, '*')&& 
          nom != "EOF"  && statut != "EOF" &&
          nom != "none" && statut != "none"&&
          tokenEOF == "EOF") {
			//valide -> renvoi hash(nom+statut)
        string to_hash = nom + statut;
        cout << "le cond : "<< to_hash << endl;
        char *hash = (char*)to_hash.c_str();
				stringInTheVector += hashString(hash) + "*";
      }
		}

	/****************************************/
 
		else if (action == "3"){
	     //RECHERCHE STATUT*REFERENCE*NOM
      while (getline(ss , token , '*') && token != "EOF" && token != "none"){
        istringstream ssToken(token);
        if (getline(ssToken, reference, ';') && getline(ssToken, nom, ';')){
          char *hash = (char*)nom.c_str();
          stringInTheVector += reference + "*" + hashString(hash) + "*";
        }
      }
    }

	/*************************************************/

	  else if (action == "4"){
		//RECUPERATION DOCUMENT
      if (getline(ss, document, '*') && getline(ss, tokenEOF, '*') && 
        document != "EOF" && document != "none" && tokenEOF == "EOF"){
        stringInTheVector = document + "*";
      }
    }

  /************************************************/

    else if (action == "6"){
    //RECHERCHE DUN GROUPE
      string yn; 
      if (getline(ss,yn,'*') && getline(ss,tokenEOF,'*') && (yn !="0" || yn != "1")
          && tokenEOF == "EOF")
        stringInTheVector = yn;
    }


		/**********************************************************/

    else if (action == "300"){
    //déja hashé!
      while (getline(ss , token , '*') && token != "EOF" && token != "NULL"){
        istringstream ssToken(token);
        if (getline(ssToken, reference, ';') && getline(ssToken, Hnom, ';')){
          stringInTheVector += reference + "*" + Hnom + "*";
        }
      }
    }

    /*********************************************************/

    else if (action == "301"){
      //RECUPERATION DOCUMENT
      if (getline(ss, document, '*') && getline(ss, tokenEOF, '*') && 
        document != "EOF" && document != "NULL" && tokenEOF == "EOF"){
        stringInTheVector = document + "*";
      }
    }

    //else{}

		/*****************************************************/

		if (!stringInTheVector.empty())   
      vectToSend.push_back(stringInTheVector);
		//on ajoute une réponse traitée dans le vector
	}

  if (vectToSend.size()==0) 
    to_send += "ERROR*";
 
	for (unsigned int i=0;i<vectToSend.size();i++){
			to_send += vectToSend[i];
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


