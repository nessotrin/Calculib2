#ifndef _CALCULIB_NETWORK_SOCKET_H_
#define _CALCULIB_NETWORK_SOCKET_H_

#include "CalculibNetworkSocket.h"

#include "CalculibSocket.h"

#include <SFML/Network.hpp>

#define CALCULIB_NETWORKSOCKET_SEND_BUFFER_SIZE 1024
#define CALCULIB_NETWORKSOCKET_RECEIVE_BUFFER_SIZE 1024

enum CALCULIB_NETWORKSOCKET_TRYPROTOCOL_RESULT
{
    CALCULIB_NETWORKSOCKET_TRYPROTOCOL_CONNECTED = 0,
    CALCULIB_NETWORKSOCKET_TRYPROTOCOL_TRYANOTHER,
    CALCULIB_NETWORKSOCKET_TRYPROTOCOL_FAILURE
};

class CalculibNetworkSocket : public CalculibSocket
{
public:
    CalculibNetworkSocket();
    ~CalculibNetworkSocket();
    void allocate();
    int getMaxSendBufferSize();
    void tick();
    bool connect();
    void disconnect();
    
private:

    void finalizeConnection();
    
    CALCULIB_NETWORKSOCKET_TRYPROTOCOL_RESULT tryCalculib1Connect();
    CALCULIB_NETWORKSOCKET_TRYPROTOCOL_RESULT tryCalculib2Connect();
    sf::Socket::Status receiveWithTimeout(char * ptr, int max, std::size_t * received, int timeoutMS);

    
    sf::TcpSocket SFMLSocket;

};

#endif //_CALCULIB_NETWORK_SOCKET_H_
