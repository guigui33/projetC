#ifndef OBJET_H_INCLUDED
#define OBJET_H_INCLUDED

/**
\fn int creerIdObjet(char **msg,int longMsg,char *idObjet,int tailleId,char *nomObjet,int tailleNom)
\brief creer l'identifiant objet, retourne le nom de l'objet, le nom de l'identifiant de l'objet
\param[in][out] msg contient l'adresse du premier element du message
\param[in] longMsg un entier, longueur du msg
\param[out] idObjet pointeur de char, qui retourne l'id de l'objet
\param[in] taille taille de l'id Objet
\param[out] nomObjet pointeur de char qui retourne le nom de l'objet
\param[in] tailleNom entier taille max du nom de l'objet
\return le type d'erreur associé, -1 si erreur fichier, 1 si tout c'est bien passé, 0 si le message n'est pas conforme
*/
int creerIdObjet(char **msg,int longMsg,char *idObjet,int tailleId,char *nomObjet,int tailleNom);

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
\brief fonction qui gère les messages de type "consulter"
\param[in] msg le message envoyé par l'utilisateur
\param[in] tailleMsg taille du message
\return -1 si erreur fichier, 0 si message non conforme, 1 sinon
*/
int consulter(char *msg,int tailleMsg);

/**
\fn int rechercherObjet(char *idUtili,char *nomRch,char *categorieRch,char *descriptionRch)
\param[in] idUtili pointeur vers une chaine, identifiant de l'utilisateur
\param[in] nomRch le nom de l'objet que l'on recherche
\param[in] categorieRch la categorie d'objet que l'on recherche
\param[in] descriptionRch la description de l'objet que l'on recherche
\param[in] f bool 1 ou 0 pour ouvrir le bon fichier, 1 pour finEnchere ou 0 pour enchere
\return 0 si le message n'est pas conforme, -1 si erreur fichier, 1 sinon
*/
int rechercherObjet(char *idUtili,char *nomRch,char *categorieRch,char *descriptionRch,int f);

/**
\fn int donneeObjet(char *idPro,char *idObjet,char *idUtilisateur,int typeUtilisateur,char *nomRch,char *categorieRch,char *descriptionRch)
\brief cherche les données de l'objet à envoyer au client, puis les données du propietaire et les données des encheres
\param[in] idPro l'identifiant du propietaire de l'objet
\param[in] idUtilisateur identifiant de l'utilisateur
\param[in] typeUtilisateur 1 si l'utilisateur est vendeur et 2 si acheteur
\param[in] nomRch le nom de l'objet que l'on recherche
\param[in] categorieRch la categorie d'objet que l'on recherche
\param[in] descriptionRch la description de l'objet que l'on recherche
\return 0 si le message n'est pas conforme, -1 si erreur fichier, 1 sinon
*/
int donneeObjet(char *idPro,char *idObjet,char *idUtilisateur,int typeUtilisateur,char *nomRch,char *categorieRch,char *descriptionRch);

/**
\fn void extraireDonneeMsg(char *message,int tailleMsg,char* rchr,char tailleRchr,int *i,char separateur)
\brief extrait les données du message vers une chaine jusuq'à lire le separateur
\param[in] message pointeur vers une chaine d'où on extrait l'information
\param[in] tailleMsg taille du message
\param[out] rchr retourne l'information désirée
\param[in] tailleRchr taille de la chaine rchr
\param[in][out] i pointeur vers un entier
\param[in] separateur le separateur qui termine la lecture du message
*/
void extraireDonneeMsg(char *message,int tailleMsg,char* rchr,char tailleRchr,int *i,char separateur);

/**
\fn void recupererInfoFichier(char *chaine,int taille,char separateur,char *c,FILE *file);
\brief extrait les données d'un fichier dans une chaine de caractère jusqu'à lire un separateur
\param[out] chaine pointeur vers une chaine
\param[in] taille taille de la chaine
\param[in] separateur le separateur qui termine la lecture du message
\param[in][out] c pointeur vers un caractère, permet de connaitre le dernier caractère lu
\param[in] file pointeur vers un fichier
*/
void recupererInfoFichier(char *chaine,int taille,char separateur,char *c,FILE *file);

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

/**
\fn int testDateEnchere(FILE *enchere)
\brief test s'il toujours possible d'encherir au moment de l'envoi du message par le client.
\param[in] enchere pointeur sur un fichier enchere.txt, le curseur est au niveau de la date de fin de l'objet
\return 0 si l'enchere est finie, 1 sinon
*/
int testDateEnchere(FILE *enchere);


/**
\fn int testDateMiseEnVente(char *message,int tailleMsg,char *dateDebut,int tailleDate)
\brief test si la date de fin d'enchere indiquée par l'utilisateur est conforme avec le debut de mise en vente de l'objet
        initialise date debut si date de fin ok.
\param[in] message le message envoyé par l'utilisateur pointant sur la date de fin de l'enchere
\param[in] tailleMsg la taille du message
\param[out] dateDebut recupère la date systeme correspondant au debut de mise en vente de l'objet
\param[in] tailleDate taille de la date
\return 0 si la date indiquée par l'utilisateur n'est pas conforme, 1 sinon
*/
int testDateMiseEnVente(char *message,int tailleMsg,char *dateDebut,int tailleDate);

/**
\fn void supprCaractere(char *message)
\brief supprime le \n du message
\param[in][out] message un tableau de char
*/
void supprCaractere(char *message);


/**
\fn int donneeEnchereCatalogue(char* idUtili,char *idObjet,char *msgClient)
\brief complète le msgClient par des informations sur l'enchere, à la fin de la fonction le msgClient contient les données
        objet + les données propriétaire + les données enchères(en fin de fonction)
\param[in] idUtili l'identifiant de l'utilisateur
\param[in] idObjet identifiant objet à rechercher
\param[out] msgClient chaine contenant les informations de l'objet et du proprietaire de l'objet, 500 caractères maximum
\return -1 si probleme fichier, 1 sinon
*/
int donneeEnchereCatalogue(char* idUtili,char *idObjet,char *msgClient);

/**
\fn int nbrVenteEnchereUtilisateur(char *idUtilisateur,char *msgClient)
\brief concatener dans le msgClient le nbre d'enchere en cours et le nombre de vente en cours de l'identifiant passé en paramètre
\param[in] idUtilisateur l'identifiant de l'utilisateur
\param[out] msgClient chaine contenant des informations
\return 1
*/
int nbrVenteEnchereUtilisateur(char *idUtilisateur,char *msgClient);

/**
void etatEnchere()
\brief scan le fichier enchere, controle la date des encheres, ecrit dans le fichier finEnchere.txt les encheres terminées
*/
void etatEnchere();


int informationFinEnchere(char *message,int taille);

#endif // OBJET_H_INCLUDED
