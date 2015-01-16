#include <stdio.h>
#include <stdlib.h>
#include "serveur.h"
#include <string.h>
#include <time.h>

int main()
{

    char *message = NULL;
    Initialisation();

    while(1)
    {
        int fini = 0;

        AttenteClient();

        message = Reception();

        printf("J'ai recu %s\n",message);

        if(message!=NULL)
        {
            if(!strncmp(message,"quitter",7)){
                fini=1;
            }
            //   menu();
            //free(message);
            // message=NULL;
        }

        if(fini){
            TerminaisonClient();
        }
    }

    return 0;
}
