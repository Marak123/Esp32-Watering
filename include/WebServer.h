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
    Serial.println("Wysyłam info");
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
      struct s_date todayDate = {
          TimeS.Day(),
          TimeS.Month(),
          TimeS.Year()
      };

      String cookie = request->header("Cookie");
      // Serial.println(cookie);
      for(int i=0; i < Accounts.size(); i++){
        for(int j=0; j < Accounts[i].session.size(); j++){
          String cook = "ESPSESSIONID=" + String(Accounts[i].session[j].token);
          if (cookie.indexOf(cook) != -1) if (ownTime::compareDate(Accounts[i].session[j].expDate, todayDate))  return true;
        }
      }
    }
    return false;
  }

  //Inicjacja Serwera
  void ServerInit()
  {
    //Glowna Strona
    server.on("/", [this](AsyncWebServerRequest *request) {
      if(is_authentified(request)){
        request->send(SPIFFS, "/index.html", String(), false, [this](const String &var) -> String { return this->processor(var); });
        return;
      }

      if (request->hasArg("LOGOUT"))
      {
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/login/index.html");
        response->addHeader("Set-Cookie", "ESPSESSIONID=0");
        request->send(response);
        return;
      }

      if (request->hasArg("username") && request->hasArg("password"))
      {
        for (int i = 0; i < Accounts.size(); i++)
        {
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
        }
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
            if (request->hasParam("token", true) && request->hasParam("what", true) && request->hasParam("pinID", true) && request->hasParam("action", true)){
              if (request->arg("what") == "power-now")
              {
                struct s_date todayData = {
                    TimeS.Day(),
                    TimeS.Month(),
                    TimeS.Year(),
                    TimeS.Hour(),
                    TimeS.Minute()
                };
                StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonArg;
                for(int i=0; i < request->params(); i++){
                  jsonArg[request->argName(i)] = request->arg(i);
                }
                for (int i = 0; i < Accounts.size(); i++){
                  for(int j=0; j < Accounts[i].session.size();j++){
                    if(String(Accounts[i].session[j].token) == jsonArg["token"]){
                      if (ownTime::compareDate(Accounts[i].session[j].expDate, todayData))
                      {
                        String pin = jsonArg["pinID"];
                        bool act = false;
                        if (jsonArg["action"] == "true") act = true;
                        digitalWrite(pin.toInt(), act);
                        pins::changePower(pin.toInt(), act);

                        StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
                        jsonRet["pinID"] = jsonArg["pinID"];
                        jsonRet["action"] = jsonArg["action"];
                        jsonRet["what"] = jsonArg["what"];
                        notifyClients(jsonRet);
                        request->send(200, "SUCCESS");
                      }
                    }
                  }
                }
              }
            }
              request->send(401, "FAILED");
        });

        server.on(
        "/delay", HTTP_POST, [this](AsyncWebServerRequest *request)
        {
            if (request->hasParam("token", true) && request->hasParam("what", true) && request->hasParam("pinID", true) && request->hasParam("action", true) && request->hasParam("delay", true))
            {
              if (request->arg("what") == "delay")
              {
                struct s_date todayData = {
                    TimeS.Day(),
                    TimeS.Month(),
                    TimeS.Year(),
                    TimeS.Hour(),
                    TimeS.Minute()
                };
                StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonArg;
                for(int i=0; i < request->params(); i++){
                    jsonArg[request->argName(i)] = request->arg(i);
                }
                for (int i = 0; i < Accounts.size(); i++){
                  for(int j=0; j < Accounts[i].session.size();j++){
                    if(String(Accounts[i].session[j].token) == jsonArg["token"]){
                      if(ownTime::compareDate(Accounts[i].session[j].expDate, todayData)){
                        bool act = false;
                        if(jsonArg["action"] == "true") act = true;
                        std::vector<int> arrayPin = convArray(jsonArg["pinID"]);

                        int idPin = -1;
                        for(int k=0; k < arrayPin.size(); k++){
                          idPin = -1;
                          for(int l=0; l < varPins.pins.size(); l++) if(varPins.pins[l].nrPin == arrayPin[k]) idPin = varPins.pins[l].idPin;
                          if(idPin == -1) continue;

                          varPins.actionList.push_back({
                            ownTime::addHours(jsonArg["delay"]),
                            act,
                            idPin,
                            arrayPin[k]
                          });
                        }
                        StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
                        jsonRet["what"] = jsonArg["what"];
                        jsonRet["arrayListActivity"] = arrCreator::listActionElm(true);

                        notifyClients(jsonRet);
                        request->send(200, "SUCCESS");
                      }
                    }
                  }
                }
              }
            }
            request->send(401, "FAILED");
        });

        server.on(
        "/plan", HTTP_POST, [this](AsyncWebServerRequest *request)
        {
            int args = request->args();
            for (int i = 0; i < args; i++)
            {
              Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
            }
            if (request->hasParam("token", true) && request->hasParam("what", true) && request->hasParam("pinID", true) && request->hasParam("action", true) && request->hasParam("hours", true) && (request->hasParam("dayWeek", true) || request->hasParam("date", true)))
            {
              if (request->arg("what") == "planDate" || request->arg("what") == "planWeek")
              {
            Serial.println("Dzialaaa");
                struct s_date todayData = {
                    TimeS.Day(),
                    TimeS.Month(),
                    TimeS.Year(),
                    TimeS.Hour(),
                    TimeS.Minute()
                };
                StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonArg;
                for(int i=0; i < request->params(); i++){
                    jsonArg[request->argName(i)] = request->arg(i);
                }
                for (int i = 0; i < Accounts.size(); i++){
                  for(int j=0; j < Accounts[i].session.size();j++){
                    if(String(Accounts[i].session[j].token) == jsonArg["token"]){
                      if(ownTime::compareDate(Accounts[i].session[j].expDate, todayData)){
                        s_date dateCv;
                        std::vector<int> arrayWeek;
                        if(jsonArg["what"] == "planDate") dateCv = ownTime::convertDate(jsonArg["date"]);
                        if(jsonArg["what"] == "planWeek") arrayWeek = convArray(jsonArg["dayWeek"]);
                        s_hour hourCv = ownTime::convertHours(jsonArg["hours"]);
                        std::vector<int> arrayPin = convArray(jsonArg["pinID"]);
                        bool act = false;
                        if(jsonArg["action"] == "true") act = true;

                        int idPin = -1;
                        for (int k = 0; k < arrayPin.size(); k++)
                        {
                          idPin = -1;
                          for (int l = 0; l < varPins.pins.size(); l++)
                            if (varPins.pins[l].nrPin == arrayPin[k]) idPin = varPins.pins[l].idPin;
                          if (idPin == -1) continue;

                          if (jsonArg["what"] == "planDate")
                            varPins.actionList.push_back({
                              {dateCv.day, dateCv.month, dateCv.year, hourCv.hour, hourCv.minute},
                              act,
                              idPin,
                              arrayPin[k]
                            });
                          if (jsonArg["what"] == "planWeek")
                            varPins.actionWeek.push_back({
                              arrayWeek,
                              act,
                              idPin,
                              arrayPin[k],
                              0
                            });
                        }
                        StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
                        jsonRet["what"] = jsonArg["what"];
                        jsonRet["arrayListActivity"] = arrCreator::listActionElm(true);

                        notifyClients(jsonRet);
                        request->send(200, "SUCCESS");
                      }
                    }
                  }
                }
              }
            }
            request->send(401, "FAILED");
        });

    server.onNotFound([](AsyncWebServerRequest *request) { request->send(SPIFFS, "/notFound/index.html", "text/html"); });

    server.begin();
  }
};

#endif