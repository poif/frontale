#include <QCoreApplication>
#include <message.h>
#include "requete.h"
#include "engine_event.h"
#include "network_interface.h"
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <reception.h>
#include <QTextStream>
#include <string>
#include "clientFront.h"
#include <QtNetwork>
#include "bdd_tcp.h"

using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	
	QHostAddress hoteCourant;
	quint16 portCourant;

	bool respBool = false;

	time_t t0;
    	unsigned tmax = 15;

	//thread
	reception ser;

	bdd_tcp bdd;

	network_interface netinf(&bdd);
    	std::thread tspawn = netinf.spawnThread();

    	bdd.connection_tcp("127.0.0.1",2211);
	const unsigned char key[]={0xB0,0xA1,0x73,0x37,0xA4,0x5B,0xF6,0x72,0x87,0x92,0xFA,0xEF,0x7C,0x2D,0x3D,0x4D, 0x60,0x3B,0xC5,0xBA,0x4B,0x47,0x81,0x93,0x54,0x09,0xE1,0xCB,0x7B,0x9E,0x17,0x88}; 

	QString message;
	QString versBdd;

	while(1){

		hoteCourant = ser.getHostAddr();
		portCourant = ser.getHostPort();
		ser.ecoute(-1); // timeout= -1 == pas de timeout

		message = ser.getMsg();

		cout << "Got somethin" << endl;
		Message msg(message,'*');
		Requete req;

		//string * showRep = new string[2];
		string showRep;

		QTextStream(stdout) << ser.getMsg() << endl;	        

		msg.dechiffrement(key);

		QTextStream(stdout) << msg.getMsg() << endl;

		if(req.decoupage(msg.getMsg().toStdString())){
			req.construction();

			if(req.getPourBdd()){
				versBdd = QString("%1").arg(req.getRequete().c_str());

				bdd.emission(versBdd);
				
				bdd.attendLecture();

    				req.tri(bdd.getMsg().toStdString());							
			}
			else
			{
				string traitement = req.getRequete();

				string option = req.getOption();

				cout << option << endl;
				cout << traitement << endl;

				if(option.compare("-n")==0){/* traitement est une affectation */
					netinf.send_look(traitement);
					cout << "ici envoi" << endl;
				}
				else if(option.compare("-e")==0){/* traitement est un statut */
					netinf.send_exist(traitement);
				}
				if(option.compare("-p")==0){
					string type = "picture";
					netinf.send_lookrec(type,traitement);
				}		


				t0 = time(NULL);
				std::cout << "Attente " << tmax << " secondes" << std::endl;

				while((respBool = netinf.getRecbool()) == false && static_cast<unsigned>(time(NULL)-t0) < tmax);
				std::cout << tmax << "s écoulées" << std::endl;

				if(respBool){

				netinf.setRecbool(false);

				string showRep = netinf.getResp();

				netinf.setResp("");

				cout << showRep << endl;
				//cout << showRep[1] << endl;

				string sep = "*";

				//char * triq = new char(showRep[0].size()+showRep[1].size()+sep.size());
				
				/*char * triq = new char(showRep.size());
				//int triqlength = showRep[0].size()+showRep[1].size()+sep.size();
				int triqlength = showRep.size();
				//triq = const_cast <char *>((showRep[0] + sep + showRep[1]).data());
				triq = const_cast <char *>(showRep.data());
				triq[triqlength] = '\0';*/

				/*cout << triqlength << endl;
				  cout << strlen(triq) << endl;
				  cout.write(triq, triqlength);*/

				req.tri(showRep);

				}
				else{
					cout << "Temps maximum écoulé, pas de réponse" << endl;
					req.setResultat("empty");
				}

			}

			QString retour;
			retour = QString("%1").arg(req.getResultat().c_str());

			Message msg2(retour,'R', '*');
			msg2.entete();
			msg2.chiffrement(key);

			string toto = msg2.getMsg().toStdString();
			cout << toto << endl;

			clientFront cli;
			cli.socBind();
			cli.emission(msg2.getChiffre(), hoteCourant, portCourant);

		}

	}

	//thread

	//    network_interface net;



	return a.exec();
}
