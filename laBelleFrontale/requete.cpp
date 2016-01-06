#include <openssl/sha.h>
#include "requete.h"
#include "traitement.h"  
#include <algorithm> 



using namespace std;

Requete::Requete()
{}

string Requete::getToken()
{	return m_token;}
string Requete::getAffectation()
{	return m_affectation;}
string Requete::getStatut()
{	return m_statut;}
string Requete::getAction()
{       return m_action;}
string Requete::getOption()
{       return m_option;}
string Requete::getParametre()
{       return m_parametre;}
string Requete::getGroupe()
{       return m_groupe;}
string Requete::getGroupeCible()		//ADDED
{       return m_groupe_cible;}
string Requete::getStatutCible()		//ADDED
{       return m_statut_cible;}
string Requete::getAffectationCible()	//ADDED
{       return m_affectation_cible;}
string Requete::getCle()
{       return m_cle;}
string Requete::getRequete()
{	return m_requete;}
string Requete::getResultat()
{	return m_resultat;}
bool Requete::getPourBdd()
{	return pourBdd;}

void Requete::setResultat(string buffer)
{	m_resultat = buffer;}

//Methods

/* =======================================================================================================================
	FONCTION TRI : TRI ET CONSTRUIT LE RESULTAT A ENVOYER AU CLIENT
=========================================================================================================================*/


int Requete::tri(list<string>& reponse) //tri les resultats recu et garde les éléments nécessaire suivant le type de requête => construit la requete à envoyer sur le réseau
{
//CONSIDEREES DU MEME TOKEN
//PAS BESOIN DUN VECTOR<STRING>, on gère les réponses l'une après l'autre
	int isError=1;	//1 si aucune réponse n'est valide
	vector<string> reponseTraitees = vector<string>();	//pareil que pour les reponses clients
	for (list<string>::iterator it = reponse.begin(); it != reponse.end(); it++){
		//recup dans l'iss de la reponse
		istringstream ss(*it);
		string numero; getline(ss, numero, '*');	//récupération du numéro pour chaque réponse
		string name;
		string hash_recu;
		string hash;
		string condensate;
		string reference;
		string inTheVector;

		if (*it == numero+"*ERROR*") continue; //on passe à la reponse suivante
		else isError = 0;

		if(m_action.compare("search") == 0) {
		/*****REQUETE 1*****/
			if(m_option.compare("-n") == 0) {
		        hash = hashString((char*)m_statut.c_str());
				while(getline(ss, name, '*') && getline(ss, hash_recu, '*')) {
					if(hash.compare(hash_recu) == 0)
						reponseTraitees.push_back(name);	//UN NOM PAR LIGNE DANS LE TABLEAU
				}

			/*ELIMINATION DES DOUBLONS*/
				/*ON DOIT TRIER LE TABLEAU*/
				sort(reponseTraitees.begin(), reponseTraitees.end() );
				reponseTraitees.erase( unique( reponseTraitees.begin(), reponseTraitees.end() ), reponseTraitees.end() );

			/*REPONSES FINALES*/
				for (unsigned int i=0; i<reponseTraitees.size(); i++){
					m_resultat += reponseTraitees[i] + "*";
				}
			}

		/*****REQUETE 2*****/
			else if (m_option.compare("-e") ==0) {
/*ADDED*/				condensate = m_nom + m_statut;
/*ADDED*/				hash = hashString((char*)condensate.c_str());
				m_resultat = "R*no";
				//si aucune réponse n'est "vraie", reste à no, sinon, dira yes
				while(getline(ss, hash_recu, '*')) {
					//s'il y en a un, on arrête
					if(hash.compare(hash_recu) ==0){ 
						m_resultat = "R*yes";
						return 1;	//on a trouvé, c'est tout ce qu'on voulait
					}
				}
			}

		/*****REQUETE 3*****/
			else if (m_option.compare("-p") ==0) {
/*ADDED*/			condensate += m_nom+m_statut;
				hash = hashString((char*)condensate.c_str());
/*A CHANGER*/			while (getline(ss,reference,'*') && getline(ss,hash_recu,'*')) {
/*REQ3->ID*3*statut*affectation*gp_cible*typedata*/
						if(hash.compare(hash_recu) ==0)
							reference = m_reference;
				}
				m_requete = "4*" + m_statut + "*none*" + m_affectation + "*none*" + m_groupe + "*none*none*" + reference + "*none*";
				/*TODO ENVOI*/
				break; //bonjour à Amine
			}

			else {	//1.6 GIGAWAT? (ne peut jamais arriver?)
				cerr << "Tri : Option inconnu" << endl ;
				isError = 1;
				break;
			}
		}

		else if(m_action.compare("insert") ==0 || m_action.compare("seek") ==0 || m_action.compare("delete") ==0 || m_action.compare("select") ==0) // Si c'était une interaction bdd, il faut juste retransmettre le message au client
			m_resultat = *it;

		else {
			cerr << "Tri : Option inconnu" << endl ;
			isError = 1;
			break;
		}
	}

	if (isError==1)	{ //Tout est erreur, ou l'option/action est invalide
		cerr << "Erreur de traitement!" << endl;
		m_resultat = "R*ERROR";
		return 0;
	}
	else
		return 1;
}


/*=========================================================================================
	FONCTION DE CONSTRUCTION : CONSTRUIT LA REQUETE A ENVOYER (AU RESEAU OU A LA BDD)
=========================================================================================*/

void Requete::construction() //construit la requete suivant action, option et parametre
{
    	string hash; 
	if(m_action.compare("search") == 0) // Fonction recherche
	{
		pourBdd=false;
		if(m_option.compare("-n") == 0) // Si on cherche un nom
			m_requete = "1*none*none*" + m_affectation + "*none*none*none*none*none*none*" ;

		else if(m_option.compare("-e") == 0) // Si on cherche l'existance
			m_requete = "2*none*none*" + m_affectation + "*none*none*none*none*none*none*" ;

		else if(m_option.compare("-p") == 0) // Si on cherche une photo(donnée)
			m_requete = "3*none*" + m_statut_cible + "*none*" + m_affectation_cible + "*none*" + m_groupe_cible + "*" + m_option + "*none*none*";

		else
			cerr << "Option inconnue" << endl ;
	}

	else if(m_action.compare("insert") == 0) // Cas d'ajout d'une donneé dans la bdd
	{
/*ADDED*/ string toHash = m_nom+m_statut;
		hash = hashString((char*)toHash.c_str());
	    pourBdd=true;
	    m_requete = "302*";
	    m_requete += m_statut + "*" + m_affectation + "*" + m_groupe + m_option + "*" + m_parametre +"*"+ hash +"*EOF";
	}

	else if(m_action.compare("delete") == 0)
	{
	    pourBdd=true;
	    string toHash = m_nom+m_statut;
		hash = hashString((char*)toHash.c_str());
	    m_requete = "303*"+m_parametre+"*"+hash+"*EOF";
	}

	else
		cerr << "Action inconnue" << endl ;
}


/*===========================================================================================
	FONCTION DE DECOUPAGE : DECOUPE LA REQUETE RECU DU CLIENT (1ere fonction appelée)
===========================================================================================*/

int Requete::decoupage(string chaine)
{
	istringstream ss(chaine);
	//remplissage + test error!
	if (!getline(ss, m_affectation, '*') ||
		!getline(ss, m_statut, '*') ||
		!getline(ss, m_action, '*') ||
		!getline(ss, m_option, '*') ||
		!getline(ss, m_parametre, '*')) 
		return 0;
	else {
	//it's ok
		if(m_action.compare("insert") == 0 || m_action.compare("delete") == 0) {// Si c'est une requête pour la bdd on a un champ en plus : le nom de la personne
			if (!getline(ss, m_nom, '*')) 
				return 0;
		}
		if(m_action.compare("insert") == 0) {	// Et si la requete est insert => champ supplémentaire : politique de partage
			if (!getline(ss, m_partage, '*')) 
				return 0;
		}
		if (!getline(ss, m_groupe, '*') || !getline(ss, m_cle, '*')) 
			return 0;
		return 1;
	}
}
