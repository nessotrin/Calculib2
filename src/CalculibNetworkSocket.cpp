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
    SFMLSocket.setBlocking(false);

    connected = true;

    printf("Network socket connected & ready ...\n");
}

#include "Calculib.h"

#include "CalculibTime.h"//Sleep

sf::Socket::Status CalculibNetworkSocket::receiveWithTimeout(char * ptr, int max, std::size_t * received, int timeoutMS)
{
    bool wasBlocking = SFMLSocket.isBlocking();
    SFMLSocket.setBlocking(false);
    
    sf::Socket::Status status;
    for(int i  = 0 ; i < timeoutMS/100 ; i++)
    {
        status = SFMLSocket.receive(ptr, max, *received);
        if(status != sf::Socket::NotReady)
        {
            break;
        }
        Sleep(100);
    }
    
    SFMLSocket.setBlocking(wasBlocking); //put it back
    
    return status;
}

CALCULIB_NETWORKSOCKET_TRYPROTOCOL_RESULT CalculibNetworkSocket::tryCalculib1Connect()
{
    char data[17];
    std::size_t received;
    
    sf::Socket::Status status = receiveWithTimeout(data,17,&received,200); // should answer quicly
    
    if(status == sf::Socket::Error || status == sf::Socket::Disconnected)
    {
        printf("Connection is broken !\n");
        return CALCULIB_NETWORKSOCKET_TRYPROTOCOL_FAILURE;
    }
    else if(status == sf::Socket::Done && strcmp(data,"SERVER_EXIT_FULL") == 0)
    {
        printf("Server is full !\n");
        return CALCULIB_NETWORKSOCKET_TRYPROTOCOL_FAILURE;
    }
    else if(status == sf::Socket::Done && strcmp(data,"SERVER_GOOD_DONE") == 0)
    {
        printf("Calculib Protocol V1 detected ...\n");
        return CALCULIB_NETWORKSOCKET_TRYPROTOCOL_CONNECTED;
    }
    else
    {
        printf("Calculib Protocol V1 not detected ...\n");
        return CALCULIB_NETWORKSOCKET_TRYPROTOCOL_TRYANOTHER;
    }
}

CALCULIB_NETWORKSOCKET_TRYPROTOCOL_RESULT CalculibNetworkSocket::tryCalculib2Connect()
{
    std::size_t transfered;
    sf::Socket::Status status = SFMLSocket.send("CALCULIB2_CONNECT",17,transfered);
    if(status == sf::Socket::Done)
    {
        char data[25];
        sf::Socket::Status receiveStatus = receiveWithTimeout(data, 25, &transfered,200); //fast answer expected
        if(receiveStatus == sf::Socket::Error || receiveStatus == sf::Socket::Disconnected)
        {
            printf("Connection is broken !\n");
            return CALCULIB_NETWORKSOCKET_TRYPROTOCOL_FAILURE;
        }
        else if(receiveStatus == sf::Socket::Done && transfered == 21 && memcmp(data,"CALCULIB2_SERVER_FULL",21) == 0)
        {
            printf("Server is full !\n");
            return CALCULIB_NETWORKSOCKET_TRYPROTOCOL_FAILURE;
        }
        else if(receiveStatus == sf::Socket::Done && transfered == 18 && memcmp(data,"CALCULIB2_ACCEPTED",18) == 0)
        {
            printf("Calculib Protocol V2 detected ...\n");
            return CALCULIB_NETWORKSOCKET_TRYPROTOCOL_CONNECTED;
        }
        else
        {
            printf("Calculib Protocol V2 not detected ... (%d %d %d)\n",transfered,receiveStatus,sf::Socket::Done);
            return CALCULIB_NETWORKSOCKET_TRYPROTOCOL_TRYANOTHER;
        }
    }
    else
    {
            printf("Connection is broken !\n");
            return CALCULIB_NETWORKSOCKET_TRYPROTOCOL_FAILURE;
    }
}


bool CalculibNetworkSocket::connect()
{
    for(int protocolId = 0 ; protocolId < 2 ; protocolId++)
    {
        printf("Attempting connection ...\n");
        if(SFMLSocket.connect("89.157.210.47", 63000, sf::milliseconds(5000)) == sf::Socket::Done)
        {
            printf("Socket connected !\n");
            CALCULIB_NETWORKSOCKET_TRYPROTOCOL_RESULT result;
            switch(protocolId)
            {
                case 0:
                    result = tryCalculib1Connect();
                    break;
                case 1:
                    result = tryCalculib2Connect();
                    break;
            };
            
            if(result == CALCULIB_NETWORKSOCKET_TRYPROTOCOL_CONNECTED)
            {
                finalizeConnection();
                return false;
            }
            else if(result == CALCULIB_NETWORKSOCKET_TRYPROTOCOL_FAILURE)
            {
                break;
            }

            SFMLSocket.disconnect();
        }
        else
        {
            printf("Failed to connect the TCP socket ...\n");
        }
    }

    
    printf("Network socket couldn't connect !\n");
    
    return true;
}

void CalculibNetworkSocket::disconnect()
{
    connected = false;
    SFMLSocket.disconnect();
}