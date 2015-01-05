#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"

int main()
{
    char *authentification=NULL;
    char *message=NULL;
    int continuer=1;
    char msgConnexion[255];
    msgConnexion[0]='\0'; // on initialise le premier caractère de msg connexion à \0

   /* if(Initialisation("localhost") != 1)
    {
        printf("Erreur d'initialisation\n");
        return 1;
    }
*/
    do
    {
        menuConnex(msgConnexion);
       // Emission(msgConnexion);
//        Reception(msg);
//        menu();
        continuer=strncmp(msgConnexion,"quitter",7);
    }
    while(continuer);
   // EmissionBinaire("quitter",7);
    Terminaison();

    return EXIT_SUCCESS;
}


/*
 faire

 msgConnexion<­MenuConnex(); //on recupère l'identifiant de connexions et le
mode de passe dans une chaîne de caractères

 émission(msgConnexion); // on envoie les informations au serveur

 reception(msg);// msg retour serveur

 si msg="identif OK" alors l'utilisateur peut accéder au menu

 Menu();  dans la fonction menu l'utilisateur interagira avec le serveur

 FinSI

 TQ(quitter!=0 && msg!="fin de connexion");
    Sinon a

*/

