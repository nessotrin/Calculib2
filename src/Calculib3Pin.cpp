#include "Calculib3Pin.h"

#include "CalculibAlgorithms.h"
#include "CalculibTime.h" //Sleep


#include <cstdlib>
#include <cstdio>
#include <cstring>

    
CalculibBuffer Calculib3Pin::sendBuffer;
CalculibBuffer Calculib3Pin::receiveBuffer;
CalculibProtocol Calculib3Pin::protocol;
sf::Mutex Calculib3Pin::updateMutex;
sf::Thread Calculib3Pin::networkThread(&Calculib3Pin::tick3Pin);
bool Calculib3Pin::continueNetworkThread;

void Calculib3Pin::tick3Pin()
{
    while(1) // can't use the condition because it needs to lock&unlock the mutex
    {
        updateMutex.lock();

        if(!continueNetworkThread)
        {
            break;
        }
        if(receiveBuffer.size < 1024)
        {
            protocol.readData(&receiveBuffer,1024);
        }
        if(sendBuffer.size > 0)
        {
            if(protocol.sendData(&sendBuffer) == false) // sucess
            {
                sendBuffer.size = 0; //remove what was sent    
            }
        }
        protocol.tick();
        
        updateMutex.unlock();
        
        sf::sleep(sf::milliseconds(50));
    }
}

void Calculib3Pin::initCalculib3Pin()
{
    receiveBuffer.buffer = (unsigned char *) malloc(1024); //Official specs
    sendBuffer.buffer = (unsigned char *) malloc(256); //Official specs
        
    if(receiveBuffer.buffer == NULL || sendBuffer.buffer == NULL)
    {
        printf("Malloc error ! (3pin's buffer)\n");
    }
    
    continueNetworkThread = true;
    networkThread.launch();
}

void Calculib3Pin::setSocket(CalculibSocket * newSocketForProtocol)
{
    updateMutex.lock();
    protocol.setSocket(newSocketForProtocol);
    updateMutex.unlock();
}

bool Calculib3Pin::isProtocolConnected()
{
    return protocol.isConnected();
}

void Calculib3Pin::disconnect()
{
    updateMutex.lock();
    printf("Trying to disconnect the 3pin ...\n");
    protocol.disconnect();
    updateMutex.unlock();
}

void Calculib3Pin::connect()
{
    updateMutex.lock();
    printf("Trying to connect the 3pin ...\n");
    if(protocol.isConnected())
    {
        printf("Already connected !\n");
    }
    else
    {
        protocol.connect();
    }
    updateMutex.unlock();
}

int Serial_ReadByte(unsigned char *dest)
{
    short unusedSize;
    return Serial_ReadBytes(dest,1,&unusedSize);
}
int Serial_ReadBytes(unsigned char *dest, int max, short *size)
{
    if(!Serial_IsOpen())
    {
        return 3;
    }
    
    Calculib3Pin::updateMutex.lock();

    *size = CALCULIB_MIN(max,Calculib3Pin::receiveBuffer.size);

    if(*size == 0)
    {
        Calculib3Pin::updateMutex.unlock();
        return 1;
    }

    memcpy(dest,Calculib3Pin::receiveBuffer.buffer, *size);

    //move forward the buffer
    memmove(Calculib3Pin::receiveBuffer.buffer,
            Calculib3Pin::receiveBuffer.buffer+*size,
            Calculib3Pin::receiveBuffer.size - *size);
    Calculib3Pin::receiveBuffer.size -= *size;

    Calculib3Pin::updateMutex.unlock();
    
    return 0;
}

int Serial_WriteByte(unsigned char byte)
{
    return Serial_WriteBytes(&byte,1);
}
int Serial_WriteBytes(unsigned char *src, int size)
{
    Calculib3Pin::updateMutex.lock();
        
    if(!Calculib3Pin::isProtocolConnected())
    {
        Calculib3Pin::updateMutex.unlock();
        return 3;
    }
    if(256-Calculib3Pin::receiveBuffer.size < size)
    {
        Calculib3Pin::updateMutex.unlock();
        return 2;
    }
    
    memcpy(Calculib3Pin::sendBuffer.buffer+Calculib3Pin::sendBuffer.size, src, size);
    Calculib3Pin::sendBuffer.size += size;
    
    Calculib3Pin::updateMutex.unlock();
    
    return 0;
}

int Serial_GetRxBufferSize(void)
{
    Calculib3Pin::updateMutex.lock();
    int size = Calculib3Pin::receiveBuffer.size;
    Calculib3Pin::updateMutex.unlock();
    return size;
}
int Serial_GetTxBufferFreeCapacity(void)
{
    Calculib3Pin::updateMutex.lock();
    int size = 256-Calculib3Pin::sendBuffer.size;
    Calculib3Pin::updateMutex.unlock();
    return size;
}

int Serial_ClearReceiveBuffer(void)
{
    Calculib3Pin::updateMutex.lock();
    Calculib3Pin::receiveBuffer.size = 0;
    Calculib3Pin::updateMutex.unlock();
}
int Serial_ClearTransmitBuffer(void)
{
    Calculib3Pin::updateMutex.lock();
    Calculib3Pin::sendBuffer.size = 0;
    Calculib3Pin::updateMutex.unlock();
}


int Serial_Open(unsigned char *conf)
{
    if(conf != NULL && conf[0] != 0) // bad config
    {
        return 4;
    }
    
    int result = 3;
    
    Calculib3Pin::connect();    
    
    Sleep(1000); // give time to get the answer
    
    Calculib3Pin::updateMutex.lock();
    
    if(Calculib3Pin::isProtocolConnected())
    {
        result = 0;
    }
    
    Calculib3Pin::updateMutex.unlock();
    
    return result;
}
int Serial_Close(int mode)
{
    Calculib3Pin::updateMutex.lock();

    if(mode == 0) //Do not send the end
    {
        Calculib3Pin::sendBuffer.size = 0;
    }

    Calculib3Pin::updateMutex.unlock();
    
    Calculib3Pin::disconnect();    

    return 0;
}

int Serial_IsOpen(void)
{
    Calculib3Pin::updateMutex.lock();
    int result = Calculib3Pin::isProtocolConnected();
    Calculib3Pin::updateMutex.unlock();
    return result;
}