#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilisateur.h"
#include "secondaire.h"
#include "client.h"

#define CheminFichierTEMP "/home/guigui/Documents/L3/Projet/client/fichiers/tmp.txt"

int authentification(char * idUti)
{
    char id[6];// contient l'identifiant de l'utilisateur
    char motPasse[16];// contient le mot de passe de l'utilisateur
    char connex[30];// message d'authentification 29 caractères maximum
    char *msgServeur=NULL;
    int retour=-1;//variable de retour

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

    sprintf(connex,"connex#%s#%s\n",id,motPasse);

    if(EmissionBinaire(connex,strlen(connex))<=1)
    {
        return -1;//probleme serveur
    }

    msgServeur=Reception();

    if(!strncmp(msgServeur,"erreurServeur",13))
    {
        retour=-1;
    }
    else if(!strncmp(msgServeur,"erreurIdMdp",11))
    {
        retour=0;
    }
    else if(!strncmp(msgServeur,"connexOk",8))
    {
        strcpy(idUti,id);
        retour=1;
    }
    else retour=0;

    free(msgServeur);
    return retour;
}


/*creerCpt*/
int creationDeCompte()
{
    char *msgServeur=NULL;
    char creerCpt[298];
    char tmp[101];//variable temporaire
    char mdp1[16];
    char mdp2[16];
    char nomPrenom[53];
    int infoOk = 0;//varaible Bool pour verifier les informations

    printf("***création de compte***\n");

    do
    {
        printf("Nom [compris entre 3 et 25 caractères]:");
        fgets(tmp,26,stdin);
        supprCara(tmp);
        infoOk=controleChaine(tmp,"nom");
    }
    while(!infoOk);

    strcpy(nomPrenom,tmp);
    strcat(nomPrenom,"#");

    do
    {
        printf("Prenom [compris entre 3 et 25 caractères]:");
        fgets(tmp,26,stdin);
        supprCara(tmp);
        infoOk=controleChaine(tmp,"nom");
    }
    while(!infoOk);

    strcat(nomPrenom,tmp);

    do
    {
        printf("Mot de passe [compris entre 6 et 15 caractères]:");
        fgets(mdp1,16,stdin);
        supprCara(mdp1);
        infoOk=controleChaine(mdp1,"motDePasse");

        if(infoOk==1)
        {
            printf("confirmation mot de passe:");
            fgets(mdp2,16,stdin);
            supprCara(mdp2);
            if(strcmp(mdp1,mdp2))
            {
                printf("les mots de passe sont diffèrents.\n");
                infoOk=0; //si les deux mots de passe sont differents
            }
        }
    }
    while(!infoOk);

    sprintf(creerCpt,"creerCpt#%s#%s",mdp1,nomPrenom);
   infoOk=0;
    while(!infoOk)
    {
        printf("Date de naissance (jj/mm/aaaa): ");
        if(dateNaissance(tmp)==1)
        {
            sprintf(creerCpt,"%s#%s#",creerCpt,tmp);
            infoOk=1;
        }
        else
        {
            printf("la date entrée n'est pas correcte.\n");
            infoOk=0;
        }
    }//fin while

    do
    {
        printf("Numero de telephone:");
        fgets(tmp,11,stdin);
        supprCara(tmp);
        infoOk=controleChaine(tmp,"");
    }
    while(!infoOk);

    strcat(creerCpt,tmp);
    strcat(creerCpt,"#");

    do
    {
        printf("Adresse mail[50 caractères maximum]:");
        fgets(tmp,51,stdin);
        supprCara(tmp);
        infoOk=controleChaine(tmp,"");
    }
    while(!infoOk);

    strcat(creerCpt,tmp);
    strcat(creerCpt,"#");

    do
    {
        printf("n° de rue + nom de la rue[100 caractères maximum]:");
        fgets(tmp,101,stdin);
        supprCara(tmp);
        infoOk=controleChaine(tmp,"");
    }
    while(!infoOk);

    strcat(creerCpt,tmp);
    strcat(creerCpt,"#");

    do
    {
        printf("Ville [50 caractères maximum]:");
        fgets(tmp,51,stdin);
        supprCara(tmp);
        infoOk=controleChaine(tmp,"");
    }
    while(!infoOk);

    strcat(creerCpt,tmp);
    strcat(creerCpt,"#");

    do
    {
        printf("Code postal:");
        fgets(tmp,6,stdin);
        supprCara(tmp);
        infoOk=controleChaine(tmp,"");
    }
    while(!infoOk);

    strcat(creerCpt,tmp);
    strcat(creerCpt,"\n");

    if(EmissionBinaire(creerCpt,strlen(creerCpt))<=0)
    {
        printf("Erreur d'emission\n");
        return -1;
    }

    msgServeur=Reception();
    int i=0;//varaible de boucle
    if(!strncmp(msgServeur,"cptEnregistre",13))
    {
        while(msgServeur[i]!='#') i++;
        i++;//on passe le #
        printf("Le compte est bien enregistré.\nVotre identifiant: ");
        while(i<strlen(msgServeur))
        {
            printf("%c",msgServeur[i]);
            i++;
        }
        printf("\n");
        infoOk=1;
    }
    else if(!strncmp(msgServeur,"erreurEnregistrement",20))
    {
        infoOk=0;
    }
    else if(!strncmp(msgServeur,"erreurServeur",13))
    {
        infoOk=-1;
    }
    else infoOk=0;

    free(msgServeur);

    return infoOk;
}


int vendre(char * idUser)
{
    char msg_serveur[338]; //max 337 caracteres
    float prix=0;//recuperrer le prix de l'objet
    int quantite=0;//recuperer quantite d'objet
    char temp[101];
    int infoOk=0;
    char *msgRetour=NULL;

    sprintf(msg_serveur,"vendre#%s#",idUser);

    printf("****Vente d'un objet****\n");

    do
    {
        printf("Nom de l'objet:");
        fgets(temp,26,stdin);
        supprCara(temp);
        infoOk=controleChaine(temp,"nom");
    }
    while(!infoOk);

    strcat(msg_serveur,temp);
    strcat(msg_serveur,"#");

    dateFinEnchere(temp);//la date de fin de l'enchere

    strcat(msg_serveur,temp);
    strcat(msg_serveur,"#");

    do
    {
        printf("Entrez un prix de départ pour l'objet que vous voulez mettre en vente.\n");
        printf("(tous objets ne peut depasser un prix de 100 000€).\n");
        printf("prix: ");
        scanf("%f",&prix);
        viderBuffer();
    }
    while(prix<0 || prix >= 100000);

    sprintf(msg_serveur,"%s%5.2f#",msg_serveur,prix);

    do
    {
        printf("entrez une categorie  [maximum 25 caractères]: ");
        fgets(temp,26,stdin);
        supprCara(temp);
        infoOk=controleChaine(temp,"");
    }
    while(!infoOk);

    strcat(msg_serveur,temp);
    strcat(msg_serveur,"#");

    do
    {
        printf("entrez une description [maximum 100 caractères]: ");
        fgets(temp,101,stdin);
        supprCara(temp);
        infoOk=controleChaine(temp,"");
    }
    while(!infoOk);

    strcat(msg_serveur,temp);
    strcat(msg_serveur,"#");

    do
    {
        printf("entrez une url de l'image produit [maximum 100 caractères]: ");
        fgets(temp,101,stdin);
        supprCara(temp);
        infoOk=controleChaine(temp,"");
    }
    while(!infoOk);

    strcat(msg_serveur,temp);
    strcat(msg_serveur,"#");

    do
    {
        printf("Entrez une quantité (>0).\n");
        printf("quantité: ");
        scanf("%d",&quantite);
        viderBuffer();
    }
    while(quantite<0 || quantite>99);

    sprintf(msg_serveur,"%s%d#",msg_serveur,quantite);

    do
    {
        printf("entrez la ville où se situe l'objet [maximum 50 caractères]: ");
        fgets(temp,51,stdin);
        supprCara(temp);
        infoOk=controleChaine(temp,"");
    }
    while(!infoOk);

    strcat(msg_serveur,temp);
    strcat(msg_serveur,"\n");

    if(EmissionBinaire(msg_serveur,strlen(msg_serveur))<=0)
    {
        printf("Erreur d'emission\n");
        return -1;
    }

    msgRetour=Reception();

    if(!strncmp(msgRetour,"objEnregistre",13))
    {
        infoOk=1;
    }
    else if(!strncmp(msgRetour,"erreurEnregistrement",20))
    {
        infoOk=0;
    }
    else if(!strncmp(msgRetour,"erreurServeur",13))
    {
        infoOk=-1;
    }

    else infoOk=0;

    free(msgRetour);

    return infoOk;
}


int rechercheObjet(char *idUser)
{
    char consulter[144];//tailles maximum du messages consulter
    char nom[26];
    char description[101];
    int infoOk=0;

    nom[0]='\0';
    description[0]='\0';

    printf("entrez un nom de produit à rechercher [26 caractères maximum]:\n");
    printf("(si aucun nom n'est rentré tous les produits disponibles sur le site vous seront envoyés.)\n");

    do
    {
        printf("nom: ");
        fgets(nom,26,stdin);
        supprCara(nom);
        infoOk=controleChaine(nom,"recherche");
    }
    while(!infoOk);

    if(strlen(nom)==0) strcpy(nom,"tous");//si rien n'est rentré par l'utilisateur

    /*si l'utilisateur entre un nom, on lui propose de rentrer une description*/
    if(strcmp(nom,"tous"))
    {
        printf("entrez une description de produit à rechercher [100 caractères maximum]:\n");
        printf("(tapez entré si aucune description.)\n");

        do
        {
            printf("description: ");
            fgets(description,101,stdin);
            supprCara(description);
            controleChaine(description,"recherche");
        }
        while(!infoOk);
    }

    sprintf(consulter,"consulter#%s#%s#%s\n",idUser,nom,description);
    if(EmissionBinaire(consulter,strlen(consulter))<=0)
    {
        return -1;//on retourne une erreur serveur
    }

    infoOk=enregistrementCatalogueTemp();

    return infoOk;
}

/*
acheter 7 #idUtilisateur 5#idObjet 5#prix 8\n
*/
void enchere(char *idUtilisateur,char *idObjet)
{
    char acheter[30]; //taille max 30 caractères
    char *msgServeur=NULL;
    float prix=0;

    printf("Proposer une enchère, l'enchère doit etre superieur au prix indiqué et ne doit pas dépasser 100000€ pour etre validée.\n");

    do
    {
        printf("prix: ");
        scanf("%f",&prix);
        viderBuffer();
    }
    while(prix<=0 && prix>=100000);

    sprintf(acheter,"acheter#%s#%s#%5.2f\n",idUtilisateur,idObjet,prix);

    if(EmissionBinaire(acheter,strlen(acheter))<=0)
    {
        printf("Problème d'emission.\n");
        return;//on retorune une erreur Serveur
    }

    msgServeur=Reception();
    if(!strncmp(msgServeur,"enchereEnregistree",18))
    {
        printf("votre enchère a été validée.\n");
    }
    else if(!strncmp(msgServeur,"erreurEnchere",13))
    {
        printf("Votre enchère n'a pas été validée.\n");
        printf("Le delais a été dépassé ou bien le prix proposé est inferieur au prix actuel.\n");
    }
    else if(!strncmp(msgServeur,"erreurServeur",13))
    {
        printf("Le serveur a rencontré des difficulté et n'a pu valider votre enchère.\n");
    }

    free(msgServeur);
    return;
}


int catalogue(char *idUtilisateur)
{
    size_t fin=0;//boolean de fin
    int choix=3;//recupere le choix de l'utilisateur
    char idObjet[6];//recupere idobjet
    FILE *tmp=NULL;//fichier temporaire
    int produit=2;//signal de produit precedent, suivant ou le meme
    int finFichier=0;//signal de fin de catalogue, la touche suivant n'est plus active

    tmp=fopen(CheminFichierTEMP,"rb");
    if(tmp==NULL)
    {
        return -1;
    }

    while(!fin)
    {
        if(produit!=-1)
        {
            printf("-------Catalogue-------\n");
            afficherObjet(tmp,idObjet,produit,&finFichier);
            printf("0)Quitter\n");
        }
        printf("Choix: ");
        scanf("%d",&choix);
        viderBuffer();
        if(choix==0 || finFichier==-1)
        {
            fin=1;//on quitte le catalogue
        }
        else if(choix==2)
        {
            produit=0;//astuce pour compter les $ dans le fichier
            enchere(idUtilisateur,idObjet);
        }
        else if(choix==1)
        {
            produit=1;//signal produit precedent
        }
        else if(choix==3 && !finFichier)//si !fin de fichier=0 touche suivante non dispo
        {
            produit=2;//signal de continuer, produit suivant
        }
        else
        {
            printf("mauvaise touches.\n");
            produit=-1;//signal erreur
        }
    }
    fclose(tmp);
    return 1;
}

int informationCompte(char *idUser)
{
    char infoCmpt[15];
    char *msgRetour=NULL;
    int infoOk=-1;

    sprintf(infoCmpt,"infoCpt#%s\n",idUser);
    if(EmissionBinaire(infoCmpt,strlen(infoCmpt))<=0)
    {
        return -1;
    }
    msgRetour=Reception();
    if(!strncmp(msgRetour,"erreurEnvoieInfo",16) || !strncmp(msgRetour,"erreurMessage",13))
    {
        infoOk=0;//erreur msg
    }
    else if(!strncmp(msgRetour,"erreurServeur",13))
    {
        infoOk=-1;//erreur serveur
    }

    else if(msgRetour!=NULL){
        infoOk=1;
        afficherUtilisateur(msgRetour,strlen(msgRetour));
    }

    free(msgRetour);
    return infoOk;
}
