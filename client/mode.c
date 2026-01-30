#include "main.h"

void change_mode(tftp_client *client)
{
    int option;
    printf("\n(1) Octet\n(2) Netascii\nEnter Option :");
    scanf(" %d", &option);

    if (option == 1)
    {
        strcpy(client->mode, "octet");
        printf("INFO : Mode Changed to \"Octet\" Successfully\n");
        return;
    }
    else if (option == 2)
    {
        strcpy(client->mode, "netascii");
        printf("INFO : Mode Changed to \"Netascii\" Successfully\n");
        return;
    }

    printf("ERROR :Changing Mode Failed\n");
    return;
}