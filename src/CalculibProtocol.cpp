#include "CalculibProtocol.h"

#include <cstring>
#include <cstdio>

void CalculibProtocol::tick()
{
    if(socket->isConnected())
    {
        socket->tick();
        CalculibBuffer unused;
        readIncommingData(&unused,0,true); //receive command messages
    }
    else
    {
        if(connected)
        {
            connected = false;
        }
    }
}

void CalculibProtocol::removeFromSocket(int count)
{
    memmove(socket->getReceiveBuffer()->buffer,
            socket->getReceiveBuffer()->buffer+count,
            socket->getReceiveBuffer()->size-count);
           
    socket->getReceiveBuffer()->size -= count;
}

bool CalculibProtocol::buildPacket(CalculibBuffer * packet, unsigned char type, CalculibBuffer * data)
{
    if(packet->size < 1+4+data->size+1) //too small
    {
        printf("Buffer is too small: Available %d, need %d\n",packet->size, 1+4+data->size+1);
        return true;
    }
    
    packet->size = 1+4+data->size+1;
    packet->buffer[0] = type;
    packet->buffer[1] = (data->size>> 0);
    packet->buffer[2] = (data->size>> 8);
    packet->buffer[3] = (data->size>>16);
    packet->buffer[4] = (data->size>>24);
    memcpy(1+4+packet->buffer,data->buffer,data->size);
    packet->buffer[1+4+data->size] = calculateChecksum(packet);
    
    return false;
}
CALCULIB_PACKET_READ_RESULT CalculibProtocol::readPacket(CalculibBuffer * packet, unsigned char * type, CalculibBuffer * data)
{
    if(packet->size < 1+4+1) //not enough data to start
    {
        //printf("Not complete\n");
        return CALCULIB_PACKET_READ_NOT_COMPLETE;
    }
    
         
    *type = packet->buffer[0];
    
    //checking data size
    int newDataSize = (packet->buffer[1]<<0) + (packet->buffer[2]<<8) + (packet->buffer[3]<<16) + (packet->buffer[4]<<24);
    if(newDataSize < 0 || newDataSize > 70000 || data->size < newDataSize) //TODO: fix the limits
    {
        printf("Whut ? %d (got %d) (type %d)\n",newDataSize,data->size,*type);
        return CALCULIB_PACKET_READ_ERROR;
    }
    data->size = newDataSize;
    
    //checking full arrival
    if(packet->size < 1+4+data->size+1)
    {
        printf("So big it's not complete\n");
        return CALCULIB_PACKET_READ_NOT_COMPLETE;
    }
    
    //checking corruption
    packet->size = 1+4+data->size+1;
    if(packet->buffer[packet->size-1] != calculateChecksum(packet))
    {
        printf("Garbage !\n");
        return CALCULIB_PACKET_READ_ERROR;
    }
    
    //writing to the buffer
    memcpy(data->buffer,1+4+packet->buffer,data->size);
    
    return CALCULIB_PACKET_READ_OK;
}

unsigned char CalculibProtocol::calculateChecksum(CalculibBuffer * packet)
{
    unsigned char value = 0;
    for(int i = 0 ; i < packet->size-1 ; i++) //-1 => do not take checksum byte in account
    {
        value += packet->buffer[i];
    }
    return value;
}

void CalculibProtocol::connect()
{
    if(!connected)
    {
        if(sendTypedData(0x2,NULL))
        {
            printf("Failed to send connect !\n");
        }
        printf("Sending connection marker\n");
    }
}

void CalculibProtocol::disconnect()
{
    if(sendTypedData(0x4,NULL))
    {
        printf("Failed to send disconnect !\n");
    }
    printf("Sending disconnect marker\n");
}

bool CalculibProtocol::sendData(CalculibBuffer * toSend)
{
    return sendTypedData(0x1, toSend);
}
    
bool CalculibProtocol::sendTypedData(unsigned char type, CalculibBuffer * toSend)
{
    //allows to use NULL
    CalculibBuffer emptyBuffer;
    emptyBuffer.size = 0;
    if(toSend == NULL)
    {
        toSend = &emptyBuffer;
    }
        
    if(socket->getMaxSendBufferSize() - socket->getSendBuffer()->size < 1+4+toSend->size+1)
    {
        return true; // out of space
    }
    
    CalculibBuffer inSocketBuffer; // writes directly in the socket's send buffer
    inSocketBuffer.buffer = socket->getSendBuffer()->buffer + socket->getSendBuffer()->size;
    inSocketBuffer.size = socket->getMaxSendBufferSize() - socket->getSendBuffer()->size;
    
    if(buildPacket(&inSocketBuffer,type,toSend))
    {
        printf("Error packing, this is impossible !\n");
        return true;
    }
    
    socket->getSendBuffer()->size += inSocketBuffer.size; // manually add to the size
    return false;
}

 
CALCULIB_PACKET_READ_RESULT CalculibProtocol::receiveIncommingData(unsigned char * type, CalculibBuffer * buffer)
{
    CALCULIB_PACKET_READ_RESULT result = readPacket(socket->getReceiveBuffer(),type,buffer);
    if(result == CALCULIB_PACKET_READ_ERROR) //CORRUPTED
    {
        removeFromSocket(socket->getReceiveBuffer()->size); //CLEAR BUFFER
    }
    return result;
}

void CalculibProtocol::readData(CalculibBuffer * toFill, int maxSize)
{
    readIncommingData(toFill,maxSize,false);
}

/*
 * MUST BE RUN REGULARLY !!
 * (is responsible for internal message reception)
 */
void CalculibProtocol::readIncommingData(CalculibBuffer * toFill, int maxSize, bool stopOnData)
{
        
    unsigned char type;
    CalculibBuffer temp;
    unsigned char tempData[1024]; //TODO: proper size
    temp.buffer = tempData;
    temp.size = 1024;
    
    CALCULIB_PACKET_READ_RESULT result = receiveIncommingData(&type,&temp);
    while(result == 0)
    {
        if(type == 0x1) //type 0x1 = raw data
        {
            if(stopOnData)
            {
                return;
            }
            if(temp.size <= maxSize-toFill->size) // enough space
            {
                memcpy(toFill->buffer+toFill->size,temp.buffer,temp.size);
                toFill->size += temp.size;
                
                removeFromSocket(1+4+temp.size+1); //remove the packet
            }
            else //out of space
            {
                return;
            }

        }
        else
        {
            treatSpecialPacket(type, &temp);
            
            removeFromSocket(1+4+temp.size+1); //remove the packet
        }
        
        //try next packet
        result = receiveIncommingData(&type,&temp);
    }
    
    if(result == CALCULIB_PACKET_READ_ERROR) //CORRUPTED
    {
        removeFromSocket(socket->getReceiveBuffer()->size); //CLEAR BUFFER
    }
    else
    {
        //printf("No data left ... \n");
    }
    
}
void CalculibProtocol::treatSpecialPacket(unsigned char type, CalculibBuffer * buffer)
{
    if(type == 0x2)
    {
        printf("Connection marker received, connection established !\n");
        sendTypedData(0x3,NULL);
        connected = true;
    }
    else if(type == 0x3)
    {
        printf("Connection ACK received, connection established !\n");
        connected = true;
    }
    else if(type == 0x4)
    {
        printf("Disconnect marker received, connection is down !\n");
        connected = false;
    }
}

void CalculibProtocol::setSocket(CalculibSocket * newSocket)
{
    socket = newSocket;
}

bool CalculibProtocol::isConnected()
{
    return connected;
}