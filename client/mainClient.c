#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"

int main()
{

     if(Initialisation("localhost") != 1)
     {
         printf("Erreur d'initialisation\n");
         return 1;
     }


    menuConnex();

    Terminaison();

    return EXIT_SUCCESS;
}
