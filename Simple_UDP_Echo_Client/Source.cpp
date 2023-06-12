//------------------------------
// Lab01. Exercise4
// Simple UDP/IP Echo Client
// Filename : simple_udp_echo_cient.cpp

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include <winsock2.h>
#include <stdio.h>

/// Link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN    1024
#define SERVER_IP_ADDRESS "127.0.0.1"
#define PORT_NUMBER       9876

int main(void)
{
    WSADATA     wsaData;
    SOCKET      RecvSocket;
    SOCKADDR_IN RecvAddr;
    SOCKADDR_IN SenderAddr;
    char        MessageBuf[DEFAULT_BUFLEN];
    int         MessageSize;
    int         iResult = 0;
    int         SenderAddrSize = sizeof(SenderAddr);
    int         i;

    ///-----------------------------------------------
    /// 1. Initialize Winsock
    iResult = WSAStartup( MAKEWORD(2, 2), &wsaData );
    if( NO_ERROR != iResult )
    {
        printf("WSAStartup failed with error %d\n", iResult);
        return 1;
    }

    ///-----------------------------------------------
    /// 2. Create a receiver socket to receive datagrams
    RecvSocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    if( INVALID_SOCKET == RecvSocket )
    {
        printf("socket failed with error %d\n", WSAGetLastError());
        return 1;
    }

    ///---------------------------------------------
    /// Set up the RecvAddr structure with the IP address of
    /// the receiver (in this example case "192.168.1.1")
    /// and the specified port number.
    RecvAddr.sin_family      = AF_INET;
    RecvAddr.sin_port        = htons(PORT_NUMBER);
    RecvAddr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);

    while(1)
    {
        printf("Enter messages : ");

        for(i = 0; i < (DEFAULT_BUFLEN - 1); i++)
        {
            MessageBuf[i] = getchar();
            if(MessageBuf[i] == '\n')
            {
                MessageBuf[i++] = '\0';
                break;
            }
        }
        MessageSize = i;

        ///---------------------------------------------
        /// Send a datagram to the receiver
        printf("Sending a datagram to the receiver...\n");
        iResult = sendto( RecvSocket, MessageBuf, MessageSize, 0,
            (SOCKADDR *)& RecvAddr, sizeof(RecvAddr) );
        if( SOCKET_ERROR == iResult )
        {
            printf("sendto failed with error: %d\n", WSAGetLastError());
            break;
        }

        ///-----------------------------------------------
        /// Call the recvfrom function to receive datagrams
        /// on the bound socket.
        printf("Receiving datagrams...\n");
        iResult = recvfrom( RecvSocket, MessageBuf, DEFAULT_BUFLEN, 0,
            (SOCKADDR *)& SenderAddr, &SenderAddrSize );
        if( SOCKET_ERROR == iResult )
        {
            printf("recvfrom failed with error %d\n", WSAGetLastError());
            break;
        }
        else if( 0 == iResult )
        {
            printf("Socket closed");
            break;
        }

        printf("Read bytes = %d, Message = [%s], from %d.%d.%d.%d:%d\n",
            iResult, MessageBuf,
            SenderAddr.sin_addr.S_un.S_un_b.s_b1,
            SenderAddr.sin_addr.S_un.S_un_b.s_b2,
            SenderAddr.sin_addr.S_un.S_un_b.s_b3,
            SenderAddr.sin_addr.S_un.S_un_b.s_b4,
            ntohs(SenderAddr.sin_port));
    }

    ///-----------------------------------------------
    /// Close the socket when finished receiving datagrams
    printf("Finished receiving. Closing socket.\n");
    iResult = closesocket( RecvSocket );
    if( SOCKET_ERROR == iResult )
    {
        printf("closesocket failed with error %d\n", WSAGetLastError());
        return 1;
    }

    ///-----------------------------------------------
    /// Clean up and exit.
    printf("Exiting.\n");
    WSACleanup();
    return 0;
}