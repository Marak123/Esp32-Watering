#ifndef WEBSOCKET
#define WEBSOCKET

#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "function.h"
#include "variablesSchemat.h"

class WebSocket
{
public:
  AsyncWebServer server;
  AsyncWebSocket ws;

  Variables *variableAddress;
  Time *time;
  FTP *ftp;

  WebSocket(Variables *vars, Time *ti, FTP *ft) : server(80), ws("/ws")
  {
    variableAddress = vars;
    time = ti;
    ftp = ft;
  };

  void notifyClients(StaticJsonDocument<JSON_OBJECT_SIZE(10)> json)
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

      String pin = json["pin"];
      int position = inArray(variableAddress->pins, pin);
      if (json["action"] == "power") { digitalWrite(pin.toInt(), json["turn"]); variableAddress->pinsPower[position] = !variableAddress->pinsPower[position];  notifyClients(json); }
      else if (json["action"] == "withDelay")
      {
        withDelayTemp withdelay;
        JsonArray pinsJson = json["pins"];
        JsonObject delayJson = json["delay"];

        const int Hour = time->Hour();
        const int Minute = time->Minute();

        const int hourDelay = delayJson["hours"];
        const int minuteDelay = delayJson["minute"];
        const int calcHour = hourDelay + Hour + 1;
        const int calcMinute = minuteDelay + Minute;

        withdelay.turn = json["turn"];
        for (int i = 0; i < pinsJson.size(); i++) withdelay.pins.push_back(pinsJson[i]);
        withdelay.hour = calcHour;
        withdelay.minute = calcMinute;

        variableAddress->withDelay.push_back(withdelay);

        notifyClients(json);
      }
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
  void initWebSocket() { using namespace std::placeholders; ws.onEvent(std::bind(&WebSocket::onEvent, this, _1, _2, _3, _4, _5, _6)); server.addHandler(&ws); }

  //Zmienne na stronie
  String processor(const String &var) { if(var == "ARRAY"){ int lengthStringArray = variableAddress->pins.size(); return createJSArray(lengthStringArray, variableAddress); } return "ERROR"; }

  bool is_authentified(AsyncWebServerRequest *request)
  {
    if (request->hasHeader("Cookie"))
    {
      struct DATE todayDate;
      todayDate.Day = time->Day();
      todayDate.Month = time->nrMonth();
      todayDate.Year = time->Year();

      String cookie = request->header("Cookie");
      Serial.println(cookie);
      for(int i=0; i < Accounts.size(); i++){
        String cook = "ESPSESSIONID=" + String(Accounts[i].sess.token);
        if (cookie.indexOf(cook) != -1) if (compareDate(Accounts[i].sess.expirationDates, todayDate) && Accounts[i].sess.session == true)  return true;
      }
    }
    return false;
  }

  //Inicjacja Serwera
  void ServerInit()
  {
    //Glowna Strona
    server.on("/", [this](AsyncWebServerRequest *request) {
      if (request->hasArg("DISCONNECT"))
      {
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/login/index.html", "text/html");
        response->addHeader("Location", "/login");
        response->addHeader("Cache-Control", "no-cache");
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
            response->addHeader("Location", "/");
            response->addHeader("Cache-Control", "no-cache");

            struct DATE expDate;
            expDate.Day = 32 - time->Day();
            expDate.Month = time->nrMonth() + 1;
            expDate.Year = time->Year();
            Accounts[i].sess.token = generateWebToken();
            Accounts[i].sess.expirationDates = expDate;
            Accounts[i].sess.session = true;

            response->addHeader("Set-Cookie", "ESPSESSIONID=" + Accounts[i].sess.token);
            request->send(response);
            return;
          }
        }
      }
      if (!is_authentified(request))
      {
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/login/index.html", "text/html");
        response->addHeader("Location", "/login");
        response->addHeader("Cache-Control", "no-cache");
        request->send(response);
        return;
      }
      // <a href="/login?DISCONNECT=YES">disconnect</a>
      request->send(SPIFFS, "/index.html", String(), false, [this](const String &var) -> String { return this->processor(var); });
    });


    // //MAIN CSS File
    // server.on("/style/style-mainContent.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/style/style-mainContent.css", "text/css"); });
    // server.on("/style/style-withDelay.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/style/style-withDelay.css", "text/css"); });
    // server.on("/style/style-planeLaunch.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/style/style-planeLaunch.css", "text/css"); });

    //MAIN CSS File
    server.on("/style/style-mainContent.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/style/style-mainContent.css", "text/css"); });
    server.on("/style/style-withDelay.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/style/style-withDelay.css", "text/css"); });
    server.on("/style/style-planeLaunch.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/style/style-planeLaunch.css", "text/css"); });

    //LOGIN CSS File
    server.on("/style/animationStyle.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/login/animationStyle.css", "text/css"); });
    server.on("/style/inter-ui.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/login/inter-ui.css", "text/css"); });
    server.on("/style/loginStyle.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/login/loginStyle.css", "text/css"); });
    server.on("/style/normalize.min.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/login/normalize.min.css", "text/css"); });
    //NOTFOUND CSS File
    server.on("/style/styleNotfound.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/notFound/styleNotfound.css", "text/css"); });


    // //MAIN JS File
    // server.on("/script/mainContent.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/script/mainContent.js", "text/css"); });
    // server.on("/script/withDelay.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/script/withDelay.js", "text/css"); });
    // server.on("/script/planeLaunch.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/script/planeLaunch.js", "text/css"); });
    // server.on("/script/changePage.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/script/changePage.js", "text/css"); });



    //MAIN JS File
    server.on("/script/mainContent.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/script/mainContent.js", "text/css"); });
    server.on("/script/withDelay.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/script/withDelay.js", "text/css"); });
    server.on("/script/planeLaunch.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/script/planeLaunch.js", "text/css"); });
    server.on("/script/changePage.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/script/changePage.js", "text/css"); });

    //LOGIN JS File
    server.on("/script/anime.min.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/login/anime.min.js", "text/css"); });
    server.on("/script/script.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/login/script.js", "text/css"); });
    //NOTFOUND JS File
    server.on("/script/scriptNotfound.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/notFound/scriptNotfound.js", "text/css"); });
    server.on("/script/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/notFound/jquery.min.js", "text/css"); });
    server.on("/script/SplitText.min.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/notFound/SplitText.min.js", "text/css"); });
    server.on("/script/TweenMax.min.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/notFound/TweenMax.min.js", "text/css"); });

    //Image file
    server.on("/image/forest.jpg", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/login/forest.jpg", "image"); });
    server.on("/logo/favicon-32x32.png", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/logo/favicon-32x32.png", "image"); });
    server.on("/images/clock-bg-sm.png", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/images/clock-bg-sm.png", "image"); });

  //JS MAIN
    server.on("/scripts/pacJS.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/scripts/pacJS.js", "text/js"); });
    server.on("/scripts/pacOriginalFileJS.js", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/scripts/pacOriginalFileJS.js", "text/js"); });

  //CSS MAIN
    server.on("/scripts/pacCSS.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/scripts/pacCSS.css", "text/css"); });
    server.on("/scripts/pacOriginalFileCSS.css", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/scripts/pacOriginalFileCSS.css", "text/css"); });


    //Aplikacja na Telefon
    server.on("/power", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        String dataReq = "";
        for (size_t i = 0; i < len; i++) { dataReq += data[i]; }
        request->send_P(200, "text/html", "success"); 
    });

    server.onNotFound([](AsyncWebServerRequest *request) { request->send(SPIFFS, "/notFound/index.html", "text/html"); });

    server.begin();
  }
};

#endif