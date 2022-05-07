#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <vector>


#include <WebServer.h>
#include <ownTime.h>
#include <configFile.h>
#include <mainFun.h>
#include <dataConn.h>
#include <variables.h>


void setup()
{
  Serial.begin(115200);
  Serial.println("\n\n");

  // Inicjacja SD_CARD
  initSD();

  // Inicjacja spiffs
  initSPIFFS();

  // Load config file
  configFile.load_configuration();
  Serial.println("Config file loaded");

  //Inicjacja polaczenia z siecia WiFi
  initWiFi();

  Serial.println(configFile.spiffsSize());
  rpLog.log(configFile.spiffsSize());

  //Inicjacja pinow
  initialPins(varPins.pins, varPins.powerPins);
  rpLog.log("Piny zostaly zainicjowane");

  //Inicjacja sensora temperatury
  initTempSensor();
  rpLog.log("Sensory temperatury zostaly zainicjowane");

  // Wlaczanie serwera WWW
  web_serv.runServer();
  rpLog.log("Serwer WWW zostal uruchomiony");

  // updateSoft.checkUpdate();
}

bool CntMain = false;
bool CntTemp = false;
unsigned long StartMain = 0;
unsigned long StartTemp = 0;
void loop()
{
  if (!CntMain)
  {
    StartMain = millis();
    CntMain = true;
  }
  if (!CntTemp)
  {
    StartTemp = millis();
    CntTemp = true;
  }

  if ((millis() - StartMain) >= 1000)
  {
    StartMain = 0;
    CntMain = false;

    StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet = inLoopActionPerform();
    if (jsonRet["what"] == "action-done")
      web_serv.notifyClients(jsonRet);
  }
  if ((millis() - StartTemp) >= 10000)
  {
    StartTemp = 0;
    CntTemp = false;
    readTemperature();

    StaticJsonDocument<JSON_OBJECT_SIZE(100)> jsonTemp;
    jsonTemp["what"] = "temperatureRead";

    for (int i = 0; i < varPins.tempData.size(); i++)
    {
      StaticJsonDocument<JSON_OBJECT_SIZE(10)> tmPin;
      for (int j = 0; j < varPins.pins.size(); j++)
        if (varPins.pins[j].idPin == varPins.tempData[i].idPinTemp)
        {
          tmPin["pinID"] = varPins.pins[j].nrPin;
          break;
        }
      tmPin["curlTemp"] = varPins.tempData[i].curlTemp;
      jsonTemp["data"].add(tmPin);
    }
    web_serv.notifyClients(jsonTemp);
  }

  // if(TimeS.Hour() == 12) updateSoft.checkUpdate();
}