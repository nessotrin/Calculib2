#include "CalculibSocket.h"

CalculibBuffer * CalculibSocket::getReceiveBuffer()
{
    return &receiveBuffer;
}
CalculibBuffer * CalculibSocket::getSendBuffer()
{
    return &sendBuffer;
}
bool CalculibSocket::isConnected()
{
    return connected;
}