#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>?
#include <sys/socket.h>>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(){
    int listen_fd, conn_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    //create socket
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set socket options
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){
        perror("setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)");
        exit(EXIT_FAILURE);
    }

    //bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if(bind(listen_fd, (struct sockaddr *) &address, sizeof(address))< 0){
        perror("[Server] bind() failed");
        exit(EXIT_FAILURE);
    }

    //listen for incoming connections
    if (listen(listen_fd, 3) < 0) {
        perror("[Server] listen() failed.");
        exit(EXIT_FAILURE);
    }

    printf("[Server] Running on port %d\n", PORT);

    //accept incoming connection 

    if ((conn_fd = accept(listen_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("[Server] accept() failed.");
        exit(EXIT_FAILURE);
    }

    //receive and process commands
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int nbytes = read(conn_fd, buffer, BUFFER_SIZE);
        if (nbytes <= 0) {
            perror("[Server] read() failed.");
            exit(EXIT_FAILURE);
        }
        printf("[Server] Received from client: %s\n", buffer);
        if (strcmp(buffer, "quit") == 0) {
            printf("[Server] Client chatter quitting...\n");
            break;
        }

        printf("[Server] Enter message: ");
        memset(buffer, 0, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer)-1] = '\0';
        if (strcmp(buffer, "quit") == 0) {
            printf("[Server] Quitting...\n");
            send(conn_fd, buffer, strlen(buffer), 0);
            break;
        }
        send(conn_fd, buffer, strlen(buffer), 0);
    }

     printf("[Server] Shutting down.\n");
    close(conn_fd);
    close(listen_fd);
    return EXIT_SUCCESS;

}