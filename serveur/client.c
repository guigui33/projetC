#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "client.h"

/*on recherche l'identifiant et le mot de passe dans le fichier*/
int chercherFichierIdMdp(char idChr[6],char mdpChr[16])
{

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

int creationId(char idChr[6])
{
    FILE *user=NULL; //fichier information client
    char id[6];//variable temp pour recupèrer id dans le fichier
    int nbrId=0; //sauvegarde la variable finale
    int tmpNbrId=0;//variable temp pour récuperer numero id dans le fichier
    char c;

    id[0]='\0';

    user=fopen("user.txt","rb"); //ouverture fichier en ajout, si fichier inexistant il sera créé
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
            if(!strcmp(id,idChr)) //on compare id avec id recherché
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
        sprintf(idChr,"%s%d%d",idChr,0,nbrId);
    }
    else
        sprintf(idChr,"%s%d",idChr,nbrId);

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

    users=fopen("user.txt","rb"); // on ouvre le fichier en lecture
    if(users==NULL)
    {
        perror("le fichier user.txt n'existe pas\n");
        return -1;
    }

    while(i<tailleMsg && message[i]!=' ') i++;
    i++;
    j=0;
    while(i<tailleMsg && message[i]!='#')
    {
        id[j]=message[i];
        j++;
        i++;
    }
    j=0;
    while(i<tailleMsg && message[i]!='\n')
    {
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

    user=fopen("user.txt","ab"); //ouverture fichier en ajout, si fichier inexistant il sera créé
    if(user==NULL)
    {
        perror("erreur d'ouverture de fichier");
        return -1; //on returne une erreur
    }

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

    /*on actualise les 3 premières lettres de l'id*/
    id[0]=nom[0];
    id[1]=nom[1];
    id[2]=prenom[0];

    if(creationId(id)==-1)
    {
        perror("erreur d'ouverture fichier user.txt\n");
        return -1; //on retourne une erreur d'ouverture
    }

    /*$identifiant utilisateur#Nom#Prenom#date de naissance#N° de téléphone#adresse
    mail#N° de rue#nom rue#ville#code postal#mot de passe#nombre d’objet acheté#nombre
    d’objet vendu */
    fprintf(user,"$%s#%s#%s#%s#%s#%s#%s#%s#%s#%s#0#0",id,mdp,nom,prenom,date,NTel,adMail,adresse,ville,codePostal);

    fclose(user);

    return 1;
}
