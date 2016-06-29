#include "CalculibGraphics.h"
#include "CalculibML.h"


#ifdef CALCULIB
unsigned char CalculibGraphics::VRAM[128*64/8];

void Bdisp_AllClr_DD(void)
{
    ML_clear_screen();
}
void Bdisp_AllClr_VRAM(void)
{
    ML_clear_vram();
}
void Bdisp_AllClr_DDVRAM(void)
{
    ML_clear_screen();
    ML_clear_vram();
}
void Bdisp_AreaReverseVRAM(int x1, int y1, int x2, int y2)
{
    ML_rectangle(x1,y1,x2,y2,0,ML_XOR,ML_XOR);
}
void Bdisp_PutDisp_DD(void)
{
    ML_display_vram();
}
void Bdisp_SetPoint_VRAM(int x, int y, unsigned char point)
{
    ML_pixel(x,y,point?ML_BLACK:ML_WHITE);
}
int  Bdisp_GetPoint_VRAM(int x, int y)
{
    return ML_pixel_test(x,y)==ML_BLACK;
}
#endif
