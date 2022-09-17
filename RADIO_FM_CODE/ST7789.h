#ifndef __ST7789_H
#define __ST7789_H

#define LCD_RS  5
#define LCD_WIDTH_LIMIT   320
#define LCD_HEIGHT_LIMIT  240
// ST7789 commands
#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13

#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E
#define ST7789_RAMCTRL  0xB0 //page 258

#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

#define ST7789_RDID1   0xDA
#define ST7789_RDID2   0xDB
#define ST7789_RDID3   0xDC
#define ST7789_RDID4   0xDD
//
#define ST7789_COLOR_MODE_16bit 0x55    //  RGB565 (16bit)
#define ST7789_COLOR_MODE_18bit 0x66    //  RGB666 (18bit)
// bits in MADCTL
#define ST7789_MADCTL_MY  0x80
#define ST7789_MADCTL_MX  0x40
#define ST7789_MADCTL_MV  0x20
#define ST7789_MADCTL_ML  0x10
#define ST7789_MADCTL_RGB 0x00
//Color
#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define GRAY        0X8430
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define BROWN       0XBC40
#define BRRED       0XFC07
#define DARKBLUE    0X01CF
#define LIGHTBLUE   0X7D7C
#define GRAYBLUE    0X5458
#define LIGHTGREEN  0X841F
#define LGRAY       0XC618
#define LGRAYBLUE   0XA651
#define LBBLUE      0X2B12

//#define ABS(x) ((x) > 0 ? (x) : -(x))

//INIT
void LCD_Begin(); //System init
void LCD_Update(); //Set flag update
void LCD_Backlight(uint32_t level); //1-10
void LCD_WritePixel(uint16_t x, uint16_t y, uint16_t color); //No Limit (no update)
void LCD_Fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);  //(no update)
void LCD_SetColor(uint16_t color);
void LCD_SetColorRGB(uint8_t r, uint8_t g, uint8_t b);
void LCD_SetBgColor(uint16_t color);
void LCD_SetBgTrans(); //BG Transparent
void LCD_DataClear(); //Clear Frame Buffer (no update)
void LCD_LineClear(uint16_t start, uint16_t line); //Clear Line Frame Buffer (no update)
void LCD_Clear(uint16_t color); //Fill Frame Buffer (update)
void LCD_FontClose();
bool LCD_FontLoad(const uint8_t *font); //Load font PROGMEM
//bool LCD_FontFile(uint8_t file); //file=16 -> /font/16.f   Load font file, not support
void LCD_Wait(); //RTOS LOCK, wait Draw Pixel to Frame Buffer !!!
//Draw Limit
void LCD_DrawInit(int x, int y, int w, int h); //set goto x-y, w-h limit
void LCD_DrawPixel(int x, int y, uint16_t color);  //(no update)
void LCD_DrawFill(int w, int h, uint16_t color); //(no update)
bool LCD_DrawGraphicVertical(uint8_t *data, int width, int height); //(no update)
bool LCD_DrawUnicodeChar(uint32_t c); //(no update + check wait)
bool LCD_DrawText(char *str, int len); //(update)
bool LCD_DrawString(char *str); //(update)
//
typedef struct
{
    char Magic[2];
    uint8_t TableS;
    uint8_t Height;
    uint16_t Number;
    uint16_t Offset;
}FONT_HEADER; //8B

bool LCD_DrawText(uint8_t *str, int len){
    return LCD_DrawText((char*)str, len);
}

bool LCD_DrawString(const char *str){
    return LCD_DrawString((char*)str);
}

bool LCD_DrawString(String str){
    return LCD_DrawString((char*)str.c_str());
}

void LCD_DisplayTask(void * parameter); //RTOS TASK

#endif
