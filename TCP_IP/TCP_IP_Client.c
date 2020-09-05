#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>
#include <stdbool.h> 
#include <math.h>
#include <time.h>
#include <TCP_IP_Client.h>

typedef struct TCP_IP_Client TCP_IP_Client;
//========================================
// Create a Socket for server communication
short SocketCreate(TCP_IP_Client *client)
{
    client->hSocket = socket(AF_INET, SOCK_STREAM, 0);
    return client->hSocket;
}

// Try to connect with server
int SocketConnect(TCP_IP_Client *client)
{
    int iRetval=-1;
    int ServerPort = client->port; // 25010 - 25012
    struct sockaddr_in remote = {0};
    remote.sin_addr.s_addr = inet_addr(client->ipAddress); //"169.254.183.22"
    remote.sin_family = AF_INET;
    remote.sin_port = htons(ServerPort);
    iRetval = connect(client->hSocket,(struct sockaddr *)&remote,sizeof(struct sockaddr_in));
    return iRetval;
}

// Send the data to the server and set the timeout of 20 seconds
int SocketSend(TCP_IP_Client *client)
{
    int shortRetval = -1;
    struct timeval tv;
    tv.tv_sec = client->RECEIVE_TIMEOUT;
    tv.tv_usec = 0;
    if(setsockopt(client->hSocket,SOL_SOCKET,SO_SNDTIMEO,(char *)&tv,sizeof(tv)) < 0)
    {
        printf("Time Out\n");
        return -1;
    }
    shortRetval = send(client->hSocket, client->toSendData, strlen(client->toSendData), 0);
    return shortRetval;
}

// Receive the data from the server
int SocketReceive(TCP_IP_Client *client)
{
    int shortRetval = -1;
    struct timeval tv;
    tv.tv_sec = client->RECEIVE_TIMEOUT; // Timeout duration, in seconds
    tv.tv_usec = 0;
    if(setsockopt(client->hSocket, SOL_SOCKET, SO_RCVTIMEO,(char *)&tv,sizeof(tv)) < 0)
    {
        printf("Time Out\n");
        return -1;
    }
    shortRetval = recv(client->hSocket, client->receivedData, client->RECEIVED_DATA_LENGTH, 0);
    if(client->PRINT_RECEIVED_DATA)
    {
        printf("%s\n\n", client->receivedData);
    }
    return shortRetval;
}

// Terminate the connection
void SocketTerminate(TCP_IP_Client *client)
{
    close(client->hSocket);
    shutdown(client->hSocket,0);
    shutdown(client->hSocket,1);
    shutdown(client->hSocket,2);
}