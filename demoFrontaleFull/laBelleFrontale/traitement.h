#ifndef TRAITEMENT
#define TRAITEMENT

#include <string>
#include <vector>

std::string* traitement_look(std::string& affectation);
std::string traitement_exist(std::string& status);
std::string* traitement_lookrec(std::string& datatype, std::string& status);
std::string traitement_pull(std::string& reference, std::vector<std::string>& groupes_client);

#endif