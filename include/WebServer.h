#ifndef WEBSOCKET
#define WEBSOCKET
#pragma once

#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "struct.h"
#include "variables.h"
#include "ownTime.h"
#include "function.h"
#include "ownUpdate.h"
#include "configFile.h"

class WebServer
{
private:
  AsyncWebServer server;
  AsyncWebSocket ws;

public:
  WebServer() : server(80), ws("/ws") {};
  ~WebServer() {};

  void notifyClients(StaticJsonDocument<JSON_OBJECT_SIZE(20)> json)
  {
    char data[500];
    size_t len = serializeJson(json, data);
    ws.textAll(data, len);
  }

  void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
  {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
      const uint8_t size = JSON_OBJECT_SIZE(10);
      StaticJsonDocument<size> json;
      DeserializationError err = deserializeJson(json, data);
      if (err){ Serial.printf("Wystapił blad z funkcja \"deserializeJson()\": %s\n", err.c_str()); return; }
    }
  }

  void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
  {
    switch (type)
    {
    case WS_EVT_CONNECT:
      Serial.printf("Klient o ip \"%s\" polaczyl sie z WebSocket i nadano mu id #%u\n", client->remoteIP().toString().c_str(), client->id());
      rpLog.log("Klient o ip " + client->remoteIP().toString() + " polaczyl sie z WebSocket i nadano mu id #" + String(client->id()));
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("Klient WebSocket o id #%u zakonczyl polaczenie.\n", client->id());
      rpLog.log("Klient WebSocket o id #" + String(client->id()) + " zakonczyl polaczenie.");
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
      Serial.printf("PING: Uzytkownik o ip \"%s\" pingoje WebSocket\n", client->remoteIP().toString().c_str());
      rpLog.log("PING: Uzytkownik o ip " + client->remoteIP().toString() + " pingoje WebSocket");
      break;
    case WS_EVT_ERROR:
      break;
    }
  }

  //Inicjacja WebSocket
  void initWebSocket() { using namespace std::placeholders; ws.onEvent(std::bind(&WebServer::onEvent, this, _1, _2, _3, _4, _5, _6)); server.addHandler(&ws); }

  //Zmienne na stronie
  String processor(const String &var, String send_url="/", String messageError="Błędne dane logowania") {
    if (var == "ARRAYS_WITH_DATA") return arrCreator::allArray();
    else if(var == "ERROR_MESSAGE") return messageError;
    else if(var == "SEND_URL") return send_url;
    else if(var == "ARRAY_VERSION_SOFTWARE") return arrCreator::versionArray();
    return "%";
  }

  bool is_authentified(AsyncWebServerRequest *request, bool updateAuth=false)
  {
    if (request->hasHeader("Cookie"))
    {
      String cookie = request->header("Cookie");
      for(s_AccountSchema acc : Accounts)
        if(!updateAuth){
          for(s_session ss : acc.session){
            String cook = "ESPSESSIONID=" + String(ss.token);
            if (cookie.indexOf(cook) != -1) if (ownTime::compareDate(ss.expDate))  return true;
          }
        }else{
          for(s_session ss : acc.adminSession){
            String cook = "ESPSESSIONUPDATEID=" + String(ss.token);
            if (cookie.indexOf(cook) != -1) if (ownTime::compareDate(ss.expDate)) return true;
          }
        }
    }
    return false;
  }

  //Inicjacja Serwera
  void initServer()
  {
    //Glowna Strona
    server.on("/", [this](AsyncWebServerRequest *request) {
      if (request->hasArg("logout"))
      {
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/login/index.html", String(), false, [this](const String &var) -> String { return this->processor(var, "/", ""); });
        response->addHeader("Set-Cookie", "ESPSESSIONID=0");
        response->addHeader("Location", "/");
        request->send(response);

        String cookies = request->header("Cookie");
        std::size_t pos_esp = cookies.indexOf("ESPSESSIONID=");
        cookies = cookies.substring(pos_esp);
        pos_esp = cookies.indexOf(";");
        cookies = cookies.substring(0, pos_esp);
        String espToken = cookies.substring(13);

        for(int i=0; i < Accounts.size(); i++)
          for(int j=0; j < Accounts[i].session.size(); j++)
            if(Accounts[i].session[j].token == espToken){
              Accounts[i].session.erase(Accounts[i].session.begin() + j);
              rpLog.user_info(Accounts[i].username, "Wylogowano uzytkownika", request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
            }
        return;
      }

      if(is_authentified(request)){
        request->send(SPIFFS, "/main/index.html", String(), false, [this](const String &var) -> String { return this->processor(var); });
        return;
      }

      if (request->hasArg("username") && request->hasArg("password"))
        for (int i = 0; i < Accounts.size(); i++)
          if (request->arg("username") == String(Accounts[i].username) && request->arg("password") == String(Accounts[i].password))
          {
            AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/main/index.html", String(), false, [this](const String &var) -> String { return this->processor(var); });
            response->addHeader("Cache-Control", "no-cache");

            struct s_session tkNew = generateWebToken();
            Accounts[i].session.push_back(tkNew);
            response->addHeader("Set-Cookie", "ESPSESSIONID=" + tkNew.token);
            request->send(response);
            configFile.save_accounts();
            return;
          }
      request->send(SPIFFS, "/login/index.html", String(), false, [this](const String &var) -> String { return this->processor(var, "/", ""); });
    });

    //LOGIN
    server.on("/script/scriptLogin.min.js", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(SPIFFS, "/login/script.min.js", "text/js"); });
    server.on("/style/styleLogin.min.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/login/style.min.css", "text/css"); });

    //NOTFOUND
    server.on("/script/scriptNotfound.min.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/notFound/script.min.js", "text/js"); });
    server.on("/style/styleNotFound.min.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/notFound/style.min.css", "text/css"); });

    //MAIN
    server.on("/scripts/all.min.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/main/all.min.js", "text/js"); });
    server.on("/styles/style.min.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/main/style.min.css", "text/css"); });

    //UPDATE
    server.on("/scripts/scriptUpdate.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/update/script.js", "text/js"); });
    server.on("/styles/styleUpdate.min.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/update/style.css", "text/css"); });

    //Image file
    server.on("/images/logo.png", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/images/logo.png", "image/png"); });
    server.on("/images/clock-bg-sm.png", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/images/clock-bg-sm.png", "image/png"); });

    server.on(
        "/power", HTTP_POST, [this](AsyncWebServerRequest *request)
        {
          if(!authenticationAccount(request, {"token", "what", "pinID", "action"}, {"power-now"})){
            request->send(401, "plain/text", "FAILED");
            return;
          }

          String pin = request->arg("pinID");
          bool act = false;
          if (request->arg("action") == "true") act = true;
          digitalWrite(pin.toInt(), act);
          pins::changePower(pin.toInt(), act);

          StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
          jsonRet["pinID"] = request->arg("pinID").toInt();
          jsonRet["action"] = request->arg("action") == "true" ? true : false;
          jsonRet["what"] = request->arg("what");
          notifyClients(jsonRet);

        String user = "Nieznay";
        for(int i=0; i < Accounts.size(); i++)
          for(int j=0; j < Accounts[i].session.size(); j++)
            if(Accounts[i].session[j].token == request->arg("token"))
              user = Accounts[i].username;

          rpLog.user_info(user, "Zmiana stanu pinu \"" + request->arg("pinID") + "\" akcja \"" + request->arg("action") + "\"", request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
          request->send(200, "plain/text", "SUCCESS");
        });

        server.on(
        "/delay", HTTP_POST, [this](AsyncWebServerRequest *request)
        {
          if(!authenticationAccount(request, {"token", "what", "pinID", "action", "delay"}, {"delay"})){
            request->send(401, "plain/text", "FAILED");
            return;
          }

          bool act = false;
          if(request->arg("action") == "true") act = true;
          std::vector<int> arrayPin = convArray(request->arg("pinID"));

          int idPin = -1;
          for(int k=0; k < arrayPin.size(); k++){
            idPin = -1;
            for(int l=0; l < varPins.pins.size(); l++) if(varPins.pins[l].nrPin == arrayPin[k]) idPin = varPins.pins[l].idPin;
            if(idPin == -1) continue;

            varPins.actionList.push_back({
              ownTime::addHours(request->arg("delay")),
              act,
              idPin,
              arrayPin[k]
            });
          }
          StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
          jsonRet["what"] = request->arg("what");
          jsonRet["arrayListActivity"] = arrCreator::listActionElm(true);

          String user = "Nieznay";
          for(int i=0; i < Accounts.size(); i++)
            for(int j=0; j < Accounts[i].session.size(); j++)
              if(Accounts[i].session[j].token == request->arg("token"))
                user = Accounts[i].username;

          configFile.save_action();
          notifyClients(jsonRet);
          rpLog.user_info(user, "Zaplanowanie akcji \"" + request->arg("action") + "\" na pinie \"" + request->arg("pinID") + "\" z opoznieniem " + request->arg("delay"), request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
          request->send(200, "plain/text", "SUCCESS");
        });

        server.on(
        "/plan", HTTP_POST, [this](AsyncWebServerRequest *request)
        {

          if(!authenticationAccount(request, {"token", "what", "pinID", "action", "hours", "dayWeek", "date"}, {"planDate", "planWeek"})){
            request->send(401, "plain/text", "FAILED");
            return;
          }

          s_date dateCv = {0, 0, 0, 0, 0};
          std::vector<int> arrayWeek;
          if(request->arg("what") == "planDate") dateCv = ownTime::convertDate(request->arg("date"));
          if(request->arg("what") == "planWeek") arrayWeek = convArray(request->arg("dayWeek"));
          s_hour hourCv = ownTime::convertHours(request->arg("hours"));
          std::vector<int> arrayPin = convArray(request->arg("pinID"));
          bool act = false;
          if(request->arg("action") == "true") act = true;

          int idPin = -1;
          for (int k = 0; k < arrayPin.size(); k++)
          {
            idPin = -1;
            for (int l = 0; l < varPins.pins.size(); l++)
              if (varPins.pins[l].nrPin == arrayPin[k]) idPin = varPins.pins[l].idPin;
            if (idPin == -1) continue;

            if (request->arg("what") == "planDate"){
              struct s_actionList ac = {  {dateCv.day, dateCv.month, dateCv.year, hourCv.hour, hourCv.minute},
                                        act,
                                        idPin,
                                        arrayPin[k] };
              if(!actionExist(ac)) varPins.actionList.push_back(ac);
              else request->send(302, "plain/text", "FAILED - action exist");
            }
            if (request->arg("what") == "planWeek"){
              struct s_actionWeek ac = {  arrayWeek,
                                          hourCv,
                                          act,
                                          idPin,
                                          arrayPin[k],
                                          0,
                                          {-1, -1} };
              if(!weekActionExist(ac)) varPins.actionWeek.push_back(ac);
              else request->send(302, "plain/text", "FAILED - action exist");
            }
          }
          StaticJsonDocument<JSON_OBJECT_SIZE(100)> jsonRet;
          jsonRet["what"] = request->arg("what");
          jsonRet["arrayListActivity"] = arrCreator::listActionElm(true);

          String user = "Nieznany";
          for(int i=0; i < Accounts.size(); i++)
            for(int j=0; j < Accounts[i].session.size(); j++)
              if(Accounts[i].session[j].token == request->arg("token"))
                user = Accounts[i].username;

          configFile.save_action();
          notifyClients(jsonRet);
          rpLog.user_info(user, "Zaplanowanie akcji \"" + request->arg("action") + "\" na pinie \"" + request->arg("pinID") + "\" na dni " + request->arg("dayWeek") + " o godzinie " + request->arg("hours"), request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
          request->send(200, "plain/text", "SUCCESS");
        });

        server.on(
            "/setTemperature", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "pinID", "temperature"}, {"setTemperature"})){
                request->send(401, "plain/text", "FAILED");
                return;
              }

              String user = "Nieznany";
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token"))
                    user = Accounts[i].username;

              int idPin = -1;
              for(int l=0; l < varPins.pins.size(); l++)
                if(varPins.pins[l].nrPin == request->arg("pinID").toInt())
                {
                  idPin = varPins.pins[l].idPin;
                  break;
                }
              for(int l=0; l < varPins.tempData.size(); l++)
                if (varPins.tempData[l].idPinTemp == idPin)
                {
                  varPins.tempData[l].primTemp = request->arg("temperature").toInt();
                  break;
                }
              StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
              jsonRet["what"] = request->arg("what");
              jsonRet["tempElem"] = arrCreator::temperatureEle(true);

              notifyClients(jsonRet);
              rpLog.user_info(user, "Ustawienie utrzymywanej temperatury na pinie \"" + request->arg("pinID") + "\" na " + request->arg("temperature"), request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
              request->send(200, "plain/text", "SUCCESS");
            });

        server.on(
            "/timetableRemove", HTTP_DELETE, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "indexElm", "action", "pinID", "data", "hours"}, {"timetable"})){
                request->send(401, "plain/text", "FAILED");
                return;
              }

              String user = "Nieznany";
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token"))
                    user = Accounts[i].username;

              int point = 0;
              for(int i=0; i < varPins.actionList.size(); i++){
                point = 0;
                if(request->arg("pinID").toInt() == varPins.actionList[i].nrPin){
                  s_date data = ownTime::convertDate(request->arg("data"));
                  s_hour hour = ownTime::convertHours(request->arg("hours"));
                  data.hours = hour.hour;
                  data.minute = hour.minute;

                  if(ownTime::compareDate(data, varPins.actionList[i].time)) point++;
                  if(varPins.actionList[i].action == request->arg("action").toInt()) point++;
                }
                if(point == 2){
                  varPins.actionList.erase(varPins.actionList.begin() + i);
                  break;
                }
              }

              StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
              jsonRet["what"] = request->arg("what");
              jsonRet["actionEle"] = arrCreator::listActionElm(true);

              notifyClients(jsonRet);
              rpLog.user_info(user, "Usunięcie akcji \"" + request->arg("action") + "\" na pinie \"" + request->arg("pinID") + "\" na dni " + request->arg("data") + " o godzinie " + request->arg("hours"), request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
              request->send(200, "plain/text", "SUCCESS");
            });

        server.on(
            "/weekActionRemove", HTTP_DELETE, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "indexElm", "action", "pinID", "dayNumber", "hours"}, {"weekactionremove"})){
                request->send(401, "plain/text", "FAILED");
                return;
              }

              String user = "Nieznany";
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token"))
                    user = Accounts[i].username;

              int point = 0;
              for(int i=0; i < varPins.actionWeek.size(); i++){
                point = 0;
                s_hour hour = ownTime::convertHours(request->arg("hours"));
                std::vector<int> arr = convArray(request->arg("dayNumber"));
                if(varPins.actionWeek[i].arrayWeek.size() != arr.size()) continue;
                if(varPins.actionWeek[i].action == request->arg("action").toInt() && varPins.actionWeek[i].nrPin == request->arg("pinID").toInt() && varPins.actionWeek[i].time.hour == hour.hour && varPins.actionWeek[i].time.minute == hour.minute)
                  for(int h=0; h < arr.size(); h++)
                    if(varPins.actionWeek[i].arrayWeek[h] == arr[h]) point++;
                if(point == arr.size())
                  varPins.actionWeek.erase(varPins.actionWeek.begin() + i);
              }

              StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
              jsonRet["what"] = request->arg("what");
              jsonRet["actionEle"] = arrCreator::weekActionList(true);

              notifyClients(jsonRet);
              rpLog.user_info(user, "Usunięcie akcji \"" + request->arg("action") + "\" na pinie \"" + request->arg("pinID") + "\" na dni " + request->arg("data") + " o godzinie " + request->arg("hours"), request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
              request->send(200, "plain/text", "SUCCESS");
            });

        server.on(
            "/changeUsername", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "newUsername", "password"}, {"changeUsername"})){
                request->send(401, "plain/text", "FAILED");
                return;
              }

              String user = "Nieznany";
              int error = 0;
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token")){
                    user = Accounts[i].username;
                    if(Accounts[i].password == request->arg("password"))
                      Accounts[i].username = request->arg("newUsername");
                    else error++;
                  }

              if(error != 0){
                request->send(401, "plain/text", "FAILED");
                return;
              }
              configFile.save_accounts();
              rpLog.user_info(user, "Zmiana nazwy użytkownika na \"" + request->arg("newUsername") + "\"", request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
              request->send(200, "plain/text", "SUCCESS");
            });

        server.on(
            "/changePassword", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "newPassword", "oldPassword"}, {"changePassword"})){
                request->send(401, "plain/text", "FAILED");
                return;
              }

              String user = "Nieznany";
              int error = 0;
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token")){
                    user = Accounts[i].username;
                    if(Accounts[i].password == request->arg("oldPassword"))
                      Accounts[i].password = request->arg("newPassword");
                    else error++;
                  }

              if(error != 0){
                request->send(401, "plain/text", "FAILED");
                return;
              }

              configFile.save_accounts();
              rpLog.user_info(user, "Zmiana hasła", request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
              request->send(200, "plain/text", "SUCCESS");
            });

        server.on(
            "/validitySession", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "exp_day"}, {"validitySession"})){
                request->send(401, "plain/text", "Blad pobierania danych, Brak dostepu.");
                return;
              }

              String user = "Nieznany";
              int error = 0;
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token")){
                    user = Accounts[i].username;
                    Accounts[i].dayExp_mainSession = request->arg("exp_day").toInt();
                  }else error++;

              if(error != 0){
                request->send(405, "plain/text", "Blad pobierania danych, Sesja wygasla.");
                return;
              }
              rpLog.user_info(user, "Zmiana dni wygasniecia sesji", request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
              request->send(200, "plain/text", "SUCCESS");
            });

        server.on(
            "/getMobileToken", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what"}, {"getMobileToken"})){
                request->send(401, "application/json", "{\"act\": \"FAILED\", \"result\": \"Blad pobierania danych, Brak dostepu.\"}");
                Serial.println("getMobileToken FAILED");
                return;
              }

              String user = "Nieznany";
              int error = 0;
              String mbtoken = "";
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token")){
                    user = Accounts[i].username;
                    if(ownTime::compareDate(Accounts[i].mobileToken.expDate))
                      mbtoken = Accounts[i].mobileToken.token;
                    else error++;
                  }
              if(error != 0){
                request->send(405, "application/json", "{\"act\": \"FAILED\", \"result\": \"Blad pobierania danych, Sesja wygasla.\"}");
                return;
              }

              rpLog.user_info(user, "Pobranie tokenu mobilnego", request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
              String result = "{\"act\": \"SUCCESS\", \"result\": \"" + mbtoken + "\"}";
              request->send(200, "plain/text", result);
            });

        server.on(
            "/generateNewMobileToken", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what"}, {"generateNewMobileToken"})){
                request->send(401, "application/json", "{\"act\": \"FAILED\", \"result\": \"Blad pobierania danych, Brak dostepu.\"}");
                return;
              }

              String user = "Nieznany";
              int error = 0;
              String mbtoken = "";
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token")){
                    user = Accounts[i].username;
                    if(ownTime::compareDate(Accounts[i].mobileToken.expDate)){
                      Accounts[i].mobileToken = generateWebToken(Accounts[i].dayExp_mobileSession);
                      mbtoken = Accounts[i].mobileToken.token;
                    }else error++;
                  }

              if(error != 0){
                request->send(405, "{\"act\": \"FAILED\", \"result\": \"Blad pobierania danych, Sesja wygasla.\"}");
                return;
              }

              configFile.save_accounts();
              String result = "{\"act\": \"SUCCESS\", \"result\": \"" + mbtoken + "\"}";
              rpLog.user_info(user, "Generowanie nowego tokena mobilnego", request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
              request->send(200, "plain/text", result);
            });

        server.on(
            "/mobileValiditySession", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "exp_day"}, {"mobileValiditySession"})){
                request->send(401, "plain/text", "Blad pobierania danych, Brak dostepu.");
                return;
              }

              String user = "Nieznany";
              int error = 0;
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token")){
                    user = Accounts[i].username;
                    Accounts[i].dayExp_mobileSession = request->arg("exp_day").toInt();
                  }
                  else error++;

              if(error != 0){
                request->send(405, "plain/text", "Blad pobierania danych, Sesja wygasla.");
                return;
              }
              rpLog.user_info(user, "Zmiana dni wygasniecia sesji mobilnej", request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
              request->send(200, "plain/text", "SUCCESS");
            });

        server.on(
            "/restart", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "password"}, {"restart"})){
                request->send(401, "plain/text", "Blad pobierania danych, Brak dostepu.");
                return;
              }

              String user = "Nieznay";
              int error = 0;
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token")){
                    if(Accounts[i].password == request->arg("password")){
                      configFile.save_complete_config();
                    }
                  }
                  else error++;

              if(error != 0){
                request->send(405, "plain/text", "Blad pobierania danych, Sesja wygasla.");
                return;
              }
              rpLog.user_info(user, "Restart Urządzenia", request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
              request->send(200, "plain/text", "SUCCESS");
              ESP.restart();
            });
        server.on(
            "/factoryreset", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "password"}, {"factoryreset"})){
                request->send(401, "plain/text", "Blad pobierania danych, Brak dostepu.");
                return;
              }

              String user = "Nieznay";
              int error = 0;
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token")){
                    if(Accounts[i].password == request->arg("password")){
                      configFile.factoryResetConfig();
                    }
                  }
                  else error++;

              if(error != 0){
                request->send(405, "plain/text", "Blad pobierania danych, Sesja wygasla.");
                return;
              }
              rpLog.user_info(user, "Przywrocenie konfiguracji fabrycznej i reset urządzenia", request->client()->remoteIP().toString(), getFromRequest(request, "User-Agent"), "", "", request->url());
              request->send(200, "plain/text", "SUCCESS");
              ESP.restart();
            });

        // server.on(
        //     "/updatesoftware/check", HTTP_POST, [this](AsyncWebServerRequest *request)
        //     {
        //       if(!authenticationAccount(request, {"token", "what"}, {"checkUpdate"})){
        //         request->send(401, "Blad pobierania danych, Brak dostepu.");
        //         return;
        //       }

        //       request->send(200, "plain/text", updateSoft.checkUpdate());
        //     });

        // server.on(
        //     "/update", HTTP_GET, [this](AsyncWebServerRequest *request)
        //     {

        //       if(is_authentified(request, true)){
        //         request->send(SPIFFS, "/update/index.html", "text/html", false, [this](const String &var) -> String { return this->processor(var, "/update", ""); });
        //         return;
        //       }

        //       request->send(SPIFFS, "/login/index.html", "text/html", false, [this](const String &var) -> String { return this->processor(var, "/update", ""); });
        //     });

        // server.on(
        //     "/update", HTTP_POST, [this](AsyncWebServerRequest *request)
        //     {
        //       String msg = "";
        //       if (request->hasParam("username", true) && request->hasParam("password", true)){
        //         for (int i = 0; i < Accounts.size(); i++)
        //           if (request->arg("username") == String(Accounts[i].username) && request->arg("password") == String(Accounts[i].password))
        //           {
        //             if(!Accounts[i].isAdmin) break;
        //             AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/update/index.html", String(), false, [this](const String &var) -> String { return this->processor(var); });
        //             response->addHeader("Cache-Control", "no-cache");

        //             struct s_session tkNew = generateWebToken(0, 0, 5);
        //             Accounts[i].adminSession.push_back(tkNew);
        //             response->addHeader("Set-Cookie", "ESPSESSIONUPDATEID=" + tkNew.token);
        //             request->send(response);
        //             return;
        //           }
        //         msg = "Błędne dane logowania.";
        //       }

        //       request->send(SPIFFS, "/login/index.html", "text/html", false, [this, msg](const String &var) -> String { return this->processor(var, "/update", msg); });
        //     });

        // server.on(
        //     "/update/make_update", HTTP_POST, [this](AsyncWebServerRequest *request)
        //     {
        //       if(!authenticationAccount(request, {"token", "what", "version"}, {"updateSoftware_adm"})){
        //         request->send(401, "Wystąpił błąd, Brak dostepu.");
        //         return;
        //       }

        //       request->send(SPIFFS, "/login/index.html", "text/html", false, [this](const String &var) -> String { return this->processor(var, "/update", ""); });
        //     });

        server.onNotFound([](AsyncWebServerRequest *request)
                          { request->send(SPIFFS, "/notFound/index.html", "text/html"); });

        server.begin();
  }

  void runServer(){
    this->initWebSocket();
    this->initServer();
  }
} web_serv;

#endif