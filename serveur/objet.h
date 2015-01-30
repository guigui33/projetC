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
\fn int consulter(char *msg,int tailleMsg)
\brief envoie les objets au client qui correspondent au nom et à la description
\param[in] msg le message envoyé par l'utilisateur
\param[in] tailleMsg taille du message
\return -1 si erreur fichier, 0 si message non conforme, 1 sinon
*/
int consulter(char *msg,int tailleMsg);

/**
\fn int extraireIdObjet(char **msg,int tailleMsg,char *idObjet,char *idUti,FILE *enchere)
\brief extrait l'identifiant objet du message, test si l'id objet est bon
        test si le vendeur n'achete pas son propore objet
\param[in][out] msg pointeur sur une chaine
\param[in] tailleMsg taille du message
\param[out] idObjet enregistre l'identifiant de l'objet du message
\param[in] idUti l'identifiant de l'utilisateur
\param[in] enchere pointeur sur le fichier enchere.txt
\return 0 si le message n'est pas conforme, -1 si erreur fichier, 1 sinon
*/
int extraireIdObjet(char **msg,int tailleMsg,char *idObjet,char *idUti,FILE *enchere);

/**
\fn int testPrix(float prix,FILE *enchere)
\brief test si le prix donné par l'utilisateur est conforme
\param[in] prix prix donné par l'utilisateur
\param[in] enchere pointeur sur le fichier enchere.txt
\return 0 si le prix n'est conforme, 1 si le prix est superieur au prix actuel, -1 en cas d'erreur
*/
int testPrix(float prix,FILE *enchere);

/**
\fn int enregistrementPrix(char *idUti,char *idObjet,float prix,FILE *enchere)
\brief copie le fichier enchere en ajoutant le nouveau prix dans le fichier fichier temporaire "fichiers/enchereTmp.txt",
        toutes les informations ont été vérifiées
\param[in] idUti identifiant de l'utilisateur
\param[in] idObjet identifiant de l'objet
\param[in] prix le prix proposé par l'utilisateur
\param[in] enchere pointeur sur le fichier enchere.txt
\return -1 en cas d'erreur fichier, 1 sinon
*/
int enregistrementPrix(char *idUti,char *idObjet,float prix,FILE *enchere);

/**
\fn int acheterObjet(char *msg,int tailleMsg)
\brief verifie le message envoyé par l'utilisateur qui propose une enchère, si le message est ok, l'enchere est enregistrée.
\param[in] msg le message envoyé par l'utilisateur sans le mot clé
\param[in] tailleMsg la taille du message
\return -1 en cas d'erreur fichier, 0 si le message n'est pas conforme, 1 si tout c'est bien passé.
*/
int acheterObjet(char *msg,int tailleMsg);

/*recuperer date systeme, on compare avec la date de fin de l'enchere */
int testDateEnchere(FILE *enchere);


#endif // OBJET_H_INCLUDED
