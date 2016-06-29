#include "CalculibNetworkSocket.h"

#include <cstring>

CalculibNetworkSocket::CalculibNetworkSocket()
{
    sendBuffer.buffer = (unsigned char *) malloc(CALCULIB_NETWORKSOCKET_SEND_BUFFER_SIZE);
    receiveBuffer.buffer = (unsigned char *) malloc(CALCULIB_NETWORKSOCKET_RECEIVE_BUFFER_SIZE);
    if(sendBuffer.buffer == NULL || receiveBuffer.buffer == NULL)
    {
        printf("Socket memory alloc failed !\n");
    }
    sendBuffer.size = 0;
    receiveBuffer.size = 0;
    printf("Alloc'ed\n");
}
CalculibNetworkSocket::~CalculibNetworkSocket()
{
    printf("Destroyed !\n");
    free(sendBuffer.buffer);
    free(receiveBuffer.buffer);
}

int CalculibNetworkSocket::getMaxSendBufferSize()
{
    return CALCULIB_NETWORKSOCKET_SEND_BUFFER_SIZE;
}

void CalculibNetworkSocket::tick()
{
    //receive
    std::size_t received;
    sf::Socket::Status resultReceive = SFMLSocket.receive(receiveBuffer.buffer+receiveBuffer.size,
                                               CALCULIB_NETWORKSOCKET_RECEIVE_BUFFER_SIZE-receiveBuffer.size,
                                               received);
    if(resultReceive == sf::Socket::Done)
    {
        receiveBuffer.size += received;
    }

    //send
    sf::Socket::Status resultSend = sf::Socket::Done;
    if(sendBuffer.size > 0)
    {
        std::size_t sent;
        resultSend = SFMLSocket.send(sendBuffer.buffer, sendBuffer.size, sent);
        if(resultSend == sf::Socket::Done)
        {
            memmove(sendBuffer.buffer,sendBuffer.buffer+sent,sendBuffer.size-sent);
            sendBuffer.size -= sent;
        }
        else
        {
            printf("Send failure !\n");
        }
    }

    
    if(resultReceive == sf::Socket::Disconnected || resultSend == sf::Socket::Disconnected)
    {
        printf("Disconnected !\n");
        connected = false;
    }
    
}

void CalculibNetworkSocket::finalizeConnection()
{   
    //final setup on the socket
    SFMLSocket.setBlocking(0);

    connected = true;

    printf("Network socket connected & ready ...\n");
}

#include "Calculib.h"

bool CalculibNetworkSocket::connect()
{
    if(SFMLSocket.connect("127.0.0.1", 63000, sf::milliseconds(5000)) == sf::Socket::Done)
    {
        char data[17];
        std::size_t received;
        sf::Socket::Status status = SFMLSocket.receive(data, 17, received);
        if(status == sf::Socket::Error)
        {
            printf("Connection is broken !\n");
        }
        else if(status == sf::Socket::Done && strcmp(data,"SERVER_EXIT_FULL") == 0)
        {
            printf("Server is full !\n");
        }
        else if(status == sf::Socket::Done && strcmp(data,"SERVER_GOOD_DONE") == 0)
        {
            printf("Calculib Protocol V1 detected ...\n");
            finalizeConnection();
            return true;
        }
        else
        {
            printf("Protocol-free server detected ...\n");
            finalizeConnection();
            return true;
        }
        
        SFMLSocket.disconnect();
    }
    
    printf("Network socket couldn't connect !\n");
    
    return false;
}

void CalculibNetworkSocket::disconnect()
{
    connected = false;
    SFMLSocket.disconnect();
}