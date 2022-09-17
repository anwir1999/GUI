#ifndef __MAIN_H
#define __MAIN_H
//#define DEBUG_SERIAL      1
#define DEBUG_SYSTEM      1
#define DEBUG_PORT        Serial
#define FFS_SYSTEM        SPIFFS

//const char SplitChar = '*';
const char auth_user[] = "admin", TextHtml[] = "text/html";

char IP_Address[16];
//int Set_Cookie;
uint32_t SPIFFS_Size, OTA_Size, EEPROM_Address, Chip_Id;
//      KEY_NAME    KEY_CODE
#define KEY_POWER   0x10
#define KEY_MENU    0x11
#define KEY_LEFT    0x12
#define KEY_RIGHT   0x13
#define KEY_UP      0x14
#define KEY_DOWN    0x15
#define KEY_OK      0x16
#define KEY_HOME    KEY_MENU
#define KEY_BACK    KEY_LEFT
#define KEY_FORWARD KEY_RIGHT
//RTC Struct
typedef struct {
    char Sec;
    char Min;
    char Hour;
    char Day;
    char Date;
    char Month;
    char Year;
    char Temp;
}RTC_TIME;
RTC_TIME  RTC_Time;
typedef struct 
{
  uint8_t mainMenu;// defaul = 1, 1- favorite, 2 - internetradio, 3 - fm radio,4 - DAB radio, 5 - setting
  uint8_t subMenu1;//defaul = 0, 0 - no submenu, 1 - kenh 1, 2 - kenh 2
  uint8_t subMenu2; // defaul = 0, 0 - no submenu2
  uint8_t subMenu3; // defaul = 0;
}scr_pointer_t;
uint32_t timeInvert = 0;
#if(DEBUG_SYSTEM==1)
  #define DEBUG_PRINT(...) {DEBUG_PORT.print(__VA_ARGS__);}
  #define DEBUG_PRINTLN(...) {DEBUG_PORT.println(__VA_ARGS__);}
  //#define DEBUG_PRINTBUFFER(buffer, len) {printBuffer(buffer, len);}
#else
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTLN(...) {}
  //#define DEBUG_PRINTBUFFER(buffer, len) {}
#endif


#endif
