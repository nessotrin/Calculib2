/** Based on MonochromeLib - Pierre "PierrotLL" Le Gall **/

#include "CalculibML.h"
#include "CalculibGraphics.h"
#include "CalculibWindow.h"

/***************/
/** Functions **/
/***************/

#define ML_sgn(x)	(x<0?-1:1)
#define ML_rnd(x)	((int)(x+0.5))
#define ML_ceil(x) ((int)x != x ? (int)x+1 : (int)x)

#ifdef CALCULIB
char * ML_vram_adress()
{
    return (char *)CalculibGraphics::VRAM;
}
unsigned long ML_vram_adress_as_ulong()
{
    return (unsigned long) CalculibGraphics::VRAM;
}
#else
typedef char*(*sc_cpv)(void);
typedef unsigned long(*sc_cpv_ulong)(void);
const unsigned int sc0135[] = { 0xD201D002, 0x422B0009, 0x80010070, 0x0135 };
char * ML_vram_adress()
{
    return (*(sc_cpv)sc0135)();
}
#endif

#define S (sizeof long)

void ML_clear_vram()
{
    char * vram;
    char * end = vram + 1023;
    while (vram & (S-1)) *(vram++) = 0;
    while (vram <= end-S) *(((long*)vram)++) = 0;
    while (vram < end) *(vram++) = 0;
} 



#ifdef CALCULIB
void ML_clear_screen()
{
    CalculibWindow::clearScreen();
}
#else
void ML_clear_screen()
{
	char *LCD_register_selector = (char*)0xB4000000, *LCD_data_register = (char*)0xB4010000;
	int i, j;
	for(i=0 ; i<64 ; i++)
	{
		*LCD_register_selector = 4;
		*LCD_data_register = i|192;
		*LCD_register_selector = 4;
		*LCD_data_register = 0;
		*LCD_register_selector = 7;
		for(j=0 ; j<16 ; j++) *LCD_data_register = 0;
	}
}
#endif

#ifdef CALCULIB
void ML_display_vram()
{
    CalculibWindow::drawVramOnScreen();
}
#else
void ML_display_vram()
{
	char *LCD_register_selector = (char*)0xB4000000, *LCD_data_register = (char*)0xB4010000, *vram;
	int i, j;
	vram = ML_vram_adress();
	for(i=0 ; i<64 ; i++)
	{
		*LCD_register_selector = 4;
		*LCD_data_register = i|192;
		*LCD_register_selector = 4;
		*LCD_data_register = 0;
		*LCD_register_selector = 7;
		for(j=0 ; j<16 ; j++) *LCD_data_register = *vram++;
	}
}
#endif


void ML_pixel(int x, int y, ML_Color color)
{
    char* vram = ML_vram_adress();
	if(x&~127 || y&~63) return;

	switch(color)
	{
		case ML_BLACK:
			vram[(y<<4)+(x>>3)] |= 128>>(x&7);
			break;
		case ML_WHITE:
			vram[(y<<4)+(x>>3)] &= ~(128>>(x&7));
			break;
		case ML_XOR:
			vram[(y<<4)+(x>>3)] ^= 128>>(x&7);
			break;
		case ML_CHECKER:
			if((y&1)^(x&1)) vram[(y<<4)+(x>>3)] &= ~(128>>(x&7));
			else vram[(y<<4)+(x>>3)] |= 128>>(x&7);
			break;
		case ML_TRANSPARENT:
			break;
	}

}


void ML_point(int x, int y, int width, ML_Color color)
{
	if(width < 1) return;
	if(width == 1) ML_pixel(x, y, color);
	else
	{
		int padding, pair;
		padding = width>>1;
		pair = !(width&1);
		ML_rectangle(x-padding+pair, y-padding+pair, x+padding, y+padding, 0, (ML_Color)0, color);
	}
}


ML_Color ML_pixel_test(int x, int y)
{
	char *vram, byte;
	if(x&~127 || y&~63) return ML_TRANSPARENT;
	vram = ML_vram_adress();
	byte = 1<<(7-(x&7));
	return (vram[(y<<4)+(x>>3)] & byte ? ML_BLACK : ML_WHITE);

}


void ML_line(int x1, int y1, int x2, int y2, ML_Color color)
{
	int i, x, y, dx, dy, sx, sy, cumul;
	x = x1;
	y = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	sx = ML_sgn(dx);
	sy = ML_sgn(dy);
	dx = abs(dx);
	dy = abs(dy);
	ML_pixel(x, y, color);
	if(dx > dy)
	{
		cumul = dx / 2;
		for(i=1 ; i<dx ; i++)
		{
			x += sx;
			cumul += dy;
			if(cumul > dx)
			{
				cumul -= dx;
				y += sy;
			}
			ML_pixel(x, y, color);
		}
	}
	else
	{
		cumul = dy / 2;
		for(i=1 ; i<dy ; i++)
		{
			y += sy;
			cumul += dx;
			if(cumul > dy)
			{
				cumul -= dy;
				x += sx;
			}
			ML_pixel(x, y, color);
		}
	}
}


void ML_horizontal_line(int y, int x1, int x2, ML_Color color)
{
    int i;
    if(y&~63 || (x1<0 && x2<0) || (x1>127 && x2>127)) return;
    if(x1 > x2)
    {
    	i = x1;
    	x1 = x2;
    	x2 = i;
    }
    if(x1 < 0) x1 = 0;
    if(x2 > 127) x2 = 127;
    switch(color)
    {
    	case ML_BLACK:
			for(int i = x1 ; i <= x2 ; i++)
            {
                ML_pixel(i,y,ML_BLACK);
            }
			break;
		case ML_WHITE:
			for(int i = x1 ; i <= x2 ; i++)
            {
                ML_pixel(i,y,ML_WHITE);
            }
			break;
		case ML_XOR:
			for(int i = x1 ; i <= x2 ; i++)
            {
                ML_pixel(i,y,ML_XOR);
            }
			break;
		case ML_CHECKER:
			for(int i = x1 ; i <= x2 ; i++)
            {
                ML_pixel(i,y,ML_BLACK);
            }
		case ML_TRANSPARENT:
		    break;
    }
}


void ML_vertical_line(int x, int y1, int y2, ML_Color color)
{
	if(x&~127 || (y1<0 && y2<0) || (y1>63 && y2>63)) return;
	if(y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if(y1 < 0) y1 = 0;
	if(y2 > 63) y2 = 63;

	switch(color)
	{
		case ML_BLACK:
			for(int i = y1 ; i <= y2 ; i++)
            {
                ML_pixel(x,i,ML_BLACK);

            }
			break;
		case ML_WHITE:
			for(int i = y1 ; i <= y2 ; i++)
            {
                ML_pixel(x,i,ML_WHITE);
            }
			break;
		case ML_XOR:
			for(int i = y1 ; i <= y2 ; i++)
            {
                ML_pixel(x,i,ML_XOR);
            }
		case ML_CHECKER:
			for(int i = y1 ; i <= y2 ; i++)
            {
                ML_pixel(x,i,ML_BLACK);
            }
			break;
		case ML_TRANSPARENT:
		    break;
	}
}


void ML_rectangle(int x1, int y1, int x2, int y2, int border_width, ML_Color border_color, ML_Color fill_color)
{
	int i;
	if(x1 > x2)
	{
		i = x1;
		x1 = x2;
		x2 = i;
	}
	if(y1 > y2)
	{
		i = y1;
		y1 = y2;
		y2 = i;
	}
	if(border_width > (x2-x1)/2+1) border_width = (x2-x1)/2+1;
	if(border_width > (y2-y1)/2+1) border_width = (y2-y1)/2+1;
	if(border_color != ML_TRANSPARENT && border_width > 0)
	{
		for(i=0 ; i<border_width ; i++)
		{
			ML_horizontal_line(y1+i, x1, x2, border_color);
			ML_horizontal_line(y2-i, x1, x2, border_color);
		}
		for(i=y1+border_width ; i<=y2-border_width ; i++)
		{
			ML_horizontal_line(i, x1, x1+border_width-1, border_color);
			ML_horizontal_line(i, x2-border_width+1, x2, border_color);
		}
	}
	if(fill_color != ML_TRANSPARENT)
	{
		for(i=y1+border_width ; i<=y2-border_width ; i++)
			ML_horizontal_line(i, x1+border_width, x2-border_width, fill_color);
	}
}


void ML_polygon(const int *x, const int *y, int nb_vertices, ML_Color color)
{
	int i;
	if(nb_vertices < 1) return;
	for(i=0 ; i<nb_vertices-1 ; i++)
		ML_line(x[i], y[i], x[i+1], y[i+1], color);
	ML_line(x[i], y[i], x[0], y[0], color);
}


static int ML_filled_polygon_quicksord_partition(int *t, int p, int r)
{
	int i, j, x, tmp;
	j = p - 1;
	x = t[r];
	for(i=p ; i<r ; i++)
	{
		if(x > t[i])
		{
			j++;
			tmp = t[j];
			t[j] = t[i];
			t[i] = tmp;
		}
	}
	t[r] = t[j+1];
	t[j+1] = x;
	return j + 1;
}

static void ML_filled_polygon_quicksord(int* t, int p, int r)
{
	int q;
	if(p < r)
	{
		q = ML_filled_polygon_quicksord_partition(t, p, r);
		ML_filled_polygon_quicksord(t, p, q-1);
		ML_filled_polygon_quicksord(t, q+1, r);
	}
}


void ML_filled_polygon(const int *x, const int *y, int nb_vertices, ML_Color color)
{
	int i, j, dx, dy, ymin, ymax;
	int *cut_in_line, nb_cut;
	if(nb_vertices < 3) return;
	cut_in_line = (int *) malloc(nb_vertices*sizeof(int));
	if(!cut_in_line) return;
	ymin = ymax = y[0];
	for(i=1 ; i<nb_vertices ; i++)
	{
		if(y[i] < ymin) ymin = y[i];
		if(y[i] > ymax) ymax = y[i];
	}
	for(i=ymin ; i<=ymax ; i++)
	{
		nb_cut = 0;
		for(j=0 ; j<nb_vertices ; j++)
		{
			if((y[j]<=i && y[(j+1)%nb_vertices]>=i) || (y[j]>=i && y[(j+1)%nb_vertices]<=i))
			{
				dy = abs(y[j]-y[(j+1)%nb_vertices]);
				if(dy)
				{
					dx = x[(j+1)%nb_vertices]-x[j];
					cut_in_line[nb_cut] = x[j] + ML_rnd(abs(i-y[j]+ML_sgn(i-y[j])/2)*dx/dy);
					nb_cut++;
				}
			}
		}
		ML_filled_polygon_quicksord(cut_in_line, 0, nb_cut-1);
		j = 0;
		while(j<nb_cut-2 && cut_in_line[j]==cut_in_line[j+1]) j++;
		while(j < nb_cut)
		{
			if(j == nb_cut-1) ML_horizontal_line(i, cut_in_line[j-1]+1, cut_in_line[j], color);
			else
			{
				dx = 1;
				while(j+dx<nb_cut-1 && cut_in_line[j+dx]==cut_in_line[j+dx+1]) dx++;
				ML_horizontal_line(i, cut_in_line[j], cut_in_line[j+dx], color);
				j += dx;
			}
			j++;
		}
	}
	free(cut_in_line);
}


void ML_circle(int x, int y, int radius, ML_Color color)
{
	int plot_x, plot_y, d;

	if(radius < 0) return;
	plot_x = 0;
	plot_y = radius;
	d = 1 - radius;

	ML_pixel(x, y+plot_y, color);
	if(radius)
	{
		ML_pixel(x, y-plot_y, color);
		ML_pixel(x+plot_y, y, color);
		ML_pixel(x-plot_y, y, color);
	}
	while(plot_y > plot_x)
	{
		if(d < 0)
			d += 2*plot_x+3;
		else
		{
			d += 2*(plot_x-plot_y)+5;
			plot_y--;
		}
		plot_x++;
		if(plot_y >= plot_x)
		{
			ML_pixel(x+plot_x, y+plot_y, color);
			ML_pixel(x-plot_x, y+plot_y, color);
			ML_pixel(x+plot_x, y-plot_y, color);
			ML_pixel(x-plot_x, y-plot_y, color);
		}
		if(plot_y > plot_x)
		{
			ML_pixel(x+plot_y, y+plot_x, color);
			ML_pixel(x-plot_y, y+plot_x, color);
			ML_pixel(x+plot_y, y-plot_x, color);
			ML_pixel(x-plot_y, y-plot_x, color);
		}
	}
}


void ML_filled_circle(int x, int y, int radius, ML_Color color)
{
	int plot_x, plot_y, d;

	if(radius < 0) return;
	plot_x = 0;
	plot_y = radius;
	d = 1 - radius;

	ML_horizontal_line(y, x-plot_y, x+plot_y, color);
	while(plot_y > plot_x)
	{
		if(d < 0)
			d += 2*plot_x+3;
		else {
			d += 2*(plot_x-plot_y)+5;
			plot_y--;
			ML_horizontal_line(y+plot_y+1, x-plot_x, x+plot_x, color);
			ML_horizontal_line(y-plot_y-1, x-plot_x, x+plot_x, color);
		}
		plot_x++;
		if(plot_y >= plot_x)
		{
			ML_horizontal_line(y+plot_x, x-plot_y, x+plot_y, color);
			ML_horizontal_line(y-plot_x, x-plot_y, x+plot_y, color);
		}
	}
}


void ML_ellipse(int x, int y, int radius1, int radius2, ML_Color color)
{
	int plot_x, plot_y;
	float d1, d2;
	if(radius1 < 1 || radius2 < 1) return;
	plot_x = 0;
	plot_y = radius2;
	d1 = radius2*radius2 - radius1*radius1*radius2 + radius1*radius1/4;
	ML_pixel(x, y+plot_y, color);
	ML_pixel(x, y-plot_y, color);
	while(radius1*radius1*(plot_y-.5) > radius2*radius2*(plot_x+1))
	{
		if(d1 < 0)
		{
			d1 += radius2*radius2*(2*plot_x+3);
			plot_x++;
		} else {
			d1 += radius2*radius2*(2*plot_x+3) + radius1*radius1*(-2*plot_y+2);
			plot_x++;
			plot_y--;
		}
		ML_pixel(x+plot_x, y+plot_y, color);
		ML_pixel(x-plot_x, y+plot_y, color);
		ML_pixel(x+plot_x, y-plot_y, color);
		ML_pixel(x-plot_x, y-plot_y, color);
	}
	d2 = radius2*radius2*(plot_x+.5)*(plot_x+.5) + radius1*radius1*(plot_y-1)*(plot_y-1) - radius1*radius1*radius2*radius2;
	while(plot_y > 0)
	{
		if(d2 < 0)
		{
			d2 += radius2*radius2*(2*plot_x+2) + radius1*radius1*(-2*plot_y+3);
			plot_y--;
			plot_x++;
		} else {
			d2 += radius1*radius1*(-2*plot_y+3);
			plot_y--;
		}
		ML_pixel(x+plot_x, y+plot_y, color);
		ML_pixel(x-plot_x, y+plot_y, color);
		if(plot_y > 0)
		{
			ML_pixel(x+plot_x, y-plot_y, color);
			ML_pixel(x-plot_x, y-plot_y, color);
		}
	}
}


void ML_ellipse_in_rect(int x1, int y1, int x2, int y2, ML_Color color)
{
	int radius1, radius2;
	if(x1 > x2)
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if(y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	radius1 = (x2-x1)/2;
	radius2 = (y2-y1)/2;
	ML_ellipse(x1+radius1, y1+radius2, radius1, radius2, color);
}


void ML_filled_ellipse(int x, int y, int radius1, int radius2, ML_Color color)
{
	int plot_x, plot_y;
	float d1, d2;
	if(radius1 < 1 || radius2 < 1) return;
	plot_x = 0;
	plot_y = radius2;
	d1 = radius2*radius2 - radius1*radius1*radius2 + radius1*radius1/4;
	while(radius1*radius1*(plot_y-.5) > radius2*radius2*(plot_x+1))
	{
		if(d1 < 0)
		{
			d1 += radius2*radius2*(2*plot_x+3);
			plot_x++;
		} else {
			d1 += radius2*radius2*(2*plot_x+3) + radius1*radius1*(-2*plot_y+2);
			ML_horizontal_line(y+plot_y, x-plot_x, x+plot_x, color);
			ML_horizontal_line(y-plot_y, x-plot_x, x+plot_x, color);
			plot_x++;
			plot_y--;
		}
	}
	ML_horizontal_line(y+plot_y, x-plot_x, x+plot_x, color);
	ML_horizontal_line(y-plot_y, x-plot_x, x+plot_x, color);
	d2 = radius2*radius2*(plot_x+.5)*(plot_x+.5) + radius1*radius1*(plot_y-1)*(plot_y-1) - radius1*radius1*radius2*radius2;
	while(plot_y > 0)
	{
		if(d2 < 0)
		{
			d2 += radius2*radius2*(2*plot_x+2) + radius1*radius1*(-2*plot_y+3);
			plot_y--;
			plot_x++;
		} else {
			d2 += radius1*radius1*(-2*plot_y+3);
			plot_y--;
		}
		ML_horizontal_line(y+plot_y, x-plot_x, x+plot_x, color);
		if(plot_y > 0)
			ML_horizontal_line(y-plot_y, x-plot_x, x+plot_x, color);
	}
}


void ML_filled_ellipse_in_rect(int x1, int y1, int x2, int y2, ML_Color color)
{
	int radius1, radius2;
	if(x1 > x2)
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if(y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	radius1 = (x2-x1)/2;
	radius2 = (y2-y1)/2;
	ML_filled_ellipse(x1+radius1, y1+radius2, radius1, radius2, color);
}


void ML_horizontal_scroll(int scroll)
{
	int i, j;
	char line[16], shift, *vram;
	unsigned char next;
	unsigned short word;
	vram = ML_vram_adress();
	scroll %= 128;
	shift = 8-(scroll&7);
	for(i=0 ; i<64 ; i++)
	{
		for(j=0 ; j<16 ; j++) line[j] = vram[(i<<4)+((j-(scroll>>3)+15)&15)];
		next = line[15];
		vram[(i<<4)+15] = 0;
		for(j=15 ; j>0 ; j--)
		{
			word = next << shift;
			next = line[j-1];
			vram[(i<<4)+j] |= *((char*)&word+1);
			vram[(i<<4)+j-1] = *((char*)&word);
		}
		word = next << shift;
		vram[(i<<4)] |= *((char*)&word+1);
		vram[(i<<4)+15] |= *((char*)&word);
	}
}


void ML_vertical_scroll(int scroll)
{
	int i, j;
	char column[64], *vram = ML_vram_adress();
	scroll %= 64;
	for(i=0 ; i<16 ; i++)
	{
		for(j=0 ; j<64 ; j++) column[j] = vram[(j<<4)+i];
		for(j=0 ; j<64 ; j++) vram[(j<<4)+i] = column[(j-scroll+64)&63];
	}
}



#ifdef CALCULIB
#define MIN(a,b) (((a)<(b)?(a):(b)))
// replaces the endian sensitive bitmap functions
void DrawBitmap(const unsigned char * bmp, int x, int y, int width, int height, int mode)
{

    const int LineAfterConversion = ML_ceil((float)width/8);

    int i = 0;
    int j = 0;
    int id;
    for(i = 0 ; i < height ; i++)
    {
        for(j = 0 ; j < width ; j+= 8)
        {
            id = i*LineAfterConversion+(j/8);

            for(int d = MIN(width - j,8)-1; d>=0 ; d--)
            {
                int result = (bmp[id]>>(7-d)) & 1;
                if(mode == 0 && result)
                {
                    ML_pixel(j+x+d,i+y, ML_BLACK );
                }
                else if(mode == 1 && !result)
                {
                    ML_pixel(j+x+d,i+y,ML_WHITE);
                }
                else if(mode == 2 && result)
                {
                    ML_pixel(j+x+d,i+y,ML_XOR);
                }
            }
        }
    }

}
void DrawBitmapShort(const unsigned short * bmp, int x, int y, int width, int height, int mode)
{
    const int LineAfterConversion = ML_ceil((double)width/16);
    int i = 0;
    int j = 0;
    int id;
    for(i = 0 ; i < height ; i++)
    {
        for(j = 0 ; j < width ; j+= 16)
        {
            id = i*LineAfterConversion+(j/16);

            for(int d = MIN(width - j,16)-1; d>=0 ; d--)
            {
                int result = ((bmp[id]>>(15-d))) & 1;
                if(mode == 0)
                {
                    ML_pixel(j+x+d,i+y,result == 1 ? ML_BLACK : ML_WHITE);
                }
                else if(mode == 1 && !result)
                {
                    ML_pixel(j+x+d,i+y,ML_WHITE);
                }
                else if(mode == 2 && result)
                {
                    ML_pixel(j+x+d,i+y,ML_XOR);
                }
            }
        }
    }

}

#endif // CALCULIB

void ML_bmp_or(const unsigned char *bmp, int x, int y, int width, int height)
{
    #ifdef CALCULIB
    DrawBitmap(bmp,x,y,width,height,0);
    #else
	unsigned short line;
	char shift, *screen, *p=(char*)&line;
	int i, j, begin=0, end=height, real_width=(width-1>>3<<3)+8;
	if(!bmp || x<0 || x>128-width || y<1-height || y>63 || width<1 || height<1) return;
	if(y < 0) begin = -y;
	if(y+height > 64) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		for(j=0 ; j<width-1>>3 ; j++)
		{
			line = bmp[i*(real_width>>3)+j]<<shift;
			screen[j] |= *p;
			if(shift!=8) screen[j+1] |= *(p+1);
		}
		line = (bmp[i*(real_width>>3)+j] & -1<<(real_width-width))<<shift;
		screen[j] |= *p;
		if(shift!=8 && x+real_width<129) screen[j+1] |= *(p+1);
		screen += 16;
	}
	#endif
}


void ML_bmp_and(const unsigned char *bmp, int x, int y, int width, int height)
{
    #ifdef CALCULIB
    DrawBitmap(bmp,x,y,width,height,1);
    #else
	unsigned short line;
	char shift, *screen, *p=(char*)&line;
	int i, j, begin=0, end=height, real_width=(width-1>>3<<3)+8;
	if(!bmp || x<0 || x>128-width || y<1-height || y>63 || width<1 || height<1) return;
	if(y < 0) begin = -y;
	if(y+height > 64) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		for(j=0 ; j<width-1>>3 ; j++)
		{
			line = ~((unsigned char)~bmp[i*(real_width>>3)+j]<<shift);
			screen[j] &= *p;
			if(shift!=8) screen[j+1] &= *(p+1);
		}
		line = ~((unsigned char)~(bmp[i*(real_width>>3)+j] | (unsigned char)-1>>8-(width&7))<<shift);
		screen[j] &= *p;
		if(shift!=8 && x+real_width<129) screen[j+1] &= *(p+1);
		screen += 16;
	}
	#endif
}


void ML_bmp_xor(const unsigned char *bmp, int x, int y, int width, int height)
{
    #ifdef CALCULIB
    DrawBitmap(bmp,x,y,width,height,2);
    #else
	unsigned short line;
	char shift, *screen, *p=(char*)&line;
	int i, j, begin=0, end=height, real_width=(width-1>>3<<3)+8;
	if(!bmp || x<0 || x>128-width || y<1-height || y>63 || width<1 || height<1) return;
	if(y < 0) begin = -y;
	if(y+height > 64) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		for(j=0 ; j<width-1>>3 ; j++)
		{
			line = bmp[i*(real_width>>3)+j]<<shift;
			screen[j] ^= *p;
			if(shift!=8) screen[j+1] ^= *(p+1);
		}
		line = (bmp[i*(real_width>>3)+j] & -1<<(real_width-width))<<shift;
		screen[j] ^= *p;
		if(shift!=8 && x+real_width<129) screen[j+1] ^= *(p+1);
		screen += 16;
	}
	#endif
}


void ML_bmp_or_cl(const unsigned char *bmp, int x, int y, int width, int height)
{
    #ifdef CALCULIB
    DrawBitmap(bmp,x,y,width,height,0);
    #else
	unsigned short line;
	char shift, *screen, *p;
	int i, j, real_width, begin_x, end_x, begin_y, end_y;
	char bool1=1, bool2=1, bool3;
	if(!bmp || x<1-width || x>127 || y<1-height || y>63 || height<1 || width<1) return;
	p = (char*)&line;
	real_width = (width-1>>3<<3)+8;
	if(y < 0) begin_y = -y;
	else begin_y = 0;
	if(y+height > 64) end_y = 64-y;
	else end_y = height;
	shift = 8-(x&7);
	if(x<0)
	{
		begin_x = -x>>3;
		if(shift != 8) bool1 = 0;
	} else begin_x = 0;
	if(x+real_width > 128) end_x = 15-(x>>3), bool2 = 0;
	else end_x = real_width-1>>3;
	bool3 = (end_x == real_width-1>>3);
	screen = ML_vram_adress()+(y+begin_y<<4)+(x>>3);

	for(i=begin_y ; i<end_y ; i++)
	{
		if(begin_x < end_x)
		{
			line = bmp[i*(real_width>>3)+begin_x] << shift;
			if(bool1) screen[begin_x] |= *p;
			if(shift!=8) screen[begin_x+1] |= *(p+1);
			for(j=begin_x+1 ; j<end_x ; j++)
			{
				line = bmp[i*(real_width>>3)+j] << shift;
				screen[j] |= *p;
				if(shift!=8) screen[j+1] |= *(p+1);
			}
		}
		line = bmp[i*(real_width>>3)+end_x];
		if(bool3) line &= -1<<real_width-width;
		line <<= shift;
		if(begin_x < end_x || bool1) screen[end_x] |= *p;
		if(bool2) screen[end_x+1] |= *(p+1);
		screen += 16;
	}
	#endif
}


void ML_bmp_and_cl(const unsigned char *bmp, int x, int y, int width, int height)
{
    #ifdef CALCULIB
    DrawBitmap(bmp,x,y,width,height,1);
    #else
	unsigned short line;
	char shift, *screen, *p;
	int i, j, real_width, begin_x, end_x, begin_y, end_y;
	char bool1=1, bool2=1, bool3;
	if(!bmp || x<1-width || x>127 || y<1-height || y>63 || height<1 || width<1) return;
	p = (char*)&line;
	real_width = (width-1>>3<<3)+8;
	if(y < 0) begin_y = -y;
	else begin_y = 0;
	if(y+height > 64) end_y = 64-y;
	else end_y = height;
	shift = 8-(x&7);
	if(x<0)
	{
		begin_x = -x>>3;
		if(shift != 8) bool1 = 0;
	} else begin_x = 0;
	if(x+real_width > 128) end_x = 15-(x>>3), bool2 = 0;
	else end_x = real_width-1>>3;
	bool3 = (end_x == real_width-1>>3);
	screen = ML_vram_adress()+(y+begin_y<<4)+(x>>3);

	for(i=begin_y ; i<end_y ; i++)
	{
		if(begin_x < end_x)

		{
			line = ~((unsigned char)~bmp[i*(real_width>>3)+begin_x]<<shift);
			if(bool1) screen[begin_x] &= *p;
			if(shift!=8) screen[begin_x+1] &= *(p+1);
			for(j=begin_x+1 ; j<end_x ; j++)
			{
				line = ~((unsigned char)~bmp[i*(real_width>>3)+j]<<shift);
				screen[j] &= *p;
				if(shift!=8) screen[j+1] &= *(p+1);
			}
		}
		line = (unsigned char)~bmp[i*(real_width>>3)+end_x];
		if(bool3) line &= -1<<real_width-width;
		line = ~(line << shift);
		if(begin_x < end_x || bool1) screen[end_x] &= *p;
		if(bool2) screen[end_x+1] &= *(p+1);
		screen += 16;
	}
	#endif
}


void ML_bmp_xor_cl(const unsigned char *bmp, int x, int y, int width, int height)
{
    #ifdef CALCULIB
    DrawBitmap(bmp,x,y,width,height,2);
    #else
	unsigned short line;
	char shift, *screen, *p;
	int i, j, real_width, begin_x, end_x, begin_y, end_y;
	char bool1=1, bool2=1, bool3;
	if(!bmp || x<1-width || x>127 || y<1-height || y>63 || height<1 || width<1) return;
	p = (char*)&line;
	real_width = (width-1>>3<<3)+8;
	if(y < 0) begin_y = -y;
	else begin_y = 0;
	if(y+height > 64) end_y = 64-y;
	else end_y = height;
	shift = 8-(x&7);
	if(x<0)
	{
		begin_x = -x>>3;
		if(shift != 8) bool1 = 0;
	} else begin_x = 0;
	if(x+real_width > 128) end_x = 15-(x>>3), bool2 = 0;
	else end_x = real_width-1>>3;
	bool3 = (end_x == real_width-1>>3);
	screen = ML_vram_adress()+(y+begin_y<<4)+(x>>3);

	for(i=begin_y ; i<end_y ; i++)
	{
		if(begin_x < end_x)
		{
			line = bmp[i*(real_width>>3)+begin_x] << shift;
			if(bool1) screen[begin_x] ^= *p;
			if(shift!=8) screen[begin_x+1] ^= *(p+1);
			for(j=begin_x+1 ; j<end_x ; j++)
			{
				line = bmp[i*(real_width>>3)+j] << shift;
				screen[j] ^= *p;
				if(shift!=8) screen[j+1] ^= *(p+1);
			}
		}
		line = bmp[i*(real_width>>3)+end_x];
		if(bool3) line &= -1<<real_width-width;
		line <<= shift;
		if(begin_x < end_x || bool1) screen[end_x] ^= *p;
		if(bool2) screen[end_x+1] ^= *(p+1);
		screen += 16;
	}
	#endif
}


void ML_bmp_8_or(const unsigned char *bmp, int x, int y)
{
    #ifdef CALCULIB
    DrawBitmap(bmp,x,y,8,8,0);
    #else
	unsigned short line;
	char i, shift, begin=0, end=8, *screen, *p=(char*)&line;
	if(!bmp || x<0 || x>120 || y<-7 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 56) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		screen[0] |= *p;
		if(shift!=8) screen[1] |= *(p+1);
		screen += 16;
	}
	#endif
}


void ML_bmp_8_and(const unsigned char *bmp, int x, int y)
{
    #ifdef CALCULIB
    DrawBitmap(bmp,x,y,8,8,1);
    #else
	unsigned short line;
	char i, shift, begin=0, end=8, *screen, *p=(char*)&line;
	if(!bmp || x<0 || x>120 || y<-7 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 56) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = ~((unsigned char)~bmp[i]<<shift);
		screen[0] &= *p;
		if(shift!=8) screen[1] &= *(p+1);
		screen += 16;
	}
	#endif
}


void ML_bmp_8_xor(const unsigned char *bmp, int x, int y)
{
    #ifdef CALCULIB
    DrawBitmap(bmp,x,y,8,8,2);
    #else
	unsigned short line;
	char i, shift, begin=0, end=8, *screen, *p=(char*)&line;
	if(!bmp || x<0 || x>120 || y<-7 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 56) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		screen[0] ^= *p;
		if(shift<8) screen[1] ^= *(p+1);
		screen += 16;
	}
	#endif
}


void ML_bmp_8_or_cl(const unsigned char *bmp, int x, int y)
{
    #ifdef CALCULIB
    DrawBitmap(bmp,x,y,8,8,0);
    #else
	unsigned short line;
	char i, shift, begin=0, end=8, bool1=1, bool2=1, *screen, *p=(char*)&line;
	if(!bmp || x<-7 || x>127 || y<-7 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 56) end = 64-y;
	shift = 8-(x&7);
	if(x < 0) bool1 = 0;
	if(x>120 || shift==8) bool2 = 0;
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		if(bool1) screen[0] |= *p;
		if(bool2) screen[1] |= *(p+1);
		screen += 16;
	}
	#endif
}


void ML_bmp_8_and_cl(const unsigned char *bmp, int x, int y)
{
    #ifdef CALCULIB
    DrawBitmap(bmp,x,y,8,8,1);
    #else
	unsigned short line;
	char i, shift, begin=0, end=8, bool1=1, bool2=1, *screen, *p=(char*)&line;
	if(!bmp || x<-7 || x>127 || y<-7 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 56) end = 64-y;
	shift = 8-(x&7);
	if(x < 0) bool1 = 0;
	if(x>120 || shift==8) bool2 = 0;
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = ~((unsigned char)~bmp[i]<<shift);
		if(bool1) screen[0] &= *p;
		if(bool2) screen[1] &= *(p+1);
		screen += 16;
	}
	#endif
}


void ML_bmp_8_xor_cl(const unsigned char *bmp, int x, int y)
{
    #ifdef CALCULIB
    DrawBitmap(bmp,x,y,8,8,2);
    #else
	unsigned short line;
	char i, shift, begin=0, end=8, bool1=1, bool2=1, *screen, *p=(char*)&line;
	if(!bmp || x<-7 || x>127 || y<-7 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 56) end = 64-y;
	shift = 8-(x&7);
	if(x < 0) bool1 = 0;
	if(x>120 || shift==8) bool2 = 0;
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		if(bool1) screen[0] ^= *p;
		if(bool2) screen[1] ^= *(p+1);
		screen += 16;
	}
	#endif
}


void ML_bmp_16_or(const unsigned short *bmp, int x, int y)
{
    #ifdef CALCULIB
    DrawBitmapShort(bmp,x,y,16,16,0);
    #else
	unsigned long line;
	char i, shift, begin=0, end=16, *screen, *p=(char*)&line+1;
	if(!bmp || x<0 || x>112 || y<-15 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 48) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		screen[0] |= *p;
		screen[1] |= *(p+1);
		if(shift!=8) screen[2] |= *(p+2);
		screen += 16;
	}
	#endif
}


void ML_bmp_16_and(const unsigned short *bmp, int x, int y)
{
    #ifdef CALCULIB
    DrawBitmapShort(bmp,x,y,16,16,1);
    #else
	unsigned long line;
	char i, shift, begin=0, end=16, *screen, *p=(char*)&line+1;
	if(!bmp || x<0 || x>112 || y<-15 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 48) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = ~((unsigned short)~bmp[i]<<shift);
		screen[0] &= *p;
		screen[1] &= *(p+1);
		if(shift!=8) screen[2] &= *(p+2);
		screen += 16;
	}
	#endif
}


void ML_bmp_16_xor(const unsigned short *bmp, int x, int y)
{
    #ifdef CALCULIB
    DrawBitmapShort(bmp,x,y,16,16,2);
    #else
	unsigned long line;
	char i, shift, begin=0, end=16, *screen, *p=(char*)&line+1;
	if(!bmp || x<0 || x>112 || y<-15 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 48) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		screen[0] ^= *p;
		screen[1] ^= *(p+1);
		if(shift!=8) screen[2] ^= *(p+2);
		screen += 16;
	}
	#endif
}


void ML_bmp_16_or_cl(const unsigned short *bmp, int x, int y)
{
    #ifdef CALCULIB
    DrawBitmapShort(bmp,x,y,16,16,0);
    #else
	unsigned long line;
	char i, shift, begin=0, end=16, bool1=1, bool2=1, bool3=1, *screen, *p=(char*)&line+1;
	if(!bmp || x<-15 || x>127 || y<-15 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 48) end = 64-y;
	shift = 8-(x&7);
	if(x < 0) bool1 = 0;
	if(x<-8 || x>119) bool2 = 0;
	if(x>111 || shift==8) bool3 = 0;
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		if(bool1) screen[0] |= *p;
		if(bool2) screen[1] |= *(p+1);
		if(bool3) screen[2] |= *(p+2);
		screen += 16;
	}
    #endif
}


void ML_bmp_16_and_cl(const unsigned short *bmp, int x, int y)
{
    #ifdef CALCULIB
    DrawBitmapShort(bmp,x,y,16,16,1);
    #else
	unsigned long line;
	char i, shift, begin=0, end=16, bool1=1, bool2=1, bool3=1, *screen, *p=(char*)&line+1;
	if(!bmp || x<-15 || x>127 || y<-15 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 48) end = 64-y;
	shift = 8-(x&7);
	if(x < 0) bool1 = 0;
	if(x<-8 || x>119) bool2 = 0;
	if(x>111 || shift==8) bool3 = 0;
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = ~((unsigned short)~bmp[i]<<shift);
		if(bool1) screen[0] &= *p;
		if(bool2) screen[1] &= *(p+1);
		if(bool3) screen[2] &= *(p+2);
		screen += 16;
	}
	#endif
}


void ML_bmp_16_xor_cl(const unsigned short *bmp, int x, int y)
{
    #ifdef CALCULIB
    DrawBitmapShort(bmp,x,y,16,16,2);
    #else
	unsigned long line;
	char i, shift, begin=0, end=16, bool1=1, bool2=1, bool3=1, *screen, *p=(char*)&line+1;
	if(!bmp || x<-15 || x>127 || y<-15 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 48) end = 64-y;
	shift = 8-(x&7);
	if(x < 0) bool1 = 0;
	if(x<-8 || x>119) bool2 = 0;
	if(x>111 || shift==8) bool3 = 0;
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		if(bool1) screen[0] ^= *p;
		if(bool2) screen[1] ^= *(p+1);
		if(bool3) screen[2] ^= *(p+2);
		screen += 16;
	}
    #endif
}

