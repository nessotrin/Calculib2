#include "CalculibTime.h"

#include "CalculibWindow.h"

sf::Clock CalculibTime::timeCounter;

void Sleep(int millisecond)
{
    sf::sleep(sf::milliseconds(millisecond));
    CalculibWindow::handleEvent(); //Needs to be in the main thread, sleep is the best time for it
}

int RTC_GetTicks()
{
    return CalculibTime::timeCounter.getElapsedTime().asMilliseconds()*128/1024; //128 ticks/second
}