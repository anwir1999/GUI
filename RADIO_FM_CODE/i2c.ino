//I2C
#define I2C_SCL           15
#define I2C_SDA           13

#define I2C_SCL_High()    digitalWrite(I2C_SCL,1)
#define I2C_SCL_Low()     digitalWrite(I2C_SCL,0)
#define I2C_SDA_High()    digitalWrite(I2C_SDA,1)
#define I2C_SDA_Low()     digitalWrite(I2C_SDA,0)
#define I2C_SDA_Input()   digitalRead(I2C_SDA)
//#define I2C_RTCADDR       0xD0
//#define I2C_EEPADDR       0xA0
//#define RTC_ERROR_TIME    200 //time*10ms
#define I2C_DELAY_CLK()   delayMicroseconds(1)   //Write Clock
#define I2C_DELAY_SDA()   delayMicroseconds(1)   //Read Clock, Res PU 2.2K
#define I2C_SDA_OUT()     pinMode(I2C_SDA,OUTPUT)        //Output PP
#define I2C_SCL_OUT()     pinMode(I2C_SCL,OUTPUT)        //Output PP
#define I2C_SDA_IN()      pinMode(I2C_SDA,INPUT_PULLUP)  //Input PU
#define I2C_SCL_IN()      pinMode(I2C_SCL,INPUT_PULLUP)  //Input PU
/*********************************************************** I2C FW *************************************************************/
static uint32_t I2C_Slave_Address = 0, I2C_Wait = 0; //8bit

void I2C_SlaveBegin(uint8_t addr){
    while(I2C_Slave_Address){ //Lock, wait
        //if(I2C_Slave_Address == addr)return;
        I2C_Wait = 1;
        delay(1); //next task
    }
    I2C_Slave_Address = addr;
}

void I2C_SlaveEnd(){ //Unlock
    I2C_Slave_Address = 0;
    if(I2C_Wait){
        I2C_Wait = 0;
        delay(1);
    }
}
//
uint8_t I2C_Write(uint8_t Data){
    uint32_t i;
    for(i=0; i<8; i++){
        if(Data&0x80)I2C_SDA_High();
        else I2C_SDA_Low();
        Data <<= 1;
        I2C_DELAY_CLK();
        I2C_SCL_High();
        I2C_DELAY_CLK();
        I2C_SCL_Low();
        I2C_DELAY_CLK();
    }
    I2C_SDA_IN();
    I2C_DELAY_CLK();
    I2C_SCL_High();
    I2C_DELAY_CLK();
    i = I2C_SDA_Input();
    I2C_SCL_Low();
    I2C_DELAY_CLK();
    I2C_SDA_OUT();
    return i; //ACK
}

uint8_t I2C_Read(uint8_t Ack){ //ACK=0, NACK=1
    uint32_t I2C_data=0, i;
    I2C_SDA_IN();
    I2C_DELAY_SDA();
    for(i=0;i<8;i++){
        I2C_SCL_High();
        I2C_DELAY_SDA();
        I2C_data <<= 1;
        if(I2C_SDA_Input())I2C_data |= 1;
        I2C_SCL_Low();
        I2C_DELAY_SDA();
    }
    I2C_SDA_OUT();
    if(Ack)I2C_SDA_High();
    else I2C_SDA_Low();
    I2C_DELAY_CLK();
    I2C_SCL_High();
    I2C_DELAY_CLK();
    I2C_SCL_Low();
    I2C_DELAY_CLK();
    return I2C_data;
}

uint8_t I2C_Start(uint8_t Slave){
    if(!Slave)return 1; //NACK
    //Init
    I2C_SCL_High();
    I2C_SDA_High();
    I2C_SCL_OUT();
    I2C_SDA_OUT();
    //Start
    I2C_DELAY_CLK();
    I2C_SDA_Low();
    I2C_DELAY_CLK();
    I2C_SCL_Low();
    I2C_DELAY_CLK();
    Slave = I2C_Write(Slave);
    if(Slave)I2C_Stop(); //NACK
    return Slave; //ACK
}

void I2C_Stop(void){
    //Stop
    I2C_SDA_Low();
    I2C_DELAY_CLK();
    I2C_SCL_High();
    I2C_DELAY_CLK();
    I2C_SDA_High();
    I2C_DELAY_CLK();
    //DeInit
    I2C_SCL_IN();
    I2C_SDA_IN();
}

uint16_t I2C_WriteByte(uint8_t Reg, uint8_t Data){
    if(I2C_Start(I2C_Slave_Address & 0xFE))return 0xFF00;
    I2C_Write(Reg);
    I2C_Write(Data);
    I2C_Stop();
    return Data;
}

uint16_t I2C_ReadByte(uint8_t Reg){
    uint8_t Data;
    if(I2C_Start(I2C_Slave_Address & 0xFE))return 0xFF00;
    I2C_Write(Reg);
    I2C_Start(I2C_Slave_Address | 0x01);
    Data = I2C_Read(1);
    I2C_Stop();
    return Data;
}

uint32_t I2C_ReadWord(uint8_t Reg){
    uint32_t Data0, Data1;
    if(I2C_Start(I2C_Slave_Address & 0xFE))return 0xFF0000;
    I2C_Write(Reg);
    I2C_Start(I2C_Slave_Address | 0x01);
    Data0 = I2C_Read(0);
    Data1 = I2C_Read(1);
    I2C_Stop();
    return (Data0 << 8) | Data1; //MSB
}

void I2C_WriteBuffer(uint8_t Reg, uint8_t *Data, uint8_t Len){
    if(I2C_Start(I2C_Slave_Address & 0xFE))return;
    I2C_Write(Reg);
    while(Len--)if(I2C_Write(*Data++))break; //ACK=0, NACK=1
    I2C_Stop();
}

void I2C_ReadBuffer(uint8_t Reg, uint8_t *Data, uint8_t Len){
    if(I2C_Start(I2C_Slave_Address & 0xFE))return;
    I2C_Write(Reg);
    I2C_Start(I2C_Slave_Address | 0x01);
    while(Len--)*Data++ = I2C_Read((Len)?0:1);  //ACK=0, NACK=1
    I2C_Stop();
}
