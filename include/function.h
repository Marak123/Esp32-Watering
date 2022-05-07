#ifndef FUNCTION
#define FUNCTION
#pragma once

#include <SPIFFS.h>
#include <time.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "struct.h"
#include "ownTime.h"
#include "variables.h"
#include "ownUpdate.h"

void deepSleep(String message = "", int second = 60)
{
  Serial.println(message);
  esp_sleep_enable_timer_wakeup(second * 1000000);
  esp_deep_sleep_start();
}

namespace arrCreator{
  String allPins(){
    String arrJS = "var allPins = [";
    for (s_pin pin : varPins.pins)  arrJS += String("[") + pin.nrPin + ", \"" + pin.namePin + "\", \"" + pin.actionPin + "\"],";
    arrJS = arrJS.substring(0, arrJS.length() - 1);
    arrJS += String("];");
    return arrJS;
  }
  String elementsPins(){
    String arrJS = String("var elementsPins = [");
    for (s_powerPin pin : varPins.powerPins)  arrJS += String("[") + pin.idPin + ", " + pin.power + "],";
    arrJS = arrJS.substring(0, arrJS.length() - 1);
    arrJS += String("];");
    return arrJS;
  }
  String tmpEm(){
    String arrJS = String("var tmpEm = [");
    for (s_tempPin pin : varPins.tempPins) arrJS += String("[") + String(pin.idPinTemp) + ", " + String(pin.idPinAir) + ", " + String(pin.idPinHeat) + "],";
    arrJS = arrJS.substring(0, arrJS.length() - 1);
    arrJS += String("];");
    return arrJS;
  }
  String temperatureEle(bool sameArray=false){
    String arrJS = "";
    if(!sameArray) arrJS += String("var temperatureEle = [");
    else arrJS += "[";
    for (s_tempData dt : varPins.tempData)  arrJS += String("[") + dt.idTemp + ", \"" + dt.primTemp + "\", " + dt.powerAir + ", " + dt.powerHeat + ", \"" + dt.curlTemp + "\"],";
    arrJS = arrJS.substring(0, arrJS.length() - 1);
    if(!sameArray) arrJS += String("];");
    else arrJS += String("]");
    return arrJS;
  }
  String listActionElm(bool sameArray=false){
    String arrJS = "";
    if(!sameArray) arrJS = String("var listActionElm = [");
    else arrJS = String("[");
    if(varPins.actionList.size() != 0){
      for (s_actionList acL : varPins.actionList)  arrJS += String("[[") + acL.time.day + ", " + acL.time.month + ", " + acL.time.year + "], [" + acL.time.hours + ", " + acL.time.minute + "], " + acL.action + ", " + acL.idPin + "],";
      arrJS = arrJS.substring(0, arrJS.length() - 1);
    }
    if(!sameArray) arrJS += String("];");
    else arrJS += String("]");
    return arrJS;
  }

  String weekActionList(bool sameArray=false){
    String arrJS = "";
    if(!sameArray) arrJS = String("var weekActionList = [");
    else arrJS = String("[");
    if(varPins.actionWeek.size() != 0){
     for (s_actionWeek acW : varPins.actionWeek) {
       arrJS += String("[[");
       for(int wd=0; wd < acW.arrayWeek.size(); wd++) arrJS += String(acW.arrayWeek[wd]) + ", ";
       arrJS += String("], [") + String(acW.time.hour) + ", " + String(acW.time.minute) + "], " + String(acW.action) + ", " + String(acW.nrPin) + "],";
     }
    }
    if(!sameArray) arrJS += String("];");
    else arrJS += String("]");
    return arrJS;
  }

  String versionArray(){
    return "const info_desc = " + updateSoft.getVersion() + ";";
  }


  String allArray(){
    return allPins() + "\n" + elementsPins() + "\n" + tmpEm() + "\n" + temperatureEle() + "\n" + listActionElm() + "\n" + weekActionList();
  }
}

//Konwersja tablicy z parametru http do tablicy cpp
std::vector<int> convArray(String par)
{
  String har = "";
  std::vector<int> arr;
  for(int i=0; i < par.length(); i++){
    if (isDigit(par[i])) har += par[i];
    if (par[i] == ',' || i == par.length() - 1){
      arr.push_back(har.toInt());
      har = "";
      if (i == par.length() - 1) break;
    }
  }
  Serial.println("");
  return arr;
}

///Inicjacja pinow
void initialPins(std::vector<s_pin> array, std::vector<s_powerPin> power)
{
  for (int i = 0; i < array.size(); i++)
  {
    if (array[i].actionPin == "power" || array[i].actionPin == "air" || array[i].actionPin == "heat")
    {
      pinMode(array[i].nrPin, OUTPUT);
      for (int j = 0; j < power.size(); j++)
      {
        if (power[j].idPin == array[i].idPin)
        {
          digitalWrite(array[i].nrPin, power[j].power);
        }
      }
    }
    else if (array[i].actionPin == "temperature")
      pinMode(array[i].nrPin, INPUT);
  }
}

///Wyszukiwanie slow w tablicy
int inArray(std::vector<std::vector<String>> array, String find)
{
  for (int i = 0; i < array.size(); i++)
    if (array[i][0] == find)
      return i;
  return -1;
}

//Generowanie tokenu użytkownika
struct s_session generateWebToken(int exp_day=30, int exp_hour=0, int exp_min=0, String version="session", std::vector<std::vector<s_AccountSchema>> whereFind = {Accounts})
{
  String smallLeter = "abcdefghijklmnopqrstuvwxyz";
  String bigLeter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  String number = "0123456789";
  String special = "!@#$%&?";
  String wordToken = smallLeter + bigLeter + number + special;
  String token = "";
  const int wordLength = wordToken.length();

  bool isUnique = false;
  while(!isUnique){
    token = "";
    srand(time(NULL));
    int randomNumber = 0;
    const int maxNb = rand() % 40 + 40;
    for (int i = 0; i < maxNb; i++)
    {
      randomNumber = rand() % wordLength;
      token += wordToken[randomNumber];
    }
    for(std::vector<s_AccountSchema> as : whereFind){
      bool isIt = false;
      if(version == "session")
        for(struct s_AccountSchema ac : as){
          for(struct s_session ss : ac.session){
            if(ss.token == token){
              isIt = true;
              break;
            }
          }
          if(isIt) break;
        }
      else if(version == "mobile")
        for(struct s_AccountSchema ac : as){
          if(ac.mobileToken.token == token){
            isIt = true;
            break;
          }
        }
      else return s_session();
      if(isIt) {
        isUnique = false;
        break;
      }
      else isUnique = true;
    }
  }
  struct s_session tkNew{
    ownTime::addDates({exp_day, 0, 0, exp_hour, exp_min}),
    token
  };
  return tkNew;
}

//Tworzenie konta użytkownika
struct s_AccountSchema createAccount(String username, String password, bool isAdmin=false)
{
  for(s_AccountSchema ac : Accounts)
    if(ac.username == username){
      Serial.println("Użytkownik o takiej nazwie już istnieje.");
      return {};
    }

  struct s_AccountSchema defaultAccount{username, password, { TimeS.Day(), TimeS.Month(), TimeS.Year(), TimeS.Hour(), TimeS.Minute() }, {}, {}, 30, 365, isAdmin, {} };
  return defaultAccount;
}

bool authenticationAccount(AsyncWebServerRequest *request, std::vector<String> arg, std::vector<String> argWhat, bool adminSession=false){
  for(String a : arg)
    if (request->hasParam(a, true))
      for(String aw : argWhat)
        if (request->arg("what") == aw)
        {
          StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonArg;
          for (int i = 0; i < request->params(); i++) jsonArg[request->argName(i)] = request->arg(i);
          for (s_AccountSchema acc : Accounts)
            if(!adminSession)
              for (s_session s : acc.session) if (String(s.token) == jsonArg["token"]) return ownTime::compareDate(s.expDate);
            else
              for (s_session s : acc.adminSession) if (String(s.token) == jsonArg["token"]) return ownTime::compareDate(s.expDate);
        }
  return false;
}

namespace pins
{
  bool changePower(int pinNR, bool power)
  {
    int idPin = -1;
    for (int i = 0; i < varPins.pins.size(); i++)
      if (varPins.pins[i].nrPin == pinNR)
      {
        idPin = varPins.pins[i].idPin;
        break;
      }
    for (int i = 0; i < varPins.powerPins.size(); i++)
      if (varPins.powerPins[i].idPin == idPin)
      {
        varPins.powerPins[i].power = power;
        return true;
      }
    return false;
  }
}

char *ToChar(String str)
{
  char *cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());
  return cstr;
}

bool validAction(struct s_actionList obj){
  const int yea = obj.time.year;
  const int mon = obj.time.month;
  const int day = obj.time.day;
  const int hou = obj.time.hours;
  const int min = obj.time.minute;
  const int nYea = TimeS.Year();
  const int nMon = TimeS.Month();
  const int nDay = TimeS.Day();
  const int nHou = TimeS.Hour();
  const int nMin = TimeS.Minute();
  if(yea > nYea) return true;
  else if(yea < nYea) return false;

  if(mon > nMon) return true;
  else if(mon < nMon) return false;

  if(day > nDay) return true;
  else if(day < nDay) return false;

  if(hou > nHou) return true;
  else if(hou < nHou) return false;

  if(min > nMin) return true;
  else if(min < nMin) return false;

  int good = 0;
  for(int p : output_pin)
    if(obj.nrPin == p) {
      good = 1;
      break;
    }
  if(!good) return false;

  if(obj.action != true && obj.action != false) return false;

  digitalWrite(obj.nrPin, obj.action);

  return false;
}

bool validAction(struct s_actionWeek obj){
  if(obj.arrayWeek.size() == 0 || obj.arrayWeek.size() > 7) return false;
  for(int i : obj.arrayWeek)
    if(i < 0 || i > 6) return false;

  if((obj.time.hour > 23 && obj.time.hour < 0) && (obj.time.minute > 59 && obj.time.minute < 0) && (obj.time.second > 59 && obj.time.second < 0)) return false;
  int good = 0;
  for(int p : output_pin)
    if(obj.nrPin == p) {
      good = 1;
      break;
    }
  if(!good) return false;
  if(obj.action != true && obj.action != false) return false;

  return true;
}

String getFromRequest(AsyncWebServerRequest *request, String whatGet){
    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      if(String(h->name().c_str()) == whatGet)
        // Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
        return h->value().c_str();
    }
    return "";
}

#endif
