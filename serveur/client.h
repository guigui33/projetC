#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

/**
\fn int verificationAuthentification(char *message,int tailleMsg)
\brief fonction qui verifie l'identifiant et le mot de passe du client
\param[in] message un pointeur sur une chaine de caractères
\param[in] tailleMsg taille du message
\return un entier qui vaut 1 si connexion ok, -1 si erreur fichier, 0 sinon
*/
int verificationAuthentification(char *message,int tailleMsg);

/**
\fn verifCreationCompte(char *message,int longMsg)
\brief enregistre les données du nouvel utilisateur, les données sont verifiées, la creation est annulée si erreur.
\param[in] message un pointeur sur une chaine de caractère
\param[in] longMsg taille du message
\return retourne 0 en cas d'erreur message, -1 en cas d'erreur fichier, 1 sinon.
*/
int creationCompte(char *message,int longMsg);

/**
\fn int chercherFichierIdMdp(char idChr[6],char mdpChr[16])
\brief cherche un identifiant et son mot de passe dans le fichier user.txt
\param char idChr[6] l'identifiant à rechercher
\param char mdpChr[16] le mot de passe à rechercher
\return 1 si l'identifiant et le mode de passe sont corrects, 0 si l'identifiant ou le mot de passe est incorrect, -1 si erreur fichier
*/
int chercherFichierIdMdp(char idChr[6],char mdpChr[16]);


/**
\fn int creationId(char id[6])
\brief creer l'identifiant d'un nouveau utilisateur
\param char id[6] tableau de char de taille 6 contenant les 3 premières lettres de l'id
\return 1 si id a été créé, -1 si erreur d'ouverture fichier
*/
int creationId(char idChr[6]);

/**
\fn int extraireIdClient(char *message,int tailleMsg,char *id)
\brief extraire l'id de l'utilisateur du message
\param[in][out] message tableau de char contenant l'id de l'utilisateur + les données du message
\param[in] tailleMsg taille du message
\param[out] id tableau de char de taille 6
\return 1 si id a été extrait, -1 si erreur d'ouverture fichier, 0 sinon
*/
int extraireIdClient(char *message,int tailleMsg,char *id);

#endif // CLIENT_H_INCLUDED
