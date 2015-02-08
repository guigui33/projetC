#include <stdio.h>
#include <stdlib.h>
#include "serveur.h"
#include "client.h"
#include "objet.h"
#include <string.h>
#include <time.h>

int main()
{

    char *message = NULL;

    Initialisation();

    while(1)
    {
        char typeRequete[11];//variable pour connaitre le type de requète
        int retour=0; //variable d'aide pour les retours de fonction
        int fin=0;
        char messageRetour[100]; //message retour
        char *donnee=NULL; //pointeur sur le message où commence les donnees
        etatEnchere();
        AttenteClient();

        while(!fin)
        {
            printf("***etat des encheres***\n");

            message = Reception();

            printf("J'ai recu %s\n",message);

            if(message!=NULL)
            {
                /*on extrait le type de requète*/
                extraireTypeRequete(message,strlen(message),&donnee,typeRequete);
                if(!strncmp(message,"creerCpt",8))
                {
                    char id[6];
                    retour=creationCompte(donnee,strlen(donnee),id);
                    if(retour==1)
                    {
                        sprintf(messageRetour,"%s#%s","cptEnregistre",id);
                    }
                    else if(retour==0)
                    {
                        sprintf(messageRetour,"%s","erreurEnregistrement");
                    }

                }
                else if(!strncmp(typeRequete,"connex",6))
                {
                    /*verification de l'id et du mot de passe de l'utilisateur*/
                    retour=verificationAuthentification(donnee,strlen(donnee));
                    if(retour==1)
                    {
                        sprintf(messageRetour,"%s","connexOk");
                    }
                    else if(retour==0)
                    {
                        sprintf(messageRetour,"%s","erreurIdMdp");
                    }
                }
                else if(!strncmp(typeRequete,"Deco",4))
                {
                    sprintf(messageRetour,"%s","deconnexion");
                    fin=1;
                }
                else if (!strncmp(typeRequete,"acheter",7))
                {
                    retour=acheterObjet(donnee,strlen(donnee));
                    if(retour==1)
                    {
                        sprintf(messageRetour,"%s","enchereEnregistree");
                    }
                    else if(retour==0)
                    {
                        sprintf(messageRetour,"%s","erreurEnchere");
                    }
                }
                else if(!strncmp(typeRequete,"consulter",9))
                {
                    retour=consulter(donnee,strlen(donnee));
                    if(retour==1)
                    {
                        sprintf(messageRetour,"%s","EOF"); //fin de fichier
                    }
                    else if(retour==0)
                    {
                        sprintf(messageRetour,"%s","erreurConsulter");
                    }
                }
                else if(!strncmp(typeRequete,"vendre",6))
                {
                    retour=enregistrementObjet(donnee,strlen(donnee));
                    if(retour==1)
                    {
                        sprintf(messageRetour,"%s","objEnregistre");
                    }
                    else if(retour==0)
                    {
                        sprintf(messageRetour,"%s","erreurEnregistrement");
                    }
                }
                else if(!strncmp(typeRequete,"infoCpt",7))
                {
                    retour=informationUtilisateur(donnee,strlen(donnee));
                    if(retour==1)
                    {
                        sprintf(messageRetour,"%s","infoCptEnvoyee");
                    }
                    else if(retour==0)
                    {
                        sprintf(messageRetour,"%s","erreurEnvoieInfo");
                    }
                }
                else if(!strncmp(typeRequete,"finEnchere",10))//a faire
                {
                    retour=informationFinEnchere(donnee,strlen(donnee));

                }
                else
                {
                    sprintf(messageRetour,"%s","erreurMessage");
                }
                if(retour==-1)
                {
                    sprintf(messageRetour,"%s","erreurServeur");
                }
                printf("envoie des données: %s\n",messageRetour);
                strcat(messageRetour,"\n");
                EmissionBinaire(messageRetour,strlen(messageRetour));
                free(message);
                message=NULL;
            }
        }

        TerminaisonClient();

    }//fin while(1)

    return 0;
}
