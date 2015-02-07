#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "secondaire.h"
#include "client.h"

#define CheminFichierTEMP "/home/guigui/Documents/L3/Projet/client/fichiers/tmp.txt"

int controleChaine(char *chaine,char *indice)
{
    if(strchr(chaine,'#'))
    {
        printf("vous avez entré un caractère interdit '#'\n");
        return 0;
    }

    if(!strcmp(indice,"motDePasse"))
    {

        if(strlen(chaine)>5)
        {
            if(strchr(chaine,' '))
            {
                printf("vous avez entré un des caractères interdits ' '.\n");
                return 0;
            }
            return 1;
        }
        else
        {
            printf("le mot de passe doit etre superieur ou égale à 6 caractères.\n");
            return 0;
        }
    }
    if(!strcmp(indice,"nom"))
    {
        if(strlen(chaine)<3)
        {
            printf("la chaine entrée par l'utilisateur doit etre superieur à 3 caractères.\n");
            return 0;
        }
        else
        {
                printf("Le nom rentré par l'utilisateur n'est pas conforme.\n");
        }
    }

    if(!strcmp(indice,"recherche"))
    {
        return 1;
    }

    if(strlen(chaine)==0)
    {
        strcpy(chaine,"[non renseigné]");
    }

    return 1;
}

void dateFinEnchere(char *date)
{
    struct tm finEnchere;//pointeur sur une structure de type tm, pour contenir la date de fin de l' enchere
    time_t secondes; //recupere la date systeme en secondes.
    struct tm instant;//struct pour la date actuelle
    int jours=0;

    time(&secondes);//date systeme
    instant=*localtime(&secondes);//initialise la struture instant qui est la date systeme actuelle
    finEnchere=*localtime(&secondes);

    printf("Entrez la date de fin d'enchère.\n");
    printf("date actuelle: %d:%d le %d/%d/%d.\n",instant.tm_hour,instant.tm_min,instant.tm_mday, instant.tm_mon+1,instant.tm_year+1900);
    do
    {
        printf("Entrez la durée (en jours) de l'enchère (>1jour).\n");
        printf("Jours: ");
        scanf("%d",&jours);
        viderBuffer();
    }
    while(jours<2);

    finEnchere.tm_mday+=jours;
    mktime(&finEnchere);//complète la structure finEnchere

    printf("Votre enchère prendra fin le %d/%d/%d à %d:%d\n", finEnchere.tm_mday, finEnchere.tm_mon+1,finEnchere.tm_year+1900,finEnchere.tm_hour, finEnchere.tm_min);

    strftime(date,13,"%H%M%d%m%Y",&finEnchere);

    return;
}


int dateNaissance(char *date)
{
    struct tm naissance;//struct pour la date actuel
    if(scanf("%2d/%2d/%4d",&(naissance.tm_mday),&(naissance.tm_mon),&(naissance.tm_year))!=3){
        viderBuffer();//on vide le buffer
        return 0;
    }
    viderBuffer();//on vide le buffer
    if(naissance.tm_mday<1 || naissance.tm_mday>31 || naissance.tm_mon<1 || naissance.tm_mon>12 || naissance.tm_year>2016 || naissance.tm_year<1920){
        return 0;
    }
    naissance.tm_mon  -=1 ;   // commence à janvier=0 pour les mois
    naissance.tm_year -= 1900;  //annee attention(annee-1900)
    naissance.tm_hour=6;
    naissance.tm_min=0;
    naissance.tm_sec  = 0;   /* 0 seconde */
    naissance.tm_isdst=-1;
    mktime(&naissance);
    strftime(date,13, "%d%m%Y",&naissance);
    return 1; //la date est ok
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

int enregistrementCatalogueTemp()
{
    FILE *catalogue=NULL;
    char *msgServeur=NULL;
    int noErreur=1;

    catalogue=fopen(CheminFichierTEMP,"wb");
    if(catalogue==NULL)
    {
        return -1;
    }
    msgServeur=Reception();
    /*tant que le msgServeur est different de "EOF" et aucune erreur on ecrit dans le fichier*/
    while(msgServeur!=NULL && noErreur==1 && strncmp(msgServeur,"EOF",3))
    {
        if(!strncmp(msgServeur,"erreurConsulter",15))
        {
            noErreur=0;
        }
        else if (!strncmp(msgServeur,"erreurServeur",13))
        {
            noErreur=-1;
        }
        else
        {
            supprCara(msgServeur);//on suppr \n
            fprintf(catalogue,"%s",msgServeur);
            free(msgServeur);
            msgServeur=NULL;
            msgServeur=Reception();
        }
    }

    if(msgServeur!=NULL)
        free(msgServeur);
    fclose(catalogue);
    return noErreur;
}

void afficherObjet(FILE *file,char *idObjet,int choix,int *finFichier)
{
    char c;
    int fin=0;

    c=fgetc(file);

    if(choix!=2)
    {
        int compteur=0;
        fseek(file,-2,SEEK_CUR);//on recule de 2octets
        c=fgetc(file);
        while(c!=EOF && compteur!=(choix+1) && ftell(file)!=0)
        {
            fseek(file,-2,SEEK_CUR);
            c=fgetc(file);
            if(c=='$') compteur++;
        }
    }

    if(c==EOF)//si fichier vide
    {
        printf("\nAucun objet ne correspond à votre recherche.\n\n");
        *finFichier=-1;//signal fichier vide
        return ;
    }

    while(c!=EOF && !fin)
    {
        char dateDeb[20];
        char dateFin[20];
        float prixDepart=0;
        struct tm tempsDepart;
        struct tm tempsFin;
        fgets(idObjet,6,file);//on initialise l'identifiant de l'objet si l'utilisteur veut achter le produit
        fgetc(file);
        afficherInformation("\t",'#',file,&c);
        fgets(dateDeb,13,file);//date de debut de l'enchere
        formatDate(dateDeb,&tempsDepart);
        fgetc(file);//on passe le #
        fgets(dateFin,13,file);//date de fin de l'enchere
        formatDate(dateFin,&tempsFin);
        fgetc(file);//on passe le #
        fscanf(file,"%f",&prixDepart);//prix de depart de l'enchere
        fgetc(file);//on passe le #
        afficherInformation("\nCatégorie: ",'#',file,&c);
        afficherInformation("\nDecription: ",'#',file,&c);
        afficherInformation("\nURL: ",'#',file,&c);
        afficherInformation("\nQuantité: ",'#',file,&c);
        afficherInformation("\nVille: ",'#',file,&c);
        afficherInformation("\nVendeur: ",'#',file,&c);//nom
        afficherInformation(" ",'#',file,&c);//prenom
        afficherInformation("\nNombre d'article acheté: ",'#',file,&c);
        afficherInformation("\nNombre d'article vendu: ",'#',file,&c);
        strftime(dateDeb,20, "%d/%m/%Y à %H:%M",&tempsDepart);//ecrit dans une chaine au format desiré
        printf("\n-Prix de départ: %5.2f€ le %s",prixDepart,dateDeb);
        afficherInformation("\n-Prix Enchere: ",'#',file,&c);
        afficherInformation("\t-Votre Enchere: ",'#',file,&c);
        afficherInformation("€\n-Nombre d'enchère: ",'$',file,&c);
        strftime(dateFin,20, "%d/%m/%Y à %H:%M",&tempsFin);//ecrit dans une chaine au format desiré
        printf("\n-Fin des encheres: %s.",dateFin);//format date plus afficher compteur de date
        fin=1;
        printf("\n\n1)Precedent 2)Acheter ");
        if(c!=EOF)
        {
            printf("3)Suivant ");
            *finFichier=0;
        }
        else
            *finFichier=1;
        fseek(file,-1,SEEK_CUR);// recule le curseur de 1 octet
    }
    return;
}

void formatDate(char *date,struct tm *time)
{
    sscanf(date,"%2d%2d%2d%2d%4d",&(time->tm_hour),&(time->tm_min),&(time->tm_mday),&(time->tm_mon),&(time->tm_year));
    time->tm_mon -=1 ;   // commence à janvier=0 pour les mois
    time->tm_year -= 1900;  //annee attention(annee-1900)
    time->tm_sec  = 0;   /* 0 seconde */
    time->tm_isdst=-1;
    mktime(time);
}

void afficherInformation(char *chaine,char separateur,FILE *file,char *c)
{
    printf("%s",chaine);
    *c=fgetc(file);
    while(*c!=separateur && *c!=EOF)
    {
        if(*c!='\n')
            printf("%c",*c);
        *c=fgetc(file);
    }
    return;
}

void afficherInformation2(char *chaine,char *information,int taille,char separateur,int *i)
{
    printf("%s",chaine);
    while(*i<taille && information[*i]!=separateur)
    {
        printf("%c",information[*i]);
        *i=*i+1;
    }
    *i=*i+1;
    return;
}

/*$idUti#nom#prenom#556700304#adressemail#nruee#ville#33160#0nbreObjeHt#nbrObjetVendu#nbreEncherencour#nbrObjetEnvent\n*/
void afficherUtilisateur(char *chaine,int taille)
{
    char *msgServeur=NULL;
    int i=1;
    msgServeur=Reception();

    if(strncmp(msgServeur,"infoCptEnvoyee",14))
    {
        free(msgServeur);
        printf("erreur information compte.\n");
        return;
    }

    printf("***Information Compte***\n");
    afficherInformation2("Identifiant: ",chaine,taille,'#',&i);
    afficherInformation2("\nNom: ",chaine,taille,'#',&i);
    afficherInformation2("\tPrenom: ",chaine,taille,'#',&i);
    afficherInformation2("\nDate de naissance: ",chaine,taille,'#',&i);
    afficherInformation2("\nTéléphone: ",chaine,taille,'#',&i);
    afficherInformation2("\nMail: ",chaine,taille,'#',&i);
    afficherInformation2("\nAdresse: ",chaine,taille,'#',&i);
    afficherInformation2("\nVille: ",chaine,taille,'#',&i);
    afficherInformation2("\t",chaine,taille,'#',&i);
    afficherInformation2("\nAchat: ",chaine,taille,'#',&i);
    afficherInformation2("\tVente: ",chaine,taille,'#',&i);
    afficherInformation2("\nEnchere en cours: ",chaine,taille,'#',&i);
    afficherInformation2("\tVente en cours: ",chaine,taille,'\n',&i);
    printf("\n\n");

    free(msgServeur);
    return;
}
