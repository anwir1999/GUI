#include <WiFi.h>

IPAddress wlan_ip;
IPAddress wlan_gateway;
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);
void Wifi_Smartconfig(void)
{
  WiFi.mode(WIFI_AP_STA);
  /* start SmartConfig */
  WiFi.beginSmartConfig();
  /* Wait for SmartConfig packet from mobile */
  //Serial.println("Waiting for SmartConfig.");
  Flag_start_connect = true;
//  while (!WiFi.smartConfigDone() || WiFi.status() != WL_CONNECTED) {
//    if(millis() - timeConnectWifi > 1000)
//    {
//      timeConnectWifi = millis();
//      timeInvert--;
//      displayCountDown();
//    }
//  }
  //Serial.println("");
  //Serial.println("SmartConfig done.");
  //wifiDisplaySuc();
  /* Wait for WiFi to connect to AP */
  //Serial.println("Waiting for WiFi");
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
    //Serial.print(".");
//  }
  //Serial.println("WiFi Connected.");
  //Serial.print("IP Address: ");
  //Serial.println(WiFi.localIP());
}

void Wifi_APconfig(void)
{
  Serial.println("ap config");
}
