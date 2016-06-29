#ifndef _CALCULIB_ML_H_
#define _CALCULIB_ML_H_

typedef enum {ML_TRANSPARENT=-1, ML_WHITE, ML_BLACK, ML_XOR, ML_CHECKER} ML_Color;

char* ML_vram_adress();

void ML_clear_vram();
void ML_clear_screen();
void ML_display_vram();

void ML_pixel(int x, int y, ML_Color color);
void ML_point(int x, int y, int width, ML_Color color);
ML_Color ML_pixel_test(int x, int y);

void ML_line(int x1, int y1, int x2, int y2, ML_Color color);
void ML_horizontal_line(int y, int x1, int x2, ML_Color color);
void ML_vertical_line(int x, int y1, int y2, ML_Color color);

void ML_rectangle(int x1, int y1, int x2, int y2, int border_width, ML_Color border_color, ML_Color fill_color);

void ML_polygon(const int *x, const int *y, int nb_vertices, ML_Color color);
void ML_filled_polygon(const int *x, const int *y, int nb_vertices, ML_Color color);

void ML_circle(int x, int y, int radius, ML_Color color);
void ML_filled_circle(int x, int y, int radius, ML_Color color);

void ML_ellipse(int x, int y, int radius1, int radius2, ML_Color color);
void ML_ellipse_in_rect(int x1, int y1, int x2, int y2, ML_Color color);
void ML_filled_ellipse(int x, int y, int radius1, int radius2, ML_Color color);
void ML_filled_ellipse_in_rect(int x, int y, int radius1, int radius2, ML_Color color);

void ML_horizontal_scroll(int scroll);
void ML_vertical_scroll(int scroll);

void ML_bmp_or(const unsigned char *bmp, int x, int y, int width, int height);
void ML_bmp_and(const unsigned char *bmp, int x, int y, int width, int height);
void ML_bmp_xor(const unsigned char *bmp, int x, int y, int width, int height);
void ML_bmp_or_cl(const unsigned char *bmp, int x, int y, int width, int height);
void ML_bmp_and_cl(const unsigned char *bmp, int x, int y, int width, int height);
void ML_bmp_xor_cl(const unsigned char *bmp, int x, int y, int width, int height);

void ML_bmp_8_or(const unsigned char *bmp, int x, int y);
void ML_bmp_8_and(const unsigned char *bmp, int x, int y);
void ML_bmp_8_xor(const unsigned char *bmp, int x, int y);
void ML_bmp_8_or_cl(const unsigned char *bmp, int x, int y);
void ML_bmp_8_and_cl(const unsigned char *bmp, int x, int y);
void ML_bmp_8_xor_cl(const unsigned char *bmp, int x, int y);

void ML_bmp_16_or(const unsigned short *bmp, int x, int y);
void ML_bmp_16_and(const unsigned short *bmp, int x, int y);
void ML_bmp_16_xor(const unsigned short *bmp, int x, int y);
void ML_bmp_16_or_cl(const unsigned short *bmp, int x, int y);
void ML_bmp_16_and_cl(const unsigned short *bmp, int x, int y);
void ML_bmp_16_xor_cl(const unsigned short *bmp, int x, int y);

#endif //_CALCULIB_ML_H_