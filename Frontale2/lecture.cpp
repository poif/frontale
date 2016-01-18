#include "lecture.h"
#include <sstream>
lecture::lecture()
{
}

string lecture::getAdresse_bdd(){

    ifstream fichier("config.ini",ios::in);

    if(fichier)
    {
        string contenu;
        getline(fichier, contenu);

        istringstream iss;
        iss.str(contenu);

        getline(iss,contenu,' ');
        getline(iss,contenu,' ');
        adresse_bdd = contenu;

        fichier.close();
    }else
    {

    }

    return adresse_bdd;
}

int lecture::getPort_bdd(){

    int port;

    ifstream fichier("config.ini",ios::in);

    if(fichier)
    {
        string contenu;
        for(int i=0;i<2; i++)
            getline(fichier, contenu);

        istringstream iss;
        iss.str(contenu);

        getline(iss,contenu,' ');
        getline(iss,contenu,' ');

        port=stoi(contenu);

        fichier.close();
        return port;
    }else
    {

        return 0;
    }
}

string lecture::getServerCentrale(){
    ifstream fichier("config.ini",ios::in);

    if(fichier)
    {
        string contenu;
        for(int i=0;i<3; i++)
            getline(fichier, contenu);

        istringstream iss;
        iss.str(contenu);

        getline(iss,contenu,' ');
        getline(iss,contenu,' ');

        return contenu;
    }else
    {

        return "erreur";
    }
}

string lecture::getServerProvider(){
    ifstream fichier("config.ini",ios::in);

    if(fichier)
    {
        string contenu;
        for(int i=0;i<4; i++)
            getline(fichier, contenu);

        istringstream iss;
        iss.str(contenu);

        getline(iss,contenu,' ');
        getline(iss,contenu,' ');

        return contenu;
    }else
    {
  
        return "erreur";
    }
}
