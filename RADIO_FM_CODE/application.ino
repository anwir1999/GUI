
/********************************************* SYSTEM ****************************************************/
//char lcd_buff[128];
//uint16_t AUDIO_Data[256];

void System_Stating(){
    DEBUG_PRINTLN("System stating...");
    
    //LCD_Begin(); //Auto begin SPI_Init();
    ENC_Begin(); //ENC INIT
    AW9523_Begin();
    WM8804_Begin();
    NAU8822_Begin();
    
    AW9523_DigitalWrite(PWR_LED, 1);// bật led nguồn
    //LCD TEST
//    LCD_SetBgColor(0);
//    LCD_DrawInit(22, 10, 320, 20);
//    LCD_SetColor(0xF800);
//    LCD_DrawString("♫ DRIVER RADIO FM DAB TEST ♫");
//    //
//    LCD_DrawInit(60, 30, 320, 20);
//    LCD_SetColor(0x001F);
//    LCD_DrawString("Kaizen Tech");
//    LCD_Update();
    //DEBUG_PRINTLN("SYSTEM OK!");
    //equalizer_set();
    //FM radio
    //fm_radio_play(9100);
    NAU8822_Volume(24);
    // khoi tao cung cac struct radio
    fm_length = 6;
    for(int i = 0; i < fm_length; i++)
    {
        fm_channel[i].channel_name ="FM " + String(90 +i) + "Hz";
    }
    dab_length = 3;
    for(int i = 0; i < dab_length; i++)
    {
        dab_channel[i].channel_name ="CHANNEL DAB " + String(i);
    }
    internet_length = 7;
    for(int i = 0; i < internet_length; i++)
    {
        internet_channel[i].channel_name ="CHANNEL IR " + String(i);
    }
    
    api_length = 7;
    for(int i = 0; i < api_length; i++)
    {
        api_channel[i].channel_name ="CHANNEL API " + String(i);
    }
    history_length = 7;
    for(int i = 0; i < history_length; i++)
    {
        history_channel[i].channel_name ="CHANNEL HIS " + String(i);
    }
    favorite_length = 7;
    for(int i = 0; i < favorite_length; i++)
    {
        favorite_channel[i].channel_name ="CHANNEL FAVORITE " + String(i);
    }
    
    init_gui();
    Wifi_Connect();
}
//
//uint32_t Test_Time=0, Test_Time2=0;
//int Test_Vol = 22, Last_Vol = 0, FM_Rssi, System_Mode = 0, DAB_Chn = 0;
//uint32_t FM_Last = 1000, FM_Freq = 9210, DAB_Last = 48;
//FM_STATUS FM_Status;
//RDS_DATA RDS_Data;
//DAB_STATUS DAB_Status; 
//DAB_INFO DAB_Info;
//DAB_SERVICELIST DAB_List;
//DAB_SERVICEDATA DAB_Data;
//DAB_AUDIOINFO DAB_AudioInfo;
//DAB_SERVICEINFO DAB_ServiceInfo;
//DAB_SUBCHANINFO DAB_SubchanInfo;
//extern uint32_t SI4684_PartNo, WM8804_DeviceID, NAU8822_DeviceID, SI4684_Flag;
//const char *Key_Name[7] = {"POWER", "MENU ", "LEFT ", "RIGHT", "UP   ", "DOWN ", "OK   "};
//uint32_t LastNo = 0, DispNew = 0, DAB_Serv = 0, Scan_Mode = 0, Radio_Vaild = 0, Radio_Time = 500;
//uint8_t ACF_Data[8]; 
//char DAB_Label[16];
//const char *smode;

void System_TestTask(){
    scanButton();
    indicateVolume(); // hien thi man hinh volume, voi thoi gian hien thi la 2s
    funcActiveGui(); // xu ly cac man hinh, su kien voi chu ky 1s, vi du: dem thoi gian scan WiFi, dem gio....
    timeCounterGui();  // dem gio 
    // xu ly su kien broadcast
    if(state_radio_bs == RADIO_PLAY && !flag_start_radio)
    {
      flag_start_radio = true;
      flag_stop_radio = false;
      switch(broad_type)
      {
        case BS_INTER:
          // chay radio internet
          Serial.println("broad cast internet radio");
          Serial.print("url:  ");
          Serial.println(broadcast_channel_now.url);
          break;
        case BS_DAB:
          // chay radio dab
//          fm_radio_play(broadcast_channel_now.frequency);
//          SPI_EndTransaction();
          Serial.println("broad cast dab radio");
          Serial.print("frequency = ");
          Serial.println(broadcast_channel_now.frequency);
          break;
        case BS_FM:
          // chay radio fm
//          fm_radio_play(9100);
          fm_radio_play(broadcast_channel_now.frequency.toInt());
          SPI_EndTransaction();
          Serial.println("broad cast fm radio");
          Serial.print("frequency = ");
          Serial.println(broadcast_channel_now.frequency);
          break;
      }
    }
    else if(state_radio_bs == RADIO_STOP)
    {
      if(!flag_stop_radio)
      {
        fm_radio_stop();
        SPI_EndTransaction();
        flag_stop_radio = true;
        flag_start_radio = false;
        Serial.println("stop radio");        
      }
    }

    if(status_power == RADIO_ON && !Flag_start_connect)
    {
      Wifi_Reconnect();
    }
    if(flag_start_scan)
    {
      flag_start_scan = false;
      if(flag_fm_scan)
      {
        Serial.println("FM scan");
      }
      else if(flag_ota_start)
      {
        Serial.println("OTA START");
      }
    }
//    uint32_t ms = millis();
//    ENC_RotaryTask();
//    if((uint32_t)(ms - Test_Time) >= 200){
//        Test_Time = ms;
        
//        LCD_SetColor(0x2771);
//        
//        LCD_DrawInit(20, 50, 320, 20);
//        if(DAB_Data.Message[0])sprintf(lcd_buff, "DAB: %s                   ", DAB_Data.Message);
//        else sprintf(lcd_buff, "FM DAB DEVICE: 0x%04X       ", SI4684_PartNo);
//        LCD_DrawString(lcd_buff);
//        LCD_DrawInit(20, 70, 320, 20);
//        sprintf(lcd_buff, "CODEC DEVICE: 0x%04X    ", NAU8822_DeviceID);
//        LCD_DrawString(lcd_buff);
//        LCD_DrawInit(20, 90, 320, 20);
//        sprintf(lcd_buff, "OPTICAL DEVICE: 0x%04X ", WM8804_DeviceID);
//        LCD_DrawString(lcd_buff);
//        //
//        uint32_t but = Get_KeyPress();
        //
//        LCD_DrawInit(20, 120, 320, 20);
//        LCD_SetColor(0x07ff);
//        sprintf(lcd_buff, "KEY PRESS: 0x%02X  %s      ", but, (but)?Key_Name[but - 0x10]:"NOKEY");
//        Serial.println(lcd_buff);
//    }
//        LCD_DrawString(lcd_buff);
//        //
//        if(but){
//            if(but == KEY_UP){
//                if(Test_Vol < 24){
//                    Test_Vol++;
//                }
//            }else if(but == KEY_DOWN){
//                if(Test_Vol){
//                    Test_Vol--;
//                }
//            }else if(but == KEY_LEFT){
//              if(!Scan_Mode){
//                if(System_Mode == 1){
//                    if(DAB_Chn > 0)DAB_Chn--;
//                }else if(System_Mode == 0){
//                    if(FM_Freq > 8750)FM_Freq -= 10; //8750
//                }
//              }else Scan_Mode = 0;
//            }else if(but == KEY_RIGHT){
//              if(!Scan_Mode){
//                if(System_Mode == 1){
//                    if(DAB_Chn < (DAB_FREQ_NUMBER-1))DAB_Chn++;
//                }else if(System_Mode == 0){
//                    if(FM_Freq < 10790)FM_Freq += 10;
//                }
//              }else Scan_Mode = 0;
//            }else if(but == KEY_POWER){
//                if(++System_Mode > 2)System_Mode = 0;
//                FM_Last = 0;
//                DAB_Last = 48;
//                DAB_Chn = 0;
//                Scan_Mode = 0;
//                Radio_Vaild = 0;
//                LCD_LineClear(160, 80); //y=160, 80 line
//                KeyPress_Wait(but);
//                if(System_Mode == 2){
//                    SI4684_End();
//                    NAU8822_Input(1);
//                    WM8804_Input(1);
//                    WAV_Play(file_audio_wav, sizeof(file_audio_wav));
//                }else{
//                    WAV_Stop();
//                    NAU8822_Input(0);
//                    WM8804_Input(0);
//                }
//            }else if(but == KEY_OK){
//                if(System_Mode < 2){
//                    if(Scan_Mode == 3 || !Scan_Mode){ //Scan/Rescan
//                        if(System_Mode == 0)Scan_Mode = 1; //Rescan FM, auto mode set = 1
//                        else Scan_Mode = 2; 
//                        Radio_Vaild = 0;
//                        if(DAB_Chn >= (DAB_FREQ_NUMBER-1))DAB_Chn = 0;
//                        if(FM_Freq >= 10790 || FM_Freq < 8750)FM_Freq = 8740;
//                        KeyPress_Wait(but);
//                        Radio_Time = 200;
//                    }
//                }
//            }else if(but == KEY_MENU){ //Stop Scan
//                if(System_Mode < 2 && Scan_Mode){
//                    //if(Scan_Mode < 3)SI4684_FM_StopScan(); //auto mode
//                    Scan_Mode = 0;
//                    Radio_Vaild = 0;
//                    DAB_Chn = 0;
//                    KeyPress_Wait(but);
//                    Radio_Time = 500;
//                }
//            }
//            Test_Time2 = ms; //Reload time
//        }
//        //
//        
//        if(Scan_Mode){
//            if(Scan_Mode == 1 || Scan_Mode == 2)smode = "[SCAN] ";
//            else if(!Radio_Vaild)smode = "[STOP] ";
//            else smode = "[VAILD]";
//        }else{
//            if(!Radio_Vaild)smode = " [RUN] ";
//            else smode = "[VAILD]";
//        }
//        LCD_DrawInit(20, 140, 320, 20);
//        LCD_SetColor(0xf81f);
//        sprintf(lcd_buff, "HP VOLUME: %u - %s     ", Test_Vol, smode);
//        LCD_DrawString(lcd_buff);
//        //
//        LCD_DrawInit(20, 160, 320, 20);
//        LCD_SetColor(0xffff);
//        if(System_Mode == 2){ //I2S MODE
//
//            sprintf(lcd_buff, "I2S RATE: %u - %u bit L/R     ", WAV_Header.SampleRate, WAV_Header.BitsPerSample);
//            LCD_DrawString(lcd_buff);
//            //
//            LCD_DrawInit(20, 180, 320, 20);
//            //sprintf(lcd_buff, "I2S DATA: L=0x%04X - R=0x%04X     ", AUDIO_Data[0], AUDIO_Data[1]);
//            sprintf(lcd_buff, "I2S DATA: WAV PCM           ");
//            LCD_DrawString(lcd_buff);
//
//            LCD_DrawInit(20, 200, 320, 20);
//            sprintf(lcd_buff, "WAV PLAY: %u / %u              ", Audio_SampleIndex, Audio_SampleSize);
//            LCD_DrawString(lcd_buff);
//            uint32_t msec = (Audio_SampleIndex * 1000) / WAV_Header.SampleRate;
//            LCD_DrawInit(20, 220, 320, 20);
//            sprintf(lcd_buff, "WAV TIME: %u.%u s               ", msec/1000, msec%1000);
//            LCD_DrawString(lcd_buff);
//            
//            //I2S_Load((uint32_t*)&AUDIO_Data[0], 128); //TEST 128 sample L+R
//            
//        }else if(System_Mode == 1){ //DAB MODE
//
//            uint32_t Freq = DAB_Status.Tune_Freq;
//            sprintf(lcd_buff, "DAB FREQ: %u.%u Mhz - CHN: %u     ", (Freq/1000), (Freq%1000), (uint32_t)DAB_Chn);
//            LCD_DrawString(lcd_buff);
//            //
//            LCD_DrawInit(20, 180, 320, 20);
//            sprintf(lcd_buff, "DAB RSSI: %i dB - SNR: %u dB       ", CharToInt(DAB_Status.RSSI), (uint32_t)DAB_Status.SNR);
//            LCD_DrawString(lcd_buff);
//            //
//            LCD_DrawInit(20, 200, 320, 20);
//            sprintf(lcd_buff, "DAB FICQ: %u %% - CNR: %u dB       ", (uint32_t)DAB_Status.FIC_Quality, (uint32_t)DAB_Status.CNR);
//            LCD_DrawString(lcd_buff);
//            //
//            
//            LCD_DrawInit(20, 220, 320, 20);
//            sprintf(lcd_buff, "DAB LAB: %s                     ", DAB_Label);
//            LCD_DrawString(lcd_buff);
//            
//        }else if(System_Mode == 0){ //FM MODE
//
//            sprintf(lcd_buff, "FM FREQ: %u.%u Mhz - ANTCAP: %u      ", FM_Freq/100, FM_Freq%100, (uint32_t)FM_Status.ANT_Cap);
//            LCD_DrawString(lcd_buff);
//            //
//            LCD_DrawInit(20, 180, 320, 20);
//            sprintf(lcd_buff, "FM RSSI: %i dBuV - SNR: %i dB       ", CharToInt(FM_Status.RSSI), CharToInt(FM_Status.SNR));
//            LCD_DrawString(lcd_buff);
//            //
//            LCD_DrawInit(20, 200, 320, 20);
//            sprintf(lcd_buff, "FM FOS: %i BPPM - MULT: %u      ", CharToInt(FM_Status.Freq_Offset), (uint32_t)FM_Status.MULT);
//            LCD_DrawString(lcd_buff);
//            //
//            LCD_DrawInit(20, 220, 320, 20);
//            if(RDS_Data.Label[0] || RDS_Data.Text[0])sprintf(lcd_buff, "RDS: %s - %s                ", RDS_Data.Label, RDS_Data.Text);
//            else sprintf(lcd_buff, "FM ACF: %02X-%02X-%02X-%02X-%02X   ", (uint32_t)ACF_Data[0], (uint32_t)ACF_Data[1], (uint32_t)ACF_Data[2], (uint32_t)ACF_Data[3], (uint32_t)ACF_Data[4]);
//            LCD_DrawString(lcd_buff);
//        }//END FM
//        //LCD_Update();
//        //VOLUME
//        if(Test_Vol != Last_Vol){
//            NAU8822_Volume(Test_Vol);
//            if(!Test_Vol)WM8804_Mute(1);
//            else if(!Last_Vol)WM8804_Mute(0);
//            Last_Vol = Test_Vol;
//        }
//    }
//    
//    if((uint32_t)(ms - Test_Time2) >= Radio_Time){ //Update Data
//        Test_Time2 = ms;
//        //
//        if(System_Mode == 1){
//            if(Scan_Mode == 1){
//                if(DAB_Chn < (DAB_FREQ_NUMBER-1)){ 
//                    DAB_Chn++; 
//                    Scan_Mode = 2;
//                    Radio_Time = 500;
//                }else{
//                    Scan_Mode = 3;
//                }
//            }
//            if(DAB_Chn != DAB_Last){
//                DAB_Last = DAB_Chn;
//                DAB_Data.Message[0] = 0;
//                DAB_Label[0] = 0;
//                DAB_Serv = 0;
//                Radio_Vaild = 0;
//                SI4684_DAB_TuneChannel(DAB_Chn);
//            }else{
//                Radio_Vaild = SI4684_DAB_VaildStatus(&DAB_Status);
//                if(Scan_Mode){
//                    if(Radio_Vaild){ //Stop and save
//                        Scan_Mode = 3;
//                        //Save
//                        goto GET_LAB;
//                    }else if(Scan_Mode == 2)Scan_Mode = 1; //Next
//                }else{
//                    GET_LAB: SI4684_DAB_GetEnsembleInfo(&DAB_Info);
//                    strcpy_len(DAB_Label, DAB_Info.Label, 16); //add null
//                }
//                //TEST
//                if((SI4684_Flag & SI4684_DSRVINT || DAB_Serv) && !Scan_Mode){
//                    if(!DAB_Serv && SI4684_DAB_ServiceListAvailable()){
//                        DAB_Serv = SI4684_DAB_GetServiceList(&DAB_List); //Number List
//                        if(DAB_Serv){
//                            SI4684_DAB_ServiceStart(DAB_List.Service[0].ServiceID, DAB_List.Service[0].ComponentID, 1);
//                            SI4684_DAB_GetServiceInfo(DAB_List.Service[0].ServiceID, &DAB_ServiceInfo);
//                            SI4684_DAB_GetSubchanInfo(DAB_List.Service[0].ServiceID, DAB_List.Service[0].ComponentID, &DAB_SubchanInfo);
//                        }
//                    }else{
//                        SI4684_DAB_GetServiceData(&DAB_Data);
//                        SI4684_DAB_GetTime(&RTC_Time);
//                        SI4684_DAB_GetAudioInfo(&DAB_AudioInfo);
//                    }
//                }
//            }
//        }else if(System_Mode == 0){
//            /*if(Scan_Mode){ //Scan auto
//                if(Scan_Mode == 1){
//                    SI4684_FM_AutoScan(1, 1); //Seek up, continue
//                    Scan_Mode = 2; 
//                    Radio_Time = 200;
//                }else{
//                    Radio_Vaild = SI4684_FM_VaildStatus(&FM_Status); //0.25s delay
//                    if(Radio_Vaild || (SI4684_Flag & SI4684_STCINT)){
//                        FM_Freq = FM_Status.Tune_Freq;
//                        FM_Last = FM_Freq;
//                        if(Scan_Mode == 2){
//                            SI4684_FM_StopScan();
//                            Scan_Mode = 3; //stop
//                            Radio_Time = 500;
//                            if(Radio_Vaild){ //save
//
//                            }
//                        }
//                    }else FM_Freq = FM_Status.Tune_Freq; //freq show
//                }
//                return;
//            }*/
//            if(Scan_Mode == 1){ //Scan manual
//                if(FM_Freq < 10790){ 
//                    FM_Freq += 10; 
//                    Scan_Mode = 2; 
//                    Radio_Time = 200;
//                }else{
//                    Scan_Mode = 3;
//                    Radio_Time = 500;
//                }
//            }
//            //
//            if(FM_Freq != FM_Last){
//                FM_Last = FM_Freq;
//                RDS_Data.Label[0] = 0;
//                RDS_Data.Text[0] = 0;
//                SI4684_FM_TuneFrequency(FM_Freq);
//                Radio_Vaild = 0;
//            }else{
//                //SI4684_FM_GetStatus(&FM_Status); //Scan stop
//                Radio_Vaild = SI4684_FM_VaildStatus(&FM_Status); //Read status and check vaild
//                SI4684_FM_GetACF(ACF_Data);
//                if(Scan_Mode){
//                    //uint32_t acf = ACF_Data[1]; //Audio flag
//                    //if(!(acf == 0x33 || acf == 0x30))Radio_Vaild = 1; //check audio signal
//                    if(Radio_Vaild){ //Stop and save
//                        Scan_Mode = 3;
//                        Radio_Time = 500;
//                        //Save
//                        //goto GET_RDS;
//                    }else if(Scan_Mode == 2)Scan_Mode = 1; //Next
//                }else{
//                    //GET_RDS: 
//                    if(SI4684_FM_RDS_Available(0))SI4684_FM_GetRDS(&RDS_Data, &RTC_Time);
//                }
//            }
//        }
//        //
//        
//        //
//        if(System_Mode < 2)SI4684_GetPartInfo(); //Check device, TEST
//        //NAU8822_GetDeviceID();
//        //WM8804_GetDeviceID();
//    }
}
