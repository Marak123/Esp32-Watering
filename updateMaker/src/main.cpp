#include <Arduino.h>

#include <WiFi.h>
#include <SPIFFS.h>
#include <updateMake.h>
#include <loadFile.h>

#include <SD.h>
#include <base64/base64.h>

void initWiFi()
{
    if(WiFi.status() == WL_CONNECTED) return;
    struct s_wifi dataWifi = lf.loadWifi();

    Serial.println("SSID: " + dataWifi.ssid + "\nPassword: " + dataWifi.password);

    WiFi.begin((const char*)dataWifi.ssid.c_str(), (const char*)dataWifi.password.c_str());
    if(dataWifi.local_ip != IPAddress(0,0,0,0) && dataWifi.gateway != IPAddress(0,0,0,0) && dataWifi.subnet != IPAddress(0,0,0,0)) WiFi.config(dataWifi.local_ip, dataWifi.gateway, dataWifi.subnet);
    WiFi.hostname((const char*)dataWifi.hostname.c_str());

    int timeout_counter = 0;
    Serial.print("Connecting to WiFi..");

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
        timeout_counter++;
        if(timeout_counter >= 5) ESP.restart();
    }

    Serial.print("Connected to the WiFi network: {\n   With IP: " + WiFi.localIP().toString() + "\n   Gateway: " + WiFi.gatewayIP().toString() + "\n   Subnet: " + WiFi.subnetMask().toString() + "\n   Hostname: " + String(WiFi.getHostname()) + "\n}\n");
}

bool initSPIFFS(){
  size_t ct = 0;
    while(!SPIFFS.begin()){
        if(ct == 15){
          Serial.println("An Error has occurred while mounting SPIFFS");
          return false;
        }
    }
    Serial.println("SPIFFS mounted");
    return true;
}

void setup() {
  Serial.begin(115200);

  initSPIFFS();
  // Init sd card
  if(!SD.begin(5)){
    Serial.println("An Error has occurred while mounting SD card");
    return;
  }
  initWiFi();

  Serial.println("COS:" + String(LOAD_FILE_ERROR));
  //if(LOAD_FILE_ERROR) ESP.restart();
  Serial.println("Update config OK");

  // um.spiffsUpdate();
  um.softwareUpdate();

  ESP.restart();
}

void loop() {
  // put your main code here, to run repeatedly:
}