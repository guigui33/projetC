#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "objet.h"
#include "client.h"
#include "serveur.h"

/*creer l'id de l'objet, sur 5 caractères, 3premieres lettre du nom+ 2chiffres*/
int creerIdObjet(char *msg,int longMsg,char *idObjet,int taille)
{
    char c; //variable temp de lecture de fichier
    int i=0;
    char idTemp[6];//variable temp id
    int tempNuObjet=0;
    int nuId=0;
    FILE *objet=NULL;

    objet=fopen("fichiers/objets_en_vente.txt","rb");
    if(objet==NULL)
    {
        printf("echec d'ouverture en lecture le fichier \"fichiers/objets_en_vente.txt\".");
        return -1;
    }

    /*on initialise les 3premiers lettres de l'identifiant*/
    while(i<longMsg && i<3 && msg[i]!='#')
    {
        idObjet[i]=msg[i];
        i++;
    }

    idObjet[3]='\0';
    idTemp[0]='\0';

    c=fgetc(objet);
    while(c!=EOF)
    {
        while(c!='$' && c!=EOF)
        {
            c=fgetc(objet); // on lit jusqu'à qu'on lise $ ou eof
        }

        if(c!=EOF)
        {
            fgets(idTemp,4,objet); //on recupère l'id, les 3 premiers caractères apres le $
            if(!strcmp(idTemp,idObjet)) //on compare id avec id recherché, si les deux sont equivalents
            {
                fscanf(objet,"%d",&tempNuObjet); //on recupère le numero de l'id

                if(tempNuObjet>nuId)
                {
                    nuId=tempNuObjet; // on met à jour l'id final
                }

            }
            else c=fgetc(objet);
        }
    }
    nuId++; //on incremente le numero de l'id
    if(nuId<10)
    {
        /*si le numero de l'id est à 1 chiffre on met 1 zero devant pour respecter le format de l'id*/
        sprintf(idObjet,"%s%d%d",idObjet,0,nuId);
    }
    else
        sprintf(idObjet,"%s%d",idObjet,nuId);

    fclose(objet);

    return 1;
}

int enregistrementObjet(char *message,int longMsg)
{
    int retour=0;//varaible de retour fonction
    int cpt=0; //compte les #
    int i=0;
    FILE *objet=NULL;
    char idUti[6];
    char idObjet[6];

    retour=extraireIdClient(&message,longMsg,idUti); //extrait l'id de l'utilisateur du message, verifie que l'id utilisateur existe
    if(retour==-1)
    {
        printf("problème d'ouverture fichier user.txt\n");
        return -1;
    }
    else if (retour==0)
    {
        printf("l'identifiant est inconnu\n");
        return 0;
    }

    i=0;
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
        return 0;
    }

    objet=fopen("fichiers/objets_en_vente.txt","ab");
    if(objet==NULL)
    {
        printf("echec d'ouverture en lecture le fichier \"fichiers/objets_en_vente.txt\".");
        return -1;
    }

    retour=creerIdObjet(message,strlen(message),idObjet,6);

    if(retour==-1)
    {
        printf("erreur id objet\n");
        fclose(objet);
        return -1;
    }

    fprintf(objet,"$%s#%s#%s",idObjet,idUti,message);

    fclose(objet);

    retour=miseAuEnchere(idUti,idObjet,message,strlen(message));
    if(retour==-1)
    {
        return -1;
    }

    return 1;
}

/*$Identifiant user#identifiant objet#date de fin de vente#prixDeBase# acheteur1#prix#acheteur2#prix#....#*/
int miseAuEnchere(char *idUti,char *idObj,char *message,int tailleMsg)
{
    char tmp[18];//date de fin de l'enchère+ prix, max 17 caractère
    int i=0;//indice msg
    int j=0;//indice dateFin
    int cpt=0;//compte les #
    FILE *enchere=NULL;//fichier enchère

    enchere=fopen("fichiers/enchere.txt","ab");//ouvre en mode ajout en fin de fichier
    if(enchere==NULL)
    {
        printf("le fichier \"fichiers/enchere.txt\" ne s'ouvre pas\n");
        return -1;
    }
    /*on passe le nom et la date de debut du fichier*/
    while(i<tailleMsg && cpt!=2)
    {
        if(message[i]=='#')
        {
            cpt++;//on incremente le compteur
        }
        i++;
    }
    cpt=0;
    while(i<tailleMsg && j<17 && cpt!=2)
    {
        if(message[i]=='#') cpt++;
        if(cpt!=2)
            tmp[j]=message[i];
        i++;
        j++;
    }
    tmp[j]='\0';
    fprintf(enchere,"$%s#%s#%s",idUti,idObj,tmp);

    fclose(enchere);

    return 1;
}


int acheterObjet(char *msg,int tailleMsg)
{
    return 1;
}


int consulter(char *message,int tailleMsg)
{
    int retour=0;
    char nom[100];//nom de l'objet à rechercher à extraire du msg
    char description[100]; //description de l'objet à rechercher à extraire du msg
    char idUti[6];
    int j=0;
    int i=0;
    FILE *objet=NULL;
    char c;
    char temp[1000];

    retour=extraireIdClient(&message,tailleMsg,idUti); //extrait l'id de l'utilisateur du message, verifie que l'id utilisateur existe
    tailleMsg=strlen(message);

    if(retour==-1)
    {
        printf("problème d'ouverture fichier user.txt\n");
        return -1;
    }
    else if (retour==0)
    {
        printf("l'identifiant est inconnu\n");
        return 0;
    }

    while(i<tailleMsg && j<100 && message[i]!='#')
    {
        nom[j]=message[i];
        i++;
        j++;
    }
    i++;// on passe le #
    nom[j]='\0';

    if(nom[0]=='\0')
    {
        printf("le message n'est pas conforme.\n");
        return 0;
    }

    j=0;
    while(i<tailleMsg && j<100 && message[i]!='\n')
    {
        description[j]=message[i];
        i++;
        j++;
    }
    i++;

    description[j]='\0';

    objet=fopen("fichiers/objets_en_vente.txt","rb");
    if(objet==NULL)
    {
        printf("erreur d'ouverture du fichier objets_en_vente.txt\n");
        return -1;
    }
    c=fgetc(objet);

    retour=strcmp(nom,"tous");
    while(c!=EOF)
    {
        temp[0]='$';
        i=1;
        c=fgetc(objet);
        while(c!=EOF && i<1000 && c!='$')
        {
            temp[i]=c;
            c=fgetc(objet);
            i++;
        }
        temp[i]='\0';
        if(!retour || (strstr(temp,nom) && strstr(temp,description)))
        {
            EmissionBinaire(temp,strlen(temp));
        }
    }

    fclose(objet);
    return 1;
}


