#include <pgmspace.h> //Phải có
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <Update.h>
#include <ETH.h>
#include <FS.h>
#include <SPIFFS.h>
#include "esp_ota_ops.h"
#include "main.h"
#include "SI4684.h"
#include "ST7789.h"
#include "audio.h"
void setup(){
    //debug config
    #if(DEBUG_SYSTEM==1)
    Serial.begin(115200, SERIAL_8N1, -1,  1); //DEBUG
    DEBUG_PRINTLN("\r\nSystem Init...");
    #else
    //không cho gửi debug
    Serial.end();
    pinMode(1, OUTPUT);
    digitalWrite(1, 1);
    #endif
    //đọc các thông số về size partition
    const esp_partition_t *data_partition = NULL;
    data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "spiffs");
    if(data_partition != NULL){
        #if(DEBUG_SYSTEM==1)
        Serial.printf("Partiton addr: 0x%08X; size: %d; label: %s\r\n", data_partition->address, data_partition->size, data_partition->label);
        #endif
        if(!SPIFFS.begin()){
            SPIFFS.format();
            DEBUG_PRINTLN("SPIFFS Format!");
            delay(1000); 
            ESP.restart();
        }
        SPIFFS_Size = data_partition->size;
    }
    data_partition = esp_ota_get_next_update_partition(NULL);
    if(data_partition != NULL){
        #if(DEBUG_SYSTEM==1)
        Serial.printf("Partiton addr: 0x%08X; size: %d; label: %s\r\n", data_partition->address, data_partition->size, data_partition->label);
        #endif
        OTA_Size = data_partition->size;
    }
    data_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "eeprom");
    if(data_partition != NULL){
        #if(DEBUG_SYSTEM==1)
        Serial.printf("Partiton addr: 0x%08X; size: %d; label: %s\r\n", data_partition->address, data_partition->size, data_partition->label);
        #endif
        EEPROM_Address = data_partition->address;
    }
    
    //ghi các chỉ số của esp
    #if(DEBUG_SYSTEM==1)
    Serial.printf("Total heap: %d\r\n", ESP.getHeapSize());
    Serial.printf("Free heap: %d\r\n", ESP.getFreeHeap());
    Serial.printf("Total PSRAM: %d\r\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d\r\n", ESP.getFreePsram());
    #endif
    delay(50);
    //bat đầu setup
    System_Stating(); //Starting
}
void loop(){
    System_TestTask();
}
