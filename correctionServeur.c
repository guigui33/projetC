#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "serveur.h"

#define TRUE 1
#define FALSE 0

/* prototypes des fonctions */
int extraitFichier(char *requete, size_t longueurRequete, char *nomFichier, size_t maxNomFichier) ;
int fichierExiste(char *nomFichier) ;
size_t longueur_fichier(char *nomFichier) ;
int envoyerReponse200HTML(char *nomFichier) ;
int envoyerReponse404(char *nomFichier) ;
int envoyerReponse500(char *message) ;
int envoyerContenuFichierTexte(char *nomFichier) ;
int envoyerReponse200JPG(char *nomFichier) ;
int envoyerContenuFichierBinaire(char *nomFichier) ;
int envoyerReponse200ICO(char *nomFichier) ;

/* fonction main */
int main() {
	/* variables */
	char *requete = NULL;
	char fichier[1024];
	char fichierAvecRacine[2048];
	char *extension;

	/* programme principal */

	Initialisation();

	while(1) {
		AttenteClient();

		requete = Reception();

		if(requete != NULL) {
			printf("J'ai recu: %s\n", requete);

			/*
			 * Si la requ�te ne commence pas par "GET "
			 */
			if(strncmp(requete, "GET ", 4)!=0) {
				envoyerReponse500("Requ�te non g�r�e");
				fprintf(stderr, "Requ�te non g�r�e");
			}
			/*
			 * Extraction du nom du fichier
			 */
			else if(extraitFichier(requete, strlen(requete)+1, fichier, 1024)==0) {
				strncpy(fichierAvecRacine, ".", 2);
				strncat(fichierAvecRacine, fichier, 2046);

				/*
				 * teste si le fichier existe
				 */
				printf("Fichier demande : %s\n", fichier);
				if(!fichierExiste(fichierAvecRacine)) {
					envoyerReponse404(fichierAvecRacine);
					fprintf(stderr, "Le fichier n'existe pas\n");
				} else {
					extension = strrchr(fichier, '.');
					if(extension != NULL && (strncmp(extension, ".jpg", 4)==0)) {
						if(envoyerReponse200JPG(fichierAvecRacine))
							envoyerContenuFichierBinaire(fichierAvecRacine);
					} else if(extension != NULL && (strncmp(extension, ".ico", 4)==0)) {
						if(envoyerReponse200ICO(fichierAvecRacine))
							envoyerContenuFichierBinaire(fichierAvecRacine);
					} else if (extension != NULL && (strncmp(extension, ".html", 5)==0)) {
						if(envoyerReponse200HTML(fichierAvecRacine))
							envoyerContenuFichierTexte(fichierAvecRacine);
					} else {
						envoyerReponse500("Fichier non g�r�");
						fprintf(stderr, "Fichier non g�r�");
					}
				}
			} else {
				envoyerReponse500("Requ�te mal form�e");
				fprintf(stderr, "Requ�te mal form�e");
			}

			free(requete);
		}

		TerminaisonClient();
	}

	return 0;
}


int extraitFichier(char *requete, size_t longueurRequete, char *nomFichier, size_t maxNomFichier) {
	int indexRequete = 0;
	int indexFichier = 0;
	int fini = 0;
	/*
	 * saute le nom de la commande
	 */
	while(!fini && indexRequete < longueurRequete) {
		/*
		 * le nom de la commande se termine par un espace
		 */
		if(requete[indexRequete] != ' ') {
			/* on n'a pas trouv� l'espace, on avance */
			indexRequete++;
		} else {
			fini = 1;
		}
	}
	/*
	 * Est-ce que ca c'est bien passe ?
	 */
	if (!fini) {
		fprintf(stderr, "La requete etait trop longue ou malformee.\n");
		return -1;
	}
	/*
	 * extraction du nom du fichier
	 */
	fini = 0;
	/*
	 * on saute l'espace apr�s le nom de la commande
	 */
	indexRequete++;
	while(!fini && indexFichier < (maxNomFichier - 1) && indexRequete < longueurRequete) {
		/*
		 * le fichier se termine par un espace
		 */
		if(requete[indexRequete] != ' ') {
			/* on n'a pas trouv� l'espace, on avance en recopiant
			   les caract�res du nom de fichier */
			nomFichier[indexFichier] = requete[indexRequete];
			indexFichier++;
			indexRequete++;
		} else {
			fini = 1;
		}
	}
	if(fini) {
		nomFichier[indexFichier] = '\0';
		return 0;
	} else {
		fprintf(stderr, "On n'a pas trouve le nom du fichier.\n");
		return -1;
	}
}

int fichierExiste(char *nomFichier) {
	struct stat infosFichier;

	/* utilise la fonction stat pour savoir si le fichier existe
	 * et S_ISREG pour savoir si c'est un fichier normal
	 * Note : on peut aussi essayer d'ouvrir le fichier en lecture
	 */
	if((stat(nomFichier, &infosFichier) != -1) &&
	   S_ISREG(infosFichier.st_mode))
	{
		return TRUE;
	} else {
		return FALSE;
	}
}


size_t longueur_fichier(char *nomFichier) {
	FILE *fichier;
	size_t taille = 0;

	/* ouvre le fichier en lecture */
	fichier = fopen(nomFichier, "r");
	if(fichier == NULL) {
		perror("Impossible d'ouvrir le fichier.");
		return -1;
	}

	/* d�place le pointeur de fichier � la fin */
	if(fseek(fichier, 0, SEEK_END) != 0) {
		perror("Impossible d'atteindre la fin du fichier");
	    return -1;
	}
	/* calcule la position du pointeur de fichier */
	taille = ftell(fichier);
	/* ferme le fichier */
	fclose(fichier);

	/* renvoie la taille ou l'erreur de ftell */
	return taille;
}

int envoyerContenuFichierTexte(char *nomFichier) {
	FILE *fichier;
	char ligne[4096];
	char *retour = NULL;

	fichier = fopen(nomFichier, "r");
	if(fichier == NULL) {
		perror("Impossible d'ouvrir le fichier.");
		return FALSE;
	}
	/*
	 * On a pu ouvrir le fichier. On envoie le contenu
	 */
	while(!feof(fichier)) {
		retour = fgets(ligne, 4096, fichier);
		if(retour == NULL && ferror(fichier)) {
			perror("Erreur de fgets.");
			fclose(fichier);
			return FALSE;
		} else {
			if (retour != NULL && !Emission(ligne)) {
				fclose(fichier);
				return FALSE;
			}
		}
	}
	fclose(fichier);
	return TRUE;
 }

int envoyerReponse200HTML(char *nomFichier)  {
	/* variables */
	size_t taille; // taille du fichier
	char ligne[1024]; // ent�te � renvoyer au client
	int retour; // code de retour

	/* calcule la taille du fichier */
	taille = longueur_fichier(nomFichier);

	/* si elle est correcte */
	if (taille > 0) {
		/* on envoit la premi�re ligne */
		retour = Emission("HTTP/1.0 200 OK\r\n");
		if(!retour) return FALSE;
		/* on envoit l'ent�te de type */
		retour = Emission("Content-Type: text/html\r\n");
		if(!retour) return FALSE;
		/* on pr�pare et envoit l'ent�te de longueur */
		sprintf(ligne, "Content-Length: %u\r\n", (unsigned int)taille);
		retour = Emission(ligne);
		if(!retour) return FALSE;
		/* on envoie la ligne vide */
		retour = Emission("\r\n");
		if(!retour) return FALSE;
		return TRUE;
	} else
		return FALSE;
}

int envoyerReponse200ICO(char *nomFichier)  {
	/* variables */
	size_t taille; // taille du fichier
	char ligne[1024]; // ent�te � renvoyer au client
	int retour; // code de retour

	/* calcule la taille du fichier */
	taille = longueur_fichier(nomFichier);

	/* si elle est correcte */
	if (taille > 0) {
		/* on envoit la premi�re ligne */
		retour = Emission("HTTP/1.0 200 OK\r\n");
		if(!retour) return FALSE;
		/* on envoit l'ent�te de type */
		retour = Emission("Content-Type: image/vnd.microsoft.icon\r\n");
		if(!retour) return FALSE;
		/* on pr�pare et envoit l'ent�te de longueur */
		sprintf(ligne, "Content-Length: %u\r\n", (unsigned int)taille);
		retour = Emission(ligne);
		if(!retour) return FALSE;
		/* on envoie la ligne vide */
		retour = Emission("\r\n");
		if(!retour) return FALSE;
		return TRUE;
	} else
		return FALSE;
}

int envoyerReponse200JPG(char *nomFichier)  {
	/* variables */
	size_t taille; // taille du fichier
	char ligne[1024]; // ent�te � renvoyer au client
	int retour; // code de retour

	/* calcule la taille du fichier */
	taille = longueur_fichier(nomFichier);

	/* si elle est correcte */
	if (taille > 0) {
		/* on envoit la premi�re ligne */
		retour = Emission("HTTP/1.0 200 OK\r\n");
		if(!retour) return FALSE;
		/* on envoit l'ent�te de type */
		retour = Emission("Content-Type: image/jpeg\r\n");
		if(!retour) return FALSE;
		/* on pr�pare et envoit l'ent�te de longueur */
		sprintf(ligne, "Content-Length: %u\r\n", (unsigned int)taille);
		retour = Emission(ligne);
		if(!retour) return FALSE;
		/* on envoie la ligne vide */
		retour = Emission("\r\n");
		if(!retour) return FALSE;
		return TRUE;
	} else
		return FALSE;
}

int envoyerReponse404(char *nomFichier) {
	/* variables */
	char entete[1024]; // entete � renvoyer au client
	char contenu[4096]; // contenu � renvoyer au client
	int retour; // code de retour

	/* on envoie la premi�re ligne */
	retour = Emission("HTTP/1.0 404 Fichier non lisible\r\n");
	if(!retour) return FALSE;
	/* on envoie l'ent�te de type */
	retour = Emission("Content-Type: text/html\r\n");
	if(!retour) return FALSE;
	/* on pr�pare le contenu */
	sprintf(contenu, "<html>\n<head>\n<title>Fichier non trouve</title>\n</head>\n"
			"<body>\n<h1>Fichier non trouve : %1023s</h1>\n</body>", nomFichier);
	/* on pr�pare et envoie l'ent�te de longueur */
	sprintf(entete, "Content-Length: %u\r\n", (unsigned int)strlen(contenu));
	retour = Emission(entete);
	if(!retour) return FALSE;
	/* on envoie la ligne vide */
	retour = Emission("\r\n");
	if(!retour) return FALSE;
	/* on envoie le contenu */
	retour = Emission(contenu);
	if(!retour) return FALSE;
	return TRUE;
}

int envoyerReponse500(char *message) {
	/* variables */
	char entete[1024]; // entete � renvoyer au client
	char contenu[4096]; // contenu � renvoyer au client
	int retour; // code de retour

	/* on envoie la premi�re ligne */
	retour = Emission("HTTP/1.0 500 Erreur serveur\r\n");
	if(!retour) return FALSE;
	/* on envoie l'ent�te de type */
	retour = Emission("Content-Type: text/html\r\n");
	if(!retour) return FALSE;
	/* on pr�pare le contenu */
	sprintf(contenu, "<html>\n<head>\n<title>Erreur du serveur</title>\n</head>\n"
			"<body>\n<h1>Erreur du serveur : %1024s</h1>\n</body>", message);
	/* on pr�pare et envoie l'ent�te de longueur */
	sprintf(entete, "Content-Length: %u\r\n", (unsigned int) strlen(contenu));
	retour = Emission(entete);
	if(!retour) return FALSE;
	/* on envoie la ligne vide */
	retour = Emission("\r\n");
	if(!retour) return FALSE;
	/* on envoie le contenu */
	retour = Emission(contenu);
	if(!retour) return FALSE;
	return TRUE;
}

int envoyerContenuFichierBinaire(char *nomFichier) {
	FILE *fichier;
	char ligne[4096];
	int retour = 0;

	fichier = fopen(nomFichier, "r");
	if(fichier == NULL) {
		perror("Impossible d'ouvrir le fichier.");
		return FALSE;
	}

	/*
	 * On a pu ouvrir le fichier. On envoie le contenu en binaire
	 */
	while(!feof(fichier)) {
		retour = fread(ligne, 1, 4096, fichier);
		if(retour < 0) {
			perror("Erreur de fread.");
			fclose(fichier);
			return FALSE;
		} else if(retour > 0) {
			retour = EmissionBinaire(ligne, retour);
			if(retour < 0) {
				fprintf(stderr, "Erreur lors de l'envoi de %s\n", nomFichier);
				fclose(fichier);
				return FALSE;
			}
		}
	}
	fclose(fichier);
	return TRUE;
}


