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
    size_t fin=0;
    char choix[2];
    int retour;
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

        supprCara(choix);
        switch(choix[0])
        {
        case '1':
            retour = authentification();
            if(retour==1)
            {
                menuUtilisateur();//Afficher catalogue
            }
            if(retour!=1)
            {
                printf("Erreur indentifiant ou mot de passe.Ou erreur Serveur. \n");
            }


            break;
        case '2':
            creationDeCompte();
            break;
        case '0':
            exit(1);
            break;
        default:
            printf("erreur dans le choix, recommencez.\n");
            break;
        }

    }

    return;
}

int authentification(char * idUser)
{
    char id[6];// contient l'identifiant de l'utilisateur
    char motPasse[16];// contient le mot de passe de l'utilisateur
    printf("*** Menu connexion ***\n");
    printf("Id: ");
    if(fgets(id,6,stdin)==NULL)
    {
        perror("erreur pour l'entrée identifiant\n");
        return -1; // on quitte le prog
    }

    supprCara(id);

    printf("mot de passe: ");
    if(fgets(motPasse,16,stdin)==NULL)
    {
        perror("erreur pour l'entrée mot de passe\n");
        return -1; // on quitte le prog
    }

    supprCara(motPasse);// suppr '\n'
    strcpy(idUser,id);
    //printf("%s\n",msgConnexion);
/*
    if(Retour serveurok){return 1;};
    if(Mdp ou identifiant incorecte){return 0;};
    if(Erreur serveur){return -1;};*/
    return 1;
}

void creationDeCompte() // a faire TomZzy
{
    char mdp1[16];
    char mdp2[16];
    char nom[50];
    char prenom[50];
    char adresse[100];//Tampon adresse mail et/ou adresse postal.
    char tmp[11];//variable tampon pour la date, le code postal.
    char ville[31];
    int mdp_ok = 0;
    char msg_serveur[1500]="CREATE";// determiner le nombre de caractere max !!
    printf("création de compte:\n");

    do{

        printf("Nom:");
        fgets(nom,50,stdin);
        supprCara(nom);
    }while(strlen(nom)<2);

    strcat(msg_serveur,"#");
    strncat(msg_serveur,nom,strlen(msg_serveur)+strlen(nom));

    do{
        printf("Prenom:");
        fgets(prenom,50,stdin);
        supprCara(prenom);
    }while(strlen(prenom)<2);

    strcat(msg_serveur,"#");
    strncat(msg_serveur,prenom,strlen(msg_serveur)+strlen(prenom));

    do{

        printf("mot de passe:");
        fgets(mdp1,16,stdin);
        supprCara(mdp1);
        mdp_ok=verifier_mdp(mdp1);


        if(mdp_ok==1)
        {
            printf("confirmation mot de passe:");
            fgets(mdp2,16,stdin);
            supprCara(mdp2);
        }
    }while( mdp_ok !=1 ||strcmp(mdp1,mdp2)!=0 );

    do{

        printf("Date de naissance Jour (ex : 01):");
        fgets(tmp,3,stdin);
        supprCara(tmp);
    }while (strlen(tmp)!=2);

    strcat(msg_serveur,"/");
    strncat(msg_serveur,tmp,strlen(msg_serveur)+strlen(tmp));

    do{

        printf("Date de naissance mois (ex : 01):");
        fgets(tmp,3,stdin);
        supprCara(tmp);
    }while (strlen(tmp)!=2);

    strcat(msg_serveur,"/");
    strncat(msg_serveur,tmp,strlen(msg_serveur)+strlen(tmp));

    do{

        printf("Date de naissance année (ex : 1994):");
        fgets(tmp,5,stdin);
        supprCara(tmp);
    }while (strlen(tmp)!=4);

    strcat(msg_serveur,"/");
    strncat(msg_serveur,tmp,strlen(msg_serveur)+strlen(tmp));

    do{
        printf("Numero de telephone:");
        fgets(tmp,11,stdin);
        supprCara(tmp);
    }while(strlen(tmp)!=10);

    strcat(msg_serveur,"#");
    strncat(msg_serveur,tmp,strlen(msg_serveur)+strlen(tmp));


    printf("Adresse mail:");
    fgets(adresse,101,stdin);
    supprCara(adresse);

    strcat(msg_serveur,"#");
    strncat(msg_serveur,adresse,strlen(msg_serveur)+strlen(adresse));


    printf("Adresse postal:");
    fgets(adresse,101,stdin);
    supprCara(adresse);

    strcat(msg_serveur,"#");
    strncat(msg_serveur,adresse,strlen(msg_serveur)+strlen(adresse));

    printf("Ville:");
    fgets(ville,31,stdin);
    supprCara(ville);

    strcat(msg_serveur,"#");
    strncat(msg_serveur,ville,strlen(msg_serveur)+strlen(ville));

    do
    {
        printf("Code postal:");
        fgets(tmp,6,stdin);
        supprCara(tmp);
    }while(strlen(tmp)!=5);

    strcat(msg_serveur,"#");
    strncat(msg_serveur,tmp,strlen(msg_serveur)+strlen(tmp));

    //Test
    printf("%s\n",msg_serveur);

    return;
}

void catalogue(){
size_t fin=0;
    char choix[2];
    int retour;
    while(!fin)
    {
        printf("-------Catalogue-------\n");
        printf("1)Suivant\n2)Precedent\n3)Acheter\n0)quitter\n");
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
//                suivant();
            break;
        case '2':

//                precedent();
            break;
        case '3':

//                acheter();
            break;
        case '0':
            menuUtilisateur();
            break;
        default:
            printf("erreur dans le choix, recommencez.\n");
            break;
        }

    }

    return;
}
void menuUtilisateur(char * idUser){
    size_t fin=0;
    char choix[2];
    int retour;
    while(!fin)
    {
        printf("-------Menu Utilisateur-------\n");
        printf("1)Consulter catalogue\n2)Vendre un objet\n0)quitter\n");
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
                catalogue();
            break;
        case '2':
                vendre(idUser);
            break;
        case '0':
                menuConnex();
            break;
        default:
            printf("erreur dans le choix, recommencez.\n");
            break;
        }

    }

    return;

}

void vendre(char * idUser){
    //Pour la date la recuperer en MS lui ajouter 15 jours en MS puis le convertir pour l'envoyer a guillaume.
    char msg_serveur[1500]="VENDRE";
    float prix=0;
    char nom[50];
    char tmp[11];
    char description[255];
    char format_debut[128];
    char format_fin[128];
    time_t temps;
    time_t secondes_fin;
    struct tm date_debut;
    struct tm date_fin;
    printf("Vente d'un objet: \n");

    printf("Nom de l'objet:");
    fgets(nom,51,stdin);
    supprCara(nom);

    strcat(msg_serveur,"#");
    strcat(msg_serveur,nom);

    do{
    printf("Prix (entre 0.00 et 99 9999.99):");
    scanf("%f",&prix);
    printf("prix=%f \n",prix);
    }while(prix<0 || prix >= 100000);

    viderBuffer();
    sprintf(msg_serveur,"%s#%5.2f",msg_serveur,prix);

    printf("Date fin d'enchere:");

    // On récupère la date et l'heure actuelles.
    time(&temps);
    date_debut=*localtime(&temps);

    // On remplit la chaîne avec le format choisi, puis on l'affiche.
    strftime(format_debut, 128, "%d/%m/%Y", &date_debut);
    puts(format_debut);

    //On y ajoute 7 jours pour  obtenir la date de fin d'enchere.
    date_fin=date_debut;
    date_fin.tm_mday+=7;
    secondes_fin=mktime(&date_fin);
    strftime(format_fin, 128, "%d/%m/%Y", &date_fin);
    puts(format_fin);

    sprintf(msg_serveur,"%s#%s#%s",msg_serveur,format_debut,format_fin);
    printf("%s",msg_serveur);
    return 0;



    }

/*A faire*/ void acheter(char * idUser){
    printf("Acheter l'objet : \n");



}

bool verifier_mdp(char * mdp)
{
    if(strlen(mdp)< 5)
    {
        return 0;
    }

    if(strchr(mdp,'#') != NULL)
    {
        return 0;
    }

    return 1;
}

void supprCara(char *chaine)
{
    char *position=NULL; // util pour suppr le \n
    position=strchr(chaine,'\n');

    if(position!=NULL)
    {
        *position='\0';
    }
    else
    {
        viderBuffer(); // \n n'est pas trouvé, l'utilisateur a tapé plus de 6 caractères on doit vider le buffer
    }

    return;
}

void viderBuffer()
{
    char c;
    c=fgetc(stdin);
    while(c!=EOF && c!='\n')
    {
        c=fgetc(stdin);
    }

    return;
}
