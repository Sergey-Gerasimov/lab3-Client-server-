#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

void* handle_client(void* arg)
{
    long socket = (long) arg;
    char buf[1024];
    FILE* file;
    int bytes_read;

    if (recv(socket, buf, 1024, 0) < 0) {
        puts("Error: receive failed");
    }

    file = fopen(buf, "rb");

    if (file == 0) {
        buf[0] = '\0';
        puts("Error: could not open input file");
        send(socket, buf, 0, 0);
    } else {
        while ((bytes_read = fread(buf, 1, 1024, file)) > 0) {
            if (send(socket, buf, bytes_read, 0) < 0) {
                puts("Error: send failed");
                break;
            }
        }

        fclose(file);
    }

    close(socket);
    puts("Connection closed");
}
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    pthread_t thread;
        
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    puts("Waiting for incoming connections...");
    
    //accept connection from an incoming client
    while (1) {
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);

        if (client_sock < 0) {
            puts("Error: accept failed");
            continue;
        }

        puts("Connection accepted");
     
        //Receive a message from client
	#if PROC
        switch(fork()) {
            case -1:
                puts("Error: could not create a process");
                close(client_sock);
                continue;

            case 0:
                handle_client((void*)(long) client_sock);
                _exit(0);

            default:
                continue;
        }
	#else
        if (pthread_create(&thread, NULL, handle_client, (void*)(long)client_sock)) {
            puts("Error: could not create a thread");
            close(client_sock);
            continue;
        }
	#endif
    }

    close(socket_desc);
    return 0;
}
