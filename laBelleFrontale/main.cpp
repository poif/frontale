#include <QCoreApplication>
#include <message.h>
#include <requete.h>
#include "engine_event.h"
#include "network_interface.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <reception.h>
#include <QTextStream>
#include <string>
#include "clientFront.h"
#include <QtNetwork>
#include "emission_tcp.h"
#include "reception_tcp.h"
using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);


	//thread
	reception ser;

	emission_tcp emi;

    	emi.connection_tcp("127.0.0.1",32154);
	const unsigned char key[]={0xB0,0xA1,0x73,0x37,0xA4,0x5B,0xF6,0x72,0x87,0x92,0xFA,0xEF,0x7C,0x2D,0x3D,0x4D, 0x60,0x3B,0xC5,0xBA,0x4B,0x47,0x81,0x93,0x54,0x09,0xE1,0xCB,0x7B,0x9E,0x17,0x88};
	
	reception_tcp recep; 


	QString message;
	QString versBdd;
	while(1){
		ser.ecoute(-1); // timeout= -1 == pas de timeout

		message = ser.getMsg();;


		Message msg(message,'*');
		Requete req;

		//string * showRep = new string[2];
		string showRep;

		QTextStream(stdout) << ser.getMsg() << endl;	        

		msg.dechiffrement(key);

		QTextStream(stdout) << msg.getMsg() << endl;

		if(req.decoupage(msg.getMsg().toStdString().c_str())){
			req.construction();
			recep.bind();		
			if(req.getPourBdd()){
				versBdd = QString("%1").arg(req.getRequete());
				emi.emission(versBdd);
				
    				if(!recep.getConnexion()){
        				//envoyer message d'erreur timeout   					
    				}else{
           				recep.attenteLecture();

         				// fonction pour interpreter le retour de la bdd(succes ou fail)  recep.getMsg() << endl;
    				}							
			}
			else
			{
				string temp = req.getRequete();

				/* Il faudrait plutôt utiliser une réference vers l'objet de type network_interface qui doit rester unique */
				network_interface netinf;

				showRep = netinf.send_look(temp);

				cout << showRep << endl;
				//cout << showRep[1] << endl;

				string sep = "*";

				//char * triq = new char(showRep[0].size()+showRep[1].size()+sep.size());
				char * triq = new char(showRep.size());
				//int triqlength = showRep[0].size()+showRep[1].size()+sep.size();
				int triqlength = showRep.size();
				//triq = const_cast <char *>((showRep[0] + sep + showRep[1]).data());
				triq = const_cast <char *>(showRep.data());
				triq[triqlength] = '\0';
				/*cout << triqlength << endl;
				  cout << strlen(triq) << endl;
				  cout.write(triq, triqlength);*/

				req.tri(triq);
			}
			QString retour;
			retour = QString("%1").arg(req.getResultat());

			Message msg2(retour,'R', '*');
			msg2.entete();
			msg2.chiffrement(key);

			//string toto = msg2.getMsg().toStdString();
			//cout << toto << endl;

			clientFront cli;
			cli.socBind();
			cli.emission(msg2.getChiffre());

		}

	}

	//thread

	//    network_interface net;



	return a.exec();
}
