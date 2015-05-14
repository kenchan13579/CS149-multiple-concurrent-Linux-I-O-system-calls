#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>
#include "socket_name.h"


FILE* fp;
void my_handler(sig_t s ){
    if (fclose(fp) == 0 )
    {
        printf("log is successfully saved. Now exiting... Bye \n");
        exit(0); 
    }
    else 
    {
        printf("Error on fclose()");
    }
}
int main()
{
    fp = fopen("server-output.txt" , "w+");
    signal (SIGINT,my_handler);
    int server_sockfd[MAX_SOCKETS], client_sockfd;
    int server_len, client_len, result;
    struct sockaddr_un server_address;
    struct sockaddr_un client_address;
    fd_set inputs, inputfds;  // sets of file descriptors
    // Remove any old socket and create an unnamed socket for the server.
    int i;
    FD_ZERO(&inputs);    // initialize inputs to the empty set
   // set file descriptor 0 (stdin)
 
    for ( i = 0 ; i < MAX_SOCKETS ; i++)
    {
        unlink(SOCKET_NAMES[i]);
        server_sockfd[i] = socket(AF_UNIX, SOCK_STREAM, 0); 
    
        server_address.sun_family = AF_UNIX;
        strcpy(server_address.sun_path, SOCKET_NAMES[i]);

        server_len = sizeof(server_address);
        bind(server_sockfd[i], (struct sockaddr *) &server_address, server_len);
        FD_SET(server_sockfd[i], &inputs); 
        // Create a connection queue and wait for clients.
        listen(server_sockfd[i], 5);
    }
    printf("Hello World, Server waiting...\n");
    fprintf(fp,"Hello World, Server waiting...\n");
    for (;;) {
       
        inputfds = inputs;
        result = select(FD_SETSIZE, &inputfds,NULL, NULL, NULL);
        
        if (result == 0)
        {
            printf("@");
            fflush(stdout);
            exit(1);
        }
        else if (result < 0)
        {
            perror("result");
            exit(1);
        }
        else
        {
             for ( i = 0 ; i < MAX_SOCKETS; i++){
                    if (FD_ISSET(server_sockfd[i], &inputfds)) {

                    //  Accept a socket connection.
                    client_len = sizeof(client_address);
                    client_sockfd = accept(server_sockfd[i],(struct sockaddr *) &client_address,&client_len);
                    char msg[255];
                    // Read a character from the socket.*
                    read(client_sockfd, &msg, sizeof(msg));
                    char writeback[255] ="Received from ";
                    strcat(writeback , SOCKET_NAMES[i]);
                    strcat(writeback , ": ");
                    strcat(writeback , msg);
                    write(client_sockfd , &writeback , 255 );
                    // Increment the character and write it back to the socket.
                    printf("Received from %s : '%s'\n", SOCKET_NAMES[i], msg);
                    fprintf(fp,"Received from %s : '%s'\n", SOCKET_NAMES[i], msg);
                    close(client_sockfd);
                    
                    // Close the socket.
                   
                    }
                   
                }
        }
    }
}
