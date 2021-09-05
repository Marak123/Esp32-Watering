#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <vector>
#include <ESP32_FTPClient.h>
//#include <mongocxx/client.hpp>
//#include <SFML/Network.hpp>

#include <ownTime.h>
#include <struct.h>
// #include <configFile.h>
#include <function.h>
#include <WebServer.h>

//Dolaczanie zmiennych
#include <variables.h>

//Inicjacja Klasy Połączenia FTP
FTP ftp(dataFtp.addrIP, dataFtp.username, dataFtp.password);

//Wywołanie klasy WebSocket
WebServer webserver(&varPins, &ftp);

// confFile config("/configFile.json");

void setup()
{
  Serial.begin(115200);
  Serial.println("\n\n");


  //Inicjacja odczytu danych z pliku
  initSPIFFS();

  //Ładowanie danych konfiguracyjnych z pliku json
  // config.loadConfig();

  //Inicjacja pinow
  initialPins(varPins.pins, varPins.powerPins);

  Serial.print("SSID: ");
  Serial.println(dataWifi.SSID);
  Serial.print("PASS: ");
  Serial.println(dataWifi.password);

  //Inicjacja WiFi
  initWiFi(dataWifi.SSID, dataWifi.password);

  Serial.println();
  Serial.println("----------------Zaczynam wypisywanie Czas ---------------");
  Serial.print(String(TimeS.Day()));
  Serial.print(" - ");
  Serial.print(String(TimeS.Month()));
  Serial.print(" - ");
  Serial.print(String(TimeS.Year()));
  Serial.print(" - ");
  Serial.print(String(TimeS.Hour()));
  Serial.print(" - ");
  Serial.println(String(TimeS.Minute()));  
  Serial.print(" - ");
  Serial.println(String(TimeS.weekDay()));
  Serial.println("----------------Koncze wypisywanie Czas ---------------");
  Serial.println();

  //Łączenie z serwerem ftp
  //ftp.Connect(true);

  //Inicjacja WebSocket
  webserver.initWebSocket();

  //Inicjacja Serwera
  webserver.ServerInit();

  //Tworzenie konta domyślnego użytkownika strony
  Accounts.push_back(createAccount("admin", "admin"));
}

void prt(int i)
{
  Serial.println();
  Serial.print("+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+==+=+=");
  Serial.print(String(varPins.actionList[i].nrPin));
  Serial.println("=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=");

  bool ff = varPins.actionList[i].time.hours <= TimeS.Hour() && varPins.actionList[i].time.minute <= TimeS.Minute() && varPins.actionList[i].time.day <= TimeS.Day() && varPins.actionList[i].time.month <= TimeS.Month() && varPins.actionList[i].time.year <= TimeS.Year();
  Serial.print("Warunek: ");
  Serial.println(String(ff));

  Serial.print("Hour: ");
  Serial.print(varPins.actionList[i].time.hours);
  Serial.print(" - ");
  Serial.println(TimeS.Hour());

  Serial.print("Minute: ");
  Serial.print(varPins.actionList[i].time.minute);
  Serial.print(" - ");
  Serial.println(TimeS.Minute());

  Serial.print("Day: ");
  Serial.print(varPins.actionList[i].time.day);
  Serial.print(" - ");
  Serial.println(TimeS.Day());

  Serial.print("Month: ");
  Serial.print(varPins.actionList[i].time.month);
  Serial.print(" - ");
  Serial.println(TimeS.Month());

  Serial.print("Year: ");
  Serial.print(varPins.actionList[i].time.year);
  Serial.print(" - ");
  Serial.println(TimeS.Year());

  Serial.println("----------------------------------------------------------------------------------------------");
  Serial.println();
}

bool change = false;
bool Cnt = false;
unsigned long Start = 0;
void loop()
{
  if (!Cnt){
    Start = millis();
    Cnt = true;
  }

  if ((millis() - Start) >= 30000)
  {
    Start = 0;
    Cnt = false;
    for (int i = 0; i < varPins.actionList.size(); i++)
    {
      // prt(i);
      if (varPins.actionList[i].time.hours <= TimeS.Hour() && varPins.actionList[i].time.minute <= TimeS.Minute() && varPins.actionList[i].time.day <= TimeS.Day() && varPins.actionList[i].time.month <= TimeS.Month() && varPins.actionList[i].time.year <= TimeS.Year())
      {
        for (int j = 0; j < varPins.pins.size(); j++){
          if (varPins.pins[j].idPin == varPins.actionList[i].idPin){
            if (varPins.pins[j].nrPin == varPins.actionList[i].nrPin){
              digitalWrite(varPins.actionList[i].nrPin, varPins.actionList[i].action);
            }
          }
        }
        varPins.actionList.erase(varPins.actionList.begin() + i);
        change = true;
      }
    }
    for(int i = 0; i < varPins.actionWeek.size(); i++){
      for (int j=0; j < varPins.actionWeek[i].arrayWeek.size(); j++){
        if(varPins.actionWeek[i].weekCount != 0 && TimeS.nrWeekDay() == 7){
            s_date actDate = ownTime::addDates({varPins.actionWeek[i].arrayWeek[j], 0, 0, 0, 0});

            varPins.actionList.push_back({
              actDate,
              varPins.actionWeek[i].action,
              varPins.actionWeek[i].idPin,
              varPins.actionWeek[i].nrPin
            });
            varPins.actionWeek[i].weekCount++;
            change = true;
        }else if(varPins.actionWeek[i].weekCount == 0){
          if (varPins.actionWeek[i].arrayWeek[j] <= TimeS.nrWeekDay())
            continue;
          int dodDay = TimeS.nrWeekDay() + varPins.actionWeek[i].arrayWeek[j];
          s_date actDate = ownTime::addDates({dodDay, 0, 0, 0, 0});

          varPins.actionList.push_back({
            actDate,
            varPins.actionWeek[i].action,
            varPins.actionWeek[i].idPin,
            varPins.actionWeek[i].nrPin
          });
          varPins.actionWeek[i].weekCount++;
          change = true;
        }
      }
    }

    if(change){
      StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
      jsonRet["what"] = "action-done";
      jsonRet["arrayListActivity"] = arrCreator::listActionElm(true);
      webserver.notifyClients(jsonRet);
      change = false;
    }
  }
}