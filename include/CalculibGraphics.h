#ifndef _CALCULIB_GRAPHICS_H_
#define _CALCULIB_GRAPHICS_H_

class CalculibGraphics
{
#ifdef CALCULIB

public:
    static unsigned char VRAM[128*64/8];

#endif
};

#ifdef CALCULIB
void Bdisp_AllClr_DD(void);
void Bdisp_AllClr_VRAM(void);
void Bdisp_AllClr_DDVRAM(void);
/*
void Bdisp_AreaClr_DD(const DISPBOX *pArea);
void Bdisp_AreaClr_VRAM(const DISPBOX *pArea);
void Bdisp_AreaClr_DDVRAM(const DISPBOX *pArea);
*/
void Bdisp_AreaReverseVRAM(int x1, int y1, int x2, int y2);
/*
void Bdisp_GetDisp_DD(unsigned char *pData);
void Bdisp_GetDisp_VRAM(unsigned char *pData);
*/
void Bdisp_PutDisp_DD(void);
/*
void Bdisp_PutDispArea_DD(const DISPBOX *PutDispArea);
void Bdisp_SetPoint_DD(int x, int y, unsigned char point);
*/
void Bdisp_SetPoint_VRAM(int x, int y, unsigned char point);
/*
void Bdisp_SetPoint_DDVRAM(int x, int y, unsigned char point);
*/
int  Bdisp_GetPoint_VRAM(int x, int y);
/*
void Bdisp_WriteGraph_DD(const DISPGRAPH *WriteGraph);
void Bdisp_WriteGraph_VRAM(const DISPGRAPH *WriteGraph);
void Bdisp_WriteGraph_DDVRAM(const DISPGRAPH *WriteGraph);
void Bdisp_ReadArea_DD(const DISPBOX *ReadArea, unsigned char *ReadData);
void Bdisp_ReadArea_VRAM(const DISPBOX *ReadArea, unsigned char *ReadData);
void Bdisp_DrawLineVRAM(int x1, int y1, int x2, int y2);
void Bdisp_ClearLineVRAM(int x1, int y1, int x2, int y2);
*/
#endif

#endif //_CALCULIB_GRAPHICS_H_