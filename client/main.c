#include "main.h"
/*server ip : 127.0.0.1*/

int main()
{
    tftp_client client;
    init_client(&client);

    while (1)
    {
        int option;
        printf(GRAYBACKGROUND RED BOLD "\t\t\t   TFTP CLIENT APPLICATION   " COLOREND COLOREND DEFAULT "\n");
        printf("(1) CONNECT\n(2) PUT FILE\n(3) GET FILE\n(4) MODE\n(5) HELP\n(6) EXIT\n\n ENTER OPTION : ");
        scanf(" %d", &option);
        char ch;
        while ((ch = getchar()) != '\n');

        switch (option)
        {
        case 1:
        {
            printf("\n" MAGENTA BOLD "\t\t\tCONNECT TO SERVER" COLOREND "\n");
            connect_server(&client);
        }
        break;

        case 2:
        {
            printf("\n" CYAN BOLD "\t\t\tUPLOAD FILE" COLOREND "\n");
            send_file_to_server(&client);
        }
        break;

        case 3:
        {
            printf("\n" GRAY BOLD "\t\t\tDOWNLOAD FILE" COLOREND "\n");
            get_file_from_server(&client);
        }
        break;
        
        case 4:
        {
            printf(BLUE BOLD "\t\t\t MODE__CHANGE\n" COLOREND);
            change_mode(&client);
        }
        break;

        case 5:
        {
            printf("\n" YELLOW BOLD "\t\t---------- HELP ----------" COLOREND "\n");
            help();
        }
        break;

        case 6:
        {
            printf(RED BOLD "EXITING THE PROGRAM..." COLOREND "\n");
            end(&client);
            exit(0);
        }

        default:
            printf(RED BOLD "\t\t\tERROR : INVALID INPUT" COLOREND "\n\n");
        }
    }

    return 0;
}