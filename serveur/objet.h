#ifndef OBJET_H_INCLUDED
#define OBJET_H_INCLUDED

/**
\fn int creerIdObjet(char *msg,int longMsg,char *idObjet,int taille)
\brief creer l'identifiant objet
\param [in]msg pointeur de char
\param [in]longMsg un entier, longueur du msg
\param [out]idObjet pointeur de char, qui represente l'id de l'objet à initialiser
\param [in]taille taille de l'id Objet
\return le type d'erreur associé, -1 si erreur fichier, 1 si tout c'est bien passé, 0 sinon
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

*/
int acheterObjet(char *msg,int tailleMsg,char *id);
/**
*/
int consulter(char *msg,int tailleMsg,char *id);

/**
*/
int vendre(char *msg,int tailleMsg,char *id);

#endif // OBJET_H_INCLUDED
