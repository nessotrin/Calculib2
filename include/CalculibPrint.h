#ifndef _CALCULIB_PRINT_H_
#define _CALCULIB_PRINT_H_

//TODO: proper print support

class CalculibPrint
{
public:

    static int calculibCursorX;
    static int calculibCursorY;
    
    static void printAt(int x, int y, const unsigned char * text, bool inBlack);
};

void locate(int x, int y);
void Print(const unsigned char *str);
/*
void PrintRev(const unsigned char *str);
void PrintC(const unsigned char *c);
void PrintRevC(const unsigned char *str);
void PrintLine(const unsigned char *str, int max);
void PrintRLine(const unsigned char *str, int max);
*/
void PrintXY(int x, int y, const unsigned char *str, int type);
/*
int PrintMini(int x, int y, const unsigned char *str, int type);
*/

#endif //_CALCULIB_PRINT_H_