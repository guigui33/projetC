#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "objet.h"
#include "client.h"
#include "serveur.h"
#include <time.h>

#define FichierObjetEnVente "/home/guigui/Documents/L3/Projet/serveur/fichiers/objets_en_vente.txt"
#define FichierEnchere "/home/guigui/Documents/L3/Projet/serveur/fichiers/enchere.txt"
#define FichierEnchereTmp "/home/guigui/Documents/L3/Projet/serveur/fichiers/enchereTmp.txt"

/*creer l'id de l'objet, sur 5 caractères, 3premieres lettre du nom+ 2chiffres*/
/*ajouter message ok*/
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


int consulter(char *message,int tailleMsg)
{
    int retour=0;//bool pour savoir si le client desire voir tous les objets mises en vente
    char nomRch[26];//nom de l'objet à rechercher à extraire du msg
    char descriptionRch[101]; //description de l'objet à rechercher à extraire du msg
    char idUti[6];//identifiant du client
    FILE *objet=NULL;
    int j=0;
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
    /*on extrait le nom de la recherche à effectuer*/
    while(i<tailleMsg && j<25 && message[i]!='#')
    {
        nomRch[j]=message[i];
        i++;
        j++;
    }
    i++;// on passe le #
    nomRch[j]='\0';

    if(nomRch[0]=='\0') // si le nom est vide problème message
    {
        printf("le message n'est pas conforme.\n");
        return 0;
    }

    j=0;
    while(i<tailleMsg && j<100 && message[i]!='\n')
    {
        descriptionRch[j]=message[i];
        i++;
        j++;
    }
    descriptionRch[j]='\0'; //la description peut etre vide

    objet=fopen(FichierObjetEnVente,"rb");
    if(objet==NULL)
    {
        printf("erreur d'ouverture du fichier objets_en_vente.txt\n");
        return -1;
    }

    retour=donneeObjetCatalogue(idUti,nomRch,descriptionRch,objet);//on va recuperer les donnees à envoyer au client dans les differents fichiers


    fclose(objet);
    return retour;//retourne 1 si ok, -1 sinon
}


int donneeObjetCatalogue(char* idUtili,char *nomRch,char *descriptionRch,FILE *objet)
{
    char msgClient[500];//message à envoyer au client contient les infos objet+proprio+enchere
    char tmp[35];//varaible utilisée pour sauvegarder les données dates et prix
    char idPro[6];//identifiant de du propietaire de l'objet
    char idObjet[6];//id de l'objet
    char nomObjet[26];//nom de l'objet
    char description[100];//description de l'objet
    char categorie[26];//categorie de l'objet sur 25 caracteres
    char c;
    int tous=0;//bool pour savoir si il faut envoyer tous les objets ou non

    tous=strcmp(nomRch,"tous");//on test si le nom=tous, mot clé pour envoyer tous les objets mises en vente

    c=fgetc(objet);
    while(c!=EOF)
    {
        int i=0;
        /*recuperation id de l'objet*/
        fgets(idObjet,6,objet);
        fgetc(objet);//on passe le #
        fgets(idPro,6,objet);//on recupere l'identifiant du proprietaire de l'objet
        /*recupere nom objet max 25 caractère*/
        fgetc(objet);//on passe le #
        c=fgetc(objet);
        while(c!=EOF && i<25 && c!='#')
        {
            nomObjet[i]=c;
            c=fgetc(objet);
            i++;
        }
        nomObjet[i]='\0';
        fgets(tmp,27,objet); //dateDeb#dateFin#
        i=26;
        c=fgetc(objet);
        /*on recupere le prix de depart de l'objet*/
        while(i<35 && c!=EOF && c!='#')
        {
            tmp[i]=c;
            i++;
            c=fgetc(objet);
        }
        tmp[i]='\0';

        i=0;
        c=fgetc(objet);
        while(i<100 && c!=EOF && c!='#')
        {
            categorie[i]=c;
            c=fgetc(objet);
            i++;
        }
        categorie[i]='\0';

        i=0;
        c=fgetc(objet);
        while(i<100 && c!=EOF && c!='#')
        {
            description[i]=c;
            c=fgetc(objet);
            i++;
        }
        description[i]='\0';

        if(!tous || (strstr(nomObjet,nomRch) && strstr(description,descriptionRch)))
        {
            sprintf(msgClient,"$%s#%s#%s#%s#%s#",idObjet,nomObjet,tmp,categorie,description);
            c=fgetc(objet);
            i=strlen(msgClient);//on se place à la fin de la chaine
            while(c!=EOF && c!='$' && i<500)
            {
                msgClient[i]=c;
                i++;
                c=fgetc(objet);
            }
            msgClient[i]='\0';
            /*on recupere les informations sur le propietaire de l'objet*/
            if(donneeUtilisateur(idPro,msgClient,"donneeObjetCatalogue")==-1)
            {
                return -1;
            }
            /*on recupere les informations sur l'enchere de l'objet*/
            if(donneeEnchereCatalogue(idUtili,idObjet,msgClient)==-1)
            {
                return -1;
            }
            /* on envoie les informations au client*/
            printf("informations: %s\n",msgClient);
            if(EmissionBinaire(msgClient,strlen(msgClient))<=0)
            {
                printf("Problème emission.\n");
                return -1;
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
/*ajouter les 15 minustes si delai<15mn*60s*/
int testDateEnchere(FILE *enchere)
{
    char date[13];
    struct tm finEnchere;//pointeur sur une structure de type tm, pour contenir la date de fin de l' enchere
    time_t secondes; //recupere la date systeme en secondes.
    struct tm instant;//struct pour la date actuel

    fgetc(enchere);//on passe le #
    fgets(date,13,enchere); //on recupere la date de fin de l'objet sur 12 caracteres

    time(&secondes);//date systeme
    /*decomposition de la date de fin enchere dans la strcuture*/
    sscanf(date,"%2d%2d%2d%2d%4d",&(finEnchere.tm_hour),&(finEnchere.tm_min),&(finEnchere.tm_mday),&(finEnchere.tm_mon),&(finEnchere.tm_year));
    finEnchere.tm_mon  -=1 ;   // commence à janvier=0 pour les mois
    finEnchere.tm_year -= 1900;  //annee attention(annee-1900)
    finEnchere.tm_sec  = 0;   /* 0 seconde */
    finEnchere.tm_isdst=-1;//empeche heure d'été et hiver
    mktime(&finEnchere);

    instant=*localtime(&secondes);//initialise la struture instant qui est la date systeme actuelle

    if(difftime(mktime(&finEnchere),mktime(&instant))>0)
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

/*refaire date*/
int enregistrementPrix(char *idUti,char *idObjet,float prix,FILE *enchere)
{
    FILE *enchereTmp=NULL;
    char idUtiTmp[6];
    char idObjtTmp[6];
    char c;

    enchereTmp=fopen(FichierEnchereTmp,"wb");
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
                c=fgetc(enchere);//on passe le #
                c=fgetc(enchere);//on lit le caractere suivant
                while(c!=EOF && c!='#')
                {
                    fprintf(enchereTmp,"%c",c);
                    c=fgetc(enchere);
                }
                fprintf(enchereTmp,"#%.2f#%s",prix,idUti);
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

    retour=testDateEnchere(enchere);
    if(retour==0)
    {
        printf("le message n'est pas conforme delai enchere depassé.\n");
        return 0; //on retourne 0 pour indiquer que le message n'est pas conforme delais enchere depassé.
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
    retour=enregistrementPrix(idUti,idObjet,prix,enchere);
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
