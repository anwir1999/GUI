//#include "SI4684.h"
#include "rom/SI4684_Rom.h"
static uint8_t SI4684_Power_Config[16] = {SI4684_POWER_UP, 0x80, 0x15, 0x3C, 0x00, 0x80, 0xBB, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00};
static uint8_t rt_buffer[2][64], ps_buffer[2][8]; //RDS Temp
extern uint8_t Volume_Value[];
uint32_t SI4684_PartNo = 0; //16bit
uint32_t SI4684_Power = 0, SI4684_Mode = 0, SI4684_Flag = 0; //8bit
uint32_t SI4684_ChipRev = 0, SI4684_RomID = 0, SI4684_VerMajor = 0, SI4684_VerMinor = 0, SI4684_VerBuild = 0; //8bit
/********************************************* FM ****************************************************/
void SI4684_End(){
  	SI4684_Mode = 0;
    SI4684_Power = 0;
    SI4684_ChipRev = 0;
    SI4684_RomID = 0;
    SI4684_PartNo = 0;
    SI4684_Flag = 0;
    AW9523_DigitalWrite(FM_RST, 0); //P1_1 Reset
    delayMicroseconds(20);
    AW9523_DigitalWrite(FM_RST, 1);
    delay(5);
}

uint8_t SI4684_GetFlag(){
    return SI4684_Flag;
}

uint8_t SI4684_ReplyN(){
    uint8_t cmd[4];
    cmd[0] = SI4684_RD_REPLY;
    cmd[1] = 0;
	  SPI_Active(0);
    SPI_ReadBuffer(cmd, 2);
    SI4684_Flag = cmd[1];
    return SI4684_Flag;
}

uint8_t SI4684_Reply(){
	  uint8_t r = SI4684_ReplyN();
	  SPI_DeActive();
	  return r;
}

uint8_t SI4684_GetFullResponseN(uint8_t *data, int len){ //Not End CS
    uint8_t r, l = 2;
    memset(data, 0, len);
    GET_LOOP: delay(5); //delay process
    r = SI4684_ReplyN();
    if(r & SI4684_ERRCMD)goto QUIT; //ERR=1 
    else if(!(r & SI4684_CTSINT)){ //CTS=0
        if(l){ l--; SPI_DeActive(); goto GET_LOOP; }
        else goto QUIT;
    }
    *data++ = r;
    if(--len > 0){
        SPI_ReadBuffer(data, len);
        return r; //1 byte flag cts (8bit)
    }
    QUIT: SPI_DeActive();
    return 0;
}

uint8_t SI4684_GetFullResponse(uint8_t *data, int len){ //End CS=1
    uint8_t r = SI4684_GetFullResponseN(data, len);
    SPI_DeActive();
    return r;
}

int SI4684_CheckBusy(int loop){
    uint8_t r;
    while(true){
        delay(10); //delay process 10ms
        r = SI4684_Reply();
        if(r & SI4684_ERRCMD)return -1; //ERROR CMD
        else if(r & SI4684_CTSINT)return 1; //CTS - CMD OK
        if(!loop)break; loop--;
    }
    return 0; //ERROR LOOP
}

void SI4684_CmdSend(uint8_t *cmd, uint32_t len){
    SPI_Active(0);
    SPI_WriteBuffer(cmd, len);
    SPI_DeActive();
}

int SI4684_Send(uint8_t *cmd, uint32_t len){
	  SI4684_CmdSend(cmd, len);
	  return SI4684_CheckBusy(2);
}


int SI4684_HostLoad_Boot(const uint8_t *fw, uint32_t len){
	  if(SI4684_CheckBusy(0) != 1)return 0;
	  uint32_t w;
	  uint8_t cmd[4], *data;
	  cmd[0] = SI4684_LOAD_INIT; cmd[1] = 0;
	  SI4684_Send(cmd, 2); 
	  data = (uint8_t*)malloc(4096);
    data[0] = SI4684_HOST_LOAD;
    data[1] = 0; data[2] = 0; data[3] = 0;
	  while(len > 0){
		    w = (len > 4092)?4092:len; 
		    memcpy_P(&data[4], fw, w);
		    SPI_Active(0);
		    SPI_WriteBuffer(data, w+4);
		    SPI_DeActive();
		    fw += w;
		    len -= w;
		    SI4684_CheckBusy(1);
	  }
	  free(data);
	  cmd[0] = SI4684_BOOT; cmd[1] = 0;
	  SI4684_CmdSend(cmd, 2);
    delay(1000);
	  if(SI4684_CheckBusy(8) <= 0)return 0;
	  return 1;
}

void SI4684_ReadOffset(uint16_t offset){
    if(!SI4684_Power)return;
    uint8_t cmd[4];
    cmd[0] = SI4684_READ_OFFSET;
    cmd[1] = 0;
    cmd[2] = offset;
    cmd[3] = offset >> 8;
	  SI4684_Send(cmd, 4);
}

uint16_t SI4684_GetPartInfo(){ //Name IC
    if(!SI4684_Power)return 0;
    uint8_t cmd[12];
    cmd[0] = SI4684_GET_PART_INFO; 
    cmd[1] = 0;
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponse(cmd, 10) == 0)return 0;
    SI4684_ChipRev = cmd[4];
    SI4684_RomID = cmd[5];
    SI4684_PartNo = ((uint32_t)cmd[9] << 8) | cmd[8];
    return SI4684_PartNo;
}

uint32_t SI4684_GetFuncInfo(){ //Firmware Info
    if(!SI4684_Power)return 0;
    uint8_t cmd[8];
    cmd[0] = SI4684_GET_FUNC_INFO; 
    cmd[1] = 0;
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponse(cmd, 8) == 0)return 0;
    SI4684_VerMajor = cmd[4];
    SI4684_VerMinor = cmd[5];
    SI4684_VerBuild = cmd[6];
    return read32(&cmd[4]) & 0xFFFFFF;
}

void SI4684_SetProperty(uint16_t propid, uint16_t value){
	  if(!SI4684_Power)return;
	  uint8_t cmd[8];
	  cmd[0] = SI4684_SET_PROPERTY;
	  cmd[1] = 0;
    cmd[2] = propid;
    cmd[3] = propid >> 8;
    cmd[4] = value;
    cmd[5] = value >> 8;
	  SI4684_Send(cmd, 6);
    delay(1);
}

void SI4684_FM_TuneFrequency(uint16_t freq){ //Freq * 10Khz, VD: 9100 (91.00Mhz)
	  if(!SI4684_Power || SI4684_Mode != 1)SI4684_FM_Begin();
	  uint8_t cmd[8];
	  cmd[0] = SI4684_FM_TUNE_FREQ;
	  cmd[1] = 0;
    cmd[2] = freq;
    cmd[3] = freq >> 8;
    cmd[4] = 0;
    cmd[5] = 0;
	  SI4684_CmdSend(cmd, 6);
    for(int i=0; i<8; i++){
        if(SI4684_CheckBusy(0) == -1)break;
        if(SI4684_Flag & SI4684_STCINT)break;
    }
}

void SI4684_FM_AutoScan(uint8_t dir, uint8_t cont){ //dir=1 up, dir=0 down, cont=1 continue seek from opposite band limit, check status
	  if(!SI4684_Power || SI4684_Mode != 1)return;
	  uint8_t cmd[8];
	  memset(cmd, 0, 6);
	  cmd[0] = SI4684_FM_SEEK_START;
	  if(dir)cmd[2] |= 0x02;
	  if(cont)cmd[2] |= 0x01;
	  SI4684_Send(cmd, 6);
}

void SI4684_FM_StopScan(){
    if(!SI4684_Power || SI4684_Mode != 1)return;
    uint8_t cmd[4];
    cmd[0] = SI4684_FM_RSQ_STATUS;
    cmd[1] = 0x02;
    SI4684_Send(cmd, 2);
    delay(1);
}

int SI4684_FM_GetScanStatus(FM_STATUS *fm, uint8_t scan_stop){ //Get signal tune, scan check
    if(!SI4684_Power || SI4684_Mode != 1)return -128;
    uint8_t cmd[20];
    cmd[0] = SI4684_FM_RSQ_STATUS;
    cmd[1] = 0x08;
    if(scan_stop)cmd[1] |= 0x02;
    if(SI4684_Flag & SI4684_STCINT)cmd[1] |= 0x01;
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponse(cmd, 17) == 0)return -128;
    if(fm)memcpy((uint8_t*)&fm->Resp_Flag, &cmd[4], sizeof(FM_STATUS)); //Flag, RSSI, SNR...
    return CharToInt(cmd[9]); //RSSI: -128 -> 127
}

int SI4684_FM_GetStatus(FM_STATUS *fm){
    return SI4684_FM_GetScanStatus(fm, 1);
}

bool SI4684_FM_VaildStatus(FM_STATUS *fm){ //Check tune/scan mode, get loop return true, fm->Tune_Freq
    int rssi = SI4684_FM_GetScanStatus(fm, 0);
    int snr = CharToInt(fm->SNR);
    if(fm->Resp_Flag & 0x0100)return 1; //auto
    else if( (rssi > 11 && snr > 6) ){ //manual
        int fos = CharToInt(fm->Freq_Offset);
        if(fos > -100 && fos < 100)return 1; //channel is valid
    }
    return 0;
}

uint8_t SI4684_FM_GetACF(uint8_t *acf){ //Get acf buffer flag/data 5 Byte
	  if(!SI4684_Power || SI4684_Mode != 1)return 0;
	  uint8_t cmd[12];
	  cmd[0] = SI4684_FM_ACF_STATUS;
	  cmd[1] = 0x01;
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponse(cmd, 9) == 0)return 0;
	  if(acf)memcpy(&acf[0], &cmd[4], 5);
	  return cmd[5]; //Flag BLEND, HIGHCUT, SMUTE...
}

uint8_t SI4684_SystemState(){  //return image name
	  if(!SI4684_Power)return 0;
	  uint8_t cmd[8];
	  cmd[0] = SI4684_GET_SYS_STATE;
	  cmd[1] = 0x00;
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponse(cmd, 5) == 0)return 0;
	  return cmd[4];
}

bool SI4684_FM_RDS_Available(bool get){
    if(get)SI4684_Reply();
    return (SI4684_Flag & SI4684_RDSINT)?1:0;
}

uint32_t SI4684_FM_GetBlockCountRDS(uint8_t clear){ //return Number of received 16bit H / Number of expected 16bit L
    if(!SI4684_Power || SI4684_Mode != 1)return 0;
    uint8_t cmd[12];
    cmd[0] = SI4684_FM_RDS_BLOCKCOUNT;
    cmd[1] = clear; //=1 clear value
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponse(cmd, 10) == 0)return 0;
    return read32(&cmd[4]);
}

static void RDS_RT_Buffer_Clear(){
    memset(&rt_buffer[0][0], 0, 128);
}

static uint8_t SI4684_RDS_Decode(RDS_DATA *rds, RTC_TIME *time, uint16_t *block){
    uint32_t i, ret = 0;
    uint32_t rds_group, offset;
    rds_group = (block[1] >> 11); //B
    if(rds_group == RDS_GROUP_0A || rds_group == RDS_GROUP_0B){
        offset = block[1] & 0x03;
        i = offset << 1;
        ps_buffer[1][i] = ps_buffer[0][i];
        ps_buffer[1][i+1] = ps_buffer[0][i+1];
        ps_buffer[0][i] = block[3] >> 8;
        ps_buffer[0][i+1] = block[3];
        if(offset == 3){
            for(i=0; i<8; i++){
                if(ps_buffer[0][i] != ps_buffer[1][i])break;
            }
            if(i == 8){
                for(i=0; i<8; i++){
                    if(rds->Label[i] != ps_buffer[0][i])break;
                }
                if(i != 8){
                    for(i=0; i<8; i++){
                        rds->Label[i] = ps_buffer[0][i];
                    }
                    ret = 0x01;
                    rds->Label[8] = 0;
                }
            }
        }
        //rds->Label[8] = 0;
    }else if(rds_group == RDS_GROUP_2A || rds_group == RDS_GROUP_2B){
        uint32_t state, same, text_size, first_zero; //8bit
        state = (block[1] & 0x0010) ? 0x80 : 0x00;
        state += rds_group;
        if(state != rds->State)RDS_RT_Buffer_Clear();
        rds->State = state;
        if(rds_group == RDS_GROUP_2A){
            offset = (block[1] & 0xf) << 2;
            rt_buffer[1][offset + 0] = rt_buffer[0][offset + 0];
            rt_buffer[1][offset + 1] = rt_buffer[0][offset + 1];
            rt_buffer[1][offset + 2] = rt_buffer[0][offset + 2];
            rt_buffer[1][offset + 3] = rt_buffer[0][offset + 3];
            rt_buffer[0][offset + 0] = (block[2] >> 8)   ? (block[2] >> 8)   : 0x20;
            rt_buffer[0][offset + 1] = (block[2] & 0xff) ? (block[2] & 0xff) : 0x20;
            rt_buffer[0][offset + 2] = (block[3] >> 8)   ? (block[3] >> 8)   : 0x20;
            rt_buffer[0][offset + 3] = (block[3] & 0xff) ? (block[3] & 0xff) : 0x20;
            if(((rt_buffer[1][offset + 0]) && (rt_buffer[1][offset + 0] != rt_buffer[0][offset + 0])) || ((rt_buffer[1][offset + 1]) && (rt_buffer[1][offset + 1] != rt_buffer[0][offset + 1])) || ((rt_buffer[1][offset + 2]) && (rt_buffer[1][offset + 2] != rt_buffer[0][offset + 2])) || ((rt_buffer[1][offset + 3]) && (rt_buffer[1][offset + 3] != rt_buffer[0][offset + 3]))){ 
                RDS_RT_Buffer_Clear();
                rt_buffer[0][offset + 0] = (block[2] >> 8)   ? (block[2] >> 8)   : 0x20;
                rt_buffer[0][offset + 1] = (block[2] & 0xff) ? (block[2] & 0xff) : 0x20;
                rt_buffer[0][offset + 2] = (block[3] >> 8)   ? (block[3] >> 8)   : 0x20;
                rt_buffer[0][offset + 3] = (block[3] & 0xff) ? (block[3] & 0xff) : 0x20;
            }
        }else{
            offset = (block[1] & 0x0F) << 1;
            rt_buffer[1][offset + 0] = rt_buffer[0][offset + 0];
            rt_buffer[1][offset + 1] = rt_buffer[0][offset + 1];
            rt_buffer[0][offset + 0] = (block[3] >> 8)   ? (block[3] >> 8)   : 0x20;
            rt_buffer[0][offset + 1] = (block[3] & 0xff) ? (block[3] & 0xff) : 0x20;
            if(((rt_buffer[1][offset + 0]) && (rt_buffer[1][offset + 0] != rt_buffer[0][offset + 0])) || ((rt_buffer[1][offset + 1]) && (rt_buffer[1][offset + 1] != rt_buffer[0][offset + 1]))){
                RDS_RT_Buffer_Clear();         
                rt_buffer[0][offset + 0] = (block[3] >> 8)   ? (block[3] >> 8)   : 0x20;
                rt_buffer[0][offset + 1] = (block[3] & 0xff) ? (block[3] & 0xff) : 0x20;
            }
        }
        same = 1;
        offset = 0;
        text_size = 0;
        first_zero = 0xff;
        while(same && (offset < 64)){
            if(rt_buffer[1][offset] != rt_buffer[0][offset])same = 0;
            if(rt_buffer[0][offset] > 0)text_size = offset;
            else if(first_zero == 0xff)first_zero = offset;
            offset++;
        }
        if(first_zero < text_size)same = 0;
        if(same){
            offset = 0;
            while (same && (offset < 64)){
                if(rds->Text[offset] != rt_buffer[0][offset])same = 0;
                offset++;
            }
            if(same == 0){
                for(i=0; i<64; i++)rds->Text[i] = rt_buffer[0][i];
                rds->Text[64] = 0;
                ret = 0x02;
            }
        }
    }else if(rds_group == RDS_GROUP_4A){
        long J, C, Y, M;
        i = (block[1] & 0x03); //mjd
        i <<= 15;
        i += ((block[2] >> 1) & 0x7FFF); 
        //
        J = i + 2400001 + 68569;
        C = 4 * J / 146097;
        J = J - (146097 * C + 3) / 4;
        Y = 4000 * (J + 1) / 1461001;
        J = J - 1461 * Y / 4 + 31;
        M = 80 * J / 2447;
        time->Date = J - ((2447 * M) / 80); J = M / 11;
        time->Month = M + 2 - (12 * J);
        time->Year = ((100 * (C - 49)) + (Y + J)) - 2000;
        time->Hour = ((block[2] << 4) & 0x10) | ((block[3] >> 12) & 0x0f);
        time->Min = ((block[3] >> 6) & 0x3f);
        ret = 0x10;
    }
    return ret;
}

uint8_t SI4684_FM_GetRDS(RDS_DATA *rds, RTC_TIME *time){ //Get RDS DATA + TIME
    if(!SI4684_Power || SI4684_Mode != 1  || !rds)return 0;
    uint8_t cmd[20], flag = 0, val;
    GET_RDS: cmd[0] = SI4684_FM_RDS_STATUS; 
    cmd[1] = 0x01; //Clear RDSINT
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponse(cmd, 20) == 0)return 0;
    val = cmd[5];
    if(val & 0x08)rds->Pi = read16(&cmd[8]);
    if(val & 0x10)rds->Pty = cmd[6];
    if(val & 0x02){ //RDSSYNC
        flag |= SI4684_RDS_Decode(rds, time, (uint16_t*)&cmd[12]); //Decode RDS
        if(cmd[10]){ delay(1); goto GET_RDS; } //FIFO
    }
    rds->Flag = flag;
    return flag; //Flag RDS Data Mask: 0x00 No RDS, 0x10 TIME, 0x02 TEXT PT, 0x01 LABEL PS
}

void SI4684_DAB_SetFreqList(){
    if(!SI4684_Power || SI4684_Mode != 2)return;
    uint8_t cmd[4];
    uint32_t f, freq;
    cmd[0] = SI4684_DAB_SET_FREQ_LIST;
    cmd[1] = DAB_FREQ_NUMBER; //1-48
    cmd[2] = 0;
    cmd[3] = 0;
    SPI_Active(0);
    SPI_WriteBuffer(cmd, 4);
    for(f=0; f < DAB_FREQ_NUMBER; f++){
        freq = pgm_read_dword_near(&DAB_FreqList[f]);
        SPI_WriteBuffer((uint8_t*)&freq, 4);
    }
    SPI_DeActive();
    SI4684_CheckBusy(1);
}

void SI4684_DAB_TuneChannel(uint8_t chn){ //Tune channel DAB
    if(chn >= DAB_FREQ_NUMBER)return;
    if(!SI4684_Power || SI4684_Mode != 2)SI4684_DAB_Begin();
    uint8_t cmd[8];
    cmd[0] = SI4684_DAB_TUNE_FREQ;
    cmd[1] = 0x00;
    cmd[2] = chn;
    cmd[3] = 0;
    cmd[4] = 0; //ANTCAP
    cmd[5] = 0;
    SI4684_Send(cmd, 6);
    for(int i=0; i<8; i++){
        if(SI4684_CheckBusy(0) == -1)break;
        if(SI4684_Flag & SI4684_STCINT)break;
    }
}

int SI4684_DAB_GetStatus(DAB_STATUS *dab){ //Check signal tune, scan check
    if(!SI4684_Power || SI4684_Mode != 2)return -128;
    uint8_t cmd[24];
    cmd[0] = SI4684_DAB_DIGRAD_STATUS;
    cmd[1] = 0x00; //0x08 Clear all flag
    if(SI4684_Flag & SI4684_STCINT)cmd[1] |= 0x01;
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponse(cmd, 22) == 0)return -128;
    if(dab)memcpy((uint8_t*)&dab->Resp_Flag, &cmd[4], sizeof(DAB_STATUS)); //Flag, RSSI, SNR, FIC_QUALITY, CNR..
    return CharToInt(cmd[6]); //RSSI: -128 -> 63
}

bool SI4684_DAB_VaildStatus(DAB_STATUS *dab){ //Check tune/scan mode, return true, dab->Tune_Freq, dab->Tune_Index
    int rssi = SI4684_DAB_GetStatus(dab);
    if( (dab->Resp_Flag & 0x0100) || ((rssi > 11) && (dab->FIC_Quality > 25)) )return 1; //channel is valid
    return 0;
}

uint8_t SI4684_DAB_GetServiceList(DAB_SERVICELIST *list){ 
    if(!SI4684_Power || SI4684_Mode != 2)return 0;
    uint8_t cmd[8];
    uint32_t number, component, index, i;
    uint8_t *data = NULL;
    cmd[0] = SI4684_DAB_GET_SERVICE_LIST;
    cmd[1] = 0; //DAB List
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponseN(cmd, 6) == 0)return 0;
    i = read16(&cmd[4]);
    if(i){
        data = (uint8_t*)malloc(i);
        SPI_ReadBuffer(data, i);
    }
    SPI_DeActive();
    if(!data)return 0;
    //PARSE DATA
    number = data[4]; index = 8;
    if(number > SERVICE_LIST_MAX)number = SERVICE_LIST_MAX;
    for(i=0; i < number; i++){
        list->Service[i].ServiceID = read32(&data[index]); index += 4;
        list->Service[i].Flag = data[index++]; //12
        component = data[index] & 0x0F; index += 3; //13
        memcpy((uint8_t*)&list->Service[i].Label[0], &data[index], 16); //16
        list->Service[i].Label[16] = 0; index += 16;
        list->Service[i].ComponentID = read32(&data[index]); //Get ComponentID 0 only
        index += component << 2; //skip
    }
    free(data);
    return number;
}

uint32_t SI4684_DAB_ServiceStart(uint32_t ServiceID, uint32_t ComponentID, uint8_t type){ //type=0 Audio, type=1 Data
    if(!SI4684_Power || SI4684_Mode != 2)return 0;
    uint8_t cmd[12];
    cmd[0] = SI4684_DAB_START_SERVICE;
    cmd[1] = type;
    cmd[2] = 0;
    cmd[3] = 0;
    write32(&cmd[4], ServiceID);
    write32(&cmd[8], ComponentID);
    if(SI4684_Send(cmd, 12) != 1)return 0;
    return ServiceID;
}

uint32_t SI4684_DAB_ServiceStop(uint32_t ServiceID, uint32_t ComponentID, uint8_t type){ //type=0 Audio, type=1 Data
	  if(!SI4684_Power || SI4684_Mode != 2)return 0;
	  uint8_t cmd[12];
	  cmd[0] = SI4684_DAB_STOP_SERVICE;
	  cmd[1] = type;
	  cmd[2] = 0;
	  cmd[3] = 0;
    write32(&cmd[4], ServiceID);
    write32(&cmd[8], ComponentID);
	  if(SI4684_Send(cmd, 12) != 1)return 0;
	  return ServiceID;
}

uint8_t SI4684_DAB_GetServiceData(DAB_SERVICEDATA *serv){
    if(!SI4684_Power || SI4684_Mode != 2 || !serv)return 0;
	  uint8_t cmd[28];
	  int len;
	  cmd[0] = SI4684_DAB_DATA_SERVICE;
	  cmd[1] = 1;
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponseN(cmd, 26) == 0)return 0;
    serv->ServiceID = read32(&cmd[8]);
    serv->ComponentID = read32(&cmd[12]);
    serv->UAType = read16(&cmd[16]);
	  len = read16(&cmd[18]) - 2; //ByteCount - 2
    memset((uint8_t*)&serv->Message[0], 0, 128); //Clear last message
	  if(len > 0){
        if((cmd[7] & 0xC0) == 0x80){ //Indicates that the payload is DLS PAD and DATA_TYPE is 0.
            if((cmd[24] & 0x10) == 0x00){ //DLS Message
                if(len > 128)len = 128;
                SPI_ReadBuffer((uint8_t*)&serv->Message[0], len); //Read payload
                serv->Message[len] = 0;
                serv->Length = len;
            }
        }
	  }else len = 0;
	  SPI_DeActive();
	  return len;
}

uint16_t SI4684_DAB_GetEnsembleInfo(DAB_INFO *info){ 
	  if(!SI4684_Power || SI4684_Mode != 2)return 0;
	  uint8_t cmd[28];
	  cmd[0] = SI4684_DAB_GET_ENSEMBLE_INFO;
	  cmd[1] = 0x00;
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponse(cmd, 26) == 0)return 0;
	  if(info)memcpy((uint8_t*)&info->EID, &cmd[4], sizeof(DAB_INFO));
	  return read16(&cmd[4]); //EID
}

uint8_t SI4684_DAB_GetAudioInfo(DAB_AUDIOINFO *audio){ 
	  if(!SI4684_Power || SI4684_Mode != 2)return 0;
	  uint8_t cmd[12];
	  cmd[0] = SI4684_DAB_GET_AUDIO_INFO;
	  cmd[1] = 0x00;
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponse(cmd, 10) == 0)return 0;
	  if(audio)memcpy((uint8_t*)&audio->Bit_Rate, &cmd[4], sizeof(DAB_AUDIOINFO));
	  return cmd[8] & 0x03; //Audio Mode
}

uint32_t SI4684_DAB_GetServiceLinkingInfo(uint16_t SelectSet, uint32_t ServiceID, DAB_SERVICELINK *link){ 
	  if(!SI4684_Power || SI4684_Mode != 2)return 0;
	  uint8_t cmd[16];
	  cmd[0] = SI4684_DAB_GET_SERVICE_LINKING_INFO;
	  cmd[1] = SelectSet;
	  cmd[2] = SelectSet >> 8;
	  cmd[3] = 0x00;
    write32(&cmd[4], ServiceID);
    SI4684_CmdSend(cmd, 8);
    if(SI4684_GetFullResponse(cmd, 16) == 0)return 0;
	  if(link)memcpy((uint8_t*)&link->Size, &cmd[4], sizeof(DAB_SERVICELINK));
	  return read32(&cmd[12]);
}

bool SI4684_DAB_ServiceListAvailable(){ //Get loop return true service list available
    if(!SI4684_Power || SI4684_Mode != 2)return 0;
    uint8_t cmd[8];
    cmd[0] = SI4684_DAB_GET_EVENT_STATUS;
    cmd[1] = 0;
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponse(cmd, 6) == 0)return 0;
    if(cmd[5] & 0x01)return 1;  //Check SVRLIST
    return 0;
}

uint8_t SI4684_DAB_GetServiceInfo(uint32_t ServiceID, DAB_SERVICEINFO *srv){ 
	  if(!SI4684_Power || SI4684_Mode != 2)return 0;
	  uint8_t cmd[28];
	  cmd[0] = SI4684_DAB_GET_SERVICE_INFO;
	  cmd[1] = 0x00;
	  cmd[2] = 0x00;
	  cmd[3] = 0x00;
	  write32(&cmd[4], ServiceID);
    SI4684_CmdSend(cmd, 8);
    if(SI4684_GetFullResponse(cmd, 26) == 0)return 0;
	  if(srv)memcpy((uint8_t*)&srv->PTY_PD, &cmd[4], sizeof(DAB_SERVICEINFO));
	  return cmd[4]; //Flag
}

uint8_t SI4684_DAB_GetSubchanInfo(uint32_t ServiceID, uint32_t ComponentID, DAB_SUBCHANINFO *sub){ 
	  if(!SI4684_Power || SI4684_Mode != 2)return 0;
	  uint8_t cmd[12];
	  cmd[0] = SI4684_DAB_GET_SUBCHAN_INFO;
	  cmd[1] = 0x00;
	  cmd[2] = 0x00;
	  cmd[3] = 0x00;
	  write32(&cmd[4], ServiceID);
    write32(&cmd[8], ComponentID);
    SI4684_CmdSend(cmd, 12);
    if(SI4684_GetFullResponse(cmd, 12) == 0)return 0;
	  if(sub)memcpy((uint8_t*)&sub->Service_Mode, &cmd[4], sizeof(DAB_SUBCHANINFO));
	  return cmd[4]; //Service mode
}

uint8_t SI4684_DAB_GetTime(RTC_TIME *time){
	  if(!SI4684_Power || SI4684_Mode != 2)return 0;
	  uint8_t cmd[12];
	  cmd[0] = SI4684_DAB_GET_TIME;
	  cmd[1] = 0x00;
    SI4684_CmdSend(cmd, 2);
    if(SI4684_GetFullResponse(cmd, 11) == 0)return 0;
	  time->Year = (((uint32_t)cmd[5] << 8) | cmd[4]) - 2000;
	  time->Month = cmd[6];
	  time->Date = cmd[7];
	  time->Hour = cmd[8];
	  time->Min = cmd[9];
	  time->Sec = cmd[10];
	  return 1;
}


void SI4684_FM_Begin(){
	  SI4684_End(); //RESET
	  SI4684_Send(SI4684_Power_Config, 16); //Power On
	  SI4684_HostLoad_Boot(SI4684_rom_fm_fw, sizeof(SI4684_rom_fm_fw));
    delay(100);
	  SI4684_SetProperty(0x1712, 0x0000);
	  SI4684_SetProperty(0x1710, 0xEE25);
	  SI4684_SetProperty(0x1711, 0x01C5);
	  SI4684_SetProperty(0x3C01, 0x0006);
	  SI4684_SetProperty(0x3C02, 0x00A1);
	  SI4684_SetProperty(0x3C03, 0x1111);
	  SI4684_SetProperty(0x3202, 0x000B); //RSSI Set 11
    SI4684_SetProperty(0x3204, 0x0006); //SNR Set 6
	  SI4684_SetProperty(0x3100, 0x222E);
	  SI4684_SetProperty(0x3101, 0x2A30);
	  SI4684_SetProperty(0x3102, 0x000A); //100Khz
	  //SI4684_SetProperty(0x3102, 0x0014); //200Khz
	  SI4684_SetProperty(0x3900, 0x0001);
	  SI4684_SetProperty(0x0302, 0x0000); //Mono off
	  SI4684_SetProperty(0x0300, 63);
    SI4684_SetProperty(0x0000, 0x00ff); //INT ALL
    //SI4684_SetProperty(0x3402, 200);
    //SI4684_SetProperty(0x3403, 0);
	  SI4684_Power = 1;
	  SI4684_Mode = 1; //FM
}

void SI4684_DAB_Begin(){
	  SI4684_End(); //RESET
	  SI4684_Send(SI4684_Power_Config, 16); //Power On
	  SI4684_HostLoad_Boot(SI4684_rom_dab_fw, sizeof(SI4684_rom_dab_fw));
    delay(100);
	  SI4684_SetProperty(0x1712, 0x0001);
	  SI4684_SetProperty(0x1710, 0xF558);
	  SI4684_SetProperty(0x1711, 0x01FA);
	  SI4684_SetProperty(0xB201, 0x000B); //RSSI set 11
	  SI4684_SetProperty(0xB400, 0x4005); //Set PAD application data
	  SI4684_SetProperty(0xB301, 0x0006);
	  SI4684_SetProperty(0xB302, 0x0006);
	  SI4684_SetProperty(0xB500, 0x0001);
	  SI4684_SetProperty(0xB501, 0x0005);
	  SI4684_SetProperty(0xB503, 0xE2C4);
	  SI4684_SetProperty(0x0302, 0x0000); //Mono off
	  SI4684_SetProperty(0x0300, 63);
    SI4684_SetProperty(0x0000, 0x00f1); //INT ALL
	  SI4684_Power = 1;
	  SI4684_Mode = 2; //DAB
	  SI4684_DAB_SetFreqList();
}


void SI4684_Volume(uint8_t vol){ //0-24
	  if(!vol){
		    SI4684_SetProperty(0x0301, 3);
		    SI4684_SetProperty(0x0300, 0);
	  }else{
		    if(vol > 24)vol = 24;
		    vol = Volume_Value[vol-1];
		    SI4684_SetProperty(0x0301, 0);
		    SI4684_SetProperty(0x0300, vol);
	  }
}
