#ifndef _CALCULIB_PROTOCOL_H_
#define _CALCULIB_PROTOCOL_H_

#include "CalculibBuffer.h"
#include "CalculibSocket.h"


enum CALCULIB_PACKET_READ_RESULT{CALCULIB_PACKET_READ_OK = 0,
                                 CALCULIB_PACKET_READ_NOT_COMPLETE,
                                 CALCULIB_PACKET_READ_ERROR};

class CalculibProtocol
{
private:
    bool connected = false;
    
    CalculibSocket * socket;
    
    void removeFromSocket(int count);
    
    bool buildPacket(CalculibBuffer * packet, unsigned char type, CalculibBuffer * data);
    CALCULIB_PACKET_READ_RESULT readPacket(CalculibBuffer * packet, unsigned char * type, CalculibBuffer * data);

    unsigned char calculateChecksum(CalculibBuffer * packet);
    
    bool sendTypedData(unsigned char type, CalculibBuffer * toSend);
    void treatSpecialPacket(unsigned char type, CalculibBuffer * buffer);
    CALCULIB_PACKET_READ_RESULT receiveIncommingData(unsigned char * type, CalculibBuffer * buffer);
    void readIncommingData(CalculibBuffer * toFill, int maxSize, bool stopOnData);

    
public:

    void connect();
    void disconnect();
    
    void tick();
    
    bool sendData(CalculibBuffer * toSend);
    void readData(CalculibBuffer * toFill, int maxSize);
    
    void setSocket(CalculibSocket * newSocket);
    bool isConnected();
};

#endif //_CALCULIB_PROTOCOL_H_