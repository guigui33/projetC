#ifndef UTILISATEUR_H_INCLUDED
#define UTILISATEUR_H_INCLUDED

/**
\fn void authentification(char * idUti)
\brief fonction pemettant d'afficher le menu d'authentification, l'utilisateur entre son mot de passe et son identifiant
\param[out] idUti l'identifiant de l'utilisateur
\return 1 si l'authentification est correcte, 0 si l'authentification est incorrecte, -1 si problème serveur
*/
int authentification(char * idUti);

/**
\fn int creationDeCompte()
\brief demande les informations à l'utilisateur pour créer un compte. Si les données sont correctes, le compte est enregistré.
\return -1 si probleme serveur, 1 si l'enregistrement du compte est valide, 0 si erreur message
*/
int creationDeCompte();

/**
\fn void vendre(char *idUser);
\brief fonction qui permet de complèter le forumulaire de mise en vente d'un objet sur le site.
\param[in] idUser l'identifiant de l'utilisateur
\return -1 en cas d'erreur du serveur, 0 si le message envoyé n'est pas correct, 1 sinon
*/
int vendre(char * idUser);

/**
\fn int rechercheObjet(char *idUser,char *choix)
\brief demande à l'utilisateur de rechercher un produit, envoie sa demande au serveur, la reponse du serveur est enregistrée
        dans un fichier temporaire
\param[in] idUser l'identifiant de l'utilisateur
\param[in] choix pointeur sur un caractère pour connaitre le choix de l'utilisateur
\return -1 en cas d'erreur du serveur, 0 si le message envoyé n'est pas correct, 1 sinon
*/
int rechercheObjet(char *idUser,char *choix);

/**
\fn int menuRechercherObjet(char *idUser);
\brief demande à l'utilisateur quel typde de recherche il souhaite effectuer.
\param[in] idUser l'identifiant de l'utilisateur
\return -1 en cas d'erreur du serveur, 0 si le message envoyé n'est pas correct, 1 sinon
*/
int menuRechercherObjet(char *idUser);

/**
\fn void enchere(char *idUtilisateur,char *idObjet)
\brief propose à l'utilisateur de rentrer un prix, le message est envoyé au serveur et elle validée si les paramètres sont bons
\param[in] idUtilisateur l'identifiant de l'utilisateur
\param[in] idObjet l'identifiant de l'objeet
*/
void enchere(char *idUtilisateur,char *idObjet);

/**
\fn int catalogue(char *idUtilisateur,int f)
\brief l'utilisateur accède au catalogue et choisit les actions qu'il veut faire
\param[in] idUtilisateur l'identifiant de l'utilisateur
\param[in] f type de fonction
\return -1 si erreur fichier, 1 sinon
*/
int catalogue(char *idUtilisateur,int f);

/**
\fn int informationCompte(char *idUser);
\brief l'utilisateur accède au information de son compte
\param[in] idUtilisateur l'identifiant de l'utilisateur
\return -1 si erreur fichier, 0 en cas d'erreur message, 1 sinon
*/
int informationCompte(char *idUser);

/**
\fn int voirVenteFinie(char *idUtilisateur);
\brief l'utilisateur demande les objets, mise en vente par lui meme, dont l'enchère est terminée
\param[in] idUtilisateur l'identifiant de l'utilisateur
\return -1 si erreur fichier, 0 en cas d'erreur message, 1 sinon
*/
int voirVenteFinie(char *idUtilisateur);

#endif // UTILISATEUR_H_INCLUDED
