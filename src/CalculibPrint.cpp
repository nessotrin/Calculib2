#include "CalculibPrint.h"

#include <cstdio>
#include <cstring>


int CalculibPrint::calculibCursorX;
int CalculibPrint::calculibCursorY;

void CalculibPrint::printAt(int x, int y, const unsigned char * text, bool inBlack)
{
    printf("STUB print ! %d %d \"%s\" in black:%d\n",x,y,text,inBlack);
}

void locate(int x, int y)
{
    CalculibPrint::calculibCursorX = x;
    CalculibPrint::calculibCursorY = y;    
}
void Print(const unsigned char *str)
{
    CalculibPrint::printAt(CalculibPrint::calculibCursorX,CalculibPrint::calculibCursorY,str,true);
    CalculibPrint::calculibCursorX += strlen((const char *)str);
}

void PrintXY(int x, int y, const unsigned char *str, int type)
{
    //TODO: type handling
    CalculibPrint::printAt(x,y,str,true);
}