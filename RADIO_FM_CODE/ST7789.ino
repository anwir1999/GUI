//#include "display.h"
#include "font/font_utf8_16.h"
uint16_t *LCD_Data; //Frame Buffer PSRAM
uint8_t *Font_Data;
const uint8_t *FontFlash;
//File FontFile;
uint32_t *Font_Table;
FONT_HEADER Font_Header;
uint32_t LCD_Flag = 0, LCD_Lock = 0, LCD_Time = 0, LCD_PixelMax = LCD_WIDTH_LIMIT * LCD_HEIGHT_LIMIT, LCD_FontDataMax;
uint32_t LCD_W_Limit = LCD_WIDTH_LIMIT, LCD_H_Limit = LCD_HEIGHT_LIMIT;
uint32_t LCD_Color = 0xFFFF, LCD_BgColor = 0, LCD_Background = 0;
uint32_t LCD_X_Start, LCD_Y_Start, LCD_X_Limit, LCD_Y_Limit, LCD_W_Value, LCD_H_Value;
/********************************************* LCD ST7789V ****************************************************/
void LCD_Active(){ //CS1=0
    SPI_Active(1);
    pinMatrixInDetach(HSPIQ_OUT_IDX, false, false); //VSPIQ_OUT_IDX
    pinMode(LCD_RS, OUTPUT);
}

void LCD_DeActive(){ //CS1=1
    pinMode(LCD_RS, INPUT);
    pinMatrixInAttach(LCD_RS, HSPIQ_OUT_IDX, false);
    SPI_DeActive();
}

void LCD_WriteData(uint8_t data){
    digitalWrite(LCD_RS, 1);
    SPI_WriteByte(data);
    LCD_DeActive();
}

void LCD_WriteBuffer(uint8_t *data, uint32_t len){
    digitalWrite(LCD_RS, 1);
    SPI_WriteBuffer(data, len);
    LCD_DeActive();
}


void LCD_WriteCommand(uint8_t cmd){
    LCD_Active();
    digitalWrite(LCD_RS, 0);
    SPI_WriteByte(cmd);
}

void LCD_Command(uint8_t cmd){
    LCD_WriteCommand(cmd);
    LCD_DeActive();
    delay(1);
}

void LCD_CommandData(uint8_t cmd, uint8_t data){
    LCD_WriteCommand(cmd);
    LCD_WriteData(data);
    delay(1);
}


void LCD_CommandData16(uint8_t cmd, uint16_t data){
    LCD_WriteCommand(cmd);
    LCD_WriteBuffer((uint8_t*)&data, 2);
    delay(1);
}

void LCD_Update(){
    LCD_Flag = 1;
}

void LCD_Backlight(uint32_t level){ //1-10
    if(level == 0)level = 1;
    else if(level > 10)level = 10;
    level *= 255;
    PWM_SetDuty(level / 10);
}

void LCD_Wait(){ //RTOS
    uint32_t timeout = 50;
    while(LCD_Lock && timeout){
        timeout--;
        delay(1);
    }
    //LCD_Lock = 0;
}

void LCD_DisplayUpdate(){ //RGB565
    LCD_Flag = 0; LCD_Lock = 1;
    LCD_SetAddressWindow(0, 0, LCD_WIDTH_LIMIT, LCD_HEIGHT_LIMIT);
    LCD_WriteCommand(ST7789_RAMWR);
    LCD_WriteBuffer((uint8_t*)&LCD_Data[0], LCD_PixelMax << 1);
    LCD_Lock = 0;
}

/*void LCD_SetRotation(uint8_t r){
    uint8_t data;
    if(r == 1)data = (ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
    else if(r == 2)data = (ST7789_MADCTL_RGB);
    else if(r == 3)data = (ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
    else data = (ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
    LCD_CommandData(ST7789_MADCTL, data);
}*/

void LCD_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    uint8_t data[4];
    data[0] = x0 >> 8; data[1] = x0; data[2] = x1 >> 8; data[3] = x1;
    LCD_WriteCommand(ST7789_CASET);
    LCD_WriteBuffer(data, 4);
    data[0] = y0 >> 8; data[1] = y0; data[2] = y1 >> 8; data[3] = y1;
    LCD_WriteCommand(ST7789_RASET);
    LCD_WriteBuffer(data, 4);
}

void LCD_Begin(){ //LCD ST7789V
	  if(!LCD_Data)LCD_Data = (uint16_t*)ps_malloc(LCD_WIDTH_LIMIT*LCD_HEIGHT_LIMIT*2);
	  LCD_Command(ST7789_SWRESET); delay(150);
    LCD_Command(ST7789_SLPOUT);
    LCD_CommandData16(ST7789_RAMCTRL, 0xF800); //RGB Little Endian 
    LCD_CommandData(ST7789_COLMOD, 0x55);
	  LCD_Command(ST7789_INVOFF);
    LCD_Command(ST7789_NORON);
    LCD_Command(ST7789_DISPON);
    LCD_CommandData(ST7789_MADCTL, ST7789_MADCTL_MX | ST7789_MADCTL_MV);
	  LCD_Clear(0);
    LCD_DisplayUpdate();
	  //LCD_Time = millis();
	  LCD_FontLoad(Font_UTF8_16);
    xTaskCreate(LCD_DisplayTask, "Display", 1024, NULL, 1, NULL);
}

void LCD_DataClear(){ //Clear Frame Buffer
    uint32_t i;
    for(i = 0; i < LCD_PixelMax; i++){
        LCD_Data[i] = 0;
    }
}

void LCD_LineClear(uint16_t start, uint16_t line){ //Clear Line Frame Buffer
    uint32_t i, e;
    i = start * LCD_W_Limit;
    e = i + (line * LCD_W_Limit);
    for(; (i < LCD_PixelMax) && (i < e); i++){
        LCD_Data[i] = 0;
    }
}

void LCD_Clear(uint16_t color){ //Fill Frame Buffer
    uint32_t i;
    for(i = 0; i < LCD_PixelMax; i++){
        LCD_Data[i] = color;
    }
	  LCD_Background = 0x80000000UL | color;
	  LCD_DrawInit(0, 0, LCD_W_Limit, LCD_H_Limit);
	  LCD_Flag = 1;
}

void LCD_WritePixel(uint16_t x, uint16_t y, uint16_t color){
	  if(x >= LCD_W_Limit || y >= LCD_H_Limit)return;
	  uint32_t index = y * LCD_W_Limit + x;
	  LCD_Data[index] = color;
}

void LCD_Fill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color){
    int i, j, index;
    for(i = y; i < h+y; i++){
        if(i >= LCD_H_Limit)break;
        index = i * LCD_W_Limit;
        for(j = x; j < w+x; j++){
            if(j >= LCD_W_Limit)break;
            LCD_Data[index+j] = color;
        }
    }
}

void LCD_DrawPixel(int x, int y, uint16_t color){
    if(x >= LCD_X_Limit || y >= LCD_Y_Limit)return;
    uint32_t index = y * LCD_W_Limit + x;
    LCD_Data[index] = color;
}

void LCD_DrawFill(int w, int h, uint16_t color){
    int x, y, index;
    for(y = LCD_Y_Start; y < h+LCD_Y_Start; y++){
        if(y >= LCD_Y_Limit)break;
        index = y * LCD_W_Limit;
        for(x = LCD_X_Start; x < w+LCD_X_Start; x++){
            if(x >= LCD_X_Limit)break;
            LCD_Data[index+x] = color;
        }
    }
}

void LCD_SetBgColor(uint16_t color){
    if(!color)LCD_BgColor = LCD_Background;
    else LCD_BgColor = color;
}

void LCD_SetBgTrans(){
    LCD_BgColor = 0;
}

void LCD_SetColor(uint16_t color){
    LCD_Color = color;
}

void LCD_SetColorRGB(uint8_t r, uint8_t g, uint8_t b){
    LCD_Color = (((uint32_t)r << 8)&0xF800) | (((uint32_t)g << 3)&0x07E0) | ((uint32_t)b >> 3);
}

void LCD_Color24bToRGB(uint32_t c, uint8_t *r, uint8_t *g, uint8_t *b){
    *r = c >> 16;
    *g = c >> 8;
    *b = c;
}

void LCD_Color16bToRGB(uint16_t c, uint8_t *r, uint8_t *g, uint8_t *b){
    *r = (c>>8) & 0xF8;
    *g = (c>>3) & 0xFC;
    *b = (c<<3) & 0xF8;
}


uint16_t LCD_ColorTo16b(uint32_t c){
    return ((c>>8)&0xF800) | ((c>>5)&0x07E0) | ((c>>3)&0x001F);
}

uint32_t LCD_ColorTo24b(uint16_t c){
    return ((uint32_t)(c&0xF800)<<8) | ((uint32_t)(c&0x07E0)<<5) | ((uint32_t)(c&0x001F)<<3);
}

void LCD_FontClose(){
    if(Font_Table){
		    free(Font_Table);
		    Font_Table = NULL;
    }
	  if(Font_Data){
		    free(Font_Data);
		    Font_Data = NULL;
	  }
	  FontFlash = NULL;
}

bool LCD_FontLoad(const uint8_t *font){
    if(font == FontFlash)return 1;
    uint32_t Size;
    FontFlash = font;
    memcpy_P((uint8_t*)&Font_Header, FontFlash, 8);
    if(Font_Header.Magic[0] == 'F' && Font_Header.Magic[1] == 'V'){ //Not support font Hori
		    if(!Font_Header.Number || Font_Header.Number >= 512 || Font_Header.TableS != 8)goto CLOSE;
        Size = (uint32_t)Font_Header.Number * Font_Header.TableS;
		    Font_Table = (uint32_t*)realloc(Font_Table, Size);
		    if(!Font_Table)goto CLOSE;
        LCD_FontDataMax = 128;
		    memcpy_P((uint8_t*)&Font_Table[0], &FontFlash[8], Size);
		    if(!Font_Data)Font_Data = (uint8_t*)malloc(LCD_FontDataMax);
		    return 1;
	  }
	  CLOSE: LCD_FontClose();
	  return 0;
}

uint32_t LCD_UnicodeIndex(FONT_HEADER *font, uint32_t *table, uint32_t utf8){
    uint32_t i, s, t = 0;
    s = font->TableS >> 2;
    for(i = 0; i < font->Number; i++){
        if(table[t] == utf8)return i;
		    t += s;
    }
    return 0x80000000; //Error
}

//uint32_t LCD_UnicodeIndex(uint32_t utf8){
//    return LCD_UnicodeIndex(&Font_Header, Font_Table, utf8);
//}

uint32_t LCD_UnicodeString(char *str, uint32_t *index){
    uint32_t code, i = *index;
    uint8_t c = str[i++];
    code = c;
    if((c & 0xC0) == 0xC0){
        code |= (uint32_t)str[i++] << 8;
        if((c & 0xE0) == 0xE0)code |= (uint32_t)str[i++] << 16;
        if((c & 0xF0) == 0xF0)code |= (uint32_t)str[i++] << 24;
    }
    *index = i;
    return code;
}


bool LCD_DrawGraphicVertical(uint8_t *data, int width, int height){ //data byte dọc
    uint32_t i, d=0, y, h, b, w;
    uint32_t nb = (height + 7) >> 3;
    //DRAW
    for(w=0; w < width; w++){
      y = LCD_Y_Start;
      i = w * nb;
      //d = 0;
      for(h=0; h < height; h++){
          b = h % 8;
          if(!b)d = data[i + (h >> 3)];
          if(d & (1 << b))LCD_Data[y * LCD_W_Limit + LCD_X_Start] = LCD_Color;
          else if(LCD_BgColor)LCD_Data[y * LCD_W_Limit + LCD_X_Start] = LCD_BgColor; 
          if(++y >= LCD_Y_Limit)break;
      }
      if(++LCD_X_Start >= LCD_X_Limit)return 0;
    }
    return 1;
}

bool LCD_DrawUnicodeChar(uint32_t c){
    uint32_t w=5, h, s=0;
    LCD_Wait(); //Check lock
    //GET FONT
    c = LCD_UnicodeIndex(&Font_Header, Font_Table, c);
    //FONT TABLE
    h = Font_Header.Height;
    if(c < 512){ //max 511 font
        if(Font_Header.TableS == 8){
            c *= 2;
            c = Font_Table[c + 1];
            w = c >> 24;
			      s = ((h + 7) >> 3) * w;
            c = Font_Header.Offset + (c & 0xFFFFFF);
        }else return 0;
    }
    //DRAW DATA
    if(s == 0){
        if(LCD_BgColor)LCD_DrawFill(w, h, LCD_BgColor);
        LCD_X_Start += w;
        if(LCD_X_Start >= LCD_X_Limit)return 0;
        else return 1;
    }else{
		    if(s > LCD_FontDataMax){ //auto
			      LCD_FontDataMax = s;
			      Font_Data = (uint8_t*)realloc(Font_Data, s);
		    }
		    memcpy_P(Font_Data, FontFlash + c, s);
		    return LCD_DrawGraphicVertical(Font_Data, w, h);
        //1-next, 0-exit
    }
}

void LCD_DrawInit(int x, int y, int w, int h){ //set goto x-y, w-h
    LCD_X_Start = x; //X
    LCD_Y_Start = y; //Y
    LCD_W_Value = w;
    LCD_H_Value = h;
    LCD_X_Limit = LCD_X_Start + LCD_W_Value;
    LCD_Y_Limit = LCD_Y_Start + LCD_H_Value;
    w = LCD_W_Limit;
    if(LCD_X_Limit > w){
        LCD_W_Value -= LCD_X_Limit - w;
        LCD_X_Limit = w;
    }
    h = LCD_H_Limit;
    if(LCD_Y_Limit > h){
        LCD_H_Value -= LCD_Y_Limit - h;
        LCD_Y_Limit = h;
    }
}

bool LCD_DrawText(char *str, int len){
    if(!Font_Table)return 0;
    uint32_t i = 0; bool e = 1;
    while(str[i] && i < len){ //STR NULL --> Exit 1
        if(!LCD_DrawUnicodeChar(LCD_UnicodeString(str, &i))){ e = 0; break; } //X LIMIT --> Exit 0
    }
    LCD_Flag = 1;
    return e;
}

bool LCD_DrawString(char *str){
    if(!Font_Table)return 0;
    uint32_t i = 0; bool e = 1;
    while(str[i]){ //STR NULL --> Exit 1
        if(!LCD_DrawUnicodeChar(LCD_UnicodeString(str, &i))){ e = 0; break; } //X LIMIT --> Exit 0
    }
    LCD_Flag = 1;
    return e;
}

void LCD_DisplayTask(void * parameter){
    while(1){
        if(LCD_Flag){ //5Hz
            LCD_DisplayUpdate(); //170ms
            delay(30);
        }else{
            delay(200);
        }
    }
}

/*void LCD_UpdateTask(){ //5Hz
	  uint32_t ms = millis();
	  if((uint32_t)(ms - LCD_Time) >= 200){ //200ms
		    LCD_Time = ms;
		    if(LCD_Flag)LCD_DisplayUpdate();
	  }
}*/
