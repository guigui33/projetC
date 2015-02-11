#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "objet.h"
#include "client.h"
#include "serveur.h"
#include <time.h>

#define FichierObjetEnVente "/home/guigui/Documents/L3/Projet/serveur/fichiers/objets_en_vente.txt"
#define FichierObjetEnVenteTmp "/home/guigui/Documents/L3/Projet/serveur/fichiers/objets_en_venteTmp.txt"
#define FichierEnchere "/home/guigui/Documents/L3/Projet/serveur/fichiers/enchere.txt"
#define FichierEnchereTmp "/home/guigui/Documents/L3/Projet/serveur/fichiers/enchereTmp.txt"
#define FichierFinEnchere "/home/guigui/Documents/L3/Projet/serveur/fichiers/finEnchere.txt"


/*creer l'id de l'objet, sur 5 caractères, 3premieres lettre du nom+ 2chiffres*/
int creerIdObjet(char **msg,int longMsg,char *idObjet,int tailleId,char *nomObjet,int tailleNom)
{
    char c; //variable temp de lecture de fichier
    int i=0;
    int j=0;
    char idTemp[6];//variable temp id
    int tempNuObjet=0;//variable tempo pour conserver le num de l'objet
    int nuId=0;
    FILE *objet=NULL;

    objet=fopen(FichierObjetEnVente,"rb");
    if(objet==NULL)
    {
        printf("echec d'ouverture en lecture le fichier \"fichiers/objets_en_vente.txt\".");
        return -1;
    }

    /*init idobjet + init nomObjet*/
    while(i<tailleNom-1 && i<longMsg && (*msg)[i]!='#')
    {
        /*on initialise les 3premiers lettres de l'identifiant, on s'assure que l'id soit construit avec des lettres*/
        if(j<3 && (((*msg)[i]>64 && (*msg)[i]<91) || ((*msg)[i]>96 && (*msg)[i]<123)))
        {
            idObjet[j]=(*msg)[i];
            j++;
        }
        nomObjet[i]=(*msg)[i];
        i++;
    }
    nomObjet[i]='\0';
    idObjet[j]='\0';
    idTemp[0]='\0';
    *msg=&(*msg)[i+1];
    if(strlen(idObjet)!=3) //si l'identifiant est different de 3 alors probleme avec le nom
    {
        printf("identifiant non valable doit contenir 3 caractères minimum.\n");
        return 0;//probleme msg
    }

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
        sprintf(idObjet,"%s0%d",idObjet,nuId);
    }
    else
        sprintf(idObjet,"%s%d",idObjet,nuId);

    fclose(objet);

    return 1;
}

int enregistrementObjet(char *message,int longMsg)
{
    int retour=0;//varaible de retour fonction
    FILE *objet=NULL;
    char idUti[6];
    char idObjet[6];
    char nomObjet[26];//recuperation du nom objet dans le msg
    char dateDebut[13];

    retour=extraireIdClient(&message,longMsg,idUti); //extrait l'id de l'utilisateur du message, verifie que l'id utilisateur existe
    if(retour==-1)
    {
        printf("problème d'ouverture fichier user.txt\n");
        return -1;
    }
    else if (retour==0)
    {
        printf("l'identifiant utilisateur est inconnu.\n");
        return 0;
    }

    objet=fopen(FichierObjetEnVente,"ab");
    if(objet==NULL)
    {
        printf("echec d'ouverture en lecture le fichier \"fichiers/objets_en_vente.txt\".");
        return -1;
    }

    retour=creerIdObjet(&message,strlen(message),idObjet,6,nomObjet,26);

    if(retour==-1)
    {
        printf("erreur id objet\n");
        fclose(objet);
        return -1;
    }
    else if(retour==0)
    {
        printf("problème lors de la creation de l'identifiant Objet.\n");
        fclose(objet);
        return 0;
    }

    retour=testDateMiseEnVente(message,strlen(message),dateDebut,13);
    if(retour==0)
    {
        printf("la date de fin d'enchere de l'utilisateur n'est pas conforme.\n");
        return 0;
    }

    supprCaractere(message); //on suppr le \n avant d'ecrire dans le fichier
    printf("$%s#%s#%s#%s#%s\n",idObjet,idUti,nomObjet,dateDebut,message);
    fprintf(objet,"$%s#%s#%s#%s#%s",idObjet,idUti,nomObjet,dateDebut,message);

    fclose(objet);

    retour=miseAuEnchere(idUti,idObjet,message,strlen(message));

    return 1;
}

int miseAuEnchere(char *idUti,char *idObj,char *message,int tailleMsg)
{
    int i=0;
    int cpt=0;//compte les #
    FILE *enchere=NULL;//fichier enchère

    enchere=fopen(FichierEnchere,"ab");//ouvre en mode ajout en fin de fichier
    if(enchere==NULL)
    {
        printf("le fichier \"fichiers/enchere.txt\" ne s'ouvre pas\n");
        return -1;
    }

    fprintf(enchere,"$%s#%s#",idUti,idObj);
    cpt=0;
    while(i<tailleMsg && cpt!=2)
    {
        if(message[i]=='#') cpt++;
        if(cpt!=2)
            fprintf(enchere,"%c",message[i]);
        i++;
    }

    fclose(enchere);
    return 1;
}

/*ecrit les informations du fichier dans chaine jusqu'à lire le separateur*/
void recupererInfoFichier(char *chaine,int taille,char separateur,char *c,FILE *file)
{
    int i=0;
    *c=fgetc(file);
    while(i<taille-1 && *c!=separateur && *c!=EOF)
    {
        if(*c!='\n')
        {
            chaine[i]=*c;
            i++;
        }
        *c=fgetc(file);
    }
    chaine[i]='\0';
}

/*extrait dans rchr, les données du messgae jusqu'a lire le separateur*/
void extraireDonneeMsg(char *message,int tailleMsg,char* rchr,char tailleRchr,int *i,char separateur)
{
    int j=0;

    while(*i<tailleMsg && j<tailleRchr-1 && message[*i]!=separateur)
    {
        rchr[j]=message[*i];
        (*i)++;
        j++;
    }
    rchr[j]='\0';
    (*i)++;
}

int consulter(char *message,int tailleMsg)
{
    int retour=0;//bool pour savoir si le client desire voir tous les objets mises en vente
    char nomRch[26];//nom de l'objet à rechercher à extraire du msg
    char categorieRch[26];//???taille
    char descriptionRch[101]; //description de l'objet à rechercher à extraire du msg
    char idUti[6];//identifiant du client
    int i=0;

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
    extraireDonneeMsg(message,tailleMsg,nomRch,26,&i,'#');
    extraireDonneeMsg(message,tailleMsg,categorieRch,26,&i,'#');
    extraireDonneeMsg(message,tailleMsg,descriptionRch,101,&i,'\n');

    retour=rechercherObjet(idUti,nomRch,categorieRch,descriptionRch);//on va recuperer les donnees à envoyer au client dans les differents fichiers

    return retour;//retourne 1 si ok, -1 sinon
}

int donneeObjet(char *idPro,char *idObjet,char *idUtilisateur,int typeUtilisateur,char *nomRch,char *categorieRch,char *descriptionRch,int f)
{
    FILE *objet=NULL;
    char c;
    char msgClient[500];//message à envoyer au client contient les infos objet+proprio+enchere
    char tmp[40];//variable utilisée pour sauvegarder les données dates et prix
    char idObjetTmp[6];//id de l'objet
    char nomObjet[26];//nom de l'objet
    char description[100];//description de l'objet
    char categorie[26];//categorie de l'objet sur 25 caracteres
    int trouve=0;
    objet=fopen(FichierObjetEnVente,"rb");
    if(objet==NULL)
    {
        return 0;
    }

    c=fgetc(objet);
    while(c!=EOF && !trouve)
    {
        int i=0;
        /*recuperation id de l'objet*/
        fgets(idObjetTmp,6,objet);
        if(!strcmp(idObjet,idObjetTmp))
        {
            int tous=0;//bool pour savoir si il faut envoyer tous les objets ou non
            int envoyer=0;
            trouve=1;
            fseek(objet,7,SEEK_CUR);//on passe #+l'id du propietaire et le #
            recupererInfoFichier(nomObjet,26,'#',&c,objet);
            fgets(tmp,27,objet); //dateDeb#dateFin#
            c=fgetc(objet);
            i=26;
            /*on recupere le prix de depart de l'objet*/
            while(i<40 && c!=EOF && c!='#')
            {
                tmp[i]=c;
                i++;
                c=fgetc(objet);
            }
            tmp[i]='\0';
            recupererInfoFichier(categorie,26,'#',&c,objet);
            recupererInfoFichier(description,101,'#',&c,objet);
            if(typeUtilisateur!=1 && typeUtilisateur!=2)
            {
                tous=strcmp(nomRch,"CatalogueComplet");//mot clé pour envoyer tous les objets mises en vente
                envoyer=(strstr(nomObjet,nomRch) && strstr(description,descriptionRch) && (strstr(categorie,categorieRch)));
            }
            if(!tous || typeUtilisateur==1 || typeUtilisateur==2 || envoyer)
            {
                sprintf(msgClient,"$%s#%s#%s#%s#%s#",idObjet,nomObjet,tmp,categorie,description);
                i=strlen(msgClient);//on se place à la fin de la chaine
                recupererInfoFichier(msgClient+i,500-i,'$',&c,objet);
                /*on recupere les informations sur le propietaire de l'objet*/
                if(f==0)//si f!=0 on n'envoie pas les données utilisateurs et enchere catalogue
                {
                    if(donneeUtilisateur(idPro,msgClient,"donneeObjetCatalogue")==-1)
                    {
                        fclose(objet);
                        return -1;
                    }
                    /*on recupere les informations sur l'enchere de l'objet*/
                    donneeEnchereCatalogue(idUtilisateur,idObjet,msgClient);
                    /* on envoie les informations au client*/
                }
                if(f==1)
                {
                    strcat(msgClient,"\n");
                }
                printf("informations: %s de taille %ld.\n",msgClient,strlen(msgClient));
                if(EmissionBinaire(msgClient,strlen(msgClient))<=0)
                {
                    printf("Problème emission.\n");
                    fclose(objet);
                    return -1;
                }
            }
        }
        else
        {
            c=fgetc(objet);
            while(c!=EOF && c!='$')
            {
                c=fgetc(objet);
            }
        }
    }

    return 1;
}

int rechercherObjet(char *idUtili,char *nomRch,char *categorieRch,char *descriptionRch)
{
    int typeUtilisateur=0;
    int retour=1;
    char c;
    FILE *fichier=NULL;

    if(!strcmp(nomRch,"MesEncheres"))
    {
        typeUtilisateur=2;
    }
    else if(!strcmp(nomRch,"MesVentes"))
    {
        typeUtilisateur=1;
    }

    fichier=fopen(FichierEnchere,"rb");

    if(fichier==NULL)
    {
        printf("le fichier des encheres ne s'ouvre pas.\n");
        return -1;
    }

    c=fgetc(fichier);
    while(c!=EOF && retour==1)
    {
        char idUtiPro[6];//identifiant proprietaire
        char idUtiEnchere[6];//identifiant utilisateur enchere
        char idObjet[6];//identifiant objet
        int trouve=0;//bool de sortie de boucle
        fgets(idUtiPro,6,fichier);
        c=fgetc(fichier);
        fgets(idObjet,6,fichier);//identifiant de l'objet
        if(typeUtilisateur==1 && !strcmp(idUtiPro,idUtili))//on cherche les objets du vendeur
        {
            retour=donneeObjet(idUtiPro,idObjet,idUtili,typeUtilisateur,nomRch,categorieRch,descriptionRch,0);
        }
        else if (typeUtilisateur==2)//on cherche les objets où il a proposé une enchere
        {
            fseek(fichier,13,SEEK_CUR);//on # +date
            c=fgetc(fichier);//#
            while(c!=EOF && c!='$' && !trouve)
            {
                c=fgetc(fichier);
                while(c!=EOF && c!='#' && c!='$')
                {
                    c=fgetc(fichier);
                }
                if(c!='$' && c!=EOF)
                {
                    fgets(idUtiEnchere,6,fichier);//l'identifiant
                    if(!strcmp(idUtiEnchere,idUtili))
                    {
                        trouve=1;//on trouve l'identifiant de l'utilisateur
                        retour=donneeObjet(idUtiPro,idObjet,idUtili,typeUtilisateur,nomRch,categorieRch,descriptionRch,0);
                    }
                    c=fgetc(fichier);
                }
            }
        }
        else if(typeUtilisateur==0)
        {
            retour=donneeObjet(idUtiPro,idObjet,idUtili,typeUtilisateur,nomRch,categorieRch,descriptionRch,0);
        }
        while(c!=EOF && c!='$' && retour==1)
        {
            c=fgetc(fichier);
        }
    }

    fclose(fichier);
    return retour;
}

int donneeEnchereCatalogue(char* idUtili,char *idObjet,char *msgClient)
{
    char idObjetTmp[6];
    char idUtilisateurTmp[6];//comparer l'identfiant de l'enchere et idUtili
    char c;
    int nbrEnchere=0;//contient le nombre d'enchere
    float prix=0;//le premier prix enregistré
    float prixUtili=0;//prix de l'enchere enrgstré par l'utilisateur, = 0 si l'utili n'a jamais fait d'enchere
    FILE *enchere=NULL;
    int trouve=0;//bool pour savoir si le produit est trouvé ou non

    enchere=fopen(FichierEnchere,"rb");
    if(enchere==NULL)
    {
        printf("impossible d'ouvrir le fichier enchere.txt.\n");
        return -1;
    }

    c=fgetc(enchere);
    while(c!=EOF && !trouve)
    {
        float prixTmp=0;//recupere prix fichier
        fseek(enchere,6,SEEK_CUR);//on passe l'identifiant utilisateur et #
        fgets(idObjetTmp,6,enchere);//on recupere idObjet du fichier
        c=fgetc(enchere);
        if(!strcmp(idObjet,idObjetTmp))
        {
            trouve=1;//on a trouvé le produit
            fseek(enchere,12,SEEK_CUR);//date fin
            c=fgetc(enchere);
            while(c!=EOF && c!='$')
            {
                fscanf(enchere,"%f",&prixTmp);//on recupère le prix enregistré
                if(prix==0)
                {
                    prix=prixTmp;//on recupere le premier prix lu, peut etre le prix de depart ou la dernière enchere
                }
                c=fgetc(enchere);
                if(c!=EOF && c!='$' && c!='\n')//si valeur==$ ou eof c'est le prix de depart, plus d'enchere
                {
                    nbrEnchere++;//on augemente le nbr d'enchere
                    fgets(idUtilisateurTmp,6,enchere);//on recup l'id
                    if(prixUtili==0 && !strcmp(idUtili,idUtilisateurTmp))//si le prix!=0 le prixUili a deja été initialisé
                    {
                        prixUtili=prixTmp;
                    }
                    c=fgetc(enchere);
                }
            }
        }//fin id objet trouvé
        else
        {
            while(c!=EOF && c!='$')
            {
                c=fgetc(enchere);
            }
        }
    }

    if(trouve)
        sprintf(msgClient,"%s#%5.2f#%5.2f#%d\n",msgClient,prix,prixUtili,nbrEnchere);
    else
        trouve=-1;//erreur identifiant non trouvé

    fclose(enchere);
    return trouve;
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
/*les 15 minustes si delai<15mn*60s*/
int testDateEnchere(FILE *enchere,char *dateFin)
{
    struct tm finEnchere;//pointeur sur une structure de type tm, pour contenir la dateFin de fin de l' enchere
    time_t secondes; //recupere la date systeme en secondes.
    struct tm instant;//struct pour la date actuel
    double sdes=0;//secondes
    fgetc(enchere);//on passe le #
    fgets(dateFin,13,enchere); //on recupere la date de fin de l'objet sur 12 caracteres

    time(&secondes);//date systeme
    /*decomposition de la date de fin enchere dans la strcuture*/
    sscanf(dateFin,"%2d%2d%2d%2d%4d",&(finEnchere.tm_hour),&(finEnchere.tm_min),&(finEnchere.tm_mday),&(finEnchere.tm_mon),&(finEnchere.tm_year));
    finEnchere.tm_mon  -=1 ;   // commence à janvier=0 pour les mois
    finEnchere.tm_year -= 1900;  //annee attention(annee-1900)
    finEnchere.tm_sec  = 0;   /* 0 seconde */
    finEnchere.tm_isdst=-1;//empeche heure d'été et hiver
    mktime(&finEnchere);

    instant=*localtime(&secondes);//initialise la struture instant qui est la date systeme actuelle
    sdes=difftime(mktime(&finEnchere),mktime(&instant));
    if(sdes<(15*60) && sdes>0)
    {
        finEnchere.tm_min+=15;//on ajoute 15mn
        finEnchere.tm_isdst=-1;//empeche heure d'été et hiver
        mktime(&finEnchere);
        strftime(dateFin,13, "%H%M%d%m%Y",&finEnchere);//ecrit dans une chaine au format desiré
        return 2;//signal il faut modifié le fichier objet
    }
    if(sdes>0)//le temps est > à 15mn
    {
        return 1; //la date est ok
    }

    return 0; //date non ok
}

int testPrix(float prix,FILE *enchere)
{
    float prixTmp=0;
    char c;

    c=fgetc(enchere);
    while(c!=EOF)
    {
        fscanf(enchere,"%f",&prixTmp);
        if(prix>=100000)
        {
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

int enregistrementPrix(char *idUti,char *idObjet,char *dateFin,float prix,FILE *enchere)
{
    FILE *enchereTmp=NULL;
    char idUtiTmp[6];
    char idObjtTmp[6];
    char c;

    enchereTmp=fopen(FichierEnchereTmp,"wb");//on ouvre un fichier tmp
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
                fprintf(enchereTmp,"%s#%.2f#%s",dateFin,prix,idUti);
                fseek(enchere,13,SEEK_CUR);
                c=fgetc(enchere);//on passe le #
            }
            else
            {
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
    char dateFin[13];//date de fin de l'enchere
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
    enchere=fopen(FichierEnchere,"rb");
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

    retour=testDateEnchere(enchere,dateFin);
    if(retour==0)
    {
        printf("le message n'est pas conforme delai enchere depassé.\n");
        fclose(enchere);
        return 0; //on retourne 0 pour indiquer que le message n'est pas conforme delai enchere depassé.
    }
    else if(retour==2)
    {
        printf("La date de fin de l'objet a été modifiée.\n");
        modifierFichierObjet(idObjet,dateFin);//modifie la date de fin d'enchere dans le fichier objet
    }

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
    retour=enregistrementPrix(idUti,idObjet,dateFin,prix,enchere);
    if(retour==-1)
    {
        fclose(enchere);
        return -1;
    }

    fclose(enchere);
    rename(FichierEnchereTmp,FichierEnchere);
    return 1;
}

int testDateMiseEnVente(char *message,int tailleMsg,char *dateDebut,int tailleDate)
{
    struct tm finEnchere;//pointeur sur une structure de type time sur la fin de l'enchere
    time_t secondes; //recupere la date systeme en secondes.
    struct tm instant;//struct pour la date actuelle

    time(&secondes);//date systeme
    /*decomposition de la date de fin enchere dans la strcuture*/
    sscanf(message,"%2d%2d%2d%2d%4d",&(finEnchere.tm_hour),&(finEnchere.tm_min),&(finEnchere.tm_mday),&(finEnchere.tm_mon),&(finEnchere.tm_year));
    finEnchere.tm_mon  -=1 ;   // commence à janvier=0 pour les mois
    finEnchere.tm_year -= 1900;  //annee attention(annee-1900)
    finEnchere.tm_sec  = 0;   /* 0 seconde */
    finEnchere.tm_isdst=-1;//empeche heure d'été et hiver
    mktime(&finEnchere); //fini d'initialiser la structure finEnchere

    instant=*localtime(&secondes);//initialise la struture instant qui est la date systeme actuelle

    if(difftime(mktime(&finEnchere),mktime(&instant))>0)
    {
        strftime(dateDebut,13, "%H%M%d%m%Y",&instant);//ecrit dans une chaine au format desiré
        return 1; //la date est ok
    }
    return 0;
}


int nbrVenteEnchereUtilisateur(char *idUtilisateur,char *msgClient)
{
    char idTmp[6];
    FILE *enchere=NULL;
    char c;
    int nbrVente=0;
    int nbrEnchere=0;

    enchere=fopen(FichierEnchere,"rb");
    if(enchere==NULL)//si fichier inexistant on retourne pas d'enchere pas de vente
    {
        printf("le fichier enchere.txt ne s'ouvre pas.\n");
        sprintf(msgClient,"%s#0#0\n",msgClient);
        return 1;
    }

    c=fgetc(enchere);
    while(c!=EOF)
    {
        int trouve=0;//bool si l'utilisateur est trouve dans l'enchere ou non
        fgets(idTmp,6,enchere);
        if(!strcmp(idTmp,idUtilisateur))//si les identifiants sont les mm, l'utili vente un objet
        {
            nbrVente++;
        }
        else
        {
            fseek(enchere,19,SEEK_CUR);//on passe 20 caractère
            c=fgetc(enchere);
            while(c!=EOF && c!='$' && !trouve)//on cherche si l'identifiant est dans les encheres
            {
                c=fgetc(enchere);
                while(c!='#' && c!=EOF && c!='$')//on passe le prix
                {
                    c=fgetc(enchere);
                }
                if(c!='$' && c!=EOF && c!='\n')//on test si y'a bien des encheres
                {
                    fgets(idTmp,6,enchere);//on recupere l'id si enchere
                    if(!strcmp(idTmp,idUtilisateur))//si les identifiants sont les mm, l'utili participe à une enchere
                    {
                        nbrEnchere++;
                        trouve=1;//on stop la boucle
                    }
                    c=fgetc(enchere);
                }
            }
        }
        while(c!=EOF && c!='$')
        {
            c=fgetc(enchere);
        }
    }
    sprintf(msgClient,"%s#%d#%d\n",msgClient,nbrEnchere,nbrVente);

    return 1;
}

void supprCaractere(char *message)
{
    char *pointeur=NULL;
    pointeur=strchr(message,'\n');
    if(pointeur!=NULL)
    {
        *pointeur='\0';
    }
}


void etatEnchere()
{
    FILE *enchere=NULL;
    FILE *enchereTmp=NULL;
    FILE *finEnchere=NULL;
    char idPro[6];
    char idObjet[6];
    char dateEnchere[13];
    char dateDebut[13];//ne sert à rien juste à respecter le format de la fonction
    char c;

    enchere=fopen(FichierEnchere,"rb");
    if(enchere==NULL)
    {
        printf("Aucune enchere en cours.\n");
        return;
    }

    enchereTmp=fopen(FichierEnchereTmp,"wb");
    finEnchere=fopen(FichierFinEnchere,"ab");
    if(enchereTmp==NULL || finEnchere==NULL)
    {
        printf("Erreur d'ouverture fichier.\n");
        fclose(enchere);
        //on ferme le fichier
        if(enchereTmp) fclose(enchereTmp);
        if(finEnchere) fclose(finEnchere);
        return;
    }

    c=fgetc(enchere);
    while(c!=EOF)
    {
        fgets(idPro,6,enchere);//on recupere id vendeur
        c=fgetc(enchere);//#
        fgets(idObjet,6,enchere);//on recupère id objet
        c=fgetc(enchere);
        fgets(dateEnchere,13,enchere);//on recupere date fin enchere
        if(testDateMiseEnVente(dateEnchere,13,dateDebut,13))//si date positive on ecrit dans fichier enchereTmp
        {
            fprintf(enchereTmp,"$%s#%s#%s",idPro,idObjet,dateEnchere);
            c=fgetc(enchere);
            while(c!='$' && c!=EOF)
            {
                fputc(c,enchereTmp);
                c=fgetc(enchere);
            }
        }
        else
        {
            float prix=0;
            char idAcheteur[6];//identifiant de l'acheteur
            c=fgetc(enchere);
            fscanf(enchere,"%f",&prix);
            fprintf(finEnchere,"$%s#%s#%s#%5.2f",idPro,idObjet,dateEnchere,prix);
            c=fgetc(enchere);
            if(c!=EOF && c!='$')//si # cela veut dire un acheteur
            {
                fgets(idAcheteur,6,finEnchere);//on recupère id
                incrementerNbrObjet(idAcheteur,idPro);//on increment le nbr d'achat vendeur acheteur.
                fprintf(finEnchere,"#%s",idAcheteur);
                c=fgetc(enchere);
                while(c!='$' && c!=EOF)
                {
                    fputc(c,finEnchere);
                    c=fgetc(enchere);
                }
            }

        }
    }
    fclose(enchere);
    fclose(enchereTmp);
    fclose(finEnchere);
    rename(FichierEnchereTmp,FichierEnchere);
    return;
}


int informationFinEnchere(char *message,int taille)
{
    char idUti[6];
    int retour=0;
    char c;
    char idComp[6];//id utilisateur qui est récupéré dans le fichier
    char msgServeur[30];

    retour=extraireIdClient(&message,taille,idUti);
    if(retour==-1)
    {
        printf("problème d'ouverture fichier user.txt\n");
        return -1;
    }
    else if (retour==0)
    {
        printf("l'identifiant utilisateur est inconnu.\n");
        return 0;
    }

    FILE *finEnchere=NULL;
    finEnchere=fopen(FichierFinEnchere,"rb");
    if(finEnchere==NULL)
    {
        printf("Aucune enchere n'est terminée.\n");
        return 0;
    }
    c=fgetc(finEnchere);
    while(c!=EOF && retour==1)
    {
        fgets(idComp,6,finEnchere);//identifiant du vendeur
        c=fgetc(finEnchere);
        if(!strcmp(idComp,idUti))
        {
            char idObjet[6];//recupere l'identifiant de l'objet
            char idAcheteur[6];
            float prixFinal=0;
            fgets(idObjet,6,finEnchere);
            if(donneeObjet(idUti,idObjet,idUti,1,"MesVentes","","",1)!=1) //envoie au client les données objets
            {
                printf("Probleme dans l'envoie des données objets.\n");
                retour=-1;//retourne une erreur serveur
            }
            else
            {
                fseek(finEnchere,14,SEEK_CUR);
                fscanf(finEnchere,"%f",&prixFinal);
                c=fgetc(finEnchere);//# ou $
                sprintf(msgServeur,"#%5.2f\n",prixFinal);
                printf("informations: %s",msgServeur);
                EmissionBinaire(msgServeur,strlen(msgServeur));
                if(c!=EOF && c!='$' && c!='\n')
                {
                    char msgTmp[8];
                    fgets(idAcheteur,6,finEnchere);//recupere l'id acheteur
                    sprintf(msgTmp,"%s\n",idAcheteur);//formatage util pour la fonction informationUtilisateur
                    if(informationUtilisateur(msgTmp,strlen(msgTmp),1)<0)//envoie les données sur l'utilisateur
                        retour=-1;//erreur serveur;
                }
            }
        }
        while(c!=EOF && c!='$')
        {
            c=fgetc(finEnchere);
        }
    }

    fclose(finEnchere);
    return retour;
}

int modifierFichierObjet(char *idObjet,char *dateFin)
{
    FILE *objet=NULL;
    FILE *objetTmp=NULL;
    char c;
    char idObjetTmp[6];

    objet=fopen(FichierObjetEnVente,"rb");
    objetTmp=fopen(FichierObjetEnVenteTmp,"wb");
    if(objet==NULL || objetTmp==NULL)
    {
        if(objet) fclose(objet);
        if(objetTmp) fclose(objetTmp);
        return -1;//erreur fichier
    }
    c=fgetc(objet);
    while(c!=EOF)
    {
        int compteur=0;//compteur de #
        fgets(idObjetTmp,6,objet);//on recupere l'id objet du fichier
        c=fgetc(objet);
        fprintf(objetTmp,"$%s",idObjetTmp);
        while(compteur!=4)
        {
            if(c!='\n')
            {
                fputc(c,objetTmp);//on ecrit les informations
            }
            if(c=='#')
                compteur++;//on compte les #
            c=fgetc(objet);
        }
        if(!strcmp(idObjet,idObjetTmp))
        {
            fprintf(objetTmp,"%s#",dateFin);
            fseek(objet,12,SEEK_CUR);
            c=fgetc(objet);
        }
        while(c!=EOF && c!='$')
        {
            if(c!='\n')
            {
                fputc(c,objetTmp);//on ecrit les informations
            }
            c=fgetc(objet);
        }
    }

    fclose(objet);
    fclose(objetTmp);
    rename(FichierObjetEnVenteTmp,FichierObjetEnVente);
    return 1;
}
