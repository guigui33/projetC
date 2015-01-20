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

/**
\fn int extraireTypeRequete(char *message,int tailleMsg,char *donnee,char *typeRequete)
\brief extrait le type de requète du message
\param[in] message tableau de char contenant le message de l'utilisateur
\param[in] tailleMsg taille du message
\param[out] donnee pointeur sur le debut des donnees utilisateur
\param[out] typeRequete tableau de char contenant le type de requète (est defini <10 char)
\return un entier, 1 si tout c'est bien passé, 0 si problème message, -1 sinon
*/
int extraireTypeRequete(char *message,int tailleMsg,char *donnee,char *typeRequete);

#endif
