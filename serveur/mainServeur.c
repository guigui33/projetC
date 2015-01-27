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
        char typeRequete[10];//variable pour connaitre le type de requète
        int retour=0; //variable d'aide pour les retours de fonction
        char *messageRetour=NULL; //message retour
        char *donnee=NULL; //pointeur sur le message où commence les donnees


        AttenteClient();

        message = Reception();

        printf("J'ai recu %s\n",message);

        if(message!=NULL)
        {
            /*on extrait le type de requète*/
            extraireTypeRequete(message,strlen(message),&donnee,typeRequete);
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
                retour=verificationAuthentification(donnee,strlen(donnee));
                if(retour==1)
                {
                    messageRetour="connexOk";
                }
                else if(retour==0)
                {
                    messageRetour="connexNon";
                }
                else
                {
                    messageRetour="prblm serveur";
                }
            }
            else if(!strncmp(typeRequete,"Deco",4))
            {
                messageRetour="deconnexion";
            }

            else if (!strncmp(typeRequete,"acheter",7))
            {
                acheterObjet(donnee,strlen(donnee));
            }
            else if(!strncmp(typeRequete,"consulter",9))
            {
                retour=consulter(donnee,strlen(donnee));
                if(retour==1)
                {
                    messageRetour="fin fichier";
                }
                else if(retour==0)
                {
                    messageRetour="probleme message";
                }
                else
                {
                    messageRetour="prblm serveur";
                }
            }
            else if(!strncmp(typeRequete,"vendre",6))
            {
                retour=enregistrementObjet(donnee,strlen(donnee));
                if(retour==1){
                    messageRetour="enregistrement OK";
                }
                else if(retour==0){
                    messageRetour="enregistrement non ok";
                }
                else messageRetour="probleme serveur";

            }
            else
            {
                messageRetour="type message inconnu";
            }


            EmissionBinaire(messageRetour,strlen(messageRetour));
            free(message);
            message=NULL;
        }

        TerminaisonClient();

    }//fin while(1)

    return 0;
}
