1.) installer qt : sudo apt-get install qt4-dev-tools
2.) installer cryptopp : sudo apt-get install libcrypto++
si le package n'est pas trouvé lancer : 
sudo apt-cache pkgnames | grep -i crypto++
OU (dépend des distributions)
sudo apt-cache pkgnames | grep -i cryptopp
puis adapter la premiere commande en fonction des résultats des suivantes
3.) OpenSSL est installé nativement normalement mais sinon, l'installer
4.) télécharger boost 1.59.0 (.tar.gz) puis
lancer le script (mettre l'archive dans votre home):
#!/bin/bash

tar zxf boost_1_59_0.tar.gz
cd boost_1_59_0
sudo ./bootstrap.sh
sudo ./b2 install

5.) localiser l'emplacement des lib boost installées (chez moi c'est /usr/local/lib/)
6.) se rendre dans le dossier de votre projet (genre dans le dossier de laBelleFrontale)
7.) executer la commande : qmake -project
8.) editer laBelleFrontale.proj en ajoutant : 'QT += network' (sans les ')
à la fin du fichier
9.) executer la commande : qmake
10.) éditer le Makefile généré :
la ligne CXX doit ressembler à cela :
CXX           = g++ -std=c++11
la ligne INCPATH doit ressembler à cela (la modif se trouve à la fin):
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++-64 -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtNetwork -I/usr/include/qt4/QtGui -I/usr/include/qt4 -I. -I. -I/votreCheminHome/vers/boost_1_59_0/
la ligne LIB doit ressembler à cela (les modifs se trouvent après -lpthread):
LIBS          = $(SUBLIBS)  -L/usr/lib/x86_64-linux-gnu -lQtGui -lQtNetwork -lQtCore -lpthread /usr/local/lib/libboost_system.a /usr/local/lib/libboost_thread.a /usr/local/lib/libboost_date_time.a /usr/local/lib/libboost_regex.a /usr/local/lib/libboost_serialization.a -lssl -lcrypto -lcrypto++
modifier -lcrypto++ par -lcryptopp en fonction du package qui a dû être installé (si vous avez installé crypto++ c'est pas la peine)
!!Pour être sûr que ça fonctionne, ne pas c/c les lignes entières mais recopier seulement ce qui a été rajouté, et adapter le chemin des libs boost (/usr/local/lib/ chez moi) par rapport au vôtre!!
11.) exécuter la commande : make
si tout se passe bien, il y a aucune erreur, juste quelques warnings
12.) vous pouvez exécuter le programme : ./laBelleFrontale, mais pour l'instant ça ne sert à rien.

NB : les étapes 6 à 12 sont à effectuer à chaque compilation d'un nouveau projet (si vous faites des modifs sur le projet courant il suffit juste d'executer make)
