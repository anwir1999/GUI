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

// radio channel gom co: ten kenh, tan so doi voi dab va fm
// url doi voi internet radio
typedef struct 
{
  String channel_name;
  String frequency;
  String url;
}radio_channel_t;

radio_channel_t favorite_channel[100];

radio_channel_t internet_channel[100];
radio_channel_t api_channel[100];
radio_channel_t history_channel[100];

radio_channel_t fm_channel[100];
radio_channel_t dab_channel[100];

radio_channel_t broadcast_channel_now;
int fm_length, dab_length, internet_length, favorite_length, api_length, history_length; // do dai list channel kenh
// quan ly trang thai cua broadcast
typedef enum
{
  RADIO_STOP,
  RADIO_PLAY,
}radio_states_t;
//quan ly trạng thái on/off của radio
typedef enum
{
  RADIO_OFF,
  RADIO_ON,
}radio_power_t;

//dinh nghia src pointerz
typedef struct 
{
  uint8_t mainMenu;// defaul = 1, 1- favorite, 2 - internetradio, 3 - fm radio,4 - DAB radio, 5 - setting
  uint8_t subMenu1;//defaul = 0, 0 - no submenu, 1 - kenh 1, 2 - kenh 2
  uint8_t subMenu2; // defaul = 0, 0 - no submenu2
  uint8_t subMenu3; // defaul = 0;
}scr_pointer_t;

// kieu broadcast cua radio
typedef enum
{
  BS_NOPE,
  BS_INTER,
  BS_FM,
  BS_DAB,
}broad_type_t;

//wifi
typedef struct 
{
  String ssid;
  String password;
  String rssi;// do manh
  String ip;
  String bssid;
  int wifi_status; // connect/disconnect 
}wifi_information_t;

wifi_information_t wifi_ip;
typedef struct
{
  uint8_t time_hour;
  uint8_t time_minutes;
  uint8_t time_second;
  uint8_t date_day;
  uint8_t date_month;
  uint8_t date_year; // chỉ có số cuối của năm vd 22 = 2022;
  uint8_t timeplay_minutes;
  uint8_t timeplay_second;
}radio_time_t;
radio_time_t timeline;
typedef enum
{
  Flat=0,
  Rock, 
  Jazz, 
  Pop, 
  Classical, 
  Ancing, 
  HeavyMeta, 
  Disco, 
  Live, 
  Soft, 
  Hall, 
  Advance
}radio_equalizer_mode_t;

typedef struct 
{
  uint8_t preamp_gain;
  uint8_t freq_105hz;
  uint8_t freq_300hz;
  uint8_t freq_850hz;
  uint8_t freq_2400hz;
  uint8_t freq_6900hz;
  uint8_t freq_3d_depth;
}radio_equalizer_frequency_t;

//uint32_t SPIFFS_Size, OTA_Size, EEPROM_Address, Chip_Id;

//String ssid;                        //string variable to store ssid
//String pss;                         //string variable to store 

broad_type_t broad_type = BS_NOPE;// kieu broadcast cua radio

scr_pointer_t lcd_pointer = {1,0,0,0}; // mac dinh

radio_states_t state_radio_bs = RADIO_STOP;
bool flag_start_radio = false; // gia tri co de bat radio 1 lan;
bool flag_stop_radio = true; // gia tri co de bat radio 1 lan;
bool flag_fm_scan = false;
bool flag_start_scan = false; // man hinh loadding 
bool flag_ota_start = false;
int volume = 0; // gia tri volume cua radio
unsigned long currentMillis;
unsigned long previousMillis = 0;
unsigned long interval = 30000;
radio_power_t status_power = RADIO_ON;// mac dinh power khi mo la on
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
