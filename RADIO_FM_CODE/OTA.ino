#include <Arduino.h>
#include <HTTPUpdate.h>
 
void update_FOTA();
 
uint32_t updateCounter = 0;
String version = "1.1";
String key = "7e723a82-6ea6-47ba-807b-b7e0928259f4";
   
void OTA_loop()
{
  Serial.print("Ver: ");
  Serial.println(version);
  delay(500);
  if (WiFi.status() == WL_CONNECTED)
  {
    updateCounter++;
    if (updateCounter > 10)
    {
      updateCounter = 0;
      Serial.println("Check update");
      update_FOTA();
    }
  }
}
 
String getChipId()
{
  String ChipIdHex = String((uint32_t)(ESP.getEfuseMac() >> 32), HEX);
  ChipIdHex += String((uint32_t)ESP.getEfuseMac(), HEX);
  return ChipIdHex;
}
 
void update_FOTA()
{
  String url = "http://otadrive.com/deviceapi/update?";
  url += "k=" + key;
  url += "&v=" + version;
  url += "&s=" + getChipId(); // định danh thiết bị trên Cloud
 
  WiFiClient client;
  httpUpdate.update(client, url, version);
}
