#ifndef __CLIENT_H__
#define __CLIENT_H__

/* Initialisation.
 * Connexion au serveur sur la machine donnee.
 * Utilisez localhost pour un fonctionnement local.
 * renvoie 1 si ça c'est bien passé 0 sinon
 */
int Initialisation(char *machine);

/* Initialisation.
 * Connexion au serveur sur la machine donnee et au service donne.
 * Utilisez localhost pour un fonctionnement local.
 * renvoie 1 si ça c'est bien passé 0 sinon
 */
int InitialisationAvecService(char *machine, char *service);

/* Recoit un message envoye par le serveur.
 * retourne le message ou NULL en cas d'erreur.
 * Note : il faut liberer la memoire apres traitement.
 */
char *Reception();

/* Envoie un message au serveur.
 * Attention, le message doit etre termine par \n
 * renvoie 1 si ça c'est bien passé 0 sinon
 */
int Emission(char *message);

/* Recoit des donnees envoyees par le serveur.
 * renvoie le nombre d'octets reçus, 0 si la connexion est fermée,
 * un nombre négatif en cas d'erreur
 */
int ReceptionBinaire(char *donnees, size_t tailleMax);

/* Envoie des données au serveur en précisant leur taille.
 * renvoie le nombre d'octets envoyés, 0 si la connexion est fermée,
 * un nombre négatif en cas d'erreur
 */
int EmissionBinaire(char *donnees, size_t taille);

/* Ferme la connexion.
 */
void Terminaison();

/**
\fn void menuConnex(char *msgConnexion)
\brief fonction permettant d'afficher le menu, l'utilisateur fait sont choix
\param char* une chaine de caractËre permettant de retourner les paramÍtres de connexion
    la chaine peut avoir la valeur "quitter" ou l'identifiant suivit du mot de passe
\return
*/
void menuConnex();

/**
\fn void authentification(char *msgConnexion)
\brief fonction pemettant d'afficher le menu d'authentification, l'utilisateur entre son mot de passe et son identifiant
\param char* une chaine de caratËre permettant de sauvegarder l'identifiant et le mot de passe
\return
*/
int authentification();

/**
\fn void creationDeCompte()
\brief fonction permettant d'afficher la creation de compte
\param
\return
*/
void creationDeCompte();

/**
\fn void viderBuffer()
\brief vide le buffer
*/
void viderBuffer();

/**
\fn void supprCara(char *chaine)
\brief fonction qui permet de remplacer le '\n' par '\0'
        si l'utilisateur a entrÈe trop de caractËre (le '\n' n'est pas trouvÈ) on vide le buffer
\param char* un pointeur sur une chaine
\return
*/
void supprCara(char *chaine);

/**
\fn void menuUtilisateur();
\brief fonction pemettant d'afficher a l'utilisateur le choix entre la vente ou la consultation d'objet. 1 pour la consultation 2 pour la vente 0 pour Quitter
\param
\return
*/
void menuUtilisateur();

/**
\fn void catalogue(char * msgConnexion)
\brief Fonction qui permet d'afficher le catalogue et de naviguer entre les objets ou d'acheter.1Suivant 2 Precendent 3 Acheter 0 Quitter
\param
\return
*/
void catalogue();

/**
\fn void vendre();
\brief Fonction qui permet d'afficher le catalogue et de naviguer entre les objets ou d'acheter.1Suivant 2 Precendent 3 Acheter 0 Quitter
\param
\return
*/
void vendre(char * idUser);

#endif
