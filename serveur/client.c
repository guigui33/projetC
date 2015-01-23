#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "client.h"

/*on recherche l'identifiant et le mot de passe dans le fichier*/
int chercherFichierIdMdp(char *idChr,char *mdpChr)
{
    char id[6]; //recuperation id fichier
    char mdp[16];//recuperation mdp fichier
    char c;
    int i=0; //var de boucle
    int trouve=0; //boolean de test de boucle
    int mdpOK=0;//valeur de retour
    FILE *user=NULL;
    user=fopen("fichiers/user.txt","rb");
    if(user==NULL)
    {
        perror("erreur d'ouverture de fichier, le fichier \"fichiers/user.txt\" n'existe pas.\n");
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

/*manque le controle d'erreur si les nom et prenom long pas respectée*/
int creationId(char *message,int taille,char *idUti)
{

    FILE *user=NULL; //fichier information client
    char id[6];//variable temporaire pour lire les identifiants du fichier
    int nbrId=0; //sauvegarde la variable finale
    int tmpNbrId=0;//variable temp pour récuperer numero id dans le fichier
    char c;
    int i=0;

    /*on initialise les 3 premiers caractères de l'identifiant*/
    while(i<taille && message[i]!='#') i++; //on passe le mot de passe
    i++;
    idUti[0]=message[i];
    i++;
    idUti[1]=message[i];
    while(i<taille && message[i]!='#') i++;
    i++;
    idUti[2]=message[i];
    idUti[3]='\0';
    id[0]='\0';

    user=fopen("fichiers/user.txt","rb"); //ouverture fichier en lecture
    if(user==NULL)
    {
        perror("erreur d'ouverture de fichier");
        return -1; //on returne une erreur
    }

    c=fgetc(user); // on lit le premier caractère
    /*on lit jusqu'à qu'on trouve l'identifiant recherché ou la fin du fichier a été atteint*/
    while(c!=EOF)
    {
        while(c!='$' && c!=EOF)
        {
            c=fgetc(user); // on lit jusqu'à qu'on lise $ ou eof
        }

        if(c!=EOF)
        {
            fgets(id,4,user); //on recupère l'id, les 3 premiers caractères apres le $
            if(!strcmp(id,idUti)) //on compare id avec id recherché
            {
                fscanf(user,"%d",&tmpNbrId); //on recupère le numero de l'id

                if(tmpNbrId>nbrId)
                {
                    nbrId=tmpNbrId; // on met à jour l'id final
                }

            }
            else c=fgetc(user);
        }
    }
    nbrId++; //on incremente le numero de l'id
    if(nbrId<10)
    {
        /*si le numero de l'id est à 1 chiffre on met 1 zero devant pour respecter le format de l'id*/
        sprintf(idUti,"%s%d%d",idUti,0,nbrId);
    }
    else
        sprintf(idUti,"%s%d",idUti,nbrId);

    fclose(user);

    return 1;
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

    users=fopen("fichiers/user.txt","rb"); // on ouvre le fichier en lecture
    if(users==NULL)
    {
        perror("le fichier fichiers/user.txt n'existe pas\n");
        return -1;
    }

    j=0;
    while(i<tailleMsg && j<6 && message[i]!='#')
    {
        id[j]=message[i];
        j++;
        i++;
    }
    i++;
    id[5]='\0';

    j=0;
    while(i<tailleMsg && j<15 && message[i]!='\n')
    {
        mdp[j]=message[i];
        j++;
        i++;
    }
    mdp[i]='\0';

    verifOK=chercherFichierIdMdp(id,mdp);

    fclose(users);

    return verifOK;
}

/*fonction a refaire, a faire controle d'erreur !!!!!!*/
int creationCompte(char *message,int longMsg)
{
    FILE *user=NULL;//fichier d'utilisateur
    char id[6];
    int retour=0; //variable de retour erreur fonction
    int cpt=0; //compteur de #
    int i=0; //indice message

    user=fopen("fichiers/user.txt","ab"); //ouverture fichier en ajout, si fichier inexistant il sera créé
    if(user==NULL)
    {
        perror("erreur d'ouverture de fichier");
        return -1; //on returne une erreur
    }
    /*verification du nombre de separateur*/
    while(i<longMsg)
    {
        if(message[i]=='#')
        {
            cpt++;
        }
        i++;
    }

    if(cpt!=8)
    {
        printf("le format du message non respecté\n");
        fclose(user);
        return 0;
    }

    retour=creationId(message,longMsg,id);

    if(retour==-1)
    {
        perror("erreur d'ouverture fichier fichiers/user.txt\n");
        fclose(user);
        return -1; //on retourne une erreur d'ouverture
    }
    else if(retour==0)
    {
        printf("problème lors de la creation de l'id utilisateur\n");
        fclose(user);
        return 0;
    }

    fprintf(user,"$%s#%s#0#0",id,message);

    fclose(user);

    return 1;
}

/*on extrait l'id de la chaine, et on verifie que l'id existe*/
//attention peu etre bug
int extraireIdClient(char **message,int tailleMsg,char *id)
{
    FILE *user=NULL;
    int i=0;
    char idTmp[6]; //variable temporaire
    char c;
    int termine=0;

    while(i<tailleMsg && i<5 && (*message)[i]!='#')
    {
        id[i]=(*message)[i];
        i++;
    }
    i++;// on passe le '#'
    id[5]='\0';
    *message=&(*message)[i];

    user=fopen("fichiers/user.txt","rb");
    if(user==NULL)
    {
        printf("erreur d'ouverture de fichier \"fichiers/user.txt\" en mode lecture\n");
        return -1;
    }

    c=fgetc(user);
    while(c!=EOF && !termine)
    {

        while(c!=EOF && c!='$')
        {
            c=fgetc(user);
        }

        fgets(idTmp,6,user);
        if(!strncmp(idTmp,id,6))
        {
            termine=1;
        }
        else c=fgetc(user);
    }

    fclose(user);

    return termine;
}
