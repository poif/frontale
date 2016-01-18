#ifndef TRAITEMENT
#define TRAITEMENT

#include <string>
#include <vector>
#include <list>


std::string traitement_req_client(std::string numero,
								  std::string action, 
								  std::string statut,
								  std::string statut_cible,
								  std::string affectation, 
								  std::string affectation_cible,
								  std::string groupes_client, 
								  std::string groupes_cible,
								  std::string typeData, 
								  std::string ref, 
								  std::string user);

std::string traitement_req_bdd(std::string numero,
							   std::string action,
							   std::string statut, 
							   std::string statut_cible,
							   std::string affectation, 
							   std::string affectation_cible,
							   std::string groupes_client, 
							   std::string groupes_cible,
							   std::string typeData, 
							   std::string ref, 
							   std::string user);

std::string traitement_rep_client(std::list<std::string>& reponses);
std::string hashString(const char * to_hash);
std::string HexFormate(const unsigned char * hash, size_t length);

#endif