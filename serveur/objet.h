#ifndef OBJET_H_INCLUDED
#define OBJET_H_INCLUDED

/**
\fn int creerIdObjet(char *msg,int longMsg,char *idObjet,int taille)
\brief creer l'identifiant objet
\param [in]msg pointeur de char
\param [in]longMsg un entier, longueur du msg
\param [out]idObjet pointeur de char, qui represente l'id de l'objet à initialiser
\param [in]taille taille de l'id Objet
\return le type d'erreur associé, -1 si erreur fichier, 1 si tout c'est bien passé
*/
int creerIdObjet(char *msg,int longMsg,char *idObjet,int taille);

/**
\fn int enregistrementObjet(char *message,int longMsg)
\param message un pointeur de char qui contient les informations sur l'objet mise en vente
\param longMsg longueur du message
\brief fonction qui verifie le message de mise en vente et enregistre les informations de l'objet dans le fichier "objet_en_vente.txt"
\return 1 si tout c'est bien passé, -1 si erreur fichier, 0 sinon
*/
int enregistrementObjet(char *message,int longMsg);

/**
\fn int miseAuEnchere(char *idUti,char *idObj,char *message,int tailleMsg)
\param[in] idUti l'identifiant de l'utilisateur
\param[in] idObj l'identifiant de l'objet
\param[in] message message envoyé par l'utilisateur
\param[in] tailleMsg taille du message
\return -1 en cas d'erreur fichier, 1 sinon
\brief enregistre les informations sur le fichier enchere.txt
*/
int miseAuEnchere(char *idUti,char *idObj,char *message,int tailleMsg);

/**
\fn int acheterObjet(char *msg,int tailleMsg)
\brief fonction qui enregistre une enchère si elle est acceptée
\param[in] msg le message envoyé par l'utilisateur
\param[in] tailleMsg taille du message
\return -1 si erreur fichier, 0 si le message est non conforme, 1 sinon
*/
int acheterObjet(char *msg,int tailleMsg);

/**
\fn int consulter(char *msg,int tailleMsg)
\brief envoie les objets au client qui correspondent au nom et à la description
\param[in] msg le message envoyé par l'utilisateur
\param[in] tailleMsg taille du message
\return -1 si erreur fichier, 0 si message non conforme, 1 sinon
*/
int consulter(char *msg,int tailleMsg);


#endif // OBJET_H_INCLUDED
