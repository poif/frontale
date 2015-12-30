#include "Tslot.h"

using namespace std;

int main(){
 
	Tslot ts;

	string token;

	cout << "Génération du token..." << endl;
	token = ts.GenToken(QHostAddress("127.0.0.1"),8173);
	cout << "Token généré." << endl;

 	ts.startTimer(token);
	cout << "Tout nu !" << endl;

	return 0;

}