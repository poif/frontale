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
#include "client.h"
#include <QtNetwork>


using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);


	//thread
	reception ser;
	ser.ecoute();

	const unsigned char key[]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF}; 

	QString message;
	while(1){
		ser.ecoute();

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

			QString retour;
			retour = QString("%1").arg(req.getResultat());

			Message msg2(retour,'R', '*');
			msg2.entete();
			msg2.chiffrement(key);

			//string toto = msg2.getMsg().toStdString();
			//cout << toto << endl;

			client cli;
			cli.socBind();
			cli.emission(msg2.getChiffre());

		}

	}

	//thread

	//    network_interface net;



	return a.exec();
}
