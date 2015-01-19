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
    char *messageRetour=NULL;
    int retour=0; //variable d'aide pour les retours de fonction
    Initialisation();

    while(1)
    {
        int fini = 0;//boolean pour fin de session
        int authentifOK=0;

        AttenteClient();

        message = Reception();

        printf("J'ai recu %s\n",message);

        if(message!=NULL)
        {
            if(!strncmp(message,"quitter",7))
            {
                fini=1;
            }

            if(!strncmp(message,"Connex",6))
            {
                /*verification de l'id et du mot de passe de l'utilisateur*/
                authentifOK=verificationAuthentification(message,strlen(message));
                if(authentifOK)
                {
                    EmissionBinaire("connexionOK",strlen("connexionOK"));
                }
                else
                {
                    EmissionBinaire("connexionNon",strlen("connexionNon"));
                }
            }
            else if(!strncmp(message,"Deco",4))
            {
                messageRetour="deconnexion";
                EmissionBinaire(messageRetour,strlen(messageRetour));
            }

            else if(!strncmp(message,"create",6))
            {
                retour=verifCreationCompte(message,strlen(message));
                messageRetour="ok"; //faire le retour erreur
                EmissionBinaire(messageRetour,strlen(messageRetour));
            }

            free(message);
            message=NULL;
        }

        TerminaisonClient();

    }//fin while(1)

    return 0;
}
