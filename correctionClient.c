#include <stdio.h>
#include <stdlib.h>
#include "client.h"

int main() {
	char *message;

	InitialisationAvecService("www.irit.fr", "http");

	Emission("GET / HTTP/1.0\r\n");
	Emission("Host: www.irit.fr\r\n");
        Emission("\r\n");

	do {
		message = Reception();
		if(message != NULL) {
			printf("J'ai recu: %s", message);
			free(message);
		}
	} while(message != NULL);

	Terminaison();

	return 0;
}




