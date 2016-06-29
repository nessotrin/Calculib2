#ifndef _CALCULIB_SOCKET_H_
#define _CALCULIB_SOCKET_H_

#include "CalculibBuffer.h"

class CalculibSocket
{
public:
    CalculibBuffer * getReceiveBuffer();
    CalculibBuffer * getSendBuffer();
    bool isConnected();
    
    virtual int getMaxSendBufferSize() = 0;
    virtual void tick() = 0;
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    
protected:
    CalculibBuffer receiveBuffer;
    CalculibBuffer sendBuffer;
    bool connected = false;

};

#endif //_CALCULIB_SOCKET_H_