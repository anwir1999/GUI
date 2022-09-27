#include <TFT_eSPI.h>
#include <Arduino.h>
#include "ui/menuItem.h"
#include "ui/IR.h"
#include "ui/skytune.h"
#include "ui/radio_dab.h"
#include "ui/bitmapItem.h"
#include "main.h"

#define CALIB_DEBOUNCE 10
#define FF18 &FreeSans12pt7b
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite frame_170 = TFT_eSprite(&tft);
TFT_eSprite frame_30 = TFT_eSprite(&tft);
TFT_eSprite frame_40 = TFT_eSprite(&tft);
TFT_eSprite frame_VL = TFT_eSprite(&tft);
TFT_eSprite frame_FS = TFT_eSprite(&tft);

uint32_t timeDebounce = 0, timeDebounceRotacy = 0, timeFrameAbv = 0, timeDisplayVL= 0, timeDisplayLoad = 0;
uint32_t timeInvert = 0;// thoi gian dem nguoc qua trinh config WiFi
uint8_t  sub_over = 0; // bien over, khi man hinh qua 5 list, thanh chon mau vang se o vi tri 5
uint8_t lstNumSub3 = 5, lstNumSub2 =5, lstNumSub1 =5;
uint8_t channelSub = 0, channelSubLength = 0;
uint8_t count = 0, count_load = 0; 
String str_load = "loading";
int deltaMenu = 0;
bool displayVL = false;
//khoi tao

String mainName[] = {"FAVORITE","INTERNET","FM","DAB","SETTING"};
String subInter[] = {"VOV","Search(Duc Hung Tech)","History"};
String subSetting[] = {"Setting Wifi","FM Setup", "DAB Setup","Equalizer","Wifi Information","Information","Set Language", "Reset Factory"}; //submenu1
String subWiFi[] = {"Scan Wifi", "Smart Config", "AP Config", "Reset Wifi"};
String subFMSet[] = {"FM Full Scan", "FM Station List"};
String subDABSet[] = {"DAB Full Scan", "DAB Station List"};
String subEqua[] = {"Flat", "Rock", "Jazz", "Pop", "Classical", "Ancing", "Heavy Meta", "Disco", "Live", "Soft", "Hall", "Advance"};
String subEquaAdv[] = {"105Hz", "300Hz", "850Hz", "2.4KHz", "6.9KHz", "3D Depth"};
String subWifiInfor[] = {"IP: ", "SSID: ", "PASSWORD: ", "RSSI: ", "BSSI: "};
String subInformation[] = {"Serial", "Model", "Made In"};
String subLang[] = {"English","Chinese","Spanish"};
// cac bien khoi tao cho qua trinh scan nut nhan
uint32_t but = 0, previousBut = 0;
bool checkBut = true;

bool Flag_start_connect = false; // biet xac nhan ket noi mang, muc dich de hien thi man hinh count dowwn
bool check_forward = true; // kiem tra xem thiet bi co forward sang sub menu tiep theo khong
bool indicate_WI = false;
const char *typeBroadCast[] = {"internet", "fm", "dab"}; // hien thi type khi thiet bi broadcast o goc phai man hinh
void init_gui()
{
  // khoi tao man hinh tft
  tft.init();
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setRotation(1);
  tft.setTextSize(2);
  tft.setSwapBytes(true);
  tft.invertDisplay(0);
  tft.fillScreen(TFT_BLACK);

  // khoi tao cac frame do dai 170  vi du: hien thi list sub
  frame_170.setTextColor(TFT_YELLOW, TFT_BLACK);
  frame_170.setTextSize(2);
  frame_170.createSprite(320, 170);
  frame_170.setSwapBytes(1);

  // khoi tao cac frame do dai 30 VD: hien thi song va dong ho
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
  // doc cong tac vo cap
  ENC_RotaryTask();
  if(millis() - timeDebounceRotacy > 300)
  {
    timeDebounceRotacy = millis();
     uint32_t but2 = Get_Rotary();
     if(but2)
     {
        // xu ly su kien khi cong tac vo cap nhan gia tri
        processRotacy(but2);
     }
  }
  // scan nut nhan theo debounce, gia tri debounce la 10 ms
  but = Get_KeyPress(); 
  if(but == previousBut && but && !checkBut)
  { 
    if(millis() - timeDebounce > CALIB_DEBOUNCE)
    {
      Serial.println("-------------");
      timeDebounce = millis();
      previousBut = 0;
      checkBut = true;
      processButton(but); // xu ly su kien khi scan duoc nut nhan
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

  // xu ly cac man hinh, su kien voi chu ky 1s, vi du: dem thoi gian scan WiFi, dem gio....
  if(millis() - timeFrameAbv > 1000)
  {
    timeFrameAbv = millis();
    RTC_Time.Sec++;
    if(RTC_Time.Sec > 59)
    {
      RTC_Time.Sec = 0;
      RTC_Time.Min++;
      if(RTC_Time.Min > 59)
      {
        RTC_Time.Min = 0;
        RTC_Time.Hour++;
        if(RTC_Time.Hour > 23)
        {
          RTC_Time.Hour = 0;
        }
      }
    }
    if(status_power == RADIO_ON)
    {
      Wifi_GetInfomation();
      if(wifi_ip.wifi_status != WL_CONNECTED)
      {
        count++;
        displaySignal(0);// 0 la dang nhap nhay cot song, chi khong co mang
      }
      else
      {
        displaySignal(1);// 1 la da co mang, hien thi gia tri cot song theo rssi
        if(indicate_WI)
        {
          indicateMenu(lcd_pointer);
        }
      }
  
      if(Flag_start_connect)
      {
        timeInvert--;
        displayCountDown();
        if(wifi_ip.wifi_status == WL_CONNECTED)
        {
          Flag_start_connect = false;
          WiFi.stopSmartConfig();
          DisplayPopup("wifi config succes");
        }
        else if(wifi_ip.wifi_status != WL_CONNECTED && timeInvert <= 0)
        {
          WiFi.stopSmartConfig();
          Flag_start_connect = false;
          DisplayPopup("wifi config fail");
        }      
      }
  
      if(broad_type != BS_NOPE)
      {
        timeline.timeplay_second++;
        if(timeline.timeplay_second > 59)
        {
          timeline.timeplay_second = 0;
          timeline.timeplay_minutes++;
          if(timeline.timeplay_minutes > 59)
          {
            timeline.timeplay_minutes = 0;
          }
        }
       int x = 215;
       if(timeline.timeplay_minutes< 10)  x+=frame_40.drawNumber(0,x,23,2);
       x+=frame_40.drawNumber(timeline.timeplay_minutes,x,23,2);
       x+=frame_40.drawChar(':',x,5,2); 
       if(timeline.timeplay_second < 10) x+=frame_40.drawNumber(0,x,23,2);
       x+=frame_40.drawNumber(timeline.timeplay_second,x,23,2);
       frame_40.pushSprite(0,200);
      }
        
      if(flag_fm_scan)
      {
        titleAbove("SCAN  FM");
        timeInvert--;
        frame170PopupWaiting("loading");
        if(timeInvert<=0)
        {
          // scan fm fail
          flag_fm_scan = false;
          DisplayPopup("Scan FM fail");
        }
        // scan fm succes ...
      }
      else if(flag_ota_start)
      {
        titleAbove("OTA  UPDATE");
        timeInvert--;
        frame170PopupWaiting("Updating");
        if(timeInvert<=0)
        {
          // ota fail
          flag_ota_start = false;
          DisplayPopup("OTA update fail");
        }
      }
    }
    else
    {
      indicateClockPWR();
    }
  }

  // hien thi man hinh volume, voi thoi gian hien thi la 2s
  if(displayVL)
  {
    if(millis() - timeDisplayVL >= 2000)
    {
      displayVL = false;
      indicateMenu(lcd_pointer);
    }
  }
}
void titleAbove(String label)
{
  frame_40.fillSprite(TFT_BLUE);
  frame_40.setTextColor(TFT_WHITE, TFT_BLUE);
  frame_40.setTextSize(1);
  frame_40.setTextDatum(MC_DATUM);
  frame_40.drawString(label, 160, 23,4);
  frame_40.pushSprite(0, 30);
}
void frame170PopupWaiting(String str_waiting)
{
   String str;
   count_load++;
   if(count_load>3)
   {
     count_load = 1;
   }
   str = str_waiting + " ";
   for(int i = 0; i < count_load; i++)
   {
     str = str + ".";
   }
   frame_170.fillSprite(TFT_BLACK);
   frame_170.setTextSize(2);
   frame_170.setTextColor(TFT_YELLOW, TFT_BLACK);
   frame_170.setTextDatum(MC_DATUM);
   frame_170.drawString(str, 160, 70, 2);
   frame_170.pushSprite(0,70);  
}
void indicateMenu(scr_pointer_t lcd_pointer)
{
  if(!lcd_pointer.subMenu1)
  {
    check_forward = true;
    mainMenuIndicate(mainMenuItem[lcd_pointer.mainMenu - 1], mainName[lcd_pointer.mainMenu - 1]);
  }
  else
  {
    switch(lcd_pointer.mainMenu)
    {
      case 1: // favorite 
        if(!lcd_pointer.subMenu2)
        {
           check_forward = true;
           lstNumSub1 = favorite_length;
           subChannelIndicate(mainName[lcd_pointer.mainMenu - 1], favorite_channel, lcd_pointer.subMenu1, lstNumSub1, fm_ic);
        }
        else
        {
          state_radio_bs = RADIO_PLAY;// bat radio
          broad_type = BS_FM;
        }
        break;
      case 2:
        if(!lcd_pointer.subMenu2)
        {
          check_forward = true;
          lstNumSub1 = sizeof(subInter)/12;
          subMenuIndicate(mainName[lcd_pointer.mainMenu - 1], subInter, lcd_pointer.subMenu1, lstNumSub1);
        }
        else
        {
          switch(lcd_pointer.subMenu1)
          {
            case 1:
              if(!lcd_pointer.subMenu3)
              {
                check_forward = true;
                lstNumSub2 = internet_length;
                subChannelIndicate(subInter[lcd_pointer.subMenu1 - 1], internet_channel, lcd_pointer.subMenu2, lstNumSub2, music_ic);
              }
              else
              {
                state_radio_bs = RADIO_PLAY;// bat radio
                broad_type = BS_INTER;
              }
              break;
            case 2:
            if(!lcd_pointer.subMenu3)
              {
                check_forward = true;
                lstNumSub2 = api_length;
                subChannelIndicate(subInter[lcd_pointer.subMenu1 - 1], api_channel, lcd_pointer.subMenu2, lstNumSub2, music_ic);
              }
              else
              {
                state_radio_bs = RADIO_PLAY;// bat radio
                broad_type = BS_INTER;
              }
              break;
            case 3:
            if(!lcd_pointer.subMenu3)
              {
                check_forward = true;
                lstNumSub2 = history_length;
                subChannelIndicate(subInter[lcd_pointer.subMenu1 - 1], history_channel, lcd_pointer.subMenu2, lstNumSub2, music_ic);
              }
              else
              {
                state_radio_bs = RADIO_PLAY;// bat radio
                broad_type = BS_INTER;
              }
              break;
          }
        }
        break;
      case 3:
        if(!lcd_pointer.subMenu2)
        {
           check_forward = true;
           lstNumSub1 = fm_length;
           subChannelIndicate(mainName[lcd_pointer.mainMenu - 1], fm_channel, lcd_pointer.subMenu1, lstNumSub1, fm_ic);
        }
        else
        {
          state_radio_bs = RADIO_PLAY;// bat radio
          broad_type = BS_FM;
        }
        break;
      case 4:
        if(!lcd_pointer.subMenu2)
        {
           check_forward = true;
           lstNumSub1 = dab_length;
           subChannelIndicate(mainName[lcd_pointer.mainMenu - 1], dab_channel, lcd_pointer.subMenu1, lstNumSub1, fm_ic);
           flag_ota_start = false;
        }
        else
        {
          state_radio_bs = RADIO_PLAY;// bat radio
          broad_type = BS_DAB;
        }
        break;
      case 5:
          if(!lcd_pointer.subMenu2)
          {
            check_forward = true;
            lstNumSub1 = sizeof(subSetting)/12;
            subMenuIndicate(mainName[lcd_pointer.mainMenu - 1], subSetting, lcd_pointer.subMenu1, lstNumSub1);
            flag_ota_start = false;
            indicate_WI = false;
          }
          else
          {
            switch(lcd_pointer.subMenu1)
            {
              case 1:                
                if(!lcd_pointer.subMenu3)
                {
                  check_forward = true;
                  lstNumSub2 = sizeof(subWiFi)/12;
                  subMenuIndicate(subSetting[lcd_pointer.subMenu1 - 1], subWiFi, lcd_pointer.subMenu2, lstNumSub2);
                  Flag_start_connect = false;
                  WiFi.stopSmartConfig();
                }
                else
                {
                  if(lcd_pointer.subMenu2 == 2)
                  {
                    // ham xu ly smart config
                    timeInvert = 120;
                    Wifi_Smartconfig();
                  }
                  else if(lcd_pointer.subMenu2 == 3)
                  {
                     //ham xu ly ap config
                  }
                }
                break;
              case 2:
                if(!lcd_pointer.subMenu3)
                {
                  check_forward = true;
                  lstNumSub2 = sizeof(subFMSet)/12;
                  subMenuIndicate(subSetting[lcd_pointer.subMenu1 - 1], subFMSet, lcd_pointer.subMenu2, lstNumSub2);
                  flag_fm_scan = false;
                }  
                else
                {
                  switch(lcd_pointer.subMenu2)
                  {
                    case 1:
                      timeInvert = 30;
                      flag_start_scan = true;
                      flag_fm_scan = true;
                      break;
                    case 2:
                      break;
                  }
                }
                break;
              case 3:
                if(!lcd_pointer.subMenu3)
                {
                  lstNumSub2 = sizeof(subDABSet)/12;
                  subMenuIndicate(subSetting[lcd_pointer.subMenu1 - 1], subDABSet, lcd_pointer.subMenu2, lstNumSub2);
                }  
                break;
              case 4:
                if(!lcd_pointer.subMenu3)
                {
                  lstNumSub2 = sizeof(subEqua)/12;
                  subMenuIndicate(subSetting[lcd_pointer.subMenu1 - 1], subEqua, lcd_pointer.subMenu2, lstNumSub2);
                }  
                break;
              case 5:
                if(!lcd_pointer.subMenu3)
                {
                  indicate_WI = true;
                  lstNumSub2 = sizeof(subWifiInfor)/12;
                  subMenuIndicate(subSetting[lcd_pointer.subMenu1 - 1], subWifiInfor, lcd_pointer.subMenu2, lstNumSub2);
                }  
                break;
               case 6:
                if(!lcd_pointer.subMenu3)
                {
                  lstNumSub2 = sizeof(subInformation)/12;
                  subMenuIndicate(subSetting[lcd_pointer.subMenu1 - 1], subInformation, lcd_pointer.subMenu2, lstNumSub2);
                }  
                break;
              case 7:
                if(!lcd_pointer.subMenu3)
                {
                  check_forward = true;
                  lstNumSub2 = sizeof(subLang)/12;
                  subMenuIndicate(subSetting[lcd_pointer.subMenu1 - 1], subLang, lcd_pointer.subMenu2, lstNumSub2);
                }
                break;
              case 8:
                timeInvert = 30;
                flag_start_scan = true;
                flag_ota_start = true;
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
        broadcastRadio(internet_channel[channelSub -1].channel_name, IR, broad_type);
        break;
      case BS_DAB:
        broadcastRadio(dab_channel[channelSub -1].channel_name,skytune, broad_type);
        break;
      case BS_FM:
        broadcastRadio(fm_channel[channelSub -1].channel_name, radio_dab, broad_type);
        break;
    }
  }
}
// xu ly man hinh khi scan nut nhan
void processButton(uint32_t but)
{
  if(but == KEY_POWER)
  {
      if(status_power == RADIO_ON)
      {
        // khi thiet bi dang phat radio, nhan power thi se stop stream truoc
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
          timeline.timeplay_second = 0;
          timeline.timeplay_minutes = 0;
        }
        int x = 0;

        // hien thi gui sleep now
//        tft.setTextDatum(MC_DATUM);
//        x = tft.drawString("sleep now", 160, 120, 2);
//        tft.drawRect(160 - x/2-10, 100, x+20, 40, TFT_YELLOW);
        DisplayPopup("sleep now");
        delay(500);
        status_power = RADIO_OFF;
      }
      else
      {
        //switch gia tri power
        status_power = RADIO_ON;
      }
      Serial.println("key power");
  }
  if(status_power == RADIO_ON)
  {
    // RADIO ON, xu ly su kien cac nut nhan khac
    switch(but)
    {
      case KEY_MENU:
      {
        lcd_pointer.subMenu3 = 0;
        lcd_pointer.subMenu2= 0;
        lcd_pointer.subMenu1 = 0;
        if(broad_type != BS_NOPE)
        {
          state_radio_bs = RADIO_STOP;
          broad_type = BS_NOPE;
          timeline.timeplay_second = 0;
          timeline.timeplay_minutes = 0;
        }
        sub_over = 0;
        break;
      }
    case KEY_BACK:
      // clear cac gia tri subMenu, neu thiet bi dang broadcast thi clear cac bien
      if(lcd_pointer.subMenu3)
      {
        lcd_pointer.subMenu3 = 0;
        if(broad_type != BS_NOPE)
        {
          lcd_pointer.subMenu2 = channelSub;
        }
      }
      else if (lcd_pointer.subMenu2)
      {
        lcd_pointer.subMenu2 = 0;
        if(broad_type != BS_NOPE)
        {
          lcd_pointer.subMenu1 = channelSub;
        }
      }
      else if (lcd_pointer.subMenu1)
      {
        lcd_pointer.subMenu1 = 0;
      }
      else if(lcd_pointer.mainMenu)
      {
        lcd_pointer.mainMenu -= 1;
        if(lcd_pointer.mainMenu < 1)
        {
          lcd_pointer.mainMenu = 5;
        }
      }

      if(broad_type != BS_NOPE)
      {
        state_radio_bs = RADIO_STOP;
        broad_type = BS_NOPE;
        timeline.timeplay_second = 0;
        timeline.timeplay_minutes = 0;
      }
      getSubOver();
      Serial.println("key back");
      break;
    case KEY_FORWARD:      
      if(!lcd_pointer.subMenu1)
      {
        lcd_pointer.mainMenu += 1;
        if(lcd_pointer.mainMenu > 5)
        {
          lcd_pointer.mainMenu = 1;
        }
      }
      else
      {
        if(broad_type != BS_NOPE)
        {
          channelSub += 1;
          if(channelSub > channelSubLength)
          {
            channelSub = 1;
          }
          // chuyen kenh radio, start lai drive, reset time broadcast
          state_radio_bs = RADIO_PLAY;
          flag_start_radio = true;
          timeline.timeplay_second = 0;
          timeline.timeplay_minutes = 0;
        }
      }
      // lay gia tri do dai cua list sub truoc khi hien thi
      Serial.println("key forward");
      break;
    case KEY_OK:
      if(!lcd_pointer.subMenu1 && check_forward)
      {
        check_forward = false;
        lcd_pointer.subMenu1 = 1;
      }
      else if(!lcd_pointer.subMenu2&& check_forward)
      {
        check_forward = false;
        lcd_pointer.subMenu2 = 1;
      }
      else if(!lcd_pointer.subMenu3 && check_forward)
      {
        check_forward = false;
        lcd_pointer.subMenu3 = 1;
      }
      sub_over = 0;
      Serial.println("key ok");
      break;
    default:
      break;
    }
    // hien thi man hinh sau khi xu ly xong nut nhat
    indicateMenu(lcd_pointer);
  }
}
void processRotacy(uint32_t but)
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
    // thiet bi dang RADIO ON, xu ly su kien cong tac vo cap
    // voi key up, thi khi xoay cong tac tang dan list sub menu
    // key dow thi se nguoc lai
    if(broad_type == BS_NOPE)
    {
      if(lcd_pointer.subMenu3)
      {
        lcd_pointer.subMenu3 += deltaMenu;
        if(lcd_pointer.subMenu3 > lstNumSub3)
        {
          lcd_pointer.subMenu3 = 1;
          sub_over = 0;
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
           sub_over = 0;
         }
        else if(lcd_pointer.subMenu2 < 1)
        {
          lcd_pointer.subMenu2= lstNumSub2;
        }
        // trong truong hop list sup nhieu hon 5, list sub van thay doi nhung hien thi van o vi tri 5
       }
       else if(lcd_pointer.subMenu1)
       {
         lcd_pointer.subMenu1+= deltaMenu;
         if(lcd_pointer.subMenu1 > lstNumSub1)
         {
           lcd_pointer.subMenu1 = 1;
           sub_over = 0;
         }
        else if(lcd_pointer.subMenu1 < 1)
        {
          lcd_pointer.subMenu1 = lstNumSub1;
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
      getSubOver();
      indicateMenu(lcd_pointer);
    }
    else
    {
      // khi dang trong trang thai broadcast, xoay cong tac vo cap se la tang giam am luong
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
      //hien thi man hinh pop up volume
      displayVolume();
    }
  }
}
void getSubOver()
{
   if(lcd_pointer.subMenu3 > 5)
   {
      sub_over = lcd_pointer.subMenu3 - 5;
   }
   else if(!lcd_pointer.subMenu3 && lcd_pointer.subMenu2 > 5)
   {
      sub_over = lcd_pointer.subMenu2 - 5;
   }
   else if(!lcd_pointer.subMenu2 && lcd_pointer.subMenu1 > 5)
       {
      sub_over = lcd_pointer.subMenu1 - 5;
   }  
   else
   {
      sub_over = 0;
   }
}
// man hinh broadcast cua radio, tuy theo broad type de hien thi
// co the set string trong case tuy theo broad type
void broadcastRadio(String channel_name, const unsigned short *image, broad_type_t broad_type)
{
    titleAbove(channel_name);
    frame_170.fillSprite(TFT_BLACK);
    frame_170.setTextSize(2);
    frame_170.setTextDatum(ML_DATUM);
    frame_170.setTextColor(TFT_YELLOW, TFT_BLACK);
    frame_170.pushImage(29,29,72,72,image);
    // xet text cho man hinh broadcast
    switch(broad_type)
    {
      case BS_INTER:
      // print cac thong tin cua channel
        frame_170.drawString("Other Talk",140,23,2);
        frame_170.drawString("ACC,40Kbps",140,57,2);
        frame_170.drawString("48000Hz,1ch",140,91,2);
        break;
      case BS_DAB:
        frame_170.drawString("DAB RADIO",140,23,2);
        frame_170.drawString(channel_name,140,57,2);
        break;
      case BS_FM:
        frame_170.drawString(channel_name,140,23,2);   
    }

    frame_170.pushSprite(0,70);

    frame_40.fillSprite(TFT_BLACK);
    frame_40.setTextDatum(ML_DATUM);
    frame_40.setTextColor(TFT_YELLOW, TFT_BLACK);
    frame_40.setTextSize(2);
    frame_40.drawChar('<', 5, 5,2);
    frame_40.drawChar('>', TFT_HEIGHT - 17, 5,2);
    int x = 215;
    if(timeline.timeplay_minutes< 10)  x+=frame_40.drawNumber(0,x,23,2);
    x+=frame_40.drawNumber(timeline.timeplay_minutes,x,23,2);
    x+=frame_40.drawChar(':',x,5,2); 
    if(timeline.timeplay_second < 10) x+=frame_40.drawNumber(0,x,23,2);
    x+=frame_40.drawNumber(timeline.timeplay_second,x,23,2);
    frame_40.pushSprite(0,200);  
}
// hien thi frame o dau man hinh gom cot song va thoi gian
// gia tri mode se thay doi hien thi cot song, khi khong co song thi cot song se nhap nhay
// khi co wifi thi cot song se hien theo gia tri rssi
void displaySignal(int status_WiFi)
{
  frame_30.fillSprite(TFT_BLACK);
  frame_30.drawXBitmap(10, 12, signal_ic, 24, 24, TFT_YELLOW);
  if(!status_WiFi)
  {
    frame_30.fillRect(58,6,4,24,TFT_YELLOW);
    frame_30.fillRect(52,12,4,18,TFT_YELLOW);
    frame_30.fillRect(46,18,4,12,TFT_YELLOW);
    frame_30.fillRect(40,24,4,6,TFT_YELLOW);
    if(count % 2)
    {
      frame_30.drawLine(40, 6, 60, 30, TFT_RED);
      frame_30.drawLine(41, 6, 61, 30, TFT_RED);
      frame_30.drawLine(42, 6, 62, 30, TFT_RED);
    
      frame_30.drawLine(40, 30, 60, 6, TFT_RED);
      frame_30.drawLine(41, 30, 61, 6, TFT_RED);
      frame_30.drawLine(42, 30, 62, 6, TFT_RED);
    }
  }
  else
  {
    // hien thi gia tri cot song
    frame_30.fillRect(40,24,4,6,TFT_YELLOW);
    if(abs(WiFi.RSSI()) < 80)
    {
      frame_30.fillRect(46,18,4,12,TFT_YELLOW);
      if(abs(WiFi.RSSI()) < 70)
      {
        frame_30.fillRect(52,12,4,18,TFT_YELLOW);
        if(abs(WiFi.RSSI()) < 60)
        {
          frame_30.fillRect(58,6,4,24,TFT_YELLOW);
        }
      }
    }
  }
  String subWifiInforLB[] = {"IP: ", "SSID: ", "PsWd: ", "RSSI: ", "BSSI: "};
  subWifiInfor[0] = subWifiInforLB[0] + wifi_ip.ip;
  subWifiInfor[1] = subWifiInforLB[1] + wifi_ip.ssid;
  subWifiInfor[2] = subWifiInforLB[2] + wifi_ip.password;
  subWifiInfor[3] = subWifiInforLB[3] + wifi_ip.rssi;
  subWifiInfor[4] = subWifiInforLB[4] + wifi_ip.bssid;
  
  
//  subWifiInfor[0] = subWifiInforLB[0] + WiFi.localIP().toString();
//  subWifiInfor[1] = subWifiInforLB[1] + WiFi.SSID();
//  subWifiInfor[3] = subWifiInforLB[3] + String(WiFi.RSSI());
//  subWifiInfor[4] = subWifiInforLB[4] + WiFi.BSSIDstr();
  if(broad_type == BS_NOPE)
  {
    // hien thi thoi gian o goc phai man hinh
    frame_30.setTextDatum(TL_DATUM);
    int x = 240, y = 0;
    if (timeline.time_hour < 10) x += frame_30.drawChar('0', x, y, 2); // Add timeline.time_hours leading zero for 24 hr clock
    x += frame_30.drawNumber(timeline.time_hour, x, y, 2);             // Draw timeline.time_hours
    x += frame_30.drawChar(':', x, 5, 2);
    if (timeline.time_minutes < 10) x += frame_30.drawChar('0', x, y, 2); // Add timeline.time_minutess leading zero
    x += frame_30.drawNumber(timeline.time_minutes, x, y, 2);             // Draw timeline.time_minutess
  }
  else
  {
    // neu thiet bi dang broad cast, se hien thi type broad cast
    frame_30.setTextDatum(MR_DATUM);
    frame_30.drawString(typeBroadCast[broad_type-1], TFT_HEIGHT - 5, 18, 2);
  }
  frame_30.pushSprite(0,0);
}
// hien thi pop up volume
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
// hien thi count down scan WiFi
void displayCountDown()
{
  int x = 153;
  frame_40.fillSprite(TFT_BLUE);
  frame_40.setTextColor(TFT_WHITE, TFT_BLUE);
  frame_40.setTextSize(1);
  frame_40.setTextDatum(ML_DATUM);
  frame_40.drawString(subWiFi[lcd_pointer.subMenu2 - 1], 5, 23,4);
  frame_40.pushSprite(0, 30);
  frame_170.fillSprite(TFT_BLACK);
  frame_170.setTextDatum(MC_DATUM);
  frame_170.setTextColor(TFT_WHITE, TFT_BLACK);
  frame_170.setTextSize(3);
  x += frame_170.drawString(String(timeInvert), x, 85, 1);
  frame_170.drawString("S", x-10, 85, 1);
  frame_170.drawCircle(160, 85, 60, TFT_YELLOW);
  frame_170.pushSprite(0, 70);
}
// hien thi pop up sáu khi scan WiFi vd: success, fail...
void DisplayPopup(String str)
{
  String strs[5];
  int StringCount = 0;
  int max_str = 0;
  int x = 0;
  int count = 0;
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextSize(2);
  while (str.length() > 0)
    {
      int index = str.indexOf(' ');
      if (index == -1) // No space found
      {
        if(strs[StringCount].length() > 0)
        {
          strs[StringCount] = strs[StringCount] + " " +str;
        }
        else
        {
          strs[StringCount] = str;
        }
        StringCount++;
        break;
      }
      else
      {
        count++;
        if(count>=2)
        {
          count = 0;
          strs[StringCount] = strs[StringCount] + " " +str.substring(0, index);
          StringCount++;
        }
        else
        {
          strs[StringCount] = str.substring(0, index);
        }
        str = str.substring(index+1);
      }
    }
  // Show the resulting substrings
  for (int i = 0; i < StringCount; i++)
  {
      x = tft.drawString(strs[i], 160, 120 + i*34, 2);
      if(max_str < x)
      {
        max_str = x;
      }
  }
  tft.fillRect(160 - max_str/2-10, 103,max_str + 20, StringCount*34,TFT_BLACK);
  for (int i = 0; i < StringCount; i++)
  {
      x = tft.drawString(strs[i], 160, 120 + i*34, 2);
      if(max_str < x)
      {
        max_str = x;
      }
  }
  tft.drawRect(160 - max_str/2-10, 103,max_str + 20, StringCount*34,TFT_YELLOW);
}
// hien thi man hinh chinh, thu tu trong lib la 1 = favorite, 2 = internet, 3 = FM, 4 = DAB, 5 = SETTING
void mainMenuIndicate(const short unsigned int *mainItem, String mainMenuString)
{
    frame_170.setTextDatum(MC_DATUM);
    frame_170.setTextSize(2);
    frame_170.fillSprite(TFT_BLACK);
    frame_170.setTextColor(TFT_YELLOW, TFT_BLACK);
    frame_170.pushImage(0,-35,320,240,mainItem); // add man hinh vao trong frame
    frame_170.drawString(mainMenuString, 160, 145,2);
    frame_170.pushSprite(0, 30);
    frame_40.fillSprite(TFT_BLACK);
    frame_40.pushSprite(0,200);
}
// hien thi man hinh sub menu, frame 40 =  hien thi frame mau xanh, frame 170 hien thi list sub
// label = gia tri mainMenu, hoac sub truoc do
// subMenu = gia tri sub hien tai,
// subMenuListLength =  do dai sub list vd setting co subMenuListLength = 5
void subMenuIndicate(String label, String *subMenuList,int subMenu, int subMenuListLength )
{
  int lengthList = 0;
  char stt[10];
  frame_170.fillSprite(TFT_BLACK);
  frame_170.setTextDatum(ML_DATUM);
  frame_170.setTextColor(TFT_WHITE, TFT_BLACK);
  frame_170.setTextSize(1);
  frame_170.setFreeFont(FF18);
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
    frame_170.drawString(subMenuList[i + sub_over], 20, 17 + 34*i,1);
  }
  frame_170.setTextColor(TFT_BLACK, TFT_YELLOW);
  frame_170.fillRect(0,34*(subMenu - 1 - sub_over),320,34,TFT_YELLOW);
  frame_170.drawString(subMenuList[subMenu - 1 ], 20, 17 + 34*(subMenu - 1 - sub_over),1);
  sprintf(stt, "%d/%d", subMenu,subMenuListLength);
  frame_170.setFreeFont(nullptr);
  frame_170.pushSprite(0, 70);
  frame_40.fillSprite(TFT_BLUE);
  frame_40.setTextColor(TFT_WHITE, TFT_BLUE);
  frame_40.setTextSize(1);
  frame_40.setTextDatum(ML_DATUM);
  frame_40.drawString(label, 5, 23,4);
  frame_40.setTextDatum(MR_DATUM);
  frame_40.drawString(stt, 310, 23,4);
  frame_40.pushSprite(0, 30);
}
// hien thi líst channel, bitmapitem = item truoc list channel
//void subChannelIndicate(char *label, const char *subMenuList[],int subMenu, int subMenuListLength, const unsigned char bitmapItem[])
void subChannelIndicate(String label, radio_channel_t *subMenuList, int subMenu, int subMenuListLength, const unsigned char bitmapItem[])
{
  int lengthList = 0;
  channelSub = subMenu;
  channelSubLength = subMenuListLength;
  char stt[10];
  frame_170.fillSprite(TFT_BLACK);
  frame_170.setTextDatum(ML_DATUM);
  frame_170.setTextColor(TFT_WHITE, TFT_BLACK);
  frame_170.setTextSize(1);
  frame_170.setFreeFont(FF18);
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
    frame_170.drawString(subMenuList[i + sub_over].channel_name, 35, 17 + 34*i ,1);
    frame_170.drawXBitmap(5, 5 + 34*i, bitmapItem,24,24, TFT_WHITE, TFT_BLACK );
  }
  frame_170.setTextColor(TFT_BLACK, TFT_YELLOW);
  frame_170.fillRect(0,34*(subMenu - 1 - sub_over),320,34,TFT_YELLOW);
  frame_170.drawString(subMenuList[subMenu - 1].channel_name, 35, 17 + 34*(subMenu - 1 - sub_over),1);
  frame_170.drawXBitmap(5, 5 + 34*(subMenu - 1 - sub_over), bitmapItem,24,24, TFT_BLACK, TFT_YELLOW );
  sprintf(stt, "%d/%d", subMenu,subMenuListLength);
  frame_170.setFreeFont(nullptr);
  frame_170.pushSprite(0, 70);
  frame_40.fillSprite(TFT_BLUE);
  frame_40.setTextColor(TFT_WHITE, TFT_BLUE);
  frame_40.setTextSize(1);
  frame_40.setTextDatum(ML_DATUM);
  frame_40.drawString(label, 5, 23,4);
  frame_40.setTextDatum(MR_DATUM);
  frame_40.drawString(stt, 310, 23,4);
  frame_40.pushSprite(0, 30);
}
// hien thi man hinh clock khi power
void indicateClockPWR()
{
  frame_FS.createSprite(320, 240);
  frame_FS.fillScreen(TFT_BLACK);
  frame_FS.setTextDatum(TL_DATUM);
  int x = 40, y = 90;
  if (RTC_Time.Hour < 10) x += frame_FS.drawChar('0', x, y, 8); // Add timeline.time_hours leading zero for 24 hr clock
  x += frame_FS.drawNumber(RTC_Time.Hour, x, y, 8);             // Draw timeline.time_hours
  x += frame_FS.drawString(":", x, y-10, 8);
  if (RTC_Time.Min < 10) x += frame_FS.drawChar('0', x, y, 8); // Add timeline.time_minutess leading zero
  x += frame_FS.drawNumber(RTC_Time.Min, x, y, 8);             // Draw timeline.time_minutesfs
  frame_FS.pushSprite(0,0);
}
