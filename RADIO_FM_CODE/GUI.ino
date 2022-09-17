#include <TFT_eSPI.h>
#include <Arduino.h>
#include "ui/menuItem.h"
#include "ui/IR.h"
#include "ui/bitmapItem.h"
#include "main.h"
#define CALIB_DEBOUNCE 10
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite frame_170 = TFT_eSprite(&tft);
TFT_eSprite frame_30 = TFT_eSprite(&tft);
TFT_eSprite frame_40 = TFT_eSprite(&tft);
TFT_eSprite frame_VL = TFT_eSprite(&tft);
TFT_eSprite frame_FS = TFT_eSprite(&tft);
uint32_t timeDebounce = 0, timeDebounceRotacy = 0, timeFrameAbv = 0, timeDisplayVL= 0;
uint8_t  sub_over = 0, sub_num = 0;
uint8_t lstNumSub3 = 5, lstNumSub2 =5, lstNumSub1 =5;
uint8_t timeBSS = 0, timeBSM = 0, hour = 0, second = 0, minute = 2;
uint8_t count = 0; 
int volume = 0;
int deltaMenu = 0;
//dinh nghia src pointer
char lcd_buff[128];
bool Flag_count_broadcast = false, displayVL = false;
//khoi tao
scr_pointer_t lcd_pointer = {1,0,0,0}; // mac dinh
radio_power_t status_power = RADIO_ON;
const char* mainName[] = {"FAVORITE","INTERNET","FM","DAB","SETTING"};
const char* subChanelInter[] = {"VOV1","VOV2","VOV3","VOV4","VOV5","VOV6"};
const char* subInter[] = {"VOV","Search(Skytune)","History"};
const char* subSetting[] = {"Setting Wifi","Equalizer","Information","Wifi Inform","Set Languge"};
const char* subWifi[] = {"Smart Config","AP Config"};
const char* subLang[] = {"English","Chinese","Spanish"};
uint32_t but = 0, previousBut = 0;
bool checkBut = true;
bool Flag_start_connect = false;
typedef enum
{
  BS_NOPE,
  BS_INTER,
  BS_FM,
  BS_DAB,
}broad_type_t;
const char *typeBroadCast[] = {"internet", "fm", "dab"};
broad_type_t broad_type = BS_NOPE;
void init_gui()
{
  tft.init();
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setRotation(1);
  tft.setTextSize(2);
  tft.setSwapBytes(true);
  tft.invertDisplay(0);
  tft.fillScreen(TFT_BLACK);

  frame_170.setTextColor(TFT_YELLOW, TFT_BLACK);
  frame_170.setTextSize(2);
  frame_170.createSprite(320, 170);
  frame_170.setSwapBytes(1);

  frame_40.setTextColor(TFT_YELLOW, TFT_BLACK);
  frame_40.setTextSize(2);
  frame_40.createSprite(320, 40);

  frame_30.setTextColor(TFT_YELLOW, TFT_BLACK);
  frame_30.setTextSize(2);
  frame_30.createSprite(320, 30);
  
  
  indicateMenu(lcd_pointer);
  
  timeDebounce = millis();
  timeDebounceRotacy = millis();
  log_d("Total heap: %d", ESP.getHeapSize());
  log_d("Free heap: %d", ESP.getFreeHeap());
}
void scanButton()
{
  ENC_RotaryTask();
  if(millis() - timeDebounceRotacy > 200)
  {
    timeDebounceRotacy = millis();
     uint32_t but2 = Get_Rotary();
     if(but2)
     {
        printRotacy(but2);
     }
  }
  but = Get_KeyPress(); 
  if(but == previousBut && but && !checkBut)
  { 
    if(millis() - timeDebounce > CALIB_DEBOUNCE)
    {
      Serial.println("-------------");
      timeDebounce = millis();
      previousBut = 0;
      checkBut = true;
      processButton(but);
    }
  }
  else if(but != 0)
  {
    previousBut = but;
    timeDebounce = millis();
  }
  else if(but == 0)
  {
    checkBut = false;
    timeDebounce = millis();
  }
  if(millis() - timeFrameAbv > 1000)
  {
    timeFrameAbv = millis();
    second++;
    if(second > 59)
    {
      second = 0;
      minute++;
      if(minute > 59)
      {
        minute = 0;
        hour++;
        if(hour > 23)
        {
          hour = 0;
        }
      }
    }
    if(status_power == RADIO_ON)
    {
      if(WiFi.status() != WL_CONNECTED)
      {
        count++;
        if(count%2 == 0)  displaySignal(0);
        else displaySignal(1);
      }
      else
      {
        displaySignal(1);
      }
  
      if(Flag_start_connect)
      {
        timeInvert--;
        displayCountDown();
        if(WiFi.status() == WL_CONNECTED)
        {
          Flag_start_connect = false;
          wifiDisplayPopup("fail");
        }
        else if(WiFi.status() != WL_CONNECTED && timeInvert <= 0)
        {
          Flag_start_connect = false;
          wifiDisplayPopup("succes");
        }      
      }
  
      if(broad_type != BS_NOPE)
      {
        timeBSS++;
        if(timeBSS > 59)
        {
          timeBSS = 0;
          timeBSM++;
          if(timeBSM > 59)
          {
            timeBSM = 0;
          }
        }
       int x = 215;
       if(timeBSM< 10)  x+=frame_40.drawNumber(0,x,23,2);
       x+=frame_40.drawNumber(timeBSM,x,23,2);
       x+=frame_40.drawChar(':',x,5,2); 
       if(timeBSS < 10) x+=frame_40.drawNumber(0,x,23,2);
       x+=frame_40.drawNumber(timeBSS,x,23,2);
       frame_40.pushSprite(0,200);
      }
    }
    else
    {
      indicateClockPWR();
    }
  }
  if(displayVL)
  {
    if(millis() - timeDisplayVL >= 2000)
    {
      displayVL = false;
      indicateMenu(lcd_pointer);
    }
  }
}
void indicateMenu(scr_pointer_t lcd_pointer)
{
  char stt[10];
  if(!lcd_pointer.subMenu1)
  {
    mainMenuIndicate(mainMenuItem[lcd_pointer.mainMenu - 1], mainName[lcd_pointer.mainMenu - 1]);
  }
  else
  {
    switch(lcd_pointer.mainMenu)
    {
      case 1:
        break;
      case 2:
        if(!lcd_pointer.subMenu2)
        {
          subMenuIndicate((char *)mainName[lcd_pointer.mainMenu - 1], subInter, lcd_pointer.subMenu1, lstNumSub1);
        }
        else
        {
          switch(lcd_pointer.subMenu1)
          {
            case 1:
              if(!lcd_pointer.subMenu3)
              {
                subChannelIndicate((char *)subInter[lcd_pointer.subMenu1 - 1], subChanelInter, lcd_pointer.subMenu2, lstNumSub2, music_ic);
              }
              else
              {
                broad_type = BS_INTER;
              }
              break;
            case 2:
              break;
            case 3:
              break;
          }
        }
        break;
      case 5:
          if(!lcd_pointer.subMenu2)
          {
            subMenuIndicate((char *)mainName[lcd_pointer.mainMenu - 1], subSetting, lcd_pointer.subMenu1, lstNumSub1);
          }
          else
          {
            switch(lcd_pointer.subMenu1)
            {
              case 1:                
                if(!lcd_pointer.subMenu3)
                {
                 subMenuIndicate((char *)subSetting[lcd_pointer.subMenu1 - 1], subWifi, lcd_pointer.subMenu2, lstNumSub2);
                }
                else
                {
                  if(lcd_pointer.subMenu2 == 1)
                  {
                    // ham xu ly smart config
                    if(WiFi.status() != WL_CONNECTED)
                    { 
                      timeInvert = 120;
                      Wifi_Smartconfig();
                    }
                  }
                  else if(lcd_pointer.subMenu2 == 2)
                  {
                     //ham xu ly ap smart config
                  }
                }
                break;
              case 2:
                break;
              case 3:
                break;
              case 4:
                break;
              case 5:
                if(!lcd_pointer.subMenu3)
                {
                  subMenuIndicate((char *)subSetting[lcd_pointer.subMenu1 - 1], subLang, lcd_pointer.subMenu2, lstNumSub2);
                }
                break;
            }
          }
          break;
      }
  }
  if(broad_type != BS_NOPE)
  {
    switch(broad_type)
    {
      case BS_INTER:
        broadcastInter();
      case BS_DAB:
        break;
    }
  }
}
void processButton(uint32_t but)
{
  if(but == KEY_POWER)
  {
      if(status_power == RADIO_ON)
      {
        if(broad_type != BS_NOPE)
        {
          frame_170.fillSprite(TFT_BLACK);
          frame_170.setTextDatum(MC_DATUM);
          frame_170.drawString("Stopping...", 160, 85, 2);
          frame_170.pushSprite(0, 70);
          delay(500);
          frame_170.drawString("Stream stopped", 160, 85, 2);
          frame_170.pushSprite(0, 70);
          delay(500);
          lcd_pointer.subMenu3 = 0;
          broad_type = BS_NOPE;
          timeBSS = 0;
          timeBSM = 0;
        }
        int x = 0;
        tft.setTextDatum(MC_DATUM);
        tft.fillScreen(TFT_BLACK);
        x = tft.drawString("sleep now", 160, 120, 2);
        tft.drawRect(160 - x/2-10, 100, x+20, 40, TFT_YELLOW);
        delay(500);
        status_power = RADIO_OFF;
      }
      else
      {
        status_power = RADIO_ON;
      }
      Serial.println("key power");
  }
  if(status_power == RADIO_ON)
  {
    switch(but)
    {
    case KEY_LEFT:
      if(lcd_pointer.subMenu3)
      {
        lcd_pointer.subMenu3 = 0;
        broad_type = BS_NOPE;
        timeBSS = 0;
        timeBSM = 0;
        Flag_start_connect = false;
      }
      else if (lcd_pointer.subMenu2)
      {
        lcd_pointer.subMenu2 = 0;
      }
      else if (lcd_pointer.subMenu1)
      {
        lcd_pointer.subMenu1 = 0;
      }
      Serial.println("key left");
      break;
    case KEY_RIGHT:
      getListSub();
      Serial.println("key right");
      break;
    case KEY_OK:
      getListSub();
      Serial.println("key ok");
      break;
    default:
      break;
    }
    sub_over = 0;
    indicateMenu(lcd_pointer);
  }
}
void printRotacy(uint32_t but)
{
  switch(but)
  {
    case KEY_UP:
      deltaMenu = 1;
      Serial.println("key up");
      break;
    case KEY_DOWN:
      deltaMenu = -1;
      Serial.println("key down");
      break;
    default:
      break;
  }
  if(status_power == RADIO_ON)
  {
    if(broad_type == BS_NOPE)
    {
      if(lcd_pointer.subMenu3)
      {
        lcd_pointer.subMenu3 += deltaMenu;
        if(lcd_pointer.subMenu3 > lstNumSub3)
        {
          lcd_pointer.subMenu3 = 1;
        }
        else if(lcd_pointer.subMenu3 < 1)
        {
          lcd_pointer.subMenu3 = lstNumSub3;
        }
      }
      else if(lcd_pointer.subMenu2)
       {
         lcd_pointer.subMenu2+= deltaMenu;
         if(lcd_pointer.subMenu2 > lstNumSub2)
         {
           lcd_pointer.subMenu2 = 1;
         }
        else if(lcd_pointer.subMenu2 < 1)
        {
          lcd_pointer.subMenu2= lstNumSub2;
        }
        if(lcd_pointer.subMenu2 > 5)
        {
           sub_over = lcd_pointer.subMenu2 - 5;
        }
        else
        {
         sub_over = 0;
        }
       }
       else if(lcd_pointer.subMenu1)
       {
         lcd_pointer.subMenu1+= deltaMenu;
         if(lcd_pointer.subMenu1 > lstNumSub1)
         {
           lcd_pointer.subMenu1 = 1;
         }
        else if(lcd_pointer.subMenu1 < 1)
        {
          lcd_pointer.subMenu1 = lstNumSub1;
        }
         if(lcd_pointer.subMenu1 > 5)
         {
            sub_over = lcd_pointer.subMenu1 - 5;
         }
         else
         {
          sub_over = 0;
         }
       }
       else if(lcd_pointer.mainMenu)
       {
         lcd_pointer.mainMenu+= deltaMenu;
         if(lcd_pointer.mainMenu > 5)
         {
           lcd_pointer.mainMenu = 1;
         }   
        else if(lcd_pointer.mainMenu < 1)
        {
          lcd_pointer.mainMenu = 5;
        }
       }
      indicateMenu(lcd_pointer);
    }
    else
    {
      displayVL = true;
      timeDisplayVL  = millis();
      volume += deltaMenu;
      Serial.println(volume);
      if(volume >24)
      {
        volume = 24;
      }
      else if(volume < 0)
      {
        volume = 0;
      }
      displayVolume();
    }
  }
}
void getListSub()
{
  switch(lcd_pointer.mainMenu)
  {
    case 5:
      switch(lcd_pointer.subMenu1)
      {
        case 1:
          if(!lcd_pointer.subMenu2)
          {
             lcd_pointer.subMenu2 = 1;
             lstNumSub2 = 2;            
          }
          else
          {
             lcd_pointer.subMenu3 = 1;
          }
          break;
        case 2:
          lstNumSub2 = 2;
          break; 
        case 3:
          lstNumSub2 = 3;
          break;
        case 4:
          lstNumSub2 = 5;
          break;
        case 5:
          if(!lcd_pointer.subMenu2)
          {
             lcd_pointer.subMenu2 = 1;
             lstNumSub2 = 3;            
          }
          else
          {
             lcd_pointer.subMenu3 = 1;
          }
          break;
        default:
          lcd_pointer.subMenu1 = 1;
          lstNumSub1 = 5;
          break;
      }
      break;
    case 2:
      switch(lcd_pointer.subMenu1)
      {
        case 1:
          if(!lcd_pointer.subMenu2)
          {
            lcd_pointer.subMenu2 = 1;
            lstNumSub2 = 6;
          }
          else
          {
            lcd_pointer.subMenu3 = 1;
          }
          break;
        case 2:
          break;
        case 3:
          break;
        default:
          lcd_pointer.subMenu1 = 1;
          lstNumSub1 = 3;
          break;
      }
      break;
  }
}
void broadcastInter()
{
    
    frame_40.fillSprite(TFT_BLUE);
    frame_40.setTextColor(TFT_WHITE, TFT_BLUE);
    frame_40.setTextSize(1);
    frame_40.setTextDatum(MC_DATUM);
    frame_40.drawString(subChanelInter[lcd_pointer.subMenu2 - 1], 160, 23,4);
    frame_40.pushSprite(0,30);

    frame_170.fillSprite(TFT_BLACK);
    frame_170.setTextDatum(ML_DATUM);
    frame_170.setTextColor(TFT_YELLOW, TFT_BLACK);
    frame_170.pushImage(29,29,72,72,IR);
    frame_170.drawString("Other Talk",140,23,2);
    frame_170.drawString("ACC,40Kbps",140,57,2);
    frame_170.drawString("48000Hz,1ch",140,91,2);
    frame_170.pushSprite(0,70);

    frame_40.fillSprite(TFT_BLACK);
    frame_40.setTextDatum(ML_DATUM);
    frame_40.setTextColor(TFT_YELLOW, TFT_BLACK);
    frame_40.setTextSize(2);
    frame_40.drawChar('<', 5, 5,2);
    frame_40.drawChar('>', TFT_HEIGHT - 17, 5,2);
    int x = 215;
    if(timeBSM< 10)  x+=frame_40.drawNumber(0,x,23,2);
    x+=frame_40.drawNumber(timeBSM,x,23,2);
    x+=frame_40.drawChar(':',x,5,2); 
    if(timeBSS < 10) x+=frame_40.drawNumber(0,x,23,2);
    x+=frame_40.drawNumber(timeBSS,x,23,2);
    frame_40.pushSprite(0,200);  
}
void displaySignal(int mode)
{
  int color1 = 0, color2 = 0;
  if(mode)
  {
  }
  frame_30.fillSprite(TFT_BLACK);
  frame_30.drawXBitmap(10, 12, signal_ic, 24, 24, TFT_YELLOW);
  frame_30.fillRect(58,6,4,24,TFT_YELLOW);
  frame_30.fillRect(52,12,4,18,TFT_YELLOW);
  frame_30.fillRect(46,18,4,12,TFT_YELLOW);
  frame_30.fillRect(40,24,4,6,TFT_YELLOW);
  
  if(!mode)
  {
    frame_30.drawLine(40, 6, 60, 30, TFT_RED);
    frame_30.drawLine(41, 6, 61, 30, TFT_RED);
    frame_30.drawLine(42, 6, 62, 30, TFT_RED);
  
    frame_30.drawLine(40, 30, 60, 6, TFT_RED);
    frame_30.drawLine(41, 30, 61, 6, TFT_RED);
    frame_30.drawLine(42, 30, 62, 6, TFT_RED);
  }
  if(broad_type == BS_NOPE)
  {
    frame_30.setTextDatum(TL_DATUM);
    int x = 240, y = 0;
    if (hour < 10) x += frame_30.drawChar('0', x, y, 2); // Add hours leading zero for 24 hr clock
    x += frame_30.drawNumber(hour, x, y, 2);             // Draw hours
    x += frame_30.drawChar(':', x, 5, 2);
    if (minute < 10) x += frame_30.drawChar('0', x, y, 2); // Add minutes leading zero
    x += frame_30.drawNumber(minute, x, y, 2);             // Draw minutes
  }
  else
  {
    frame_30.setTextDatum(MR_DATUM);
    frame_30.drawString(typeBroadCast[broad_type-1], TFT_HEIGHT - 5, 18, 2);
  }
  frame_30.pushSprite(0,0);
}
void displayVolume()
{
   frame_VL.createSprite(160, 50);
   frame_VL.setTextSize(2);
   frame_VL.setTextDatum(MC_DATUM);
   frame_VL.fillSprite(TFT_BLUE);
   frame_VL.setTextColor(TFT_WHITE, TFT_BLUE);
   frame_VL.drawXBitmap(70, 0, headphone_ic,16,16, TFT_WHITE, TFT_BLUE);
   frame_VL.fillRect(20, 20,volume*5,30, TFT_WHITE);
   frame_VL.drawString("-",10,35,4);
   frame_VL.drawString("+",150,35,4);
   frame_VL.pushSprite(80, 105);
}
void displayCountDown()
{
  int x = 153;
  frame_40.fillSprite(TFT_BLUE);
  frame_40.setTextColor(TFT_WHITE, TFT_BLUE);
  frame_40.setTextSize(1);
  frame_40.setTextDatum(ML_DATUM);
  frame_40.drawString(subWifi[lcd_pointer.subMenu2 - 1], 5, 23,4);
  frame_40.pushSprite(0, 30);
  frame_170.fillSprite(TFT_BLACK);
  frame_170.setTextDatum(MC_DATUM);
  frame_170.setTextColor(TFT_WHITE, TFT_BLACK);
  frame_170.setTextSize(3);
  x += frame_170.drawNumber(timeInvert, x, 85, 1);
  frame_170.drawString("S", x-10, 85, 1);
  frame_170.drawCircle(160, 85, 60, TFT_YELLOW);
  frame_170.setTextSize(2);
  frame_170.pushSprite(0, 70);
}
void wifiDisplayPopup(char *str)
{
  frame_170.fillSprite(TFT_BLACK);
  frame_170.setTextDatum(MC_DATUM);
  frame_170.setTextColor(TFT_YELLOW, TFT_BLACK);
  frame_170.setTextSize(2);
  frame_170.drawString(str, 160, 98, 2);
  frame_170.drawString("Wifi config", 160, 72, 2);
  frame_170.drawRect(82, 58,160, 64,TFT_YELLOW);
  frame_170.pushSprite(0, 70);
}
void mainMenuIndicate(const short unsigned int *mainItem, const char *mainMenuString)
{
    frame_170.setTextDatum(MC_DATUM);
    frame_170.fillSprite(TFT_BLACK);
    frame_170.setTextColor(TFT_YELLOW, TFT_BLACK);
    frame_170.pushImage(0,-35,320,240,mainItem);
    frame_170.drawString(mainMenuString, 160, 145,2);
    frame_170.pushSprite(0, 30);
    frame_40.fillSprite(TFT_BLACK);
    frame_40.pushSprite(0,200);
}
void subMenuIndicate(char *label, const char *subMenuList[],int subMenu, int subMenuListLength)
{
  int lengthList = 0;
  frame_40.fillSprite(TFT_BLUE);
  frame_40.setTextColor(TFT_WHITE, TFT_BLUE);
  frame_40.setTextSize(1);
  frame_40.setTextDatum(ML_DATUM);
  frame_40.drawString(label, 5, 23,4);
  char stt[10];
  frame_40.pushSprite(0, 30);
  frame_170.fillSprite(TFT_BLACK);
  frame_170.setTextDatum(ML_DATUM);
  frame_170.setTextColor(TFT_WHITE, TFT_BLACK);
  if(subMenuListLength > 5)
  {
    lengthList = 5;
  }
  else
  {
    lengthList = subMenuListLength;
  }
  for(int i = 0; i < lengthList; i++)
  {
    frame_170.drawString(subMenuList[i + sub_over], 20, 17 + 34*i ,2);
  }
  frame_170.setTextColor(TFT_BLACK, TFT_YELLOW);
  frame_170.fillRect(0,34*(subMenu - 1 - sub_over),320,34,TFT_YELLOW);
  frame_170.drawString(subMenuList[subMenu - 1 ], 20, 17 + 34*(subMenu - 1 - sub_over),2);
  sprintf(stt, "%d/%d", subMenu,subMenuListLength);
  frame_170.pushSprite(0, 70);
  frame_40.setTextDatum(MR_DATUM);
  frame_40.drawString(stt, 310, 23,4);
  frame_40.pushSprite(0, 30);
}
void subChannelIndicate(char *label, const char *subMenuList[],int subMenu, int subMenuListLength, const unsigned char bitmapItem[])
{
  int lengthList = 0;
  frame_40.fillSprite(TFT_BLUE);
  frame_40.setTextColor(TFT_WHITE, TFT_BLUE);
  frame_40.setTextSize(1);
  frame_40.setTextDatum(ML_DATUM);
  frame_40.drawString(label, 5, 23,4);
  char stt[10];
  frame_40.pushSprite(0, 30);
  frame_170.fillSprite(TFT_BLACK);
  frame_170.setTextDatum(ML_DATUM);
  frame_170.setTextColor(TFT_WHITE, TFT_BLACK);
  if(subMenuListLength > 5)
  {
    lengthList = 5;
  }
  else
  {
    lengthList = subMenuListLength;
  }
  for(int i = 0; i < lengthList; i++)
  {
    frame_170.drawString(subMenuList[i + sub_over], 35, 17 + 34*i ,2);
    frame_170.drawXBitmap(5, 5 + 34*i, bitmapItem,24,24, TFT_WHITE, TFT_BLACK );
  }
  frame_170.setTextColor(TFT_BLACK, TFT_YELLOW);
  frame_170.fillRect(0,34*(subMenu - 1 - sub_over),320,34,TFT_YELLOW);
  frame_170.drawString(subMenuList[subMenu - 1 ], 35, 17 + 34*(subMenu - 1 - sub_over),2);
  frame_170.drawXBitmap(5, 5 + 34*(subMenu - 1 - sub_over), bitmapItem,24,24, TFT_BLACK, TFT_YELLOW );
  sprintf(stt, "%d/%d", subMenu,subMenuListLength);
  frame_170.pushSprite(0, 70);
  frame_40.setTextDatum(MR_DATUM);
  frame_40.drawString(stt, 310, 23,4);
  frame_40.pushSprite(0, 30);
}
void indicateClockPWR()
{
  frame_FS.createSprite(320, 240);
  frame_FS.fillScreen(TFT_BLACK);
  frame_FS.setTextDatum(TL_DATUM);
  int x = 40, y = 90;
  if (hour < 10) x += frame_FS.drawChar('0', x, y, 8); // Add hours leading zero for 24 hr clock
  x += frame_FS.drawNumber(hour, x, y, 8);             // Draw hours
  x += frame_FS.drawString(":", x, y-10, 8);
  if (minute < 10) x += frame_FS.drawChar('0', x, y, 8); // Add minutes leading zero
  x += frame_FS.drawNumber(minute, x, y, 8);             // Draw minutes
  frame_FS.pushSprite(0,0);
}
