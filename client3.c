#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "socket_name.h"
#include <string.h>
FILE* fp;
int client_sockfd;
char msg[255] = "Client 3 user message: ";
char input[255];
void my_handler(sig_t s){
    if (fclose(fp) == 0 )
    {
        printf("\nlog is successfully saved. Now exiting... Bye \n");
        exit(0); 
    }
    else 
    {
        printf("Error on fclose()");
    }
}
void readInput()
{
    printf("\nWrite a message to send to the server :\n");
    fprintf(fp,"\nWrite a message to send to the server :\n");
    fgets (input, 255, stdin);
    /* Remove trailing newline, if there. */
    if ((strlen(input)>0) && (input[strlen (input) - 1] == '\n'))
            input[strlen (input) - 1] = '\0';

    fprintf(fp , "%s\n" , input);
    strcat(msg, input);
}
int main()
{
    fp = fopen("client3.txt" , "w+");
    signal (SIGINT,my_handler);
   
    int len;
    struct sockaddr_un address;
    int result;
   
    while (1){
        readInput();
        client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        address.sun_family = AF_UNIX;
        strcpy(address.sun_path, SOCKET_NAMES[SOCKET3]);
        len = sizeof(address);

        // Connect our socket to the server's socket.
        result = connect(client_sockfd, (struct sockaddr *)&address, len);
        if (result == -1) {
            perror("Error: client3");
            exit(1);
        }
        write(client_sockfd, msg , sizeof(msg) );
        // Read what the server sent back.
        char response[255];
        read(client_sockfd, &response, sizeof(response));
        printf("Server response received: '%s'\n", response);
        fprintf(fp,"Server response received: '%s'\n", response);
        strcpy(msg , "Client 3 user message: ");
        strcpy(input, "");
   
        close(client_sockfd);
    
     }
    exit(0);
}