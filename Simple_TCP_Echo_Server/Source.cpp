//------------------------------
// Lab02.Exercise2
// Simple TCP/IP Echo Server
// Filename : simple_tcp_echo_server.cpp

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <stdio.h>

#include <string>

/// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT   9876

int main(void)
{
    WSADATA     wsaData;                           // Variable to initialize Winsock
    SOCKET      ServerSocket = INVALID_SOCKET;  // Server socket for accept client
    SOCKET      ConnectedSocket = INVALID_SOCKET;  // Connected socket from client //Socket descriptor
    sockaddr_in ServerAddress;                     // Server address
    sockaddr_in ClientAddress;                     // Client address
    char        MessageBuffer[DEFAULT_BUFLEN];     // Message buffer to recv from socket
    int         ClientAddressLen;                  // Length for client sockaddr_in.
    int         Result = 0;                        // Return value for function result

                                                   ///----------------------
                                                   /// 1. Initiate use of the Winsock Library by a process
    Result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(NO_ERROR != Result)
    {
        printf("Error at WSAStartup()\n");
        return 1;
    } else
    {
        printf("WSAStartup success.\n");
    }

    ///----------------------
    /// 2. Create a new socket for application 
    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(INVALID_SOCKET == ServerSocket)
    {
        printf("socket function failed with error: %u\n", WSAGetLastError());
        WSACleanup();
        return 1;
    } else
    {
        printf("socket creation success.\n");
    }

    ///----------------------
    /// 3-1. The sockaddr_in structure specifies the address family,
    ///      IP address, and port for the socket that is being bound.
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(DEFAULT_PORT);
    ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    ///----------------------
    /// 3-2. Bind the socket with server address & port number
    if(SOCKET_ERROR == bind(ServerSocket, (SOCKADDR*)&ServerAddress,
        sizeof(ServerAddress)))
    {
        printf("bind failed with error %u\n", WSAGetLastError());
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    } else
    {
        printf("bind returned success\n");
    }

    ///----------------------
    /// 4. Places a socket in a state in which it is listening for an incoming
    ///    connection.
    if(SOCKET_ERROR == listen(ServerSocket, SOMAXCONN))
    {
        printf("listen function failed with error: %d\n", WSAGetLastError());
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    }

    printf("Waiting for client to connect...\n");
    ///----------------------
    /// 5. Permits an incoming connection attempt on a socket


    char Welcome_Message_Buffer[] = "Welcome to Simple TCP/IP Echo Server";
    const size_t Welcome_Message_Buffer_Length = sizeof(Welcome_Message_Buffer) / sizeof(Welcome_Message_Buffer[0]);

    ///----------------------
    /// 6. Send & receive the data on a connected socket
    while(1)
    {
        ClientAddressLen = sizeof(ClientAddress);
        ConnectedSocket = accept(ServerSocket, (struct sockaddr*)&ClientAddress, //??
            &ClientAddressLen);
        if(INVALID_SOCKET == ConnectedSocket)
        {
            printf("accept failed with error: %ld\n", WSAGetLastError());
            closesocket(ServerSocket);
            WSACleanup();
            return 1;
        } else{
            printf("Client connected. IP Address : %d.%d.%d.%d, Port Number :%d\n",
                //--------------------
                ClientAddress.sin_addr.S_un.S_un_b.s_b1,
                ClientAddress.sin_addr.S_un.S_un_b.s_b2,
                ClientAddress.sin_addr.S_un.S_un_b.s_b3,
                ClientAddress.sin_addr.S_un.S_un_b.s_b4,
                ntohs(ClientAddress.sin_port));
            if(SOCKET_ERROR == (Result = send(ConnectedSocket, Welcome_Message_Buffer, Welcome_Message_Buffer_Length, 0)))
            {
                printf("Send failed: %d\n", WSAGetLastError());
                closesocket(ConnectedSocket);
                WSACleanup();
                return 1;
            }
        }

        const char msg1st[] = "<RTN OK>";
        const size_t msg1stLen = sizeof(msg1st) / sizeof(msg1st[0]);
        const char msg2nd[] = "<RTN PRINT OK>";
        const size_t msg2ndLen = sizeof(msg2nd) / sizeof(msg2nd[0]);

        memset(MessageBuffer, '\0', DEFAULT_BUFLEN);

        Result = recv(ConnectedSocket, MessageBuffer, DEFAULT_BUFLEN, 0);

        //const int MessageBufferLen = 2;
        //Result = recv(ConnectedSocket, MessageBuffer, MessageBufferLen, 0);

        if(Result > 0){
            printf("Bytes received  : %d\n", Result);

            int msgLen = 0;
            for(int i = 0; i < DEFAULT_BUFLEN; ++i){
                if(MessageBuffer[i] != '\0'){
                    ++msgLen;
                } else{
                    break;
                }
            }
            
            if(MessageBuffer[0] == '<'
                && MessageBuffer[1] == 'M'
                && MessageBuffer[2] == 'S'
                && MessageBuffer[3] == 'G'
                && MessageBuffer[4] == ' '
                && MessageBuffer[msgLen - 1] == '>'
            ){
                printf("Buffer received : %s\n", std::string(MessageBuffer).substr(5, msgLen - 6).c_str());

                if(SOCKET_ERROR == (Result = send(ConnectedSocket, msg1st, msg1stLen, 0))){
                    printf("Send failed: %d\n", WSAGetLastError());
                    closesocket(ConnectedSocket);
                    WSACleanup();
                    return 1;
                }
            } else{
                printf("Buffer received : %s\n", MessageBuffer);

                if(SOCKET_ERROR == (Result = send(ConnectedSocket, msg2nd, msg2ndLen, 0))){
                    printf("Send failed: %d\n", WSAGetLastError());
                    closesocket(ConnectedSocket);
                    WSACleanup();
                    return 1;
                }
            }
        } else if(Result == 0){
            printf("Connection closed\n");
            break;
        } else if(Result == -1){ //Client dc
            continue;
        } else{
            printf("Recv failed: %d\n", WSAGetLastError());
            puts(std::to_string(Result).c_str());
            break;
        }

        /// Echo same message to client
        Result = send(ConnectedSocket, MessageBuffer, Result, 0);
        if(SOCKET_ERROR == Result){
            printf("Send failed: %d\n", WSAGetLastError());
            break;
        }
        printf("Bytes sent : %d\n", Result);
    }

    ///----------------------
    /// 7. Closes an existing socket
    closesocket(ConnectedSocket);
    closesocket(ServerSocket);

    ///----------------------
    /// 8. Terminate use of the Winsock Library
    WSACleanup();
    return 0;
}