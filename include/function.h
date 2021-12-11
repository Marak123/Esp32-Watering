#ifndef FUNCTION
#define FUNCTION

#include <SPIFFS.h>
#include <time.h>
#include <Arduino.h>
#include <ESP32_FTPClient.h>

#include "ownTime.h"
#include "variables.h"
#include "struct.h"

void deepSleep(String message = "", int second = 60)
{
  Serial.println(message);
  esp_sleep_enable_timer_wakeup(second * 1000000);
  esp_deep_sleep_start();
}

namespace arrCreator{
  String allPins(){
    String arrJS = "var allPins = [";
    for (int i = 0; i < varPins.pins.size(); i++) arrJS += String("[") + varPins.pins[i].nrPin + ", \"" + varPins.pins[i].namePin + "\", \"" + varPins.pins[i].actionPin + "\"],";
    arrJS = arrJS.substring(0, arrJS.length() - 1);
    arrJS += String("];");
    return arrJS;
  }
  String elementsPins(){
    String arrJS = String("var elementsPins = [");
    for (int i = 0; i < varPins.powerPins.size(); i++) arrJS += String("[") + varPins.powerPins[i].idPin + ", " + varPins.powerPins[i].power + "],";
    arrJS = arrJS.substring(0, arrJS.length() - 1);
    arrJS += String("];");
    return arrJS;
  }
  String tmpEm(){
    String arrJS = String("var tmpEm = [");
    for (int i = 0; i < varPins.tempPins.size(); i++) arrJS += String("[") + varPins.tempPins[i].idPinTemp + ", " + varPins.tempPins[i].idPinAir + ", " + varPins.tempPins[i].idPinHeat + "],";
    arrJS = arrJS.substring(0, arrJS.length() - 1);
    arrJS += String("];");
    return arrJS;
  }
  String temperatureEle(bool sameArray=false){
    String arrJS = "";
    if(!sameArray) arrJS += String("var temperatureEle = [");
    else arrJS += "[";
    for (int i = 0; i < varPins.tempData.size(); i++) arrJS += String("[") + varPins.tempData[i].idTemp + ", \"" + varPins.tempData[i].primTemp + "\", " + varPins.tempData[i].powerAir + ", " + varPins.tempData[i].powerHeat + ", \"" + varPins.tempData[i].curlTemp + "\"],";
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
      for (int i = 0; i < varPins.actionList.size(); i++) arrJS += String("[[") + varPins.actionList[i].time.day + ", " + varPins.actionList[i].time.month + ", " + varPins.actionList[i].time.year + "], [" + varPins.actionList[i].time.hours + ", " + varPins.actionList[i].time.minute + "], " + varPins.actionList[i].action + ", " + varPins.actionList[i].idPin + "],";
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
     for(int i=0; i < varPins.actionWeek.size(); i++){
       arrJS += String("[[");
       for(int wd=0; wd < varPins.actionWeek[i].arrayWeek.size(); wd++) arrJS += String(varPins.actionWeek[i].arrayWeek[wd]) + ", ";
       arrJS += String("], [") + String(varPins.actionWeek[i].time.hour) + ", " + String(varPins.actionWeek[i].time.minute) + "], " + String(varPins.actionWeek[i].action) + ", " + String(varPins.actionWeek[i].nrPin) + "],";
     }
    }
    if(!sameArray) arrJS += String("];");
    else arrJS += String("]");
    return arrJS;
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

///Inicjacja WiFi
void initWiFi(const char *ssid, const char *password)
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
}

///Inicjacja odczytu z plikow
void initSPIFFS()
{
  if (!SPIFFS.begin(true, "/data", 100))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
}

//Zapis plikow na serwer FTP
class FTP
{
  ESP32_FTPClient ftp;

public:
  FTP(char ftp_server[], char ftp_user[], char ftp_password[]) : ftp(ftp_server, ftp_user, ftp_password, 5000, 2) {}

  bool Connect(bool message = false)
  {
    ftp.OpenConnection();

    for (int i = 0; i < 15; i++)
    {
      if (ftp.isConnected())
        return true;
      delay(1000);
    }
    if (message)
      Serial.println("Nie można otworzyć połączenia z serwerem FTP.");

    return false;
  }

  bool writeFile(String FileName, String message)
  {
    char FN[FileName.length()];
    char MS[message.length()];

    FileName.toCharArray(FN, FileName.length());
    message.toCharArray(MS, message.length());

    if (!ftp.isConnected())
    {
      Serial.println("Połączenie z serwerem FTP nie jest otwarte.");
      return false;
    }
    try
    {
      ftp.InitFile("Type A");
      ftp.AppendFile(FN);
      ftp.Write(MS);
      ftp.CloseFile();
      return true;
    }
    catch (String error)
    {
      Serial.print("Wystąpił problem: ");
      Serial.println(error);
      return false;
    }
  }
};

//Generowanie tokenu użytkownika
struct s_session generateWebToken(int exp_day=30)
{
  srand(time(NULL));
  String wordToken = "1234567890poiuytrewqasdfghjklmnbvcxz!@#$%&?LPOIUYTREWQASDFGHJKMNBVCXZ";
  String token = "";
  int wordLength = wordToken.length();

  int randomNumber = 0;
  const int maxNb = rand() % 40 + 40;
  for (int i = 0; i < maxNb; i++)
  {
    randomNumber = rand() % wordLength;
    token += wordToken[randomNumber];
  }

  struct s_session tkNew{
    ownTime::addDates({exp_day}),
    token
  };

  return tkNew;
}

//Tworzenie konta użytkownika
struct s_AccountSchema createAccount(String username, String password)
{
  struct s_AccountSchema defaultAccount;
  defaultAccount.username = username;
  defaultAccount.password = password;
  defaultAccount.dateCreate = {
    TimeS.Day(),
    TimeS.Month(),
    TimeS.Year(),
    TimeS.Hour(),
    TimeS.Minute()
  };
  defaultAccount.mobileToken = generateWebToken(365);
  defaultAccount.session.push_back(generateWebToken(30));
  defaultAccount.dayExp_mainSession = 30;
  defaultAccount.dayExp_mobileSession = 365;


  return defaultAccount;
}

bool authenticationAccount(AsyncWebServerRequest *request, std::vector<String> arg, std::vector<String> argWhat){
  for(int a=0; a < arg.size(); a++)
    if (request->hasParam(arg[a], true))
      for(int h=0; h < argWhat.size(); h++)
        if (request->arg("what") == argWhat[h])
        {
          StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonArg;
          for (int i = 0; i < request->params(); i++) jsonArg[request->argName(i)] = request->arg(i);
          for (int i = 0; i < Accounts.size(); i++)
            for (int j = 0; j < Accounts[i].session.size(); j++)
              if (String(Accounts[i].session[j].token) == jsonArg["token"])
                if (ownTime::compareDate(Accounts[i].session[j].expDate)) return true;
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


#endif
