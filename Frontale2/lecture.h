#ifndef LECTURE_H
#define LECTURE_H
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class lecture
{
    string adresse_bdd;

public:
    lecture();
    string getAdresse_bdd();
    int getPort_bdd();
    string getServerCentrale();
    string getServerProvider();
};

#endif // LECTURE_H
