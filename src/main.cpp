#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <vector>
// #include <ESP32_FTPClient.h>
#include <DHT.h>

#include <ownUpdate.h>
#include <WebServer.h>
#include <ownTime.h>
#include <struct.h>
#include <configFile.h>
#include <function.h>
#include <mainFun.h>

//Dolaczanie zmiennych
#include <variables.h>
// FTP ftp(dataFtp.ip_address, dataFtp.username, dataFtp.password, dataFtp.port);

//Inicjacja Klasy Połączenia FTP
// if(dataFtp.port != 0) {FTP ftp(dataFtp.ip_address, dataFtp.username, dataFtp.password, dataFtp.port);}
// else {FTP ftp(dataFtp.ip_address, dataFtp.username, dataFtp.password);}
FTP ftp(dataFtp.ip_address, dataFtp.username, dataFtp.password, dataFtp.port);
//Wywołanie klasy WebSocket
WebServer webserver(&varPins, &ftp);

// confFile config("/configFile.json");


void setup()
{
  Serial.begin(115200);
  Serial.println("\n\n");

  //Inicjacja odczytu danych z pliku
  initSPIFFS();

  config config;
  config.begin();
  config.loadConfig();
  config.saveConfig();

  //Ładowanie danych konfiguracyjnych z pliku json
  // config.loadConfig();
  // TimeS.getTime();
  initWiFi();

  // int h = TimeS.Hour();
  // int m = TimeS.Minute();
  // Serial.print(h);
  // Serial.print(":");
  // Serial.print(m);

  //Łączenie z serwerem ftp
  //ftp.Connect(true);

  //Inicjacja WebSocket
  webserver.initWebSocket();

  //Inicjacja Serwera
  webserver.ServerInit();


  //Tworzenie konta domyślnego użytkownika strony
  // Accounts.push_back(createAccount("admin", "admin"));




  int usedBytes = SPIFFS.usedBytes();
  int totalBytes = SPIFFS.totalBytes();
  int result = usedBytes * 100 / totalBytes;
  Serial.print("SPIFFS used: ");
  Serial.print(result);
  Serial.println("%");


  //Inicjacja pinow
  initialPins(varPins.pins, varPins.powerPins);

  //Inicjacja sensora temperatury
  initTempSensor();
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
      webserver.notifyClients(jsonRet);
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
    // serializeJson(jsonTemp, Serial);
    webserver.notifyClients(jsonTemp);
  }
}