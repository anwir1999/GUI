#ifndef __SI4684_H
#define __SI4684_H

#define SI4684_RD_REPLY 0x00
#define SI4684_POWER_UP 0x01
#define SI4684_HOST_LOAD 0x04
#define SI4684_FLASH_LOAD 0x05
#define SI4684_LOAD_INIT 0x06
#define SI4684_BOOT 0x07
#define SI4684_GET_PART_INFO 0x08
#define SI4684_GET_SYS_STATE 0x09
#define SI4684_READ_OFFSET 0x10
#define SI4684_GET_FUNC_INFO  0x12
#define SI4684_SET_PROPERTY 0x13
#define SI4684_GET_PROPERTY 0x14
//FM
#define SI4684_FM_TUNE_FREQ 0x30
#define SI4684_FM_SEEK_START 0x31
#define SI4684_FM_RSQ_STATUS 0x32
#define SI4684_FM_ACF_STATUS 0x33
#define SI4684_FM_RDS_STATUS 0x34
#define SI4684_FM_RDS_BLOCKCOUNT 0x35
//DAB
#define SI4684_DAB_GET_SERVICE_LIST			      0x80
#define SI4684_DAB_START_SERVICE			        0x81
#define SI4684_DAB_STOP_SERVICE				        0x82
#define SI4684_DAB_DATA_SERVICE				        0x84
#define SI4684_DAB_TUNE_FREQ 				          0xB0
#define SI4684_DAB_DIGRAD_STATUS 			        0xB2
#define SI4684_DAB_GET_EVENT_STATUS			      0xB3
#define SI4684_DAB_GET_ENSEMBLE_INFO          0xB4
#define SI4684_DAB_GET_SERVICE_LINKING_INFO   0xB7
#define SI4684_DAB_SET_FREQ_LIST 			        0xB8
#define SI4684_DAB_GET_COMPONENT_INFO		      0xBB
#define SI4684_DAB_GET_TIME					          0xBC
#define SI4684_DAB_GET_AUDIO_INFO 			      0xBD
#define SI4684_DAB_GET_SUBCHAN_INFO 		      0xBE
#define SI4684_DAB_GET_SERVICE_INFO			      0xC0


//
#define SI4684_FM_INT_CTL_ENABLE 0x0000
#define SI4684_FM_INT_CTL_REPEAT 0x0001
#define SI4684_FM_DIGITAL_IO_OUTPUT_SELECT 0x0200
#define SI4684_DIGITAL_IO_OUTPUT_FORMAT 0x0202
#define SI4684_PIN_CONFIG_ENABLE 0x0800
#define SI4684_FM_VALID_MAX_TUNE_ERROR 0x3200
#define SI4684_FM_VALID_RSSI_TIME 0x3201
#define SI4684_FM_VALID_RSSI_THRESHOLD 0x3202
#define SI4684_FM_VALID_SNR_TIME 0x3203
#define SI4684_FM_VALID_SNR_THRESHOLD 0x3204
#define SI4684_FM_SOFTMUTE_SNR_LIMITS 0x3500
#define SI4684_FM_SOFTMUTE_SNR_ATTENUATION 0x3501
#define SI4684_FM_TUNE_FE_CFG 0x1712
#define SI4684_FM_RDS_CONFIG 0x3C02
#define SI4684_FM_AUDIO_DE_EMPHASIS 0x3900

#define SI4684_DAB_TUNE_FE_CFG 0x1712
#define SI4684_DAB_TUNE_FE_VARM 0x1710
#define SI4684_DAB_TUNE_FE_VARB 0x1711
#define SI4684_DAB_CTRL_DAB_MUTE_ENABLE 0xB400
#define SI4684_DAB_CTRL_DAB_MUTE_SIGNAL_LEVEL_THRESHOLD 0xB501
#define SI4684_DAB_CTRL_DAB_MUTE_SIGLOW_THRESHOLD 0xB505

#define SI4684_DIGITAL_SERVICE_INT_SOURCE 0x8100

#define SI4684_AUDIO_DE_EMPHASIS_EU 0x01 //0x00=75us -> defaults to USA (default), 0x01=50us -> defaults to Europe, 0x02=disabled
#define SI4684_AUDIO_DE_EMPHASIS_US 0x00 //0x00=75us -> defaults to USA (default), 0x01=50us -> defaults to Europe, 0x02=disabled
#define SI4684_AUDIO_DE_EMPHASIS_OFF 0x02 //0x00=75us -> defaults to USA (default), 0x01=50us -> defaults to Europe, 0x02=disabled

// DAB channels
#define CHAN_5A 174928
#define CHAN_5B 176640
#define CHAN_5C 178352
#define CHAN_5D 180064
#define CHAN_6A 181936
#define CHAN_6B 183648
#define CHAN_6C 185360
#define CHAN_6D 187072
#define CHAN_7A 188928
#define CHAN_7B 190640
#define CHAN_7C 192352
#define CHAN_7D 194064
#define CHAN_8A 195936
#define CHAN_8B 197648
#define CHAN_8C 199360
#define CHAN_8D 201072
#define CHAN_9A 202928
#define CHAN_9B 204640
#define CHAN_9C 206352
#define CHAN_9D 208064
#define CHAN_10A 209936
#define CHAN_10B 211648
#define CHAN_10C 213360
#define CHAN_10D 215072
#define CHAN_11A 216928
#define CHAN_11B 218640
#define CHAN_11C 220352
#define CHAN_11D 222064
#define CHAN_12A 223936
#define CHAN_12B 225648
#define CHAN_12C 227360
#define CHAN_12D 229072
#define CHAN_13A 230784
#define CHAN_13B 232496
#define CHAN_13C 234208
#define CHAN_13D 235776
#define CHAN_13E 237488
#define CHAN_13F 239200


#define RDS_GROUP_0A  0
#define RDS_GROUP_0B  1
#define RDS_GROUP_2A  4
#define RDS_GROUP_2B  5
#define RDS_GROUP_4A  8

#define SERVICE_LIST_MAX    32
//FLAG INT
#define SI4684_CTSINT       0x80
#define SI4684_ERRCMD       0x40
#define SI4684_DACQINT      0x20
#define SI4684_DSRVINT      0x10
#define SI4684_RSQINT       0x08
#define SI4684_RDSINT       0x04
#define SI4684_ACFINT       0x02
#define SI4684_STCINT       0x01

typedef struct {
    uint16_t Resp_Flag; //RESP5 H + RESP4 L
    uint16_t Tune_Freq; //x10Khz
    char Freq_Offset; //-128 - 127 BPPM (2 PPM), Scan: (-10) -> (-4) OK
    char RSSI; //-128 - 127 dBuV, Scan: RSSI >= 10dBuV OK
    char SNR; //-128 - 127 dB, Scan: SNR >= 6dB  OK
    uint8_t MULT; //0 - 255
    uint16_t ANT_Cap; //Antenna tuning cap value
    //uint8_t Rev; //Not use
    //uint8_t HD_Level; //0 - 100
    //uint8_t Filtered_HDLevel; //0 - 100
}FM_STATUS;

typedef struct { //RDS PARSE
    uint16_t Pi; //Code PI
    uint16_t Pty; //Program type 1-31
    char Label[9]; //Name PS 8 char + NULL
    char Text[65]; //RDS PT 64 char + NULL
    uint8_t Flag; //Check
    uint8_t State; //Not use
}RDS_DATA;


typedef struct {
    uint16_t Resp_Flag; //RESP5 H + RESP4 L
    char RSSI; //-128 - 63
    uint8_t SNR; //0 - 20 dB
    uint8_t FIC_Quality; //0 - 100 %
    uint8_t CNR; //0 - 54 dB
    uint16_t FIB_ErrorCount;
    uint32_t Tune_Freq; //Khz
    uint8_t Tune_Index; //channel 0 - 47
    uint8_t FFT_Offset;
    uint16_t ANT_Cap; //Antenna tuning cap value
    uint16_t CU_Level; //0 - 470 CU process
    uint8_t Fast_Dect; //DAB fast detect
}DAB_STATUS;

typedef struct {
    uint16_t EID;
    char Label[16]; //Name not null
    uint8_t ECC;
    uint8_t Charset;
    uint16_t Char_Abrev;
}DAB_INFO;

typedef struct {
    uint8_t PTY_PD;
    uint8_t Local;
    uint8_t SI_Charset;
    uint8_t SRV_ECC;
    char Label[16]; //Name not null
    uint16_t Char_Abrev;
}DAB_SERVICEINFO;

typedef struct {
    uint16_t Size;
    uint8_t Num_Linksets;
    uint8_t Rev; //Not use
    uint16_t LSN_0;
    uint8_t Flag;
    uint8_t Num_Links_0;
    uint32_t LinkID_0;
}DAB_SERVICELINK;

typedef struct {
    uint16_t Bit_Rate;
    uint16_t Sample_Rate;
    uint8_t Flag_Mode;
    uint8_t DRC_Gain;
}DAB_AUDIOINFO;

typedef struct {
    uint8_t Service_Mode;
    uint8_t Protection_Info;
    uint16_t SubChan_BitRate;
    uint16_t Num_CU;
    uint16_t CU_Address;
}DAB_SUBCHANINFO;

typedef struct {
    /*uint8_t Flag;
    uint8_t BufferCount;
    uint8_t ServiceState;
    uint8_t DataSRC_DSCTy;
    uint32_t ServiceID;
    uint32_t ComponentID;
    uint16_t UAType;
    uint16_t ByteCount;
    uint16_t SegNum;
    uint16_t NumSegs;*/
    //
    uint32_t ServiceID;
    uint32_t ComponentID;
    uint16_t UAType;
    uint8_t Length;
    char Message[129]; //DLS Message
}DAB_SERVICEDATA;

typedef struct { 
    uint32_t ServiceID;
    uint32_t ComponentID;
    uint8_t Flag;
    char Label[17]; //16 char + 1 Null
}DAB_SERVICELIST_DATA;

typedef struct {
    DAB_SERVICELIST_DATA Service[SERVICE_LIST_MAX]; //32
}DAB_SERVICELIST;

uint32_t DAB_FreqList[38] PROGMEM = { //Max 48 Freq
    174928, 176640, 178352, 180064, 181936, 183648, 185360, 187072, 188928, 190640, 192352, 194064, 195936, 197648, 199360, 201072,
    202928, 204640, 206352, 208064, 209936, 211648, 213360, 215072, 216928, 218640, 220352, 222064, 223936, 225648, 227360, 229072,
    230784, 232496, 234208, 235776, 237488, 239200
};
#define DAB_FREQ_NUMBER 38 //(sizeof(DAB_Freq)/4)


//INIT API FUNCTION
void SI4684_End(); //Stop FM Radio, Goto Sleep...
uint8_t SI4684_GetFlag(); //Return Status0 Flag (CTS, DSRV, RDS...)
int SI4684_Send(uint8_t *cmd, uint32_t len);
int SI4684_CheckBusy(int loop); //loop_check = loop + 1
uint16_t SI4684_GetPartInfo(); //return part no
uint32_t SI4684_GetFuncInfo(); //Get function revision firmware, return 24bit (Major revision number 8b L + Minor revision number 8b + Build revision number 8b H)
void SI4684_ReadOffset(uint16_t offset); //offset response
uint8_t SI4684_GetFullResponse(uint8_t *data, int len); //return cts, data buffer
//void SI4684_FM_Begin(); //Tune Call
//void SI4684_DAB_Begin(); //Tune Call
void SI4684_SetProperty(uint16_t propid, uint16_t value);
uint8_t SI4684_SystemState();//return image name: 1-FMHD is active, 2-DAB is active ....
void SI4684_Volume(uint8_t vol); //=0 Mute, 1-24 Level, use mute in scan mode
//FM API FUNCTION
void SI4684_FM_TuneFrequency(uint16_t freq); //freq * 10Khz, freq = 9100 (91.00Mhz), check status, auto begin power mode
void SI4684_FM_AutoScan(uint8_t dir, uint8_t cont); //dir=1 up, dir=0 down, cont=1 continue seek from opposite band limit, check status
void SI4684_FM_StopScan(); //stop auto scan
int SI4684_FM_GetStatus(FM_STATUS *fm); //Get signal tune/scan check, return Rssi + FM_STATUS struct
int SI4684_FM_GetScanStatus(FM_STATUS *fm, uint8_t scan_stop);
bool SI4684_FM_VaildStatus(FM_STATUS *fm); //Check tune/scan mode, get loop return true, read FM_STATUS.Tune_Freq
bool SI4684_FM_RDS_Available(bool get); //Get loop return true RDS available, get = 1 get new flag
uint32_t SI4684_FM_GetBlockCountRDS(uint8_t clear); //=1 clear, return Number of received 16bit H / Number of expected 16bit L
uint8_t SI4684_FM_GetRDS(RDS_DATA *rds, RTC_TIME *time); //Get RDS DATA + TIME, return Flag RDS Data Mask: =0x00 No RDS, 0x10 RTC, 0x02 TEXT PT, 0x01 LABEL PS
uint8_t SI4684_FM_GetACF(uint8_t *acf); //Get acf buffer flag/data 5 Byte, return buffer[1] acf BLEND, HIGHCUT, SMUTE
//DAB API FUNCTION
void SI4684_DAB_TuneChannel(uint8_t chn); //chn: 0 --> (DAB_FREQ_NUMBER-1), check status, auto begin power mode
int SI4684_DAB_GetStatus(DAB_STATUS *dab); //Get signal tune/scan check, return Rssi + DAB_STATUS struct
bool SI4684_DAB_VaildStatus(DAB_STATUS *dab); //Check tune/scan mode, get loop return true, read DAB_STATUS.Tune_Freq, DAB_STATUS.Tune_Index
uint16_t SI4684_DAB_GetEnsembleInfo(DAB_INFO *info); //Get DAB info/label, return EID + DAB_INFO struct
bool SI4684_DAB_ServiceListAvailable(); //Get loop return true service list available
uint8_t SI4684_DAB_GetServiceList(DAB_SERVICELIST *list); //Get Service List, return number service + DAB_SERVICELIST struct
uint32_t SI4684_DAB_ServiceStart(uint32_t ServiceID, uint32_t ComponentID, uint8_t type); //type=0 Audio, type=1 Data, return ServiceID Run
uint32_t SI4684_DAB_ServiceStop(uint32_t ServiceID, uint32_t ComponentID, uint8_t type); //type=0 Audio, type=1 Data, return ServiceID Stop
uint8_t SI4684_DAB_GetServiceData(DAB_SERVICEDATA *serv); //return length Message + DAB_SERVICEDATA struct
uint8_t SI4684_DAB_GetAudioInfo(DAB_AUDIOINFO *audio); //Get audio info, return Audio mode + DAB_AUDIOINFO struct
uint8_t SI4684_DAB_GetSubchanInfo(uint32_t ServiceID, uint32_t ComponentID, DAB_SUBCHANINFO *sub); //Get subchan info, return Service mode + DAB_SUBCHANINFO struct
uint8_t SI4684_DAB_GetServiceInfo(uint32_t ServiceID, DAB_SERVICEINFO *srv); //Get service info, return Flag + DAB_SERVICEINFO struct
//uint32_t SI4684_DAB_GetServiceLinkingInfo(uint16_t SelectSet, uint32_t ServiceID, DAB_SERVICELINK *link); //Get Linking info, return LinkID_0 + DAB_SERVICELINK struct    (Not use)
uint8_t SI4684_DAB_GetTime(RTC_TIME *time); //Get DAB Time 
//
#endif
