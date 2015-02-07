#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>

#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#endif

#include <errno.h>

#ifdef WIN32
#define perror(x) printf("%s : code d'erreur : %d\n", (x), WSAGetLastError())
#define close closesocket
#define socklen_t int
#endif

#include "client.h"
#include "secondaire.h"
#include "utilisateur.h"

#define TRUE 1
#define FALSE 0

#define LONGUEUR_TAMPON 4096

/* Variables cachees */
/*Typedef*/
typedef int bool;
/* le socket client */
int socketClient;
/* le tampon de reception */
char tamponClient[LONGUEUR_TAMPON];
int debutTampon;
int finTampon;
int finConnexion = FALSE;

/* Initialisation.
 * Connexion au serveur sur la machine donnee.
 * Utilisez localhost pour un fonctionnement local.
 */
int Initialisation(char *machine)
{
    return InitialisationAvecService(machine, "13214");
}

/* Initialisation.
 * Connexion au serveur sur la machine donnee et au service donne.
 * Utilisez localhost pour un fonctionnement local.
 */
int InitialisationAvecService(char *machine, char *service)
{
    int n;
    struct addrinfo	hints, *res, *ressave;

#ifdef WIN32
    WSADATA	wsaData;
    if (WSAStartup(0x202,&wsaData) == SOCKET_ERROR)
    {
        printf("WSAStartup() n'a pas fonctionne, erreur : %d\n", WSAGetLastError()) ;
        WSACleanup();
        exit(1);
    }
    memset(&hints, 0, sizeof(struct addrinfo));
#else
    bzero(&hints, sizeof(struct addrinfo));
#endif
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ( (n = getaddrinfo(machine, service, &hints, &res)) != 0)
    {
        fprintf(stderr, "Initialisation, erreur de getaddrinfo : %s", gai_strerror(n));
        return 0;
    }
    ressave = res;

    do
    {
        socketClient = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (socketClient < 0)
            continue;	/* ignore this one */

        if (connect(socketClient, res->ai_addr, res->ai_addrlen) == 0)
            break;		/* success */

        close(socketClient);	/* ignore this one */
    }
    while ( (res = res->ai_next) != NULL);

    if (res == NULL)
    {
        perror("Initialisation, erreur de connect.");
        return 0;
    }

    freeaddrinfo(ressave);

    finConnexion = FALSE;

    printf("Connexion avec le serveur reussie.\n");

    return 1;
}

/* Recoit un message envoye par le serveur.
 */
char *Reception()
{
    char message[LONGUEUR_TAMPON];
    int index = 0;
    int fini = FALSE;
    int retour = 0;
    int trouve = FALSE;

    if(finConnexion)
    {
        return NULL;
    }

    while(!fini)
    {
        /* on cherche dans le tampon courant */
        while((finTampon > debutTampon) && (!trouve))
        {
            //fprintf(stderr, "Boucle recherche char : %c(%x), index %d debut tampon %d.\n",
            //					tamponClient[debutTampon], tamponClient[debutTampon], index, debutTampon);
            if (tamponClient[debutTampon]=='\n')
                trouve = TRUE;
            else
                message[index++] = tamponClient[debutTampon++];
        }
        /* on a trouve ? */
        if (trouve)
        {
            message[index++] = '\n';
            message[index] = '\0';
            debutTampon++;
            fini = TRUE;
            //fprintf(stderr, "trouve\n");
#ifdef WIN32
            return _strdup(message);
#else
            return strdup(message);
#endif
        }
        else
        {
            /* il faut en lire plus */
            debutTampon = 0;
            //fprintf(stderr, "recv\n");
            retour = recv(socketClient, tamponClient, LONGUEUR_TAMPON, 0);
            //fprintf(stderr, "retour : %d\n", retour);
            if (retour < 0)
            {
                perror("Reception, erreur de recv.");
                return NULL;
            }
            else if(retour == 0)
            {
                fprintf(stderr, "Reception, le serveur a ferme la connexion.\n");
                finConnexion = TRUE;
                // on n'en recevra pas plus, on renvoie ce qu'on avait ou null sinon
                if(index > 0)
                {
                    message[index++] = '\n';
                    message[index] = '\0';
#ifdef WIN32
                    return _strdup(message);
#else
                    return strdup(message);
#endif
                }
                else
                {
                    return NULL;
                }
            }
            else
            {
                /*
                 * on a recu "retour" octets
                 */
                finTampon = retour;
            }
        }
    }
    return NULL;
}

/* Envoie un message au serveur.
 * Attention, le message doit etre termine par \n
 */
int Emission(char *message)
{
    int taille;
    if(strstr(message, "\n") == NULL)
    {
        fprintf(stderr, "Emission, Le message n'est pas termine par \\n.\n");
    }
    taille = strlen(message);
    if (send(socketClient, message, taille,0) == -1)
    {
        perror("Emission, probleme lors du send.");
        return 0;
    }
    printf("Emission de %d caracteres.\n", taille+1);
    return 1;
}

/* Recoit des donnees envoyees par le serveur.
 */
int ReceptionBinaire(char *donnees, size_t tailleMax)
{
    size_t dejaRecu = 0;
    int retour = 0;
    /* on commence par recopier tout ce qui reste dans le tampon
     */
    while((finTampon > debutTampon) && (dejaRecu < tailleMax))
    {
        donnees[dejaRecu] = tamponClient[debutTampon];
        dejaRecu++;
        debutTampon++;
    }
    /* si on n'est pas arrive au max
     * on essaie de recevoir plus de donnees
     */
    if(dejaRecu < tailleMax)
    {
        retour = recv(socketClient, donnees + dejaRecu, tailleMax - dejaRecu, 0);
        if(retour < 0)
        {
            perror("ReceptionBinaire, erreur de recv.");
            return -1;
        }
        else if(retour == 0)
        {
            fprintf(stderr, "ReceptionBinaire, le serveur a ferme la connexion.\n");
            return 0;
        }
        else
        {
            /*
             * on a recu "retour" octets en plus
             */
            return dejaRecu + retour;
        }
    }
    else
    {
        return dejaRecu;
    }
}

/* Envoie des donnees au serveur en precisant leur taille.
 */
int EmissionBinaire(char *donnees, size_t taille)
{
    int retour = 0;
    retour = send(socketClient, donnees, taille, 0);
    if(retour == -1)
    {
        perror("Emission, probleme lors du send.");
        return -1;
    }
    else
    {
        return retour;
    }
}


/* Ferme la connexion.
 */
void Terminaison()
{
    close(socketClient);
}

/*
*********************************************************************************************
*/

void menuConnex()
{
    size_t fin=0;//boolean de fin de boucle
    char choix[2];//recupere le choix de l'utilisateur
    int retour;//retour de fonction
    char idUtilisateur[6];//l'identifiant de l'utilisateur

    while(!fin)
    {
        printf("-------Menu-------\n");
        printf("1)Authentification\n2)Création de compte\n0)quitter\n");
        printf("choix: ");

        if(fgets(choix,2,stdin)==NULL)
        {
            perror("probleme dans l'entrée choix\n");
            exit(1); // on quitte le programme
        }

        supprCara(choix);// on suppr le \n et on vide le buffer
        switch(choix[0])
        {
        case '1':
            retour = authentification(idUtilisateur);
            if(retour==1)
            {
                printf("****connexion****\n");
                menuUtilisateur(idUtilisateur);
            }
            else if(retour==0)
            {
                printf("L'identifiant et le mot de passe entrés par l'utilisateur ne sont pas corrects.\n");
            }
            else
            {
                printf("Le serveur a rencontré un problème.\n");
            }
            break;
        case '2':
            retour=creationDeCompte();
            if(retour==1)
            {
                printf("Fin de l'enregistrement.\n");
            }
            else if(retour==0)
            {
                printf("les informations sont incorrectes.\n");
            }
            else
            {
                printf("Le serveur a rencontré un problème, veuillez recommencer.\n");
            }
            break;
        case '0':
            EmissionBinaire("Deco\n",5);
            fin=1;
            break;
        default:
            printf("Erreur dans le choix, recommencez.\n");
            break;
        }
    }
    return;
}

void menuUtilisateur(char * idUser)
{
    size_t fin=0;
    char choix[2];
    int retour=0;//retour fonction
    while(!fin)
    {
        printf("-------Menu Utilisateur-------\n");
        printf("1)Rechercher produits\n2)Vendre un produit\n3)Compte\n0)Quitter\n");
        printf("choix: ");

        if(fgets(choix,2,stdin)==NULL)
        {
            perror("probleme dans l'entrée choix\n");
            exit(1); // on quitte le programme
        }

        supprCara(choix);
        switch(choix[0])
        {
        case '1':
            retour=rechercheObjet(idUser);
            if(retour==1)
            {
                printf("Chargement catalogue...\n");
                if(catalogue(idUser)==-1)
                {
                    printf("le catalogue n'a pu etre ouvert.\n");
                }
            }
            else if(retour==0)
            {
                printf("Le catalogue n'a pu être envoyé, les données envoyées ne sont pas correctes.\n");
            }
            else
            {
                printf("Le catalogue n'a pu être envoyé, problème serveur.\n");
            }
            break;
        case '2':
            retour=vendre(idUser);
            if(retour==1)
            {
                printf("Votre objet est maintenant disponible sur le catalogue du site.\n");
            }
            else if(retour==0)
            {
                printf("L'enregistrement n'a pu se faire, les données envoyées ne sont pas correctes.\n");
            }
            else
            {
                printf("L'enregistrement n'a pu se faire, problème serveur.\n");
            }
            break;
        case '3':
            retour=informationCompte(idUser);
            if(retour==1)
            {
                printf(".......\n");
            }
            else if(retour==0)
            {
                printf("Les informations du compte ne sont pas disponible.\n");
            }
            else
            {
                printf("Suite à un problème serveur, nous ne pouvons transmettre les données du compte.\n");
            }
            break;
        case '0':
            printf("Retour Menu Connexion...\n");
            fin=1;
            break;
        default:
            printf("erreur dans le choix, recommencez.\n");
            break;
        }
    }
    return;
}

