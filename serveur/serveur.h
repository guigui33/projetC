#ifndef __SERVEUR_H__
#define __SERVEUR_H__

/* Initialisation.
 * Creation du serveur.
 * renvoie 1 si a c'est bien pass 0 sinon
 */
int Initialisation();

/* Initialisation.
 * Creation du serveur en prcisant le service ou numro de port.
 * renvoie 1 si a c'est bien pass 0 sinon
 */
int InitialisationAvecService(char *service);


/* Attends qu'un client se connecte.
 * renvoie 1 si a c'est bien pass 0 sinon
 */
int AttenteClient();

/* Recoit un message envoye par le client.
 * retourne le message ou NULL en cas d'erreur.
 * Note : il faut liberer la memoire apres traitement.
 */
char *Reception();

/* Envoie un message au client.
 * Attention, le message doit etre termine par \n
 * renvoie 1 si a c'est bien pass 0 sinon
 */
int Emission(char *message);

/* Recoit des donnees envoyees par le client.
 * renvoie le nombre d'octets reus, 0 si la connexion est ferme,
 * un nombre ngatif en cas d'erreur
 */
int ReceptionBinaire(char *donnees, size_t tailleMax);

/* Envoie des donnes au client en prcisant leur taille.
 * renvoie le nombre d'octets envoys, 0 si la connexion est ferme,
 * un nombre ngatif en cas d'erreur
 */
int EmissionBinaire(char *donnees, size_t taille);


/* Ferme la connexion avec le client.
 */
void TerminaisonClient();

/* Arrete le serveur.
 */
void Terminaison();

/*****************************************************************************************/
/**
\fn int verificationAuthentification(char *message,int tailleMsg)
\brief fonction qui verifie l'identifiant et le mot de passe du client
\param char *message un pointeur sur une chaine de caractères
\param int tailleMsg taille du message
\return un entier qui vaut 1 si connexion ok, -1 si erreur fichier, 0 sinon
*/
int verificationAuthentification(char *message,int tailleMsg);

/**
\fn verifCreationCompte(char *message,int longMsg)
\brief enregistre les données du nouvel utilisateur, les données sont verifiées, la cration est annulée si erreur.
\param char *message un pointeur sur une chaine de caractère
\param int longMsg taille du message
\return retourne 0 en cas d'erreur message, -1 en cas d'erreur fichier, 1 sinon.
*/
int verifCreationCompte(char *message,int longMsg);

/**
\fn int chercherFichierIdMdp(char idChr[6],char mdpChr[16])
\brief cherche un identifiant et son mot de passe dans le fichier user.txt
\param char idChr[6] l'identifiant à rechercher
\param char mdpChr[16] le mot de passe à rechercher
\return 1 si l'identifiant et le mode de passe sont corrects, 0 si l'identifiant ou le mot de passe est incorrect, -1 si erreur fichier
*/
int chercherFichierIdMdp(char idChr[6],char mdpChr[16]);

#endif
