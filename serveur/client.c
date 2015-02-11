#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "client.h"
#include "objet.h"
#include "serveur.h"

#define FichierNewUser "/home/guigui/Documents/L3/Projet/serveur/fichiers/newUser.txt"
#define FichierUser "/home/guigui/Documents/L3/Projet/serveur/fichiers/user.txt"

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
    user=fopen(FichierUser,"rb");

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
    int j=0;

    /*on passe le mot de passe*/
    while(i<taille && message[i]!='#')
    {
        i++;
    }
    i++; //on passe le #

    while(i<taille && message[i]!='#')
    {
        /*on initialise les 3premiers lettres de l'identifiant, on s'assure que l'id soit construit avec des lettres*/
        if(j<3 && ((message[i]>64 && message[i]<91) || (message[i]>96 && message[i]<123)))
        {
            idUti[j]=message[i];
            j++;
        }
        i++;
    }

    idUti[j]='\0';
    id[0]='\0';

    if(strlen(idUti)!=3) //si l'identifiant est different de 3 alors probleme avec le nom
    {
        printf("identifiant non valable doit contenir 3 caractères minimum.\n");
        return 0;//probleme msg
    }

    user=fopen(FichierUser,"rb"); //ouverture fichier en lecture
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
    int i=0; //variable de boucle
    int j=0; //boucle
    char id[6]; // recuperation identifiant dans le message
    char mdp[16];// recuperation mot de passe dans le message
    int verifOK=0;

    j=0;
    while(i<tailleMsg && j<5 && message[i]!='#')
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
    mdp[j]='\0';

    verifOK=chercherFichierIdMdp(id,mdp);

    return verifOK;
}

/*fonction a refaire, a faire controle d'erreur !!!!!!*/
int creationCompte(char *message,int longMsg,char *id)
{
    FILE *user=NULL;//fichier d'utilisateur
    int retour=0; //variable de retour erreur fonction

    user=fopen(FichierUser,"ab"); //ouverture fichier en ajout, si fichier inexistant il sera créé
    if(user==NULL)
    {
        perror("erreur d'ouverture de fichier");
        return -1; //on returne une erreur
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

    supprCaractere(message);//suppr le \n

    fprintf(user,"$%s#%s#0#0",id,message);

    fclose(user);

    return 1;
}

/*on extrait l'id de la chaine, et on verifie que l'id existe*/
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

    user=fopen(FichierUser,"rb");
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


int incrementerNbrObjet(char *idAcheteur,char *idVendeur)
{
    FILE *user=NULL;//ancien fichier d'utilisateur
    FILE *newUser=NULL; //fichier de remplacement.
    char tmpId[6];//variable temp d'identifiant utilisateur
    char c;
    int indice=0;//astuce pour compter les separateurs: 8 pour acheteur et 9 pour vendeur

    user=fopen(FichierUser,"rb");
    newUser=fopen(FichierNewUser,"wb");

    if(user==NULL && newUser==NULL)
    {
        printf("le fichier \"fichiers/user.txt\" et \"fichiers/newuser.txt\" ne s'ouvrent pas.\n");
        return -1;
    }

    //on incremente le nbr d'objet acheté
    c=fgetc(user);
    while(c!=EOF)
    {
        fgets(tmpId,6,user);
        fprintf(newUser,"$%s",tmpId);//on ecrit l'identifiant
        if(!strcmp(idAcheteur,tmpId))  //si on trouve l'id de l'acheteur
        {
            indice=10; //astuce pour compter les separateurs
        }
        else if(!strcmp(idVendeur,tmpId))  //si on trouve l'id du vendeur
        {
            indice=11;//astuce pour compter les separateurs
        }
        else indice=0;
        if(!indice)  //on recopie dans le fichier les mm informations
        {
            c=fgetc(user);
            while(c!=EOF && c!='$')
            {
                if(c!='\n')
                    fprintf(newUser,"%c",c);
                c=fgetc(user);
            }
        }
        else
        {
            c=fgetc(user);
            int cpt=0; //on compte les #
            int nbr=0;//on recupère le numero à incrementer
            while(c!=EOF && c!='$')
            {
                if(c=='#') cpt++;
                if(c!='\n')
                    fprintf(newUser,"%c",c);
                if(cpt==indice)
                {
                    fscanf(user,"%d",&nbr);
                    nbr++;
                    fprintf(newUser,"%d",nbr);
                }

                c=fgetc(user);
            }
        }

    }

    fclose(user);
    fclose(newUser);
    rename(FichierNewUser,FichierUser);
    return 1;
}

/*cherche le nom et prenom de l'identifiant + le nombre d'objet acheté et vendu par celui-ci*/
int donneeUtilisateur(char *idPro,char *msgClient,char *fonction)
{
    char c;
    int trouve=0;//bool 1 si le id proprietaire est trouve, 0 sinon
    FILE *user=NULL;//fichier utilisteur
    int typeFonction=0;

    user=fopen(FichierUser,"rb");
    if(user==NULL)
    {
        printf("probleme pour ouvrir le fichier user.txt.\n");
        return -1;
    }

    if(!strcmp(fonction,"information"))//informationCompte
    {
        typeFonction=1;
    }
    if(!strcmp(fonction,"donneeObjetCatalogue"))
    {
        typeFonction=0;
    }

    c=fgetc(user);
    while(c!=EOF && !trouve)
    {
        int i=0;
        char idTmp[6];//identifiant temporaire
        fgets(idTmp,6,user);
        c=fgetc(user);//on passe le #
        if(!strcmp(idTmp,idPro))//on a trouve l'identifiant recherché
        {
            trouve=1;//on a trouve l'id recherché
            c=fgetc(user);
            while(c!='#' && c!=EOF)
            {
                c=fgetc(user);//on passe le mot de passe
            }
            recupererInfoUti(msgClient,&c,'#',user);//on recup nom
            recupererInfoUti(msgClient,&c,'#',user);//on recup prenom
            while(typeFonction==1 && i<6)
            {
                i++;
                recupererInfoUti(msgClient,&c,'#',user);//on recupere les autres infos
            }
            while(typeFonction==0 && i<6)
            {
                i++;
                c=fgetc(user);
                while(c!=EOF && c!='#')
                {
                    c=fgetc(user);
                }
            }
            recupererInfoUti(msgClient,&c,'#',user);//objet acheté
            recupererInfoUti(msgClient,&c,'$',user);//objet vendu
        }
        else
        {
            while(c!=EOF && c!='$')
            {
                c=fgetc(user);
            }
        }
    }
    fclose(user);

    if(!trouve)
    {
        return -1;//l'identifiant n'a pas été trouvé on retourne une erreur
    }

    return 1;
}

int informationUtilisateur(char *message,int tailleMsg,int f)
{
    char idUtilisateur[6];
    int retour=0;
    char msgClient[500];//le pire cas 500 caractères

    if(f==0){
        strcpy(msgClient,"$");
    }
    else
        strcpy(msgClient,"");//suite de données à envoyer

    retour=extraireIdClient(&message,tailleMsg,idUtilisateur);
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
    if(f==0)
        strcat(msgClient,idUtilisateur);//pas besoin de copier le id utilisateur si f==1
    if(donneeUtilisateur(idUtilisateur,msgClient,"information")<0)
    {
        return -1;//erreur fichier
    }
    if(f==0)
    {
        if(nbrVenteEnchereUtilisateur(idUtilisateur,msgClient)<0)
        {
            return -1;//erreur fichier
        }
    }
    if(f==1){
        strcat(msgClient,"\n");
    }
    printf("informations: %s",msgClient);
    if(EmissionBinaire(msgClient,strlen(msgClient))<=0)
    {
        return -1;
    }

    return 1;
}

void recupererInfoUti(char *msg,char *c,char separateur,FILE *file)
{
    int i=0;//indice tab
    char tmp[101];//plus grande chaine dans le fichier utilisateur

    *c=fgetc(file);
    while(*c!=separateur && *c!=EOF && i<100)
    {
        if(*c!='\n')
        {
            tmp[i]=*c;
            i++;
        }
        *c=fgetc(file);
    }
    tmp[i]='\0';
    strcat(msg,"#");
    strcat(msg,tmp);
    return;
}

