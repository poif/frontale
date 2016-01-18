1.) lancer le ServerCentral : ./ServerCentral dans le repertoire ServerCentrale 
2.) lancer le server de noeud : ./SecureNodeListProvider dans le repertoire du meme nom
3.) lancer les noeuds : ./NoeudThor <adresseServerCentrale> 8080 <adresseServerNoeud> 8081
4.) dans le repertoir SBDD : lancer mysql mysql -u root -p
ensuite lancer le script bdd.sql : source bdd.sql
sortez de mysql : exit
puis lancer la base de données : ./prog_bdd

5.) dans Frontale , reglé les adresses et ports des differents server et de la base de données dans config.ini (port bdd: 22666)
puis lancez la Frontale : ./Frontale
6.) le client : dans le repertoir Cleint reglé l'adresse de la frontale et vos données personnels 
puis lancez le client ./client
