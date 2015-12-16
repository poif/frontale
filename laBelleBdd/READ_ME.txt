Le service BDD a besoin pour fonctionner des packages suivants sur un système à jour :
    - MySql (sudo apt-get install mysql-server)
    - MySql header (sudo apt-get install libmysqlclient-dev)
    - OpenSSL (sudo apt-get install openssl)
    - OpenSSl header (package : openssl-dev / openssl-devel / libssl-dev (pour Ubuntu/Debian prendre le dernier))

Paramétrer MySql :
    - connectez-vous avec le compte root (sudo mysql -u root -p)
    - lancez le script bdd.sql (mysql> source <CHEMIN>/bdd.sql) (si vous avez lancé mysql dans le répertoire de BDD : source ./bdd.sql)
    WARNING : Ne tenez pas compte des erreurs (supression d'utilisateurs inexistants lors du premier appel)
    - quitez l'invite de commandes MySql (exit)

Avant de lancer le programme :
    - vérifiez que le service mysql fonctionne (sudo service mysql status)
    - lancez le service mysql si besoin (sudo service mysql start)
    - vous devez avoir exécuté au moins une fois le script bdd.sql (le programme vous retournera une erreur si ce n'est pas le cas)
