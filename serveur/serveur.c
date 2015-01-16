#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
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

#include "serveur.h"

#define TRUE 1
#define FALSE 0
#define LONGUEUR_TAMPON 4096


#ifdef WIN32
#define perror(x) printf("%s : code d'erreur : %d\n", (x), WSAGetLastError())
#define close closesocket
#define socklen_t int
#endif

/* Variables cachees */

/* le socket d'ecoute */
int socketEcoute;
/* longueur de l'adresse */
socklen_t longeurAdr;
/* le socket de service */
int socketService;
/* le tampon de reception */
char tamponClient[LONGUEUR_TAMPON];
int debutTampon;
int finTampon;
int finConnexion = 0;


/* Initialisation.
 * Creation du serveur.
 */
int Initialisation()
{
    return InitialisationAvecService("13214");
}

/* Initialisation.
 * Creation du serveur en precisant le service ou numero de port.
 * renvoie 1 si ca c'est bien passe 0 sinon
 */
int InitialisationAvecService(char *service)
{
    int n;
    const int on = 1;
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

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ( (n = getaddrinfo(NULL, service, &hints, &res)) != 0)
    {
        printf("Initialisation, erreur de getaddrinfo : %s", gai_strerror(n));
        return 0;
    }
    ressave = res;

    do
    {
        socketEcoute = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (socketEcoute < 0)
            continue;		/* error, try next one */

        setsockopt(socketEcoute, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
#ifdef BSD
        setsockopt(socketEcoute, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on));
#endif
        if (bind(socketEcoute, res->ai_addr, res->ai_addrlen) == 0)
            break;			/* success */

        close(socketEcoute);	/* bind error, close and try next one */
    }
    while ( (res = res->ai_next) != NULL);

    if (res == NULL)
    {
        perror("Initialisation, erreur de bind.");
        return 0;
    }

    /* conserve la longueur de l'addresse */
    longeurAdr = res->ai_addrlen;

    freeaddrinfo(ressave);
    /* attends au max 4 clients */
    listen(socketEcoute, 4);
    printf("Creation du serveur reussie.\n");

    return 1;
}

/* Attends qu'un client se connecte.
 */
int AttenteClient()
{
    struct sockaddr *clientAddr;
    char machine[NI_MAXHOST];

    clientAddr = (struct sockaddr*) malloc(longeurAdr);
    socketService = accept(socketEcoute, clientAddr, &longeurAdr);
    if (socketService == -1)
    {
        perror("AttenteClient, erreur de accept.");
        return 0;
    }
    if(getnameinfo(clientAddr, longeurAdr, machine, NI_MAXHOST, NULL, 0, 0) == 0)
    {
        printf("Client sur la machine d'adresse %s connecte.\n", machine);
    }
    else
    {
        printf("Client anonyme connecte.\n");
    }
    free(clientAddr);
    /*
     * Reinit buffer
     */
    debutTampon = 0;
    finTampon = 0;
    finConnexion = FALSE;

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
            //		tamponClient[debutTampon], tamponClient[debutTampon], index, debutTampon);
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
            retour = recv(socketService, tamponClient, LONGUEUR_TAMPON, 0);
            //fprintf(stderr, "retour : %d\n", retour);
            if (retour < 0)
            {
                perror("Reception, erreur de recv.");
                return NULL;
            }
            else if(retour == 0)
            {
                fprintf(stderr, "Reception, le client a ferme la connexion.\n");
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

/* Envoie un message au client.
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
    if (send(socketService, message, taille,0) == -1)
    {
        perror("Emission, probleme lors du send.");
        return 0;
    }
    printf("Emission de %d caracteres.\n", taille+1);
    return 1;
}

/* Recoit des donnees envoyees par le client.
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
        retour = recv(socketService, donnees + dejaRecu, tailleMax - dejaRecu, 0);
        if(retour < 0)
        {
            perror("ReceptionBinaire, erreur de recv.");
            return -1;
        }
        else if(retour == 0)
        {
            fprintf(stderr, "ReceptionBinaire, le client a ferme la connexion.\n");
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

/* Envoie des donnees au client en precisant leur taille.
 */
int EmissionBinaire(char *donnees, size_t taille)
{
    int retour = 0;
    retour = send(socketService, donnees, taille, 0);
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



/* Ferme la connexion avec le client.
 */
void TerminaisonClient()
{
    close(socketService);
}

/* Arrete le serveur.
 */
void Terminaison()
{
    close(socketEcoute);
}


/*
********************************************************************************************************************
*/

/*on recherche l'identifiant et le mot de passe dans le fichier*/
int chercherFichierIdMdp(char idChr[6],char mdpChr[16]){

    char id[6]; //recuperation id fichier
    char mdp[16];//recuperation mdp fichier
    char c;
    int i=0; //var de boucle
    int trouve=0; //boolean de test de boucle
    int mdpOK=0;//boolean
    FILE *user=NULL;
    user=fopen("user.txt","rb");
    if(user==NULL)
    {
        perror("erreur d'ouverture de fichier, le fichier \"user.txt\" n'existe pas.\n");
        return -1; //on returne une erreur
    }

    c=fgetc(user); // on lit le premier caractère
    /*on lit jusqu'à qu'on trouve l'identifiant recherché ou la fin du fichier a été atteint*/
    while(!trouve && c!=EOF)
    {
        while(c!='$' && c!=EOF)
        {
            c=fgetc(user); // on lit jusqu'à qu'on lise $ ou eof
        }

        if(c!=EOF)
        {
            fgets(id,6,user); //on recupère l'id, les 5 premiers caractères apres le $
            if(!strcmp(id,idChr)) //on compare le id avec l'id recherché
            {
                //si egaux
                c=fgetc(user); // on lit le #
                c=fgetc(user); // on lit le caractère suivant
                /*on lit les caractères jusqu'à qu'on tombe sur le # (fin du mot de passe), ou bien EOF ou i==15*/
                while(c!='#' && c!= EOF && i<15)
                {
                    mdp[i]=c;
                    c=fgetc(user);
                    i++;
                }
                mdp[i]='\0'; //on met le \0 a la fin de la chaine
                trouve=1; // on actualise la varirable pour quitter la boucle
                if(!strcmp(mdp,mdpChr))
                {
                    mdpOK=1; // le mot de passe est correct
                    }
                else mdpOK=0; // le mot de passe est incorrect
            }
            else c=fgetc(user);
        }
    }
    return mdpOK; // 1 si mdp et identifiant correct, 0 sinon
}

/*verifie l'identifiant du client et son mot de passe*/
int verificationAuthentification(char *message,int tailleMsg)
{
    FILE *users=NULL;
    int i=0; //variable de boucle
    int j=0; //boucle
    char id[6]; // recuperation identifiant dans le message
    char mdp[16];// recuperation mot de passe dans le message
    int verifOK=0;

    users=fopen("user.txt","rb"); // on ouvre le fichier en lecture
    if(users==NULL){
        perror("le fichier user.txt n'existe pas\n");
        return -1;
    }

    while(i<tailleMsg && message[i]!=' ') i++;
    i++;
    j=0;
    while(i<tailleMsg && message[i]!='#'){
        id[j]=message[i];
        j++;
        i++;
    }
    j=0;
    while(i<tailleMsg && message[i]!='\n'){
        mdp[j]=message[i];
        j++;
        i++;
    }

    verifOK=chercherFichierIdMdp(id,mdp);
    fclose(users);

    return verifOK;
}


int verifCreationCompte(char *message,int longMsg)
{
    FILE *user=NULL;//fichier d'utilisateur
    int i=0; //indice message
    int j=0; //indice parcourt de chaine

    char nom[50];
    char prenom[50];
    char date[7];
    char NTel[10];
    char adMail[100];
    char adresse[100];
    char ville[100];
    char codePostal[5];
    char mdp[16];
    char id[6];

    /*on cherche l'espace qui delimite le debut des informations du client*/
    while(i<longMsg && message[i]!=' ') i++;
    i++; //on passe le ' '

    while(i<longMsg && message[i]!='#')
    {
        nom[j]=message[i];
        i++;
        j++;
    }

    j=0;

    while(i<longMsg && message[i]!='#')
    {
        prenom[j]=message[i];
        i++;
        j++;
    }
    j=0;
    while(i<longMsg && message[i]!='#')
    {
        date[j]=message[i];
        i++;
        j++;
    }

    j=0;
    while(i<longMsg && message[i]!='#')
    {
        NTel[j]=message[i];
        i++;
        j++;
    }

    j=0;
    while(i<longMsg && message[i]!='#')
    {
        adMail[j]=message[i];
        i++;
        j++;
    }

    j=0;
    while(i<longMsg && message[i]!='#')
    {
        adresse[j]=message[i];
        i++;
        j++;
    }

    j=0;
    while(i<longMsg && message[i]!='#')
    {
        ville[j]=message[i];
        i++;
        j++;
    }

    j=0;
    while(i<longMsg && message[i]!='#')
    {
        codePostal[j]=message[i];
        i++;
        j++;
    }

    j=0;
    while(i<longMsg && message[i]!='\n')
    {
        mdp[j]=message[i];
        i++;
        j++;
    }
    /*faire fonction creation id*/
    id[0]=nom[0];
    id[1]=nom[1];
    id[2]=prenom[0];
    id[3]='0';
    id[4]='1';
    id[5]='\0';

    user=fopen("user.txt","ab"); //ouverture fichier en ajout, si fichier inexistant il sera créé
    if(user==NULL){
        perror("erreur d'ouverture de fichier");
        return -1; //on returne une erreur
    }

    /*$identifiant utilisateur#Nom#Prenom#date de naissance#N° de téléphone#adresse
mail#N° de rue#nom rue#ville#code postal#mot de passe#nombre d’objet acheté#nombre
d’objet vendu */
    fprintf(user,"$%s#%s#%s#%s#%s#%s#%s#%s#%s#%s#0#0",id,mdp,nom,prenom,date,NTel,adMail,adresse,ville,codePostal);

    fclose(user);

    return 1;
}
