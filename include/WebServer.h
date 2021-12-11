#ifndef WEBSOCKET
#define WEBSOCKET

#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "ownTime.h"
#include "function.h"
#include "struct.h"

class WebServer
{
private:
  AsyncWebServer server;
  AsyncWebSocket ws;

  s_Var *variableAddress;
  FTP *ftp;

public:
  WebServer(struct s_Var *vars, FTP *ft) : server(80), ws("/ws")
  {
    variableAddress = vars;
    ftp = ft;
  };

  void notifyClients(StaticJsonDocument<JSON_OBJECT_SIZE(20)> json)
  {
    char data[500];
    size_t len = serializeJson(json, data);
    // Serial.println("Wysyłam info");
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
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("Klient WebSocket o id #%u zakonczyl polaczenie.\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
      Serial.printf("PING: Uzytkownik o ip \"%s\" pingoje WebSocket\n", client->remoteIP().toString().c_str());
      break;
    case WS_EVT_ERROR:
      break;
    }
  }

  //Inicjacja WebSocket
  void initWebSocket() { using namespace std::placeholders; ws.onEvent(std::bind(&WebServer::onEvent, this, _1, _2, _3, _4, _5, _6)); server.addHandler(&ws); }

  //Zmienne na stronie
  String processor(const String &var) {
    if (var == "ARRAYS_WITH_DATA")
    {
      return arrCreator::allArray();
    }
    return "%";
  }

  bool is_authentified(AsyncWebServerRequest *request)
  {
    if (request->hasHeader("Cookie"))
    {
      String cookie = request->header("Cookie");
      for(int i=0; i < Accounts.size(); i++)
        for(int j=0; j < Accounts[i].session.size(); j++){
          String cook = "ESPSESSIONID=" + String(Accounts[i].session[j].token);
          if (cookie.indexOf(cook) != -1) if (ownTime::compareDate(Accounts[i].session[j].expDate))  return true;
        }
    }
    return false;
  }

  //Inicjacja Serwera
  void ServerInit()
  {
    //Glowna Strona
    server.on("/", [this](AsyncWebServerRequest *request) {
      if (request->hasArg("logout"))
      {
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/login/index.html");
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
            if(Accounts[i].session[j].token == espToken)
              Accounts[i].session.erase(Accounts[i].session.begin() + j);

        return;
      }

      if(is_authentified(request)){
        request->send(SPIFFS, "/index.html", String(), false, [this](const String &var) -> String { return this->processor(var); });
        return;
      }

      if (request->hasArg("username") && request->hasArg("password"))
        for (int i = 0; i < Accounts.size(); i++)
          if (request->arg("username") == String(Accounts[i].username) && request->arg("password") == String(Accounts[i].password))
          {
            AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/index.html", String(), false, [this](const String &var) -> String { return this->processor(var); });
            response->addHeader("Cache-Control", "no-cache");

            struct s_session tkNew = generateWebToken();
            Accounts[i].session.push_back(tkNew);
            response->addHeader("Set-Cookie", "ESPSESSIONID=" + tkNew.token);
            request->send(response);
            return;
          }
      request->send(SPIFFS, "/login/index.html");
    });

        //LOGIN
    server.on("/script/scriptLogin.min.js", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(SPIFFS, "/login/script.min.js", "text/js"); });
    server.on("/style/styleLogin.min.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/login/style.min.css", "text/css"); });

    //NOTFOUND
    server.on("/script/scriptNotfound.min.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/notFound/script.min.js", "text/js"); });
    server.on("/style/styleNotFound.min.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/notFound/style.min.css", "text/css"); });

  //MAIN
    server.on("/scripts/all.min.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/scripts/all.min.js", "text/js"); });
    server.on("/styles/style.min.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/styles/style.min.css", "text/css"); });

    //Image file
    server.on("/images/logo.png", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/images/logo.png", "image/png"); });
    server.on("/images/clock-bg-sm.png", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/images/clock-bg-sm.png", "image/png"); });

    server.on(
        "/power", HTTP_POST, [this](AsyncWebServerRequest *request)
        {
          if(!authenticationAccount(request, {"token", "what", "pinID", "action"}, {"power-now"})){
            request->send(401, "FAILED");
            return;
          }

          String pin = request->arg("pinID");
          bool act = false;
          if (request->arg("action") == "true") act = true;
          digitalWrite(pin.toInt(), act);
          pins::changePower(pin.toInt(), act);

          StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
          jsonRet["pinID"] = request->arg("pinID");
          jsonRet["action"] = request->arg("action");
          jsonRet["what"] = request->arg("what");
          notifyClients(jsonRet);
          request->send(200, "SUCCESS");
        });

        server.on(
        "/delay", HTTP_POST, [this](AsyncWebServerRequest *request)
        {
          if(!authenticationAccount(request, {"token", "what", "pinID", "action", "delay"}, {"delay"})){
            request->send(401, "FAILED");
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

          notifyClients(jsonRet);
          request->send(200, "SUCCESS");
        });

        server.on(
        "/plan", HTTP_POST, [this](AsyncWebServerRequest *request)
        {

          if(!authenticationAccount(request, {"token", "what", "pinID", "action", "hours", "dayWeek", "date"}, {"planDate", "planWeek"})){
            request->send(401, "FAILED");
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
              varPins.actionList.push_back({
                {dateCv.day, dateCv.month, dateCv.year, hourCv.hour, hourCv.minute},
                act,
                idPin,
                arrayPin[k]
              });
            }
            if (request->arg("what") == "planWeek"){
              varPins.actionWeek.push_back({
                arrayWeek,
                hourCv,
                act,
                idPin,
                arrayPin[k],
                0
              });
            }
          }
          StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
          jsonRet["what"] = request->arg("what");
          jsonRet["arrayListActivity"] = arrCreator::listActionElm(true);

          notifyClients(jsonRet);
          request->send(200, "SUCCESS");
        });

        server.on(
            "/setTemperature", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "pinID", "temperature"}, {"setTemperature"})){
                request->send(401, "FAILED");
                return;
              }

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
              request->send(200, "SUCCESS");
            });

        server.on(
            "/timetableRemove", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "indexElm", "action", "pinID", "data", "hours"}, {"timetable"})){
                request->send(401, "FAILED");
                return;
              }

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
                  varPins.actionList.erase(varPins.actionList.begin());
                  break;
                }
              }

              StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
              jsonRet["what"] = request->arg("what");
              jsonRet["actionEle"] = arrCreator::listActionElm(true);

              notifyClients(jsonRet);
              request->send(200, "SUCCESS");
            });

        server.on(
            "/changeUsername", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "newUsername", "password"}, {"changeUsername"})){
                request->send(401, "FAILED");
                return;
              }

              int error = 0;
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token")){
                    if(Accounts[i].password == request->arg("password"))
                      Accounts[i].username = request->arg("newUsername");
                    else error++;
                  }

              if(error != 0){
                request->send(401, "FAILED");
                return;
              }

              request->send(200, "SUCCESS");
            });

        server.on(
            "/changePassword", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "newPassword", "oldPassword"}, {"changePassword"})){
                request->send(401, "FAILED");
                return;
              }

              int error = 0;
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token")){
                    if(Accounts[i].password == request->arg("oldPassword"))
                      Accounts[i].password = request->arg("newPassword");
                    else error++;
                  }

              if(error != 0){
                request->send(401, "FAILED");
                return;
              }

              request->send(200, "SUCCESS");
            });

        server.on(
            "/validitySession", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "exp_day"}, {"validitySession"})){
                request->send(401, "Blad pobierania danych, Brak dostepu.");
                return;
              }

              int error = 0;
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token"))
                    Accounts[i].dayExp_mainSession = request->arg("exp_day").toInt();
                  else error++;

              if(error != 0){
                request->send(405, "Blad pobierania danych, Sesja wygasla.");
                return;
              }
              request->send(200, "SUCCESS");
            });

        server.on(
            "/getMobileToken", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what"}, {"getMobileToken"})){
                request->send(401, "{\"act\": \"FAILED\", \"result\": \"Blad pobierania danych, Brak dostepu.\"}");
                return;
              }

              int error = 0;
              String mbtoken = "";
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token")){
                    if(ownTime::compareDate(Accounts[i].mobileToken.expDate))
                      mbtoken = Accounts[i].mobileToken.token;
                    else error++;
                  }

              if(error != 0){
                request->send(405, "{\"act\": \"FAILED\", \"result\": \"Blad pobierania danych, Sesja wygasla.\"}");
                return;
              }

              String result = "{\"act\": \"SUCCESS\", \"result\": \"" + mbtoken + "\"}";
              request->send(200, "plain/text", result);
            });

        server.on(
            "/generateNewMobileToken", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what"}, {"generateNewMobileToken"})){
                request->send(401, "{\"act\": \"FAILED\", \"result\": \"Blad pobierania danych, Brak dostepu.\"}");
                return;
              }

              int error = 0;
              String mbtoken = "";
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token")){
                    if(ownTime::compareDate(Accounts[i].mobileToken.expDate)){
                      Accounts[i].mobileToken = generateWebToken(Accounts[i].dayExp_mobileSession);
                      mbtoken = Accounts[i].mobileToken.token;
                    }else error++;
                  }

              if(error != 0){
                request->send(405, "{\"act\": \"FAILED\", \"result\": \"Blad pobierania danych, Sesja wygasla.\"}");
                return;
              }

              String result = "{\"act\": \"SUCCESS\", \"result\": \"" + mbtoken + "\"}";
              request->send(200, "plain/text", result);
            });

        server.on(
            "/mobileValiditySession", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
              if(!authenticationAccount(request, {"token", "what", "exp_day"}, {"mobileValiditySession"})){
                request->send(401, "Blad pobierania danych, Brak dostepu.");
                return;
              }

              int error = 0;
              for(int i=0; i < Accounts.size(); i++)
                for(int j=0; j < Accounts[i].session.size(); j++)
                  if(Accounts[i].session[j].token == request->arg("token"))
                    Accounts[i].dayExp_mobileSession = request->arg("exp_day").toInt();
                  else error++;

              if(error != 0){
                request->send(405, "Blad pobierania danych, Sesja wygasla.");
                return;
              }
              request->send(200, "SUCCESS");
            });

        server.onNotFound([](AsyncWebServerRequest *request)
                          { request->send(SPIFFS, "/notFound/index.html", "text/html"); });

        server.begin();
  }
};

#endif