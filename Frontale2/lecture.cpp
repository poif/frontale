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
        cout << "erreur de lecture du fichier de config" << endl;

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
        cout << "erreur de lecture du fichier de config" << endl;
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
        cout << "erreur de lecture du fichier de config" << endl;
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
        cout << "erreur de lecture du fichier de config" << endl;
        return "erreur";
    }
}
