#ifndef TCP_IP_CLIENT
#define TCP_IP_CLIENT

// Struct containing the TCP/IP client data
typedef struct TCP_IP_Client TCP_IP_Client;
struct TCP_IP_Client{
    int RECEIVED_DATA_LENGTH;
    //====
    int hSocket, read_size;
    char *toSendData;
    //====
    char *receivedData; // Array of the received data from the unity TCP/IP server
    bool PRINT_RECEIVED_DATA;
    int RECEIVE_TIMEOUT;
    //====
    char *ipAddress;
    int port;
    //====
    short (*m_SocketCreate)(TCP_IP_Client *client);
    int (*m_SocketConnect)(TCP_IP_Client *client);
    int (*m_SocketSend)(TCP_IP_Client *client);
    int (*m_SocketReceive)(TCP_IP_Client *client);
    void (*m_SocketTerminate)(TCP_IP_Client *client);
};
//==========
short SocketCreate(TCP_IP_Client *client);
int SocketConnect(TCP_IP_Client *client);
//==========
int SocketSend(TCP_IP_Client *client);
int SocketReceive(TCP_IP_Client *client);
void SocketTerminate(TCP_IP_Client *client);
//==========
#endif