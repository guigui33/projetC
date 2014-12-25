#include <stdio.h>
#include <stdlib.h>
#include "client.h"

int main()
{
    char *authentification=NULL;
    char *message=NULL;
    char *msgConnexion=NULL;
    size_t fin=0;

    if(Initialisation("localhost") != 1)
    {
        printf("Erreur d'initialisation\n");
        return 1;
    }

    do
    {
        menuConnex(msgConnexion);
        Emission(msgConnexion);
        Reception(msg);
        menu();
    }
    while(!fin);

    Terminaison();

    return EXIT_SUCCESS;
}


/*
 faire

 msgConnexion<­MenuConnex(); //on recupère l'identifiant de connexions et le
mode de passe dans une chaîne de caractères

 émission(msgConnexion); // on envoie les informations au serveur

 reception(msg);// msg retour serveur

 si msg="identif OK" alors /*l'utilisateur peut accéder au menu */

 /*Menu(); /* dans la fonction menu l'utilisateur interagira avec le serveur

 FinSI

 TQ(quitter!=0 && msg!="fin de connexion");
    Sinon a

*/

