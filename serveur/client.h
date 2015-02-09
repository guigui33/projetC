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
\param[out] id identifiant utilisateur
\return retourne 0 en cas d'erreur message, -1 en cas d'erreur fichier, 1 sinon.
*/
int creationCompte(char *message,int longMsg,char *id);

/**
\fn int chercherFichierIdMdp(char *idChr,char *mdpChr)
\brief cherche un identifiant et son mot de passe dans le fichier user.txt
\param[in] idChr l'identifiant à rechercher
\param[in] *mdpChr le mot de passe à rechercher
\return 1 si l'identifiant et le mode de passe sont corrects, 0 si l'identifiant ou le mot de passe est incorrect, -1 si erreur fichier
*/
int chercherFichierIdMdp(char *idChr,char *mdpChr);


/**
\fn int creationId(char *message,int taille,char *id)
\brief creer l'identifiant d'un nouveau utilisateur
\param[in] message tableau de char contenant les informations de l'utilisateur
\param[in] taille la taille du message
\param[out] char idUti[6] tableau de char de taille 6
\return 1 si id a été créé, -1 si erreur d'ouverture fichier, 0 sinon
*/
int creationId(char *message,int taille,char *idUti);

/**
\fn int extraireIdClient(char *message,int tailleMsg,char *id)
\brief extraire l'id de l'utilisateur du message, on verifie que l'id existe, le "message" pointe à la fin de la fonction sur ce qui precede l'id.
\param[in][out] message tableau de char contenant l'id de l'utilisateur + les données du message
\param[in] tailleMsg taille du message
\param[out] id tableau de char de taille 6
\return 1 si id a été extrait et qu'il existe, -1 si erreur d'ouverture fichier, 0 identifiant n'existe pas.
*/
int extraireIdClient(char **message,int tailleMsg,char *id);

/**
\fn int incrementerNbrObjet(char *idAcheteur,char *idVendeur)
\brief permet de modifier le fichier utilisateur en modifiant les valeurs de vente et achat effectuées par les utilisateurs
\param[in] idAcheteur identifiant de l'acheteur de taille 5
\param[in] idVendeur identifiant du vendeur de taille 5
\return un entier -1 si erreur de fichier, 1 sinon
*/
int incrementerNbrObjet(char *idAcheteur,char *idVendeur);

/**
\fn int donneeUtilisateur(char *idUtilisateur,char *msgClient)
\brief concatene les informations de l'utilisateur de l'identifiant dans le msgClient
\param[in] idUtilisateur l'identifiant de l'utilisateur
\param[out] msgClient chaine contenant des informations
\param[in] fonction nom de la fonction qui utilise cette fonction
\return -1 si probleme fichier, 1 sinon
*/
int donneeUtilisateur(char *idPro,char *msgClient,char *fonction);

/**
\fn int informationUtilisateur(char *message,int tailleMsg)
\brief envoie les informations concernant l'utilisateur
\param[in] message tableau de char contenant les informations de l'utilisateur
\param[in] taille la taille du message
\param[in] f determine le type d'information à envoyer
\return -1 si probleme fichier, 0 si probleme message, 1 sinon
*/
int informationUtilisateur(char *message,int tailleMsg,int f);

/**
\fn void recupererInfoUti(char *msg,char *c,char separateur,FILE *file)
\brief recuperer une information cilbée dans le fichier utilisateur et concatene dans le msg
\param[out] msg pointeur sur une chaine
\param[in][out] c pointeur sur un caractère, permet de savoir où on est dans le fichier
\param[in] separateur un caratère qui termine la lecture de l'information
\param[in] file un pointeur sur un fichier
*/
void recupererInfoUti(char *msg,char *c,char separateur,FILE *file);

#endif // CLIENT_H_INCLUDED
