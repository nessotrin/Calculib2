#ifndef _CALCULIB_NETWORK_SOCKET_H_
#define _CALCULIB_NETWORK_SOCKET_H_

#include "CalculibNetworkSocket.h"

#include "CalculibSocket.h"

#include <SFML/Network.hpp>

#define CALCULIB_NETWORKSOCKET_SEND_BUFFER_SIZE 1024
#define CALCULIB_NETWORKSOCKET_RECEIVE_BUFFER_SIZE 1024

class CalculibNetworkSocket : public CalculibSocket
{
public:
    CalculibNetworkSocket();
    ~CalculibNetworkSocket();
    int getMaxSendBufferSize();
    void tick();
    bool connect();
    void disconnect();
    
private:

    void finalizeConnection();
    
    sf::TcpSocket SFMLSocket;

};

#endif //_CALCULIB_NETWORK_SOCKET_H_
