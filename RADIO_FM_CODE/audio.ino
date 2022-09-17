//#include "audio.h"
#include "driver/i2s.h"
//#include "freertos/queue.h"
/********************************************* NAU8822 CODEC ****************************************************/
uint8_t Volume_Value[24] = {3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 24, 27, 30, 33, 36, 39, 42, 45, 48, 51, 54, 57, 60, 63};
uint32_t NAU8822_Sleep_Reg = 0, NAU8822_DeviceID = 0;

//
void NAU8822_Write(uint8_t reg, uint16_t data){ //reg 7 bit, data 9 bit
    I2C_SlaveBegin(NAU8822_ADDRESS); //Lock
    reg <<= 1;
    if(data & 0x100)reg |= 1;
    I2C_WriteByte(reg, data & 0xFF);
    I2C_SlaveEnd(); //Unlock
}

uint16_t NAU8822_Read(uint8_t reg){
    uint32_t data = 0;
    I2C_SlaveBegin(NAU8822_ADDRESS); //Lock
    data = I2C_ReadWord(reg << 1);
    I2C_SlaveEnd(); //Unlock
    return data;
}

uint16_t NAU8822_GetDeviceID(){
    uint8_t id[4];
    I2C_SlaveBegin(NAU8822_ADDRESS); //Lock
    I2C_ReadBuffer(NAU8822_REG_DEVICE_REVISION<<1, id, 4);
    I2C_SlaveEnd(); //Unlock
    NAU8822_DeviceID = ((uint32_t)id[3] << 8) | (uint32_t)id[1];
    if(NAU8822_DeviceID >= 0xFF00)NAU8822_DeviceID = 0;
    return NAU8822_DeviceID;
}

void NAU8822_Sleep(){
    NAU8822_Write(NAU8822_REG_POWER_MANAGEMENT_2, 0x040); //sleep
}

void NAU8822_Wakeup(){
    if(!NAU8822_Sleep_Reg)NAU8822_Sleep_Reg = 0x1BF;
    NAU8822_Write(NAU8822_REG_POWER_MANAGEMENT_2, NAU8822_Sleep_Reg);
}

void NAU8822_Equalizer(uint8_t eq, uint8_t freq, uint8_t gain){ //0-4 EQ, 2 bit freq select 0 -> 3, 5 bit gain control 0 -> 24 (+12dB -> -12dB)
    uint32_t value;
    if(eq > 4)return;
    if(freq > 3)freq = 1; //default
    if(gain > 24)gain = 12;
    value = (freq << 5) | gain;
    if(eq == 0)value |= 0x100; //DAC CUTOFF
    NAU8822_Write(NAU8822_REG_EQ1 + eq, value);
}


void NAU8822_3D_Control(uint8_t value){ //3D Stereo Enhancement effect depth control, +6.67% per binary bit value,  0x03 = 20% effect
    NAU8822_Write(NAU8822_REG_3D_CONTROL, value & 0x0F);
}

void NAU8822_Volume(uint8_t vol){ //0-24
    uint32_t hp, spk;
    if(vol){
		    hp = 0x180 | Volume_Value[vol-1];
        //spk = 0x1B9; //0dB
        spk = hp;
    }else{
        hp = 0x1C0; //Mute
        spk = 0x1F9;
    }
    NAU8822_Write(NAU8822_REG_LHP_VOLUME, hp);
    NAU8822_Write(NAU8822_REG_RHP_VOLUME, hp);
    NAU8822_Write(NAU8822_REG_LSPKOUT_VOLUME, spk);
    NAU8822_Write(NAU8822_REG_RSPKOUT_VOLUME, spk);
}


void NAU8822_Input(uint8_t input){ //Input Select: 0-RADIO MIC IN, 1-PCM BLUETOOTH/FM_ONLINE
    if(!NAU8822_DeviceID)return;
    NAU8822_Write(NAU8822_REG_POWER_MANAGEMENT_3, 0x000);
    NAU8822_Write(NAU8822_REG_POWER_MANAGEMENT_2, 0x000);
    if(!input){
        NAU8822_Write(NAU8822_REG_INPUT_CONTROL, 0x033);
        NAU8822_Write(NAU8822_REG_LEFT_INP_PGA_CONTROL, 0x108); //-6dB
        NAU8822_Write(NAU8822_REG_RIGHT_INP_PGA_CONTROL, 0x108);
        NAU8822_Write(NAU8822_REG_COMPANDING_CONTROL, 0x001);
        NAU8822_Write(NAU8822_REG_POWER_MANAGEMENT_3, 0x06f); //OUT EN, DAC
        NAU8822_Write(NAU8822_REG_POWER_MANAGEMENT_2, 0x1bf); //RUN LR HP, ADC, PGA..
        //NAU8822_Write(NAU8822_REG_GPIO_CONTROL, 0x006); //Set GPIO1=1 TEST RUN
        NAU8822_Sleep_Reg = 0x1bf;
    }else{
        NAU8822_Write(NAU8822_REG_INPUT_CONTROL, 0x000);
        NAU8822_Write(NAU8822_REG_LEFT_INP_PGA_CONTROL, 0x140);//mute
        NAU8822_Write(NAU8822_REG_RIGHT_INP_PGA_CONTROL, 0x140);
        NAU8822_Write(NAU8822_REG_COMPANDING_CONTROL, 0x000);
        NAU8822_Write(NAU8822_REG_POWER_MANAGEMENT_3, 0x06f); //OUT EN, DAC
        NAU8822_Write(NAU8822_REG_POWER_MANAGEMENT_2, 0x180); //RUN LR HP
        //NAU8822_Write(NAU8822_REG_GPIO_CONTROL, 0x007); //Set GPIO1=0 TEST RUN
        NAU8822_Sleep_Reg = 0x180;
    }
}

void NAU8822_Begin(){
    NAU8822_Write(NAU8822_REG_RESET, 0); delay(1);
    if(!NAU8822_GetDeviceID())return;
    NAU8822_Write(NAU8822_REG_POWER_MANAGEMENT_1, 0x00F);
    NAU8822_Write(NAU8822_REG_POWER_MANAGEMENT_3, 0x000);
    //NAU8822_Write(NAU8822_REG_GPIO_CONTROL, 0x006); //Set GPIO1=1 TEST
    NAU8822_Write(NAU8822_REG_CLOCK_CONTROL_1, 0x000); //MCLK pin 11
    NAU8822_Write(NAU8822_REG_AUDIO_INTERFACE, 0x010); //16bit
    NAU8822_Write(NAU8822_REG_CLOCK_CONTROL_2, 0x001); //EN 48K
    NAU8822_Write(NAU8822_REG_COMPANDING_CONTROL, 0x000);
    //
    NAU8822_Write(NAU8822_REG_LEFT_DAC_DIGITAL_VOLUME, 0x1ff); //0dB
    NAU8822_Write(NAU8822_REG_RIGHT_DAC_DIGITAL_VOLUME, 0x1ff);
    NAU8822_Write(NAU8822_REG_LEFT_ADC_DIGITAL_VOLUME, 0x1ff);
    NAU8822_Write(NAU8822_REG_RIGHT_ADC_DIGITAL_VOLUME, 0x1ff);
    //
    NAU8822_Write(NAU8822_REG_LEFT_INP_PGA_CONTROL, 0x110); //0dB   (0x2D)
    NAU8822_Write(NAU8822_REG_RIGHT_INP_PGA_CONTROL, 0x110);
    //
    NAU8822_Write(NAU8822_REG_LHP_VOLUME, 0x1C0); //volume mute
    NAU8822_Write(NAU8822_REG_RHP_VOLUME, 0x1C0);
    NAU8822_Write(NAU8822_REG_LSPKOUT_VOLUME, 0x1F9);
    NAU8822_Write(NAU8822_REG_RSPKOUT_VOLUME, 0x1F9);
    //
    NAU8822_Write(NAU8822_REG_DAC_CONTROL, 0x00C); //c
    NAU8822_Write(NAU8822_REG_ADC_CONTROL, 0x108); //100-64X  108-128X
    NAU8822_Write(NAU8822_REG_ALC_CONTROL_1, 0x038); //dis ALC
    NAU8822_Write(NAU8822_REG_ALC_CONTROL_2, 0x00f);
    //
    NAU8822_Write(NAU8822_REG_INPUT_CONTROL, 0x000);
    NAU8822_Write(NAU8822_REG_OUTPUT_CONTROL, 0x006); //SPKOUT = +1.5x
    //
    NAU8822_Write(NAU8822_REG_LEFT_INP_PGA_CONTROL, 0x140);//mute
    NAU8822_Write(NAU8822_REG_RIGHT_INP_PGA_CONTROL, 0x140);
    NAU8822_Write(NAU8822_REG_LEFT_ADC_BOOST_CONTROL, 0x100);
    NAU8822_Write(NAU8822_REG_RIGHT_ADC_BOOST_CONTROL, 0x100);
    //
    NAU8822_Write(NAU8822_REG_LEFT_MIXER_CONTROL, 0x001); //DAC to MIX
    NAU8822_Write(NAU8822_REG_RIGHT_MIXER_CONTROL, 0x001); 
    NAU8822_Write(NAU8822_REG_AUX1_MIXER, 0x042); //RMIX to AUX1 MIXER  (0x39)
    NAU8822_Write(NAU8822_REG_AUX2_MIXER, 0x042); //LMIX to AUX2 MIXER  (0x38)
    //
    NAU8822_Write(NAU8822_REG_POWER_MANAGEMENT_2, 0x040);  //Stop wait call NAU8822_Input()
    //NAU8822_Input(1);
}

/********************************************* WM8804 OPTICAL ****************************************************/
uint32_t WM8804_DeviceID = 0;

void WM8804_Write(uint8_t reg, uint8_t data){
    I2C_SlaveBegin(WM8804_ADDRESS); //Lock
    I2C_WriteByte(reg, data);
    I2C_SlaveEnd(); //Unlock
}

uint8_t WM8804_Read(uint8_t reg){
    uint8_t data;
    I2C_SlaveBegin(WM8804_ADDRESS); //Lock
    data = I2C_ReadByte(reg);
    I2C_SlaveEnd(); //Unlock
    return data;
}

void WM8804_Sleep(){
    WM8804_Write(WM8804_PWRDN, 0x16);
    AW9523_DigitalWrite(PWR_OFF, 1); //P0_4=1, Laser OFF
}

void WM8804_Wakeup(){
    AW9523_DigitalWrite(PWR_OFF, 0); //P0_4=0, Laser ON
    WM8804_Write(WM8804_PWRDN, 0x02);
}

void WM8804_Mute(uint8_t mute){ //mute = 1
    if(mute)WM8804_Write(WM8804_PWRDN, 0x12);
    else WM8804_Write(WM8804_PWRDN, 0x02);
}

void WM8804_PLL_Output(uint32_t freq){ //98304 KHz
    uint32_t pll_k, pll_n;
    double r = freq; r /= 12000;
    pll_n = (uint32_t)r;
    pll_k = (uint32_t)((r - pll_n) * 0x400000);
    WM8804_Write(WM8804_PLL1, pll_k);
    WM8804_Write(WM8804_PLL2, pll_k >> 8);
    WM8804_Write(WM8804_PLL3, pll_k >> 16);
    WM8804_Write(WM8804_PLL4, pll_n);
    WM8804_Write(WM8804_PLL5, 0x16);
    WM8804_Write(WM8804_PLL6, 0x10);
}

uint16_t WM8804_GetDeviceID(){
    WM8804_DeviceID = WM8804_Read(WM8804_RST_DEVID1);
    WM8804_DeviceID |= (uint32_t)WM8804_Read(WM8804_DEVID2) << 8; //0x8805 
    if(WM8804_DeviceID >= 0xFF00)WM8804_DeviceID = 0;
    return WM8804_DeviceID;
}

void WM8804_Begin(){
    WM8804_Write(WM8804_RST_DEVID1, 0); delay(1);
    if(!WM8804_GetDeviceID())return;
    WM8804_Write(WM8804_SPDTX5, 0xD2);
    WM8804_Write(WM8804_SPDTX4, 0x72);
    WM8804_Write(WM8804_AIFTX, 0x02); //16bit
    WM8804_Write(WM8804_AIFRX, 0x02);
    WM8804_Write(WM8804_PLL6, 0x10);
    WM8804_Write(WM8804_PWRDN, 0x07);
    WM8804_PLL_Output(98304);
    WM8804_Write(WM8804_PWRDN, 0x06); //Stop wait call WM8804_Input()
}



void WM8804_Input(uint8_t input){//Input Select: 0-RADIO PCM, 1-ESP BT PCM/FM ONLINE		(I2S_DAT)
    if(!WM8804_DeviceID)return;
    WM8804_Write(WM8804_PWRDN, 0x12);
    AW9523_Data(PWR_OFF, 0); //ON OPT Laser
    AW9523_DigitalWrite(OPT_SLT, input); //P1_0
  	if(input){
        WM8804_Write(WM8804_AIFRX, 0x02); //slave 32bclk/sample L+R
        I2S_Begin();
    }else{
        I2S_End();
        WM8804_Write(WM8804_AIFRX, 0x42); //master 64bclk/sample L+R
    }
    WM8804_Write(WM8804_PWRDN, 0x02);
}

/********************************************* AW9523 GPIO ****************************************************/
static uint32_t AW9523_IOData = 0;
//
void AW9523_Data(uint8_t pin, uint8_t val){ //set to io data
    if(pin < 16){
        if(val)AW9523_IOData |= (1UL << pin);
        else AW9523_IOData &= ~(1UL << pin);
    }
}

void AW9523_Update(){
    I2C_SlaveBegin(AW9523_ADDRESS); //Lock
    I2C_WriteBuffer(0x02, (uint8_t*)&AW9523_IOData, 2);
    I2C_SlaveEnd(); //Unlock
}

void AW9523_DigitalWrite(uint8_t pin, uint8_t val){ //pin 0-15, val 0-1
    AW9523_Data(pin, val);
    AW9523_Update();
}

void AW9523_PortWrite(uint8_t port, uint8_t val){ //port 0-1, val 0-255
    if(port == 0){
        AW9523_IOData &= 0xFF00;
        AW9523_IOData |= val;
        AW9523_Update();
    }else if(port == 1){
        AW9523_IOData &= 0x00FF;
        AW9523_IOData |= (uint32_t)val << 8;
        AW9523_Update();
    }
}

void AW9523_Begin(){
    AW9523_IOData = 0x0010; //P0_4=1
    I2C_SlaveBegin(AW9523_ADDRESS); //Lock
    I2C_WriteBuffer(0x02, (uint8_t*)&AW9523_IOData, 4); //Init Output
    I2C_WriteByte(0x11, 0);
    I2C_SlaveEnd(); //Unlock
}

/********************************************* MASTER I2S ****************************************************/
uint32_t I2S_Sampe_Rate = 48000, I2S_Mono_Mode = 0;

void I2S_End(){
    i2s_driver_uninstall(I2S_NUM_0);
    pinMode(I2S_LRCLK, INPUT); //I2S_LR
    pinMode(I2S_BCLK, INPUT); //I2S_BCLK
    pinMode(I2S_DOUT, OUTPUT); //I2S_DOUT
    digitalWrite(I2S_DOUT, 0); //DATA ZERO
}

void I2S_Begin(){
    i2s_config_t i2s_config;
    i2s_pin_config_t pin_config;
    //
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    i2s_config.sample_rate = I2S_Sampe_Rate;
    i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    i2s_config.communication_format = I2S_COMM_FORMAT_I2S; //16bit
    i2s_config.intr_alloc_flags = 0;
    i2s_config.dma_buf_count = 16;
    i2s_config.dma_buf_len = 64; //FIFO
    i2s_config.use_apll = false;
    i2s_config.tx_desc_auto_clear = false;
    i2s_config.fixed_mclk = 0; //MCLK
    //
    pin_config.bck_io_num = I2S_BCLK;
    pin_config.ws_io_num = I2S_LRCLK;
    pin_config.data_out_num = I2S_DOUT;
    pin_config.data_in_num = I2S_PIN_NO_CHANGE;
    //
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM_0);
}

void I2S_Sleep(){
    i2s_stop(I2S_NUM_0);
    i2s_zero_dma_buffer(I2S_NUM_0);
}

void I2S_Wakeup(){
    i2s_start(I2S_NUM_0);
}

void I2S_SampleRate(uint32_t rate, uint32_t channel){  //Mono = 1, Stereo = 2
    //i2s_set_sample_rates(I2S_NUM_0, rate);
    if(channel > 1){
        I2S_Mono_Mode = 0;
        i2s_set_clk(I2S_NUM_0, rate, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);
    }else{
        I2S_Mono_Mode = 1;
        i2s_set_clk(I2S_NUM_0, rate, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
    }
    I2S_Sampe_Rate = rate;
}

uint32_t I2S_Write(uint32_t *data, uint32_t len){ //length byte, check return < len => DMA Buffer Full
    uint32_t w = 0;
    i2s_write(I2S_NUM_0, data, len, &w, 1);
    return w;
}

void I2S_Load(uint32_t *data, uint32_t sample){ //Load Audio PCM to I2S DMA, task call polling
    uint32_t w, len, s, a;
    if(I2S_Mono_Mode)a = 1; //Mono: 1 data 32bit = 2 sample (length byte div 2)
    else a = 2; //Stereo: 1 data 32bit = 1 sample (length byte div 4)
    //
    while(sample){
        s = (sample > 64)?64:sample;
        len = s << a;
        w = I2S_Write(data, len);
        if(w < len){
            delay(1); //Wait buffer empty
            s = w >> a;
        }
        data += w >> 2;
        sample -= s;
    }
}
