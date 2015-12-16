/** ====================================================================
**   Auteur  : Delvarre / Bourillon      | Date    : DD/MM/2015
**  --------------------------------------------------------------------
**   Langage : C                         | Systeme : Linux
**  --------------------------------------------------------------------
**   Nom fichier : bdd_request.c         | Version : 1.0
**  --------------------------------------------------------------------
**   Description : /
** =====================================================================*/
#include "bdd.h"
#include "crypt.h"
#include "res.h"


//----------------------------------------------------------
// int bdd_parser ( char *trame )
//----------------------------------------------------------
// Permet de traiter la trame reçu par la frontale de la forme ACTION*CHAMPS...*EOF

int bdd_parser ( char *trame )
{
    // Déclaration variable
    int flag = 0 ;              // Premier entier de la trame qui détermine ce qu'on doit faire
    char* morceau = NULL ;      // Variable contenant chaque morceau de la trame (strtok_r)
    char* safe_trame ;          // Permet d'appeler strtok_r sur le bon contenu.

    // On découpe la trame en morceaux repérés via un "*"
    if ( ( morceau = strtok_r ( trame, "*", &safe_trame ) ) == NULL )
    {
        perror ( "Erreur_bdd_parser : information manquante " ) ;
        bdd_send_msg ( ERROR, "ERROR" ) ;
        return ERRNO ;
    }

    // On vérifie que l'information est présente
    if ( ( ( strcmp ( morceau, "NULL") ) == 0 ) || ( strlen ( morceau ) == 0 ) )
    {
        perror ( "Erreur_bdd_do_insert : information manquante " ) ;
        bdd_send_msg ( ERROR, "ERROR" ) ;
        return ERRNO ;
    }

    // Le premier morceau est le flag qui détermine l'action à traiter
    flag = atoi ( morceau ) ;

    // En fonction du flag, on appelle le bon traitement
    switch ( flag )
    {
        case INSERT :
            if ( bdd_do_insert ( safe_trame ) == ERRNO )
            {
                perror ( "Erreur_bdd_parser : bdd_do_insert() " ) ;
                return ERRNO ;
            }
            break ;

        case DELETE :
            if ( bdd_do_delete ( safe_trame ) == ERRNO )
            {
                perror ( "Erreur_bdd_parser : bdd_do_delete() " ) ;
                return ERRNO ;
            }
            break ;

        case SEEK :
            if ( bdd_do_seek ( safe_trame ) == ERRNO )
            {
                perror ( "Erreur_bdd_parser : bdd_do_seek() " ) ;
                return ERRNO ;
            }
            break ;

        case SELECT :
            if ( bdd_do_select ( safe_trame ) == ERRNO )
            {
                perror ( "Erreur_bdd_parser : bdd_do_select() " ) ;
                return ERRNO ;
            }
            break ;

        default :
            perror ( "Erreur_bdd_parser : flag incorrect " ) ;
            bdd_send_msg ( flag, "ERROR" ) ;
            return ERRNO ;
            break ;
    }

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int bdd_do_insert ( char *safe_trame )
//----------------------------------------------------------
// Permet de traiter une trame de type insertion : 302*statut*affectation*groupes*type*val*user*politique*EOF

int bdd_do_insert ( char *safe_trame )
{
    // Déclaration variable
    Ref modele ;                    // Modèle propre à la trame qui sera appliqué en requête sur la BDD
    char* morceau = NULL ;          // Variable contenant chaque morceau de la trame (strtok_r)
    char* safe_morceau = NULL ;     // Variable contenant chaque morceau de la trame (strtok_r)
    char* groupes = NULL ;
    char* groupes_test = NULL ;
    int groupes_free = FALSE ;
    char* valeur = NULL ;
    char* user = NULL ;
    char* politiques = NULL ;
    char* politiques_test = NULL ;
    int politiques_free = FALSE ;
    int cpt = 0 ;                   // Compteur

    // On découpe les 7 éléments restant de notre trame
    for ( cpt = 0; cpt < 7; cpt++ )
    {
        // On découpe la trame en morceaux repérés via un "*"
        if ( ( morceau = strtok_r ( NULL, "*", &safe_trame ) ) == NULL )
        {
            perror ( "Erreur_bdd_do_insert : information manquante " ) ;
            if ( politiques_free == TRUE )
                free ( politiques ) ;
            if ( groupes_free == TRUE )
                free ( groupes ) ;
            bdd_send_msg ( INSERT, "ERROR" ) ;
            return ERRNO ;
        }

        // On vérifie que l'information est présente
        if ( strcmp ( morceau, "EOF") == 0 )
        {
            perror ( "Erreur_bdd_do_insert : information manquante II " ) ;
            if ( politiques_free == TRUE )
                free ( politiques ) ;
            if ( groupes_free == TRUE )
                free ( groupes ) ;
            bdd_send_msg ( INSERT, "ERROR" ) ;
            return ERRNO ;
        }
        else
        {
            // On affecte l'information
            switch ( cpt )
            {
                case ZERO :
                    modele.statut =  multiple ( calculate_SHA256 ( morceau ) ) ;
                    break ;

                case UN :
                    modele.affectation =  multiple ( calculate_SHA256 ( morceau ) ) ;
                    break ;

                case DEUX :
                    groupes_test = morceau ;
                    groupes = (char*) malloc ( ( strlen ( groupes_test ) + 1 ) * sizeof ( char ) ) ;
                    memset ( groupes, '\0', strlen ( groupes_test ) + 1 ) ;
                    sprintf ( groupes, "%s", groupes_test ) ;
                    groupes_free = TRUE ;
                    break ;

                case TROIS :
                    modele.type =  multiple ( calculate_SHA256 ( morceau ) ) ;
                    break ;

                case QUATRE :
                    valeur = morceau ;
                    break ;

                case CINQ :
                    user = morceau ;
                    break ;

                case SIX :
                    politiques_test = morceau ;
                    politiques = (char*) malloc ( ( strlen ( politiques_test ) + 1 ) * sizeof ( char ) ) ;
                    memset ( politiques, '\0', strlen ( politiques_test ) + 1 ) ;
                    sprintf ( politiques, "%s", politiques_test ) ;
                    politiques_free = TRUE ;
                    break ;

                default :
                    perror ( "Erreur_do_insert : overflow compteur cpt " ) ;
                    if ( politiques_free == TRUE )
                        free ( politiques ) ;
                    if ( groupes_free == TRUE )
                        free ( groupes ) ;
                    bdd_send_msg ( INSERT, "ERROR" ) ;
                    return ERRNO ;
                    break ;
            }
        }
    }

    // On vérifie le format des politiques
    morceau = NULL ;
    cpt = 0 ;
    do
    {
        // On découpe
        if ( cpt == 0 )
            morceau = strtok_r ( politiques_test, "$", &safe_morceau ) ;
        else
            morceau = strtok_r ( NULL, "$", &safe_morceau ) ;

        // On inc cpt
        cpt++ ;
    } while ( morceau != NULL ) ;
    if ( cpt != 4 )
    {
        perror ( "Erreur_bdd_do_insert : politiques() " ) ;
        free ( politiques ) ;
        free ( groupes ) ;
        bdd_send_msg ( INSERT, "ERROR" ) ;
        return ERRNO ;
    }

    // On compte le nombre de groupes
    morceau = NULL ;
    safe_morceau = NULL ;
    int nbr_grp = 0 ;
    do
    {
        // On découpe
        if ( nbr_grp == 0 )
            morceau = strtok_r ( groupes_test, ";", &safe_morceau ) ;
        else
            morceau = strtok_r ( NULL, ";", &safe_morceau ) ;

        // On inc cpt
        nbr_grp++ ;
    } while ( morceau != NULL ) ;

    // PHASE 2
    // Déclaration variables
    char* reference_str = NULL ;
    char* valeur_finale = NULL ;
    char* reponse = NULL ;
    char* temporaire = NULL ;
    int first = TRUE ;
    unsigned long taille = 0 ;

    // Ini
    reference_str = (char*) malloc ( TAILLE_MAX_REF * sizeof ( char ) ) ;
    memset ( reference_str, '\0', TAILLE_MAX_REF ) ;
    taille = strlen ( valeur ) + strlen ( user ) + strlen ( politiques ) + 3 ;
    valeur_finale = (char*) malloc ( taille * sizeof ( char ) ) ;
    memset ( valeur_finale, '\0', taille ) ;
    temporaire = (char*) malloc ( TAILLE_MAX_REF * nbr_grp * sizeof ( char ) ) ;
    memset ( temporaire, '\0', TAILLE_MAX_REF * nbr_grp ) ;

    // On créé autant de référence qu'il y a de groupes
    morceau = NULL ;
    safe_morceau = NULL ;
    cpt = 0 ;
    do
    {
        // On découpe
        if ( cpt == 0 )
            morceau = strtok_r ( groupes, ";", &safe_morceau ) ;
        else
            morceau = strtok_r ( NULL, ";", &safe_morceau ) ;

        // On vérifie
        if ( morceau != NULL )
        {
            // On prépare la référénce qui va être inséreée en BDD
            modele.groupe =  multiple ( calculate_SHA256 ( morceau ) ) ;
            memset ( reference_str, '\0', TAILLE_MAX_REF ) ;
            if ( ref_concatenation ( &modele, reference_str )  == ERRNO )
            {
                perror ( "Erreur_bdd_do_insert : ref_concatenation() " ) ;
                free ( politiques ) ;
                free ( reference_str ) ;
                free ( valeur_finale ) ;
                free ( groupes ) ;
                free ( temporaire ) ;
                bdd_send_msg ( INSERT, "ERROR" ) ;
                return ERRNO ;
            }

            // On prépare la valeur_final qui va être insrérée en BDD
            memset ( valeur_finale, '\0', taille ) ;
            sprintf ( valeur_finale, "%s*%s*%s", user, valeur, politiques ) ;

            // On réalise l'insertion
            if ( bdd_do_request ( mysql_bdd, INSERT, reference_str, valeur_finale, NULL ) == ERRNO )
            {
                perror ( "Erreur_bdd_do_insert : bdd_do_request() " ) ;
                free ( politiques ) ;
                free ( reference_str ) ;
                free ( valeur_finale ) ;
                free ( groupes ) ;
                free ( temporaire ) ;
                bdd_send_msg ( INSERT, "ERROR" ) ;
                return ERRNO ;
            }
            else
                nbr_tuples++ ;

            // On ajoute la référence à la réponse
            if ( first == TRUE )
            {
                sprintf ( temporaire, "%s", reference_str ) ;
                first = FALSE ;
            }
            else if ( first == FALSE )
                sprintf ( temporaire + strlen ( temporaire ), ";%s", reference_str ) ;

            // On inc cpt
            cpt++ ;
        }
    } while ( morceau != NULL ) ;

    // On envoie la réponse
    taille = strlen ( temporaire ) + 10 ;
    reponse = (char*) malloc ( taille * sizeof ( char ) ) ;
    memset ( reponse, '\0', taille ) ;
    sprintf ( reponse, "%d*%s*EOF", INSERT, temporaire ) ;
    res_send ( reponse ) ;

    // Free
    free ( politiques ) ;
    free ( reference_str ) ;
    free ( valeur_finale ) ;
    free ( groupes ) ;
    free ( temporaire ) ;
    free ( reponse ) ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int bdd_do_delete ( char *safe_trame )
//----------------------------------------------------------
// Permet de traiter une trame de type suppression : 303*ref*user*EOF

int bdd_do_delete ( char *safe_trame )
{
    // Déclaration variable
    char* morceau = NULL ;      // Variable contenant chaque morceau de la trame (strtok_r)
    char* reference = NULL ;
    char* user = NULL ;
    char* user_ref = NULL ;
    int cpt = 0 ;               // Compteur

    // On découpe les 2 éléments restant de notre trame
    for ( cpt = 0; cpt < 2; cpt++ )
    {
        // On découpe la trame en morceaux repérés via un "*"
        if ( ( morceau = strtok_r ( NULL, "*", &safe_trame ) ) == NULL )
        {
            perror ( "Erreur_bdd_do_delete : information manquante " ) ;
            bdd_send_msg ( DELETE, "ERROR" ) ;
            return ERRNO ;
        }

        // On vérifie que l'information est présente
        if ( strcmp ( morceau, "EOF") == 0 )
        {
            perror ( "Erreur_bdd_do_delete : information manquante II" ) ;
            bdd_send_msg ( DELETE, "ERROR" ) ;
            return ERRNO ;
        }
        else
        {
            // On affecte l'information
            switch ( cpt )
            {
                case ZERO :
                    reference = morceau ;
                    break ;

                case UN :
                    user = morceau ;
                    break ;

                default :
                    perror ( "Erreur_do_delete : overflow compteur cpt " ) ;
                    bdd_send_msg ( DELETE, "ERROR" ) ;
                    return ERRNO ;
                    break ;
            }
        }
    }

    // On récupère auprès de la BDD, le user associé à cette référence (hash)
    user_ref = (char*) malloc ( TAILLE_HASH_NOM * sizeof ( char ) ) ;
    memset ( user_ref, '\0', TAILLE_HASH_NOM ) ;
    if ( bdd_search_nom ( reference, user_ref ) == ERRNO )
    {
        perror ( "Erreur_bdd_do_delete : bdd_search_nom() " ) ;
        free ( user_ref ) ;
        bdd_send_msg ( DELETE, "ERROR" ) ;
        return ERRNO ;
    }

    // On compare la valeur avec la valeur user reçue dans la trame
    if ( strcmp ( user, user_ref ) == 0 )
    {
        // L'utilisateur est propriétaire, on exécute le delete
        if ( bdd_do_request ( mysql_bdd, DELETE, reference, NULL, NULL ) == ERRNO )
        {
            perror ( "Erreur_bdd_do_delete : bdd_do_request() " ) ;
            free ( user_ref ) ;
            bdd_send_msg ( DELETE, "ERROR" ) ;
            return ERRNO ;
        }
        else
            nbr_tuples-- ;
    }
    else
    {
        printf ( "Erreur_bdd_do_delete : bad user " ) ;
        free ( user_ref ) ;
        bdd_send_msg ( DELETE, "BAD-PERMISSION" ) ;
        return FALSE ;
    }

    // Free
    free ( user_ref ) ;

    // On indique que tout s'est bien déroulé
    bdd_send_msg ( DELETE, "DONE" ) ;
    return TRUE ;
}


//----------------------------------------------------------
// int bdd_do_seek ( char *safe_trame )
//----------------------------------------------------------
// Permet de traiter une trame de type recherche liste référence : 300*statut*affectation*groupe*type*EOF

int bdd_do_seek ( char *safe_trame )
{
    // Déclaration variable
    char* morceau = NULL ;      // Variable contenant chaque morceau de la trame (strtok_r)
    Ref modele ;
    int relevant = FALSE ;
    int cpt = 0 ;               // Compteur

    // On découpe les 4 éléments restant de notre trame
    for ( cpt = 0; cpt < 4; cpt++ )
    {
        // On découpe la trame en morceaux repérés via un "*"
        if ( ( morceau = strtok_r ( NULL, "*", &safe_trame ) ) == NULL )
        {
            perror ( "Erreur_bdd_do_seek : information manquante " ) ;
            bdd_send_msg ( SEEK, "ERROR" ) ;
            return ERRNO ;
        }

        // On vérifie que l'information est présente
        if ( strcmp ( morceau, "EOF" ) == 0 )
        {
            perror ( "Erreur_bdd_do_seek : information manquante II" ) ;
            bdd_send_msg ( SEEK, "ERROR" ) ;
            return ERRNO ;
        }
        else
        {
            // On regarde si l'information est relevante
            if ( ( strcmp ( morceau, "NULL" ) ) == 0 )
                    relevant = FALSE ;
            else
                relevant = TRUE ;

            // On affecte l'information
            switch ( cpt )
            {
                case ZERO :
                    modele.statut = relevant ? calculate_SHA256 ( morceau ) : 0 ;
                    break ;

                case UN :
                    modele.affectation = relevant ? calculate_SHA256 ( morceau ) : 0 ;
                    break ;

                case DEUX :
                    modele.groupe = relevant ? calculate_SHA256 ( morceau ) : 0 ;
                    break ;

                case TROIS :
                    modele.type = relevant ? calculate_SHA256 ( morceau ) : 0 ;
                    break ;

                default :
                    perror ( "Erreur_do_seek : overflow compteur cpt " ) ;
                    bdd_send_msg ( SEEK, "ERROR" ) ;
                    return ERRNO ;
                    break ;
            }
        }
    }

    // Maintenant qu'on a le modele de référence, on cherche toutes les références y répondant
    if ( bdd_search_ref ( &modele ) == ERRNO )
    {
        perror ( "Erreur_bdd_do_seek : bdd_search_ref() " ) ;
        return ERRNO ;
    }

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int bdd_do_select ( char *safe_trame )
//----------------------------------------------------------
// Permet de traiter une trame de type selection : 301*statut*affectation*groupes*ref*EOF

int bdd_do_select ( char *safe_trame )
{
    // Déclaration variable
    char* morceau = NULL ;          // Variable contenant chaque morceau de la trame (strtok_r)
    char* statut = NULL ;
    char* affectation = NULL ;
    char* groupes = NULL ;
    char* reference = NULL ;
    int cpt = 0 ;                   // Compteur
    MYSQL_RES* resultat_req ;       // Object MYSQL décrivant le résultat d'une requête
    MYSQL_ROW tuple ;               // Object MYSQL décrivant un tuple dans la liste résultat
    char* safe_tuple = NULL ;       // Permet d'appeler strtok_r sur le bon contenu.
    char* valeur = NULL ;
    char* politique = NULL ;
    int code_retour = 0 ;

    // On découpe les 4 éléments restant de notre trame
    for ( cpt = 0; cpt < 4; cpt++ )
    {
        // On découpe la trame en morceaux repérés via un "*"
        if ( ( morceau = strtok_r ( NULL, "*", &safe_trame ) ) == NULL )
        {
            perror ( "Erreur_bdd_do_select : information manquante " ) ;
            bdd_send_msg ( SELECT, "ERROR" ) ;
            return ERRNO ;
        }

        // On vérifie que l'information est présente
        if ( strcmp ( morceau, "EOF") == 0 )
        {
            perror ( "Erreur_bdd_do_select : information manquante II" ) ;
            bdd_send_msg ( SELECT, "ERROR" ) ;
            return ERRNO ;
        }
        else
        {
            // On affecte l'information
            switch ( cpt )
            {
                case ZERO :
                    statut = morceau ;
                    break ;

                case UN :
                    affectation = morceau ;
                    break ;

                case DEUX :
                    groupes = morceau ;
                    break ;

                case TROIS :
                    reference = morceau ;
                    break ;

                default :
                    perror ( "Erreur_do_select : overflow compteur cpt " ) ;
                    bdd_send_msg ( SELECT, "ERROR" ) ;
                    return ERRNO ;
                    break ;
            }
        }
    }

    // On récupère auprès de la BDD, la valeur associée à cette référence
    if ( bdd_do_request ( mysql_bdd, SELECT, reference, NULL, &resultat_req ) == ERRNO )
    {
        perror ( "Erreur_bdd_do_select : bad requete " ) ;
        mysql_free_result ( resultat_req ) ;
        bdd_send_msg ( SELECT, "ERROR" ) ;
        return ERRNO ;
    }

    // On récupère le tuple
    if ( ( tuple = mysql_fetch_row ( resultat_req ) ) ==  NULL )
    {
        perror ( "Erreur_bdd_do_select : bad tuple " ) ;
        mysql_free_result ( resultat_req ) ;
        bdd_send_msg ( SELECT, "ERROR" ) ;
        return ERRNO ;
    }

    // On déchiffre le tuple
    char* output = (char*) malloc ( RSA_size ( bdd_key ) * sizeof ( char ) ) ;
    memset ( output, '\0', RSA_size ( bdd_key ) ) ;
    if ( RSA_chiffrement ( (unsigned char*) tuple[0], (unsigned char*) output, DECHIFFREMENT ) != TRUE )
    {
        perror ( "Erreur_bdd_do_select : RSA_dechiffrement " ) ;
        free ( output ) ;
        mysql_free_result ( resultat_req ) ;
        bdd_send_msg ( SELECT, "ERROR" ) ;
        return ERRNO ;
    }

    // On récupère les 3 informations du tuple
    morceau = NULL ;
    for ( cpt = 0; cpt < 3; cpt++ )
    {
        // On découpe le tuple en morceaux repérés via un "*"
        if ( cpt == 0 )
            morceau = strtok_r ( output, "*", &safe_tuple ) ;
        else
            morceau = strtok_r ( NULL, "*", &safe_tuple ) ;

        // Vérification
        if ( morceau == NULL )
        {
            perror ( "Erreur_bdd_do_select : information manquante III " ) ;
            free ( output ) ;
            mysql_free_result ( resultat_req ) ;
            bdd_send_msg ( SELECT, "ERROR" ) ;
            return ERRNO ;
        }

        // On affecte l'information
        switch ( cpt )
        {
            case ZERO :
                break ;

            case UN :
                valeur = morceau ;
                break ;

            case DEUX :
                politique = morceau ;
                break ;

            default :
                perror ( "Erreur_do_select : overflow compteur cpt " ) ;
                free ( output ) ;
                mysql_free_result ( resultat_req ) ;
                bdd_send_msg ( SELECT, "ERROR" ) ;
                return ERRNO ;
                break ;
        }
    }

    // On regarde les politique de partage de la ressource
    morceau = NULL ;
    safe_tuple = NULL ;
    for ( cpt = 0; cpt < 3; cpt++ )
    {
        // On découpe les politiques de partage en morceaux repérés via un "$"
        if ( cpt == 0 )
            morceau = strtok_r ( politique, "$", &safe_tuple ) ;
        else
            morceau = strtok_r ( NULL, "$", &safe_tuple ) ;
        if ( morceau == NULL )
        {
            perror ( "Erreur_bdd_do_select : information manquante IV " ) ;
            free ( output ) ;
            mysql_free_result ( resultat_req ) ;
            bdd_send_msg ( SELECT, "ERROR" ) ;
            return ERRNO ;
        }

        // On regarde d'abord si on est en accès interdit
        if ( strcmp ( morceau, "-" ) == 0 )
        {
            printf ( "Erreur_bdd_do_select : BAD-PERMISSION " ) ;
            free ( output ) ;
            mysql_free_result ( resultat_req ) ;
            bdd_send_msg ( SELECT, "BAD-PERMISSION" ) ;
            return FALSE ;
        }
        else if ( strcmp ( morceau, "+") != 0 ) // Puis si on n'est pas en accès total
        {
            // Alors on traite le cas par cas
            switch ( cpt )
            {
                case ZERO : // Statut
                    code_retour = bdd_verification_partage ( morceau, statut ) ;
                    break ;

                case UN :   // Affectation
                    code_retour = bdd_verification_partage ( morceau, affectation ) ;
                    break ;

                case DEUX : // Groupes
                    code_retour = bdd_verification_partage ( morceau, groupes ) ;
                    break ;

                default :
                    perror ( "Erreur_do_select : overflow compteur cpt " ) ;
                    free ( output ) ;
                    mysql_free_result ( resultat_req ) ;
                    bdd_send_msg ( SELECT, "ERROR" ) ;
                    return ERRNO ;
                    break ;
            }

            // On vérifie le code retour
            if ( code_retour == FALSE )
            {
                printf ( "Erreur_bdd_do_select : bad permission " ) ;
                free ( output ) ;
                mysql_free_result ( resultat_req ) ;
                bdd_send_msg ( SELECT, "BAD-PERMISSION" ) ;
                return FALSE ;
            }
            else if ( code_retour == ERRNO )
            {
                printf ( "Erreur_bdd_do_select : bdd_verification_partage " ) ;
                free ( output ) ;
                mysql_free_result ( resultat_req ) ;
                bdd_send_msg ( SELECT, "ERROR" ) ;
                return ERRNO ;
            }
        }
    }

    // Si on arrive ici c'est que la poltique de partage est respectée : on retourne la valeur
    int taille = strlen ( valeur ) + 10 ;
    char* reponse = (char*) malloc ( taille * sizeof ( char ) ) ;
    memset ( reponse, '\0', taille ) ;
    sprintf ( reponse, "%d*%s*EOF", SELECT, valeur ) ;
    res_send ( reponse ) ;

    // On free l'espace de résultat
    free ( output ) ;
    mysql_free_result ( resultat_req ) ;
    free ( reponse ) ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int bdd_search_nom ( char* reference, char* nom )
//----------------------------------------------------------
// Permet de récupérer le nom associé à une référence (hash)

int bdd_search_nom ( char* reference, char* nom )
{
    // Initialisation
    MYSQL_RES* resultat_req ;       // Object MYSQL décrivant le résultat d'une requête
    MYSQL_ROW tuple ;               // Object MYSQL décrivant un tuple dans la liste résultat
    char* safe_tuple = NULL ;       // Permet d'appeler strtok_r sur le bon contenu.

    // On commence par remonter la valeur de la référence
    if ( bdd_do_request ( mysql_bdd, SELECT, reference, NULL, &resultat_req ) == ERRNO )
    {
        perror ( "Erreur_bdd_search_nom : bad requete " ) ;
        mysql_free_result ( resultat_req ) ;
        return ERRNO ;
    }

    // On récupère le tuple
    if ( ( tuple = mysql_fetch_row ( resultat_req ) ) ==  NULL )
    {
        perror ( "Erreur_bdd_search_nom : bad tuple " ) ;
        mysql_free_result ( resultat_req ) ;
        return ERRNO ;
    }

    // On déchiffre le tuple
    char* output = (char*) malloc ( RSA_size ( bdd_key ) * sizeof ( char ) ) ;
    memset ( output, '\0', RSA_size ( bdd_key ) ) ;
    if ( RSA_chiffrement ( (unsigned char*) tuple[0], (unsigned char*) output, DECHIFFREMENT ) != TRUE )
    {
        printf ( "ERR = %s  %lu \n", output, strlen ( output ) ) ;
        perror ( "Erreur_bdd_serach_nom : RSA_dechiffrement " ) ;
        free ( output ) ;
        mysql_free_result ( resultat_req ) ;
        return ERRNO ;
    }

    // On récupère le nom dans le tuple
    sprintf ( nom, "%s", strtok_r ( output, "*", &safe_tuple ) ) ;

    // On free l'espace de résultat
    mysql_free_result ( resultat_req ) ;
    free ( output ) ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int bdd_search_ref ( Ref* modele )
//----------------------------------------------------------
// Permet de trouver toutes les correspondances à un modèle avec le nom de l'utilisateur associé à la référence (hash)

int bdd_search_ref ( Ref* modele )
{
    // Initialisation
    MYSQL_RES* resultat_req ;       // Object MYSQL décrivant le résultat d'une requête

    // On commence par remonter toutes les références
    if ( bdd_do_request ( mysql_bdd, SELECT_ALL, NULL, NULL, &resultat_req ) == ERRNO )
    {
        perror ( "Erreur_bdd_search_ref : bad requete " ) ;
        mysql_free_result ( resultat_req ) ;
        bdd_send_msg ( SEEK, "ERROR" ) ;
        return ERRNO ;
    }

    // Puis on cherche toutes les références associées à notre modèle
    if ( bdd_search_modele ( resultat_req, modele ) == ERRNO )
    {
        perror ( "Erreur_bdd_search_ref : bad search " ) ;
        mysql_free_result ( resultat_req ) ;
        return ERRNO ;
    }

    // On free l'espace de résultat
    mysql_free_result ( resultat_req ) ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int bdd_search_modele ( MYSQL_RES* resultat_req, Ref* modele )
//----------------------------------------------------------
// Permet de traiter le résultat d'une requête sur la base de donnée : on recherche un ensemble de références correspondant à un modèle

int bdd_search_modele ( MYSQL_RES* resultat_req, Ref* modele )
{
    // Initialisation
    Ref reference ;
    int return_value ;
    int suite = TRUE ;
    MYSQL_ROW tuple ;
    char* user = NULL ;
    char* safe_tuple = NULL ;              // Permet d'appeler strtok_r sur le bon contenu.
    char* reponse = NULL ;
    int first = TRUE ;

    // Initialisation
    user = (char*) malloc ( TAILLE_HASH_NOM * sizeof ( char ) ) ;
    memset ( user, '\0', TAILLE_HASH_NOM ) ;

    // Initialisation
    char* output = (char*) malloc ( RSA_size ( bdd_key ) * sizeof ( char ) ) ;
    memset ( output, '\0', RSA_size ( bdd_key ) ) ;

    // Initialisation
    char* temporaire = (char*) malloc ( ( TAILLE_MAX_REF + TAILLE_HASH_NOM + 3 ) * nbr_tuples * sizeof ( char ) ) ;
    memset ( temporaire, '\0', ( TAILLE_MAX_REF + TAILLE_HASH_NOM + 3 ) * nbr_tuples ) ;

    // On parcourt chaque tuple
    while ( suite == TRUE )
    {
        // On récupère un tuple
        if ( ( tuple = mysql_fetch_row ( resultat_req ) ) ==  NULL )
            suite = FALSE ;

        // On le traite
        if ( suite == TRUE )
        {
            // On découpe la référence
            if ( ( return_value = ref_split ( tuple[0], &reference ) ) == ERRNO )
            {
                perror ( "Erreur_bdd_search_modele : ref_split " ) ;
                free ( user ) ;
                free ( output ) ;
                bdd_send_msg ( SEEK, "ERROR" ) ;
                return ERRNO ;
            }

            // On regarde si elle correspond à ce que l'on recherche
            if ( ( return_value = ref_cmp ( &reference, modele ) ) == ERRNO )
            {
                perror ( "Erreur_bdd_traitement_request : ref_cmp " ) ;
                free ( user ) ;
                free ( output ) ;
                bdd_send_msg ( SEEK, "ERROR" ) ;
                return ERRNO ;
            }
            else if ( return_value == TRUE )    // La référence nous intéresse
            {

                // On déchiffre le tuple
                memset ( output, '\0', RSA_size ( bdd_key ) ) ;
                if ( RSA_chiffrement ( (unsigned char*) tuple[1], (unsigned char*) output, DECHIFFREMENT ) != TRUE )
                {
                    perror ( "Erreur_bdd_serach_modele : RSA_dechiffrement " ) ;
                    free ( output ) ;
                    free ( user ) ;
                    bdd_send_msg ( SEEK, "ERROR" ) ;
                    return ERRNO ;
                }

                // On récupère le hash du nom associé
                memset ( user, '\0', TAILLE_HASH_NOM ) ;
                sprintf ( user, "%s", strtok_r ( output, "*", &safe_tuple ) ) ;

                // On ajoute la référence et le nom associé à la réponse
                if ( first == TRUE )
                {
                    sprintf ( temporaire, "%s;%s", tuple[0], user ) ;
                    first = FALSE ;
                }
                else if ( first == FALSE )
                    sprintf ( temporaire + strlen ( temporaire ), "*%s;%s", tuple[0], user ) ;
            }
        }
    }

    // On envoie la réponse
    int taille = strlen ( temporaire ) + 10 ;
    reponse = (char*) malloc ( taille * sizeof ( char ) ) ;
    memset ( reponse, '\0', taille ) ;
    sprintf ( reponse, "%d*%s*EOF", SEEK, temporaire ) ;
    res_send ( reponse ) ;

    // Free
    free ( output ) ;
    free ( user ) ;
    free ( temporaire ) ;
    free ( reponse ) ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int bdd_do_request ( MYSQL* mysql_bdd, int action, char* reference, char* valeur, MYSQL_RES** resultat_req )
//----------------------------------------------------------
// Permet de réaliser une requête sur la base de donnée

int bdd_do_request ( MYSQL* mysql_bdd, int action, char* reference, char* valeur, MYSQL_RES** resultat_req )
{
    // Initialisation
    char *requete = (char*) malloc ( TAILLE_REQUETE * sizeof ( char ) ) ;
    memset ( requete, '\0', TAILLE_REQUETE ) ;
    int return_value ;

    // On prépare la zone mémoire de sortie (chiffrement)
    int i = 0 ;
    int good = FALSE ;
    char* insertion = (char*) malloc ( RSA_size ( bdd_key ) * sizeof ( char ) ) ;
    memset ( insertion, '\0', RSA_size ( bdd_key ) ) ;

    // Chiffrement BDD
    if ( action == INSERT )
    {
        // On vérifie la taille du buffer d'entrée
        if ( strlen ( valeur ) <= ( TAILLE_MAX_UTILE - 1 ) )
        {
            // Tant que le chiffré n'est pas bon
            while ( good != TRUE )
            {
                // Ini
                memset ( insertion, '\0', RSA_size ( bdd_key ) ) ;

                // On chiffre
                if ( RSA_chiffrement ( (unsigned char*) valeur, (unsigned char*) insertion, CHIFFREMENT ) != TRUE )
                {
                    perror ( "Erreur_bdd_do_request : RSA_chiffrement " ) ;
                    free ( insertion ) ;
                    free ( requete ) ;
                    return ERRNO ;
                }

                // On suppose chiffré bon
                good = TRUE ;

                // On vérifie la non présence du caractère ", \ et \0
                for ( i = 0; i < TAILLE_MAX_RSA; i ++ )
                {
                    if ( ( (int) insertion[i] == APOSTROPHE ) || ( (int) insertion[i] == BACK_SLASH ) || ( (int) insertion[i] == ZERO_TERMINAL ) )
                        good = FALSE ;
                }
            }
        }
    }

    // On prépare la requête selon l'action à faire
    switch ( action )
    {
        case SELECT_ALL :
            sprintf ( requete, "select REF, VAL from %s;", TABLE ) ;
            break ;

        case SELECT :
            sprintf ( requete, "select VAL from %s where REF = \"%s\";", TABLE, reference ) ;
            break ;

        case INSERT :
            sprintf ( requete, "insert into %s (REF, VAL) values(\"%s\",\"%s\");", TABLE, reference, insertion ) ;
            break ;

        case DELETE :
            sprintf ( requete, "delete from %s where REF = \"%s\";", TABLE, reference ) ;
            break ;

        case DELETE_ALL :
            sprintf ( requete, "delete from %s;", TABLE ) ;
            break ;

        default :
            perror ( "Erreur_bdd_do_request : bad action " ) ;
            free ( requete ) ;
            free ( insertion ) ;
            return ERRNO ;
            break ;
    }

    // On réalise la requête
    if ( ( return_value = mysql_query ( mysql_bdd, requete ) ) != 0 )
    {
        fprintf ( stderr, "Erreur_bdd_do_request : mysql_query avec code action : %d \n", action ) ;
        fprintf ( stderr, "Erreur_bdd_do_request : mysql_query avec requete : %s \n", requete ) ;
        fprintf ( stderr, "Erreur_bdd_do_request : mysql_query avec retour : %d \n", return_value ) ;
        free ( requete ) ;
        free ( insertion ) ;
        return ERRNO ;
    }

    // Selon l'action réalisée, on récupère des résultats
    if ( ( action == SELECT_ALL ) || ( action == SELECT ) )
    {
        if ( ( *resultat_req = mysql_use_result ( mysql_bdd ) ) == NULL )
        {
            perror ( "Erreur_bdd_do_request : mysql_use_result " ) ;
            free ( requete ) ;
            free ( insertion ) ;
            return ERRNO ;
        }
    }

    // Free
    free ( requete ) ;
    free ( insertion ) ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}
