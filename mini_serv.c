#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>


void fatal_error()
{
    write(2, "Fatal error\n", 12);
    exit(1);
}

void send_to_all(char *mess, int server, int client, int fd_max)
{
    for(int fd = 0; fd <= fd_max; fd++)
    {
        if (fd != server && fd != client)
            send(fd, mess, strlen(mess), 0);
    }
}

int main(int argc, char**argv)
{
    int server, client, bytes_read, fd_max, max_client[65565] = {0}, id = 0;
    fd_set all_fds, temp_fds;
    struct sockaddr_in servaddr;
    char mess_to_send[400000];
    char mess_recv[400000];

    if(argc != 2)
    {
        write(2, "Wrong number of arguments\n", 26);
        exit(1);
    }
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1)
        fatal_error();

    bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433);
	servaddr.sin_port = htons(atoi(argv[1]));

    if ((bind(server, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
        fatal_error();
    
    if (listen(server, 10) != 0)
        fatal_error();

    FD_ZERO(&all_fds);
    FD_ZERO(&temp_fds);
    FD_SET(server, &all_fds);
    fd_max = server;

    while(1)
    {
        temp_fds = all_fds;
        if (select(fd_max + 1, &temp_fds, NULL, NULL, NULL) < 0)
            fatal_error();
        for(int client_connected = 0; client_connected <= fd_max; client_connected++)
        {
            if(FD_ISSET(client_connected, &temp_fds) != 1)
                continue;
            if (client_connected == server)
            {
                if ((client = accept(server, NULL, NULL)) < 0)
                    fatal_error();
                if (client > fd_max)
                    fd_max = client;
                max_client[client] = id++;
                FD_SET(client, &all_fds);
                bzero(mess_to_send, 400000);
                sprintf(mess_to_send, "server: client %d just arrived\n", max_client[client]);
                send_to_all(mess_to_send, server, client, fd_max);
            }
            else
            {
                bzero(mess_to_send, 400000);
                bzero(mess_recv, 400000);
                bytes_read = 1;
                while (bytes_read == 1 && ((!mess_recv[0] || mess_recv[strlen(mess_recv) - 1] != '\n')))
                    bytes_read = recv(client_connected, &mess_recv[strlen(mess_recv)], 1, 0);
                if (bytes_read == 0)
                {
                    sprintf(mess_to_send, "server: client %d just left\n", max_client[client_connected]);
                    send_to_all(mess_to_send, server, client_connected, fd_max);
                    FD_CLR(client_connected, &all_fds);
                    close(client_connected);
                }
                else
                {
                    sprintf(mess_to_send, "client %d: %s" ,max_client[client_connected], mess_recv);
                    send_to_all(mess_to_send, server, client_connected, fd_max);
                }
            }
        }
    }
    return (0);
}
