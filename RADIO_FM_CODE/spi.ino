#include <SPI.h>
//SPI
#define HSPI_MISO   5
#define HSPI_MOSI   23
#define HSPI_SCLK   18
#define HSPI_CS0    12
#define HSPI_CS1    14
SPIClass            SPI_H(HSPI);
#define FM_CS(b)    digitalWrite(HSPI_CS0, b)
#define LCD_CS(b)   digitalWrite(HSPI_CS1, b)
/********************************************* SPI ****************************************************/
static uint32_t SPI_Lock = 0, SPI_Wait = 0, SPI_Run = 0;

void SPI_Init(){
    SPI_H.begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_CS0);
    SPI_H.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
    pinMode(HSPI_CS0, OUTPUT); FM_CS(1);
    pinMode(HSPI_CS1, OUTPUT); LCD_CS(1);
    SPI_Lock = 0; SPI_Wait = 0; SPI_Run = 1;
}

void SPI_Active(uint8_t cs){ //Lock, CS0, CS1
    if(!SPI_Run)SPI_Init(); //Auto begin
    while(SPI_Lock){
        SPI_Wait = 1;
        delay(1); //next task
    }
    SPI_Lock = cs + 1;
    if(SPI_Lock == 1)FM_CS(0);
    else if(SPI_Lock == 2)LCD_CS(0);
}

void SPI_DeActive(){ //Unlock
    if(SPI_Lock == 1)FM_CS(1);
    else if(SPI_Lock == 2)LCD_CS(1);
    SPI_Lock = 0;
    if(SPI_Wait){
        SPI_Wait = 0;
        delay(1);
    }
}

/*void SPI_ClockFastMode(bool high){
    if(high)SPI_H.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
    else SPI_H.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
}*/

uint8_t SPI_ReadByte(){
    if(!SPI_Lock)return 0;
    return SPI_H.transfer(0);
}

void SPI_WriteByte(uint8_t data){
    if(!SPI_Lock)return;
    SPI_H.write(data);
}

void SPI_WriteBuffer(uint8_t *data, uint32_t len){
    if(!SPI_Lock)return;
    SPI_H.writeBytes(data, len);
}

void SPI_ReadBuffer(uint8_t *data, uint32_t len){
    if(!SPI_Lock)return;
    SPI_H.transferBytes(data, data, len);
}
