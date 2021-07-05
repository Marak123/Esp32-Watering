#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <vector>
#include <ESP32_FTPClient.h>
//#include <mongocxx/client.hpp>
//#include <SFML/Network.hpp>

#include "function.h"
#include "variablesSchemat.h"
#include "WebSocket.h"

//Dolaczanie zmiennych
#include "variables.h"

int long lCnt = 0;
unsigned long lStart;

//Zmienne ustawien czasu
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

//Inicjacja Klasy z Czasem
Time t("pool.ntp.org", 3600, 3600);

//Inicjacja Klasy Połączenia FTP
FTP ftp(ftp_serverIP, ftp_username, ftp_password);

//Wywołanie klasy WebSocket
WebSocket websocket(&var, &t, &ftp);

void setup()
{
  Serial.begin(115200);
  Serial.println("\n\n");

  //Inicjacja odczytu danych z pliku
  initSPIFFS();

  //Inicjacja pinow
  initialPins(var.pins);

  //Inicjacja WiFi
  initWiFi(ssid, password);

  //Pobieranie daty i godziny z serwera
  t.getTime();

  //Łączenie z serwerem ftp
  //ftp.Connect(true);

  //Inicjacja WebSocket
  websocket.initWebSocket();

  //Inicjacja Serwera
  websocket.ServerInit();

  //Tworzenie konta domyślnego użytkownika strony
  Accounts.push_back(createAccount(&t, "admin", "admin"));
}
void loop()
{
  if (lCnt == 0)
    lStart = millis();
  lCnt++;
  if ((millis() - lStart) >= 60000)
  {
    lStart = 0;
    lCnt = 0;

    //std::vector<int> whatPlace;
    // for(int i=0; i < var.withDelay.size(); i++){
    //   Serial.print("Dziala Petla: ");
    //   Serial.println(i);
    //   if(var.withDelay[i].hour == timeClient.getHours() + 1)
    //     if (var.withDelay[i].minute == timeClient.getMinutes()){
    //       for(int j=0; j < var.withDelay[i].pins.size(); j++) {
    //         digitalWrite(var.withDelay[i].pins[j], var.withDelay[i].turn);
    //       }
    //       if (i != 0) var.withDelay.erase(var.withDelay.begin() + i - 1);
    //       else var.withDelay.erase(var.withDelay.begin());
    //       continue;
    //     }
    // }
    // Serial.println();
  }
  // websocket.ws.cleanupClients();
}