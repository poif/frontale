#include <openssl/sha.h>
#include "requete.h"
#include "traitement.h"  
#include <algorithm> 



using namespace std;

Requete::Requete()
{
pourThomas = false; //Ce booleen est initialisé à false
}

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
bool Requete::getPourThomas()
{	return pourThomas;}

void Requete::setResultat(string buffer)
{	m_resultat = buffer;}

//Methods

/* =======================================================================================================================
	FONCTION TRI : TRI ET CONSTRUIT LE RESULTAT A ENVOYER AU CLIENT
=========================================================================================================================*/


int Requete::tri(list<string>& reponse) //tri les resultats recu et garde les Ã©lÃ©ments nÃ©cessaire suivant le type de requÃªte => construit la requete Ã  envoyer sur le rÃ©seau
{
//CONSIDEREES DU MEME TOKEN
//PAS BESOIN DUN VECTOR<STRING>, on gÃ¨re les rÃ©ponses l'une aprÃ¨s l'autre
	int isError=1;	//1 si aucune rÃ©ponse n'est valide
	vector<string> reponseTraitees = vector<string>();	//pareil que pour les reponses clients
	int afficherNum=0;
	int verboseTest=0;

	for (list<string>::iterator it = reponse.begin(); it != reponse.end(); it++){
		//recup dans l'iss de la reponse
		istringstream ss(*it);
		string numero; getline(ss, numero, '*');	//rÃ©cupÃ©ration du numÃ©ro pour chaque rÃ©ponse

		if (afficherNum==0){
			afficherNum=1;
			cout << "=================================================================================="<<endl;
			cout << "Traitement des réponses à la requête " << numero <<" envoyées par les frontales."<<endl;
			cout << "=================================================================================="<<endl;
		}
		cout << "Traitement de la réponse "<< *it << endl;
		string name;
		string hash_recu;
		string hash;
		string condensate;
		string reference;
		string inTheVector;
		string data;

		if (*it == numero+"*ERROR*") {
			cout << "La requête n'a pas eu de réponse positive de la part de cette frontale."<<endl;
			continue;
		}

		else isError = 0;

		if(m_action.compare("search") == 0) {
		/*****REQUETE 1*****/
			if(m_option.compare("-n") == 0){
				m_requete = "R*1*";
				verboseTest = 1;
				cout << "=================================================================================="<<endl;
				cout << "Hash du statut envoyé par le client émetteur."<<endl;
				hash = hashString((char*)m_statut.c_str());
				while(getline(ss, name, '*') && getline(ss, hash_recu, '*')) {
					cout << "**"<<endl;
					cout << "Traitement du couple : " << name << " " << hash_recu << endl;
					cout << "Comparaison du hash reçu avec le hash du statut intialement envoyé."<<endl;
					if(hash.compare(hash_recu) == 0){
						reponseTraitees.push_back(name);	//UN NOM PAR LIGNE DANS LE TABLEAU
						cout << "Le nom " << name << " a été ajouté à la réponse finale renvoyée au client émetteur!" << endl;
					}
					else
						cout << "Les hashs ne correspondent pas."<<endl;
					cout << "**"<<endl;
				}

			/*ELIMINATION DES DOUBLONS*/
				/*ON DOIT TRIER LE TABLEAU*/
				cout << "Elimination d'éventuels doublons..."<<endl;
				sort(reponseTraitees.begin(), reponseTraitees.end() );
				reponseTraitees.erase( unique( reponseTraitees.begin(), reponseTraitees.end() ), reponseTraitees.end() );

			/*REPONSES FINALES*/
				for (unsigned int i=0; i<reponseTraitees.size(); i++){
					m_resultat += reponseTraitees[i] + "*";
				}
			}

		/*****REQUETE 2*****/
			else if (m_option.compare("-e") ==0) {
				condensate = m_nom + m_statut;
				hash = hashString((char*)condensate.c_str());
				cout << "=================================================================================="<<endl;
				cout << "Hash de la concaténation nom+statut, paramètres envoyés par le client émetteur."<<endl;
				m_resultat = "R*2*0";
				//si aucune rÃ©ponse n'est "vraie", reste Ã  no, sinon, dira yes
				while(getline(ss, hash_recu, '*')) {
					cout << "Comparaison du hash reçu " << hash_recu << " avec le hash généré."<<endl;
					//s'il y en a un, on arrÃªte
					if(hash.compare(hash_recu) ==0){ 
						m_resultat = "R*2*1";
						cout << "Les hashs correspondent."<<endl;
						cout << "On envoie "<< m_resultat << "indiquant que la personne cherchée existe."<<endl;
						cout << "Puisque l'on a trouvé la personne, on arrête le traitement."<<endl;
						cout <<"=================================================================================="<<endl;
						return 1;	//on a trouvÃ©, c'est tout ce qu'on voulait
					}
					else {
						cout << "Les hashs ne correspondent pas."<<endl;
					}
				}
				cout <<"=================================================================================="<<endl;
			}

		/*****REQUETE 3*****/
			else if (m_option.compare("-r") ==0 && pourThomas == false) //Si ce booleen est à false => c'est la requête à envoyer à Thomas
			{
				pourThomas = true; //Ce message est pour Thomas (comme ça la frontale sait qu'il faut lui envoyer à lui et non pas au client)
				condensate += m_nom;
				cout <<"=================================================================================="<<endl;		
				hash = hashString((char*)condensate.c_str());
				cout <<"Hash du nom envoyé par le client émetteur."<<endl;
				while (getline(ss,reference,'*') && getline(ss,hash_recu,'*')) {
					cout << "Comparaison du hash reçu "<< hash_recu << " avec le hash généré."<<endl;
					if(hash.compare(hash_recu) ==0){
						reference = m_reference;
						cout << "Le hash correspond, la référence "<<reference<< " est ajoutée à la réponse!"<<endl;
						cout << "On a obtenu la référence voulue, on arrête le traitement!"<<endl;
						m_requete = "4*" + m_statut + "*none*" + m_affectation + "*none*" + m_groupe + "*none*none*" + reference + "*none*";
						cout << "On renvoie la requête de récupération de données : " << m_requete<<endl;
						cout <<"=================================================================================="<<endl;		
						return 1;
					}
					else {
						cout <<"Les hashs ne correspondent pas."<< endl;
					}
				}
				cout << "Aucune référence sélectionnée pour cette frontale."<<endl;
			}


			else {	//1.6 GIGAWAT? (ne peut jamais arriver?)
				cerr << "Tri : Option inconnu" << endl ;
				isError = 1;
				break;
			}
		}

		else if(m_action.compare("group") ==0)
		{
			string found_group;
			cout <<"=================================================================================="<<endl;		
			while(getline(ss, found_group, '*')){
				if (found_group == "1"){		
					m_resultat= "R*6*1";
					cout << "Le groupe cherché a été trouvé, on renvoie la message " << m_requete << "au client et on arrête le traitement."<<endl;
					cout <<"=================================================================================="<<endl;			
					return 1 ;
				}
			}
			m_resultat = "R*6*0";
			cout <<"Le groupe souhaité n'a pas été trouvé dans cette frontale."<<endl;
			cout <<"=================================================================================="<<endl;		
		}


		else if(m_action.compare("insert") ==0 || m_action.compare("seek") ==0 || m_action.compare("delete") ==0 || m_action.compare("select") ==0) // Si c'Ã©tait une interaction bdd, il faut juste retransmettre le message au client			
		{
				m_resultat = *it;
				cout << "Interaction BDD : on retransmet la requête "<< *it << "à la BDD."<<endl;
				pourThomas = false; // On remet le booleen à false
		}

		else if(m_action.compare("search") && m_option.compare("-r") && pourThomas == true) // Si c'est la requête 4 (sachant que la requête pour Thomas a déjà était envoyé car pourThomas = true), cette fois ci c'est pour le client
		{
			getline(ss, data, '*');
			if(data != "ERROR")
			{
				m_resultat = "R*4*"+data;
				cout << "La donnée demandée a été trouvée!"<<endl;
				cout << "On la renvoie au client sous la forme " << m_resultat << endl;
				cout << "On arrête le traitement."<<endl;
				cout <<"=================================================================================="<<endl;		
				return 1;
			}
			cout << "La donnée n'a pas été trouvée dans la réponse de cette frontale!"<<endl;
			cout <<"=================================================================================="<<endl;		
		}


		else {
			cerr << "Tri : Option inconnu" << endl ;
			isError = 1;
			break;
		}
	}

	if (verboseTest==1){
		cout << "La réponse finale renvoyée au client émetteur est pour le moment : " << m_resultat <<"."<< endl;
		cout <<"=================================================================================="<<endl;
	}


	if (isError==1)	{ //Tout est erreur, ou l'option/action est invalide
		cerr << "Erreur de traitement!" << endl;
		m_resultat = "R*ERROR";
		cout << "On renvoie R*ERROR au client";
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
		if(m_option.compare("-n") == 0){ // Si on cherche un nom
			m_requete = "1*none*none*" + m_affectation + "*none*none*none*none*none*none*" ;
			cout << "Formatage de la requête de recherche d'une personne: " << m_requete << endl;
		}

		else if(m_option.compare("-e") == 0) {// Si on cherche l'existance
			m_requete = "2*none*none*" + m_affectation + "*none*none*none*none*none*none*" ;
			cout << "Formatage de la requête de recherche de l'existence d'une personne: " << m_requete << endl;
		}

		else if(m_option.compare("-r") == 0) {// Si on cherche une photo(donnÃ©e)
			m_requete = "3*none*" + m_statut_cible + "*none*" + m_affectation_cible + "*none*" + m_groupe_cible + "*" + m_option + "*none*none*";
			cout << "Formatage de la requête de recherche de référence: " << m_requete << endl;
		}
		else
			cerr << "Option inconnue" << endl ;
	}
//requete 6
	else if(m_action.compare("group") == 0) // Action groupe
        {
                pourBdd=false;
                m_requete = "6*none*none*none*none*none*" + m_parametre + "*none*none*none*";
                cout << "Formatage de la requête : " << m_requete << endl;
        }

	else if(m_action.compare("insert") == 0) // Cas d'ajout d'une donneÃ© dans la bdd
	{
		string toHash = m_nom+m_statut;
		hash = hashString((char*)toHash.c_str());
	    pourBdd=true;
	    m_requete = "302*";
	    m_requete += m_statut + "*" + m_affectation + "*" + m_groupe + m_option + "*" + m_parametre +"*"+ hash +"*EOF";
	    cout << "Formatage de la requête d'insertion d'une donnée dans la BDD : " << m_requete << endl;
	}

	else if(m_action.compare("delete") == 0)
	{
	    pourBdd=true;
	    string toHash = m_nom+m_statut;
		hash = hashString((char*)toHash.c_str());
	    m_requete = "303*"+m_parametre+"*"+hash+"*EOF";
	    cout << "Formatage de la requête de supression d'une donnée dans la BDD : " << m_requete << endl;
	}

	else
		cerr << "Action inconnue" << endl ;
}


/*===========================================================================================
	FONCTION DE DECOUPAGE : DECOUPE LA REQUETE RECU DU CLIENT (1ere fonction appelÃ©e)
===========================================================================================*/

int Requete::decoupage(string chaine)
{
	istringstream ss(chaine);
	cout << "Découpage de la requête client." << endl;
	//remplissage + test error!
	if(m_action.compare("search") && m_option.compare("-r"))  //cas particulier de decoupage pour requete 3
	{
		if(!getline(ss, m_affectation_cible, '*') ||
			!getline(ss, m_statut_cible, '*') ||
			!getline(ss, m_action, '*') ||
			!getline(ss, m_option, '*') ||
			!getline(ss, m_parametre, '*') ||
			!getline(ss, m_nom, '*') ||
			!getline(ss, m_affectation, '*') ||
			!getline(ss, m_statut, '*') ||
			!getline(ss, m_groupe_cible, '*') ||
			!getline(ss, m_groupe, '*'))
				return 0;
	}
	else
	{
		if (!getline(ss, m_affectation, '*') ||
			!getline(ss, m_statut, '*') ||
			!getline(ss, m_action, '*') ||
			!getline(ss, m_option, '*') ||
			!getline(ss, m_parametre, '*')) 
			return 0;
		else {
		//it's ok
			if(m_action.compare("insert") == 0 || m_action.compare("delete") == 0) {// Si c'est une requÃªte pour la bdd on a un champ en plus : le nom de la personne
				if (!getline(ss, m_nom, '*')) 
					return 0;
			}

			if(m_action.compare("insert") == 0) {	// Et si la requete est insert => champ supplÃ©mentaire : politique de partage
				if (!getline(ss, m_partage, '*')) 
					return 0;
			}
			if (!getline(ss, m_groupe, '*') || !getline(ss, m_cle, '*')) 
				return 0;
			return 1;
		}
	}
	return 1;
}

