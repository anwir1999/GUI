#include <WiFi.h>
#include <Audio.h>
#include <Arduino_JSON.h>
#include <string.h>

Audio internet_radio;

#define MAX_CHANNEL  25
#define MIN_CHANNEL  0
const char InternetRadio_Channel[MAX_CHANNEL][100] = {
                                 "http://media.kythuatvov.vn:7001", \
                                 "http://media.kythuatvov.vn:7003", \
                                 "http://media.kythuatvov.vn:7005", \
                                 "http://media.kythuatvov.vn:7037", \
                                 "http://media.kythuatvov.vn:7039", \
                                 "http://media.kythuatvov.vn:7031", \
                                 "http://media.kythuatvov.vn:7033", \
                                 "http://media.kythuatvov.vn:7035", \
                                 "http://media.kythuatvov.vn:7007", \
                                 "http://media.kythuatvov.vn:7007", \
                                 "http://media.kythuatvov.vn:7029", \
                                 "http://media.kythuatvov.vn:7011", \
                                 "http://media.kythuatvov.vn:7011", \
                                 "http://media.kythuatvov.vn:7015", \
                                 "http://media.kythuatvov.vn:7019", \
                                 "http://media.kythuatvov.vn:7029", \
                                 "http://media.kythuatvov.vn:7011", \
                                 "http://media.kythuatvov.vn:7019", \
                                 "http://media.kythuatvov.vn:7025", \
                                 "http://media.kythuatvov.vn:7023", \
                                 "http://media.kythuatvov.vn:7021", \
                                 "http://media.kythuatvov.vn:7029"};
                                 
//String ssid =    "ThanhNguyen1";
//String password = "25251325";

String internet_radio_channel_url = "http://radio.duchungtech.com/json";
String channelJsonBuffer;



void internet_radio_init(void)
{
  internet_radio.setPinout(I2S_BCLK, I2S_LRCLK, I2S_DOUT);
}

void internet_radio_play(const char channel[])
{
  SI4684_End();
  NAU8822_Input(1);
  WM8804_Input(1);
  internet_radio.connecttohost(channel);
  internet_radio.loop();
}
void internet_radio_stop(void)
{
  internet_radio.stopSong();
}
void internet_radio_setVolume(uint8_t volume)
{
  internet_radio.setVolume(volume);
}

void internet_radio_getChannel()
{
  channelJsonBuffer = httpGETRequest(internet_radio_channel_url.c_str());
  //Serial.println(channelJsonBuffer);
  JSONVar channelJson = JSON.parse(channelJsonBuffer);
  if(JSON.typeof_(channelJson) == "undefined")
  {
    //Serial.println("Parsing fail");
    return;
  }
  //Serial.println();
  //channel = channelJson["main"]["temp"];
  //Serial.print("Temperature: ");
  //Serial.println(temp);
 
}

String httpGETRequest(const char* Url)
{
  HTTPClient http;
  http.begin(Url);
  int responseCode = http.GET();
  String responseBody = "{}";
  if(responseCode > 0)
  {
    Serial.print("responseCode:");
    Serial.println(responseCode);
    responseBody = http.getString();
  }
  else
  {
    Serial.print("Error Code: ");
    Serial.println(responseCode);
  }
  http.end();
  return responseBody;
}
