#ifndef SECONDAIRE_H_INCLUDED
#define SECONDAIRE_H_INCLUDED
#include <time.h>

/**
\fn int controleChaine(char *chaine,char *indice)
\brief controle que l'utilisateur ne rentre pas de #, controle sur certaines informations importantes
\param[in] chaine chaine entrée par l'utilisateur
\param[in] indice type de verification à faire
\return 1 pas de probleme, 0 erreur
*/
int controleChaine(char *chaine,char *indice);

/**
\fn void dateFinEnchere(char *date)
\brief permet de définir la date de fin de l'enchere.
\param[out] date retourne la date définit par l'utilisateur
*/
void dateFinEnchere(char *date);

/**
\fn void supprCara(char *chaine)
\brief fonction qui permet de remplacer le '\n' par '\0'
        si l'utilisateur a entrée trop de caractère (le '\n' n'est pas trouvé) on vide le buffer
\param char* un pointeur sur une chaine
\return
*/
void supprCara(char *chaine);

/**
\fn void viderBuffer()
\brief vide le buffer
*/
void viderBuffer();

/**
\fn int enregistrementCatalogueTemp()
\brief enregistre le catalogue envoyé par le site dans un fichier temporaire.
\return -1 en cas d'erreur du serveur, 0 si le message envoyé n'est pas correct, 1 sinon
*/
int enregistrementCatalogueTemp();

/**
\fn void afficherObjet(FILE *file,char *idObjet,int choix,int *finFichier)
\brief affiche les informations à l'ecran sur un objet du catalogue, retourne l'identifiant objet, retourne un indicateur de EOF
\param[in] file un pointeur sur un fichier "tmp.txt"
\param[out] idObjet l'identifiant objet qui est lu dans le fichier.
\param[in] choix le choix de l'utilisateur, 1 l'utilisateur choisit de voir l'objet precedent, 2 voir le suivant, 0 le meme objet
\param[out] finFichier pointeur sur un entier, permet de retourner si la fin du fichier a été atteinte
            1 si fin de fichier, -1 si fichier vide, 0 sinon
*/
void afficherObjet(FILE *file,char *idObjet,int choix,int *finFichier);

/**
\fn void formatDate(char *date,struct tm *time)
\brief initialise un type struct tm
\param[in] date un pointeur sur chaine contentant une date sous format hhmmjjmmaaaa
\param[out] time un pointeur sur un structure de type tm
*/
void formatDate(char *date,struct tm *time);

/**
\fn void afficherInformation(char *chaine,char separateur,FILE *file,char *c)
\brief lit un fichier et affiche les informations jusqu'à lire le separateur
\param[in] chaine pointeur sur une chaine de caractere, elle est affichée à l'ecran
\param[in] separateur un caratère qui termine la lecture du fichier
\param[in] file un pointeur sur un fichier
\param[out] c pointeur sur un caractère, permet de savoir où on est dans le fichier
*/
void afficherInformation(char *chaine,char separateur,FILE *file,char *c);

/**
\fn void afficherInformation2(char *chaine,char *information,int taille,char separateur,int *i)
\brief lit une chaine passée en paramètre et affiche les informations jusqu'à lire le separateur
\param[in] chaine pointeur sur une chaine de caractere, elle est affichée à l'ecran
\param[in] information pointeur sur chaine de caractère
\param[in] taille taille de la chaine "information"
\param[in] separateur un caratère qui termine la lecture de la chaine
\param[out] i pointeur sur un entier.
*/
void afficherInformation2(char *chaine,char *information,int taille,char separateur,int *i);

/**
\fn void afficherUtilisateur(char *chaine,int taille)
\brief affiche les informations concernant un utilisateur à l'ecran
\param[in] chaine un pointeur sur une chaine
\param[in] taille la taille de la chaine
*/
void afficherUtilisateur(char *chaine,int taille);

/**
\fn int dateNaissance(char *date)
\brief retourne dans date la date de naissance de l'utilisateur bien formatée, controle sommaire de l'entrée utilisateur
\param[out] date pointeur sur une chaine de caractère
\return 1 si date ok, 0 sinon
*/
int dateNaissance(char *date);


/**
\fn void afficherMesVentesTerminée(FILE *file,int choix,int *finFichier)
\brief affiche les informations à l'ecran sur un objet du catalogue, retourne l'identifiant objet, retourne un indicateur de EOF
\param[in] file un pointeur sur un fichier "tmp.txt"
\param[in] choix le choix de l'utilisateur, 1 l'utilisateur choisit de voir l'objet precedent, 2 voir le suivant, 0 le meme objet
\param[out] finFichier pointeur sur un entier, permet de retourner si la fin du fichier a été atteinte
            1 si fin de fichier, -1 si fichier vide, 0 sinon
*/
void afficherMesVentesTerminee(FILE *file,int choix,int *finFichier);
#endif // SECONDAIRE_H_INCLUDED
