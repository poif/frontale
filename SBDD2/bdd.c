﻿/** ====================================================================
**   Auteur  : Delvarre / Bourillon      | Date    : 07/01/2016
**  --------------------------------------------------------------------
**   Langage : C                         | Systeme : Linux
**  --------------------------------------------------------------------
**   Nom fichier : bdd.c                 | Version : 1.0
**  --------------------------------------------------------------------
**   Description : Gestion de la BDD
** =====================================================================*/
#include "bdd.h"
#include "crypt.h"
#include "res.h"


//----------------------------------------------------------
// Initialisation des variables globales
//----------------------------------------------------------
MYSQL* mysql_bdd = NULL ;
int nbr_tuples = 0 ;
int traitement = FALSE ;


//----------------------------------------------------------
// MYSQL* bdd_start_connection ()
//----------------------------------------------------------
// Permet de se connecter à une base de donnée dans un objet MYSQL qui sera retourné

MYSQL* bdd_start_connection ()
{
    // Initialisation
    MYSQL* mysql_bdd ;
    if ( ( mysql_bdd = mysql_init ( NULL ) ) == NULL )
        return NULL ;

    // Applications d'options supplémentaires
    if ( ( mysql_options ( mysql_bdd, MYSQL_READ_DEFAULT_GROUP, "Lien avec my.cnf" ) ) != 0 )
        return NULL ;

    // On se connecte
    if ( ( mysql_bdd = mysql_real_connect ( mysql_bdd, HOST, USER, PWD, DB, 0,  NULL, 0 ) ) == NULL )
        return NULL ;

    // On retourne l'accès à la BDD
    return mysql_bdd ;
}


//----------------------------------------------------------
// void bdd_close_connection ( MYSQL* mysql_bdd )
//----------------------------------------------------------
// Permet de se déconnecter d'une base de donéne passée en paramètre

void bdd_close_connection ( MYSQL* mysql_bdd )
{
   mysql_close ( mysql_bdd ) ;
}


//----------------------------------------------------------
// int ref_concatenation ( Ref* reference, char* reference_finale )
//----------------------------------------------------------
// Permet d'assembler 4 sub-references en une référence

int ref_concatenation ( Ref* reference, char* reference_finale )
{
    // Initialisation
    char* tmp ;                 // Chaîne temporaire
    int taille ;                // Taille de la chaîne tmp
    int i ;                     // Compteur

    // Allocation chaînes
    tmp = (char*) malloc ( TAILLE_MAX_SUB_REF * sizeof ( char ) ) ;
    memset ( reference_finale, '\0', TAILLE_MAX_REF ) ;

    // Concaténation des 4 parties
    for ( i = 0; i < 4; i++ )
    {
        // On charge la valeur hexa sous forme de caractères
        memset ( tmp, '\0', TAILLE_MAX_SUB_REF ) ;
        switch ( i )
        {
            case STATUT :
                sprintf ( tmp, "%lx", reference->statut ) ;
                break ;

            case AFFECTATION :
                sprintf ( tmp, "%lx", reference->affectation ) ;
                break ;

            case GROUPE :
                sprintf ( tmp, "%lx", reference->groupe ) ;
                break ;

            case TYPE :
                sprintf ( tmp, "%lx", reference->type ) ;
                break ;

            default :
                free ( tmp ) ;
                return ERRNO ;
                break ;
        }

        // On réalise un bourrage si besoin et on concatène la référence
        taille = (int) strlen ( tmp ) ;
        switch ( taille )
        {
            case ZERO :
                free ( tmp ) ;
                return ERRNO ;
                break ;

            case UN :
                sprintf ( reference_finale + strlen ( reference_finale ), "0000000%s", tmp ) ;
                break ;

            case DEUX :
                sprintf ( reference_finale + strlen ( reference_finale ), "000000%s", tmp ) ;
                break ;

            case TROIS :
                sprintf ( reference_finale + strlen ( reference_finale ), "00000%s", tmp ) ;
                break ;

            case QUATRE :
                sprintf ( reference_finale + strlen ( reference_finale ), "0000%s", tmp ) ;
                break ;

            case CINQ :
                sprintf ( reference_finale + strlen ( reference_finale ), "000%s", tmp ) ;
                break ;

            case SIX :
                sprintf ( reference_finale + strlen ( reference_finale ), "00%s", tmp ) ;
                break ;

            case SEPT :
                sprintf ( reference_finale + strlen ( reference_finale ), "0%s", tmp ) ;
                break ;

            case HUIT :
                sprintf ( reference_finale + strlen ( reference_finale ), "%s", tmp ) ;
                break ;

            default :
                free ( tmp ) ;
                return ERRNO ;
                break ;
        }
    }

    // Free
    free ( tmp ) ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int ref_split ( char* reference_input, Ref* reference )
//----------------------------------------------------------
// Permet de transformer une référence en 4 sub-references

int ref_split ( char* reference_input, Ref* reference )
{
    // Initialisation
    char* tmp = NULL ;          // Chaîne temporaire
    int i ;                     // Compteur

    // Allocation chaînes
    tmp = (char*) malloc ( TAILLE_MAX_SUB_REF * sizeof ( char ) ) ;

    // Split en 4 parties
    for ( i = 0; i < 4; i++ )
    {
        // On charge les 8 premiers octets de reference_input
        memset ( tmp, '\0', TAILLE_MAX_SUB_REF ) ;
        memcpy ( &tmp[0], reference_input + i * ( TAILLE_MAX_SUB_REF - 1 ), TAILLE_MAX_SUB_REF - 1 ) ;

        // Initialisation de errno
        errno = 0 ;

        // On transforme en hexa avec affectation à la bonne sub-section
        switch ( i )
        {
            case STATUT :
                reference->statut = strtol ( tmp, NULL, TAILLE_HEXA ) ;
                break ;

            case AFFECTATION :
                reference->affectation = strtol ( tmp, NULL, TAILLE_HEXA ) ;
                break ;

            case GROUPE :
                reference->groupe = strtol ( tmp, NULL, TAILLE_HEXA ) ;
                break ;

            case TYPE :
                reference->type = strtol ( tmp, NULL, TAILLE_HEXA ) ;
                break ;

            default :
                free ( tmp ) ;
                return ERRNO ;
                break ;
        }

        // Traitement errno
        if ( errno != 0 )
        {
            free ( tmp ) ;
            return ERRNO ;
        }
    }

    // Free
    free ( tmp ) ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}


//----------------------------------------------------------
// int ref_cmp ( Ref* reference, Ref* modele )
//----------------------------------------------------------
// Permet de comparer une référence à un modele

int ref_cmp ( Ref* reference, Ref* modele )
{
    // Initialisation
    int i ;
    int calcul ;

    // On traite les 4 sous-parties
    for ( i = 0; i < 4; i++ )
    {
        // On traite une sous-partie par tour
        switch ( i )
        {
            case STATUT :
                // On regarde si c'est un champ relevant
                if ( modele->statut != 0 )
                {
                    // On calcul le modulo que l'on vérifie
                    if ( ( calcul = reference->statut % modele->statut ) != 0 )
                        return FALSE ;
                }
                break ;

            case AFFECTATION :
                // On regarde si c'est un champ relevant
                if ( modele->affectation != 0 )
                {
                    // On calcul le modulo que l'on vérifie
                    if ( ( calcul = reference->affectation % modele->affectation ) != 0 )
                        return FALSE ;
                }
                break ;

            case GROUPE :
                // On regarde si c'est un champ relevant
                if ( modele->groupe != 0 )
                {
                    // On calcul le modulo que l'on vérifie
                    if ( ( calcul = reference->groupe % modele->groupe ) != 0 )
                        return FALSE ;
                }
                break ;

            case TYPE :
                // On regarde si c'est un champ relevant
                if ( modele->type != 0 )
                {
                    // On calcul le modulo que l'on vérifie
                    if ( ( calcul = reference->type % modele->type ) != 0 )
                        return FALSE ;
                }
                break ;

            default :
                return ERRNO ;
                break ;
        }
    }

    // Il y a match
    return TRUE ;
}


//----------------------------------------------------------
// int bdd_verification_partage ( char* politique, char* atester )
//----------------------------------------------------------
// Permet de vérifier les politiques de partages

int bdd_verification_partage ( char* politiques, char* atester )
{
    // Déclaration variables
    char* politique = NULL ;          // Variable contenant chaque morceau de la trame (strtok_r)
    char* safe_politique = NULL ;
    char* morceau = NULL ;
    char* safe_morceau = NULL ;
    int verif = FALSE ;

    // Initialisation
    int taille = strlen ( politiques ) + 2 ;
    char* duplicata_politiques = (char*) malloc ( taille * sizeof ( char ) ) ;

    // On sélectionne chaque valeur à tester
    if ( ( morceau = strtok_r ( atester, ";", &safe_morceau ) ) == NULL )
    {
        free ( duplicata_politiques ) ;
        return ERRNO ;
    }

    // On le traite
    while ( morceau != NULL )
    {
        // Ini
        verif = FALSE ;
        safe_politique = NULL ;

        // On duplique les politiques
        memset ( duplicata_politiques, '\0', taille ) ;
        sprintf ( duplicata_politiques, "%s", politiques ) ;

        // On compare la valeur à tester à chaque champs de la politiques concernée
        if ( ( politique = strtok_r ( duplicata_politiques, ";", &safe_politique ) ) == NULL )
        {
            free ( duplicata_politiques ) ;
            return ERRNO ;
        }

        while ( politique != NULL )
        {
            // On compare
            if ( strcmp ( politique, morceau ) == 0 )
                verif = TRUE ;

            // On prend les autres valeurs de politiques
            politique = strtok_r ( NULL, ";", &safe_politique ) ;
        }

        // Si on n'a pas trouvé de correspondance, on stop
        if ( verif != TRUE )
        {
            free ( duplicata_politiques ) ;
            return FALSE ;
        }

        // On prend les autres valeurs de atester
        morceau = strtok_r ( NULL, ";", &safe_morceau ) ;
    }

    // Free
    free ( duplicata_politiques ) ;

    // Si on arrive ici, c'est que la politique de partage est vérifiée
    return TRUE ;
}


//----------------------------------------------------------
// void bdd_send_msg ( int id, int flag, char* texte )
//----------------------------------------------------------
// Permet d'envoyer un code d'erreur / message à la frontale

void bdd_send_msg ( int id, int flag, char* texte, int chiffre )
{
    // Initialisation
    char* trame = (char*) malloc ( TAILLE_MAX_TRAME * sizeof ( char ) ) ;
    memset ( trame, '\0', TAILLE_MAX_TRAME ) ;

    // On créé le message
    sprintf ( trame, "%d*%d*%s*EOF*", id, flag, texte ) ;
    int ilen = strlen ( trame ) ;

    // Chiffré ou non ?
    if ( chiffre == TRUE )
    {
        // On chiffre le message
        int olen = strlen ( trame ) * 2  ;
        char* out = (char*) malloc ( olen * sizeof ( char ) ) ;
        memset ( out, '\0', olen ) ;
        if ( AES_chiffrement ( trame, out, olen, &ilen, CHIFFREMENT ) != TRUE )
        {
            free ( out ) ;
            free ( trame ) ;
        }
        else
        {
            // On calcul la nouvelle taille
            int taille = ilen ;
            if ( ilen < 10 )
                taille += 3 ;
            else if ( ilen < 100 )
                taille += 4 ;
            else if ( ilen < 1000 )
                taille += 5 ;
            else
                taille += 6 ;

            // Finalisation de la réponse
            free ( trame ) ;
            trame = (char*) malloc ( taille * sizeof ( char ) ) ;
            memset ( trame, '\0', taille ) ;
            sprintf ( trame, "%d*", ilen ) ;
            if ( ilen < 10 )
                memcpy ( &trame[2], out, ilen ) ;
            else if ( ilen < 100 )
                memcpy ( &trame[3], out, ilen ) ;
            else if ( ilen < 1000 )
                memcpy ( &trame[4], out, ilen ) ;
            else
                memcpy ( &trame[5], out, ilen ) ;

            // On envoie la réponse
            res_send ( trame, taille ) ;

            // Free
            free ( trame ) ;
            free ( out ) ;
        }
    }
    else if ( chiffre == FALSE )
    {
        // On envoi le message
        res_send ( trame, ilen ) ;

        // Free
        free ( trame ) ;
    }
}


//----------------------------------------------------------
// int bdd_send_reponse ( char *texte, int id, int code )
//----------------------------------------------------------
// Permet d'envoyer une réponse à la frontale

int bdd_send_reponse ( char *texte, int id, int code )
{
    // On prépare la réponse
    int taille = strlen ( texte ) + 20 ;
    char* reponse = (char*) malloc ( taille * sizeof ( char ) ) ;
    memset ( reponse, '\0', taille ) ;
    sprintf ( reponse, "%d*%d*%s*EOF*", id, code, texte ) ;

    // On chiffre la réponse
    int ilen = strlen ( reponse ) ;
    int olen = taille * 2  ;
    char* out = (char*) malloc ( olen * sizeof ( char ) ) ;
    memset ( out, '\0', olen ) ;
    if ( AES_chiffrement ( reponse, out, olen, &ilen, CHIFFREMENT ) != TRUE )
    {
        free ( reponse ) ;
        free ( out ) ;
        return ERRNO ;
    }

    // On calcul la nouvelle taille
    taille = ilen ;
    if ( ilen < 10 )
        taille += 3 ;
    else if ( ilen < 100 )
        taille += 4 ;
    else if ( ilen < 1000 )
        taille += 5 ;
    else
        taille += 6 ;

    // Finalisation de la réponse
    free ( reponse ) ;
    reponse = (char*) malloc ( taille * sizeof ( char ) ) ;
    memset ( reponse, '\0', taille ) ;
    sprintf ( reponse, "%d*", ilen ) ;
    if ( ilen < 10 )
        memcpy ( &reponse[2], out, ilen ) ;
    else if ( ilen < 100 )
        memcpy ( &reponse[3], out, ilen ) ;
    else if ( ilen < 1000 )
        memcpy ( &reponse[4], out, ilen ) ;
    else
        memcpy ( &reponse[5], out, ilen ) ;

    // On envoie la réponse
    res_send ( reponse, taille ) ;

    // Free
    free ( reponse ) ;
    free ( out ) ;

    // On indique que tout s'est bien déroulé
    return TRUE ;
}
