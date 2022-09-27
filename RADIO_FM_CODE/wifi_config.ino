#include <WiFi.h>

IPAddress wlan_ip;
IPAddress wlan_gateway;
IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);
void Wifi_Smartconfig(void)
{
    WiFi.disconnect(true);
    delay(200);
    WiFi.mode(WIFI_AP_STA);
    WiFi.beginSmartConfig();
    Flag_start_connect = true;
    delay(200);
    Wifi_GetInfomation();
    Serial.println("Waiting for SmartConfig.");
//    while (!WiFi.smartConfigDone()) {
//      delay(500);
//      Serial.print(".");
//    }
//    Serial.println("");
//    Serial.println("SmartConfig done.");
//    
//    Serial.println("Waiting for WiFi");
//    while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print(".");
//    }
//    Serial.println("WiFi Connected.");
}
void Wifi_GetInfomation(void)
{
    wifi_ip.wifi_status = WiFi.status();
    Serial.println(wifi_ip.wifi_status);
    if( wifi_ip.wifi_status == WL_CONNECTED)
    {
      wifi_ip.ip = WiFi.localIP().toString();
      wifi_ip.bssid = WiFi.BSSIDstr();
      wifi_ip.ssid = WiFi.SSID();
      wifi_ip.password = WiFi.psk();
      wifi_ip.rssi = String(WiFi.RSSI());
//      #if(DEBUG_SYSTEM==1)
//          Serial.printf("IP: ");
//          Serial.println(wifi_ip.ip);
//          Serial.printf("SSID: ");
//          Serial.println(wifi_ip.ssid);
//          Serial.printf("PASSWORD: ");
//          Serial.println(wifi_ip.password);
//          Serial.printf("RSSI: ");
//          Serial.println(wifi_ip.rssi);
//      #endif
    }
}
void Wifi_Reset() {
  //Wifi.disconnect(true);  // wifi disconnect and erase
  //test
//  WiFi.begin();
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(500);
//    Serial.print('.');
//  }
//  Serial.println();
//  Serial.println("Connected: FAIL because SSID/password should have been erased.");
}

void Wifi_Connect(void)
{
  wifi_ip.ssid = "Thanh binh T2 2.4G";
  wifi_ip.password = "03trungkinh";
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ip.ssid.c_str(), wifi_ip.password.c_str());
  Serial.println("Connecting Wifi...");
}

void Wifi_Reconnect(void)
{
  currentMillis = millis();
  
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
    Serial.println("wifi reconnect");
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}
void Wifi_APconfig(void)
{
  Serial.println("ap config");
}
