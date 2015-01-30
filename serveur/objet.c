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

int extraireIdObjet(char **msg,int tailleMsg,char *idObjet,char *idUti,FILE *enchere)
{
    int i=0;
    char c;
    char idObjetTemp[6]; //identifiant objet pour recuperer dans le fichier enchere
    char idUtiTemp[6];
    int trouve=0; //variable boolean

    while(i<tailleMsg && (*msg)[i]!='#')
    {
        idObjet[i]=(*msg)[i];
        i++;
    }
    idObjet[i]='\0';
    i++;
    *msg=*msg+i;

    c=fgetc(enchere);
    while(c!=EOF && !trouve)
    {

        while(c!=EOF && c!='$')
        {
            c=fgetc(enchere);
        }
        if(c!=EOF)
        {
            fgets(idUtiTemp,6,enchere);//on recupere l'id utilisateur
            fgetc(enchere);//on passe le #
            fgets(idObjetTemp,6,enchere);//on recupere l'id objet
            if(!strcmp(idObjet,idObjetTemp))
            {
                trouve=1;
                if(!strcmp(idUti,idUtiTemp))
                {
                    /*si les deux id sont les meme, l'utilisateur achete son propre objet*/
                    printf("L'utilisateur achete son propre objet\n");
                    return 0;//on retourne une erreur, message incorrect
                }
            }
            else c=fgetc(enchere);
        }
    }

    return trouve;
}

/*recuperer date systeme, on compare avec la date de fin de l'enchere */
int testDateEnchere(FILE *enchere)
{
    char date[9];
    fgetc(enchere);//on passe le #
    fgets(date,9,enchere);
    printf("%s\n",date);
    return 1;
}

int testPrix(float prix,FILE *enchere)
{
    float prixTmp=0;
    char c;

    c=fgetc(enchere);
    while(c!=EOF)
    {
        fscanf(enchere,"%f",&prixTmp);
        printf("prix du msg= %f && prix du fichier= %f\n",prix,prixTmp);
        if(prix>=100000){
            printf("le prix est superieur à la limite fixé.\n");
            return 0;
        }
        else if(prix>prixTmp)
        {
            return 1;
        }
        else
        {
            printf("le prix est inferieur au prix actuel\n");
            return 0; //le prix n'est pas superieur au prix actuel
        }
    }
    return -1;//on retourne une erreur
}

/*refaire date*/
int enregistrementPrix(char *idUti,char *idObjet,float prix,FILE *enchere)
{
    FILE *enchereTmp=NULL;
    char idUtiTmp[6];
    char idObjtTmp[6];
    char c;

    enchereTmp=fopen("fichiers/enchereTmp.txt","wb");
    if(enchereTmp==NULL)
    {
        return -1; //probleme d'ouverture fichier
    }

    c=fgetc(enchere);
    while(c!=EOF)
    {
        while(c!=EOF && c!='$')
        {
            fprintf(enchereTmp,"%c",c);
            c=fgetc(enchere);
        }
        if(c!=EOF)
        {
            fgets(idUtiTmp,6,enchere);
            c=fgetc(enchere);
            fgets(idObjtTmp,6,enchere);
            if(!strcmp(idObjtTmp,idObjet))
            {
                fprintf(enchereTmp,"$%s#%s#",idUtiTmp,idObjtTmp);//on ecrit l'id utilisateur et le id objet
                c=fgetc(enchere);
                c=fgetc(enchere);
                while(c!=EOF && c!='#')
                {
                    fprintf(enchereTmp,"%c",c);
                    c=fgetc(enchere);
                }
                fprintf(enchereTmp,"#%.2f#%s",prix,idUti);
            }
            else {
                fprintf(enchereTmp,"$%s#%s",idUtiTmp,idObjtTmp);//on ecrit l'id utilisateur et le id objet
                c=fgetc(enchere);
            }
        }
    }

    fclose(enchereTmp);
    return 1;
}

int acheterObjet(char *msg,int tailleMsg)
{
    int retour=0;
    char idUti[6];
    char idObjet[6];
    float prix=0;
    FILE *enchere=NULL;

    retour=extraireIdClient(&msg,tailleMsg,idUti); //extrait l'id de l'utilisateur du message, verifie que l'id utilisateur existe

    if(retour==-1)
    {
        printf("problème d'ouverture fichier user.txt\n");
        return -1;
    }
    else if (retour==0)
    {
        printf("l'identifiant utilisateur est inconnu\n");
        return 0;
    }
    enchere=fopen("fichiers/enchere.txt","rb");
    if(enchere==NULL)
    {
        printf("problème d'ouverture fichier enchere.txt\n");
        return -1;
    }

    retour=extraireIdObjet(&msg,strlen(msg),idObjet,idUti,enchere);

    if (retour==0)
    {
        printf("probleme dans le message\n");
        fclose(enchere);
        return 0;
    }
/*test date ????*/
    testDateEnchere(enchere);

    sscanf(msg,"%f",&prix);
    retour=testPrix(prix,enchere);
    if(retour==0)
    {
        fclose(enchere);
        return 0;
    }
    else if(retour==-1)
    {
        fclose(enchere);
        return -1;
    }

    rewind(enchere);//on rembobine le fichier
    retour=enregistrementPrix(idUti,idObjet,prix,enchere);
    if(retour==-1)
    {
        fclose(enchere);
        return -1;
    }

    fclose(enchere);
    rename("fichiers/enchereTmp.txt","fichiers/enchere.txt");
    return 1;
}
