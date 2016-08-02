#include "Calculib.h"

#include "CalculibWindow.h"
#include "Calculib3Pin.h"
#include "CalculibNetworkSocket.h"

bool calculibInit()
{
    CalculibWindow::initWindow();
    CalculibNetworkSocket  * networkSocket = new CalculibNetworkSocket();
    networkSocket->allocate();
    networkSocket->connect();
    Calculib3Pin::setSocket(networkSocket);
    Calculib3Pin::initCalculib3Pin();
    
    //Sleep(100000);
    
    return false;
}
