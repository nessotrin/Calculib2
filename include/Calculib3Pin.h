#ifndef _CALCULIB_3PIN_H_
#define _CALCULIB_3PIN_H_

#include "CalculibBuffer.h"
#include "CalculibProtocol.h"
#include "CalculibSocket.h"

#include <SFML/System.hpp>

class Calculib3Pin
{
public:

    static void tick3Pin();
    static void initCalculib3Pin();
    static void setSocket(CalculibSocket * newSocketForProtocol);
    static bool isProtocolConnected();
    static void disconnect();
    static void connect();
    
    static CalculibBuffer sendBuffer;
    static CalculibBuffer receiveBuffer;
    
    static sf::Mutex updateMutex;
private:

    static sf::Thread networkThread;
    static bool continueNetworkThread;
    static CalculibProtocol protocol;
};

int Serial_ReadByte(unsigned char *dest);
int Serial_ReadBytes(unsigned char *dest, int max, short *size);

int Serial_WriteByte(unsigned char byte);
int Serial_WriteBytes(unsigned char *src, int size);

int Serial_GetRxBufferSize(void);
int Serial_GetTxBufferFreeCapacity(void);

int Serial_ClearReceiveBuffer(void);
int Serial_ClearTransmitBuffer(void);

int Serial_Open(unsigned char *conf);
int Serial_Close(int mode);

int Serial_IsOpen(void);



#endif //_CALCULIB_3PIN_H_