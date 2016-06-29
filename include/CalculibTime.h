#ifndef _CALCULIB_TIME_H_
#define _CALCULIB_TIME_H_

#include <SFML/System.hpp>

//TODO: Timers ! (How ?)

class CalculibTime
{
public:
    static sf::Clock timeCounter;
};

//int SetTimer(int ID, int elapse, void (*hander)(void));
//int KillTimer(int ID);
void Sleep(int millisecond);

int RTC_GetTicks();// TODO: calc syscall

#endif //_CALCULIB_TIME_H_