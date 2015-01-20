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
        int authentifOK=0;
        char typeRequete[10];//variable pour connaitre le type de requète
        char id[6];//recupère l'id de l'utilisateur
        int retour=0; //variable d'aide pour les retours de fonction
        char *messageRetour=NULL; //message retour
        char *donnee=NULL; //pointeur sur le message où commence les donnees

        AttenteClient();

        message = Reception();

        printf("J'ai recu %s\n",message);

        if(message!=NULL)
        {
            /*on extrait le type de requète*/
            extraireTypeRequete(message,strlen(message),donnee,typeRequete);
            if(!strncmp(message,"create",6))
            {
                retour=creationCompte(donnee,strlen(donnee));
                if(retour==1)
                {
                    messageRetour="ok"; //faire le retour erreur
                }
                else if(retour==0)
                {
                    messageRetour="nonOk";
                }
                else
                {
                    messageRetour="prblmServeur";
                }

            }
            else if(!strncmp(typeRequete,"Connex",6))
            {
                /*verification de l'id et du mot de passe de l'utilisateur*/
                authentifOK=verificationAuthentification(donnee,strlen(donnee));
                if(authentifOK==1)
                {
                    messageRetour="connexOk";
                }
                else if(authentifOK==0)
                {
                    messageRetour="connexNon";
                }
                else
                {
                    messageRetour="prblm serveur";
                }
            }
            else
            {
                retour=extraireIdClient(donnee,strlen(donnee),id);
                if(retour==0)
                {
                    messageRetour="prblm msg";
                }
                else if(retour==-1)
                {
                    messageRetour="prblm Serveur";
                }
                else
                {
                    if(!strncmp(message,"Deco",4))
                    {
                        messageRetour="deconnexion";
                    }

                    else if (!strncmp(typeRequete,"acheter",7))
                    {
                        acheterObjet(donnee,strlen(donnee),id);
                    }
                    else if(!strncmp(typeRequete,"consulter",9))
                    {
                        consulter(donnee,strlen(donnee),id);
                    }
                    else if(!strncmp(typeRequete,"vendre",6))
                    {
                        vendre(donnee,strlen(donnee),id);
                    }
                    else
                    {
                        messageRetour="type message inconnu"; //utilité?????????
                    }
                }
            }
            EmissionBinaire(messageRetour,strlen(messageRetour));
            free(message);
            message=NULL;
        }

        TerminaisonClient();

    }//fin while(1)

    return 0;
}
