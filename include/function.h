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
    return "const info_desc = [" + updateSoft.getVersion() + "];";
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
void initWiFi(String ssid=dataWifi.ssid, String password=dataWifi.password, String hostname="ESP32-platform-watering",IPAddress local_ip=dataWifi.local_ip, IPAddress gateway=dataWifi.gateway, IPAddress subnet=dataWifi.subnet)
{
  if(WiFi.status() == WL_CONNECTED) return;

  Serial.println("SSID: " + ssid + "\nPassword: " + password);

  WiFi.begin((const char*)ssid.c_str(), (const char*)password.c_str());
  if(local_ip != IPAddress(0,0,0,0) && gateway != IPAddress(0,0,0,0) && subnet != IPAddress(0,0,0,0)) WiFi.config(local_ip, gateway, subnet);
  WiFi.hostname((const char*)hostname.c_str());

  int timeout_counter = 0;
  Serial.print("Connecting to WiFi..");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
    timeout_counter++;
    if(timeout_counter >= 5) ESP.restart();
  }

  Serial.print("Connected to the WiFi network. \nWith IP: " + WiFi.localIP().toString() + "\nGateway: " + WiFi.gatewayIP().toString() + "\nSubnet: " + WiFi.subnetMask().toString() + "\nHostname: " + String(WiFi.getHostname()) + "\n\n");
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
  FTP(char *ftp_server, char *ftp_user, char *ftp_password, uint8_t port) : ftp(ftp_server, port, ftp_user, ftp_password, 5000, 2) {}
  FTP(char *ftp_server, char *ftp_user, char *ftp_password) : ftp(ftp_server, ftp_user, ftp_password, 5000, 2) {}

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
struct s_session generateWebToken(int exp_day=30, int exp_hour=0, int exp_min=0)
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

  struct s_AccountSchema defaultAccount{
    username,
    password,
    {
      TimeS.Day(),
      TimeS.Month(),
      TimeS.Year(),
      TimeS.Hour(),
      TimeS.Minute()
    },
    {},
    {},
    30,
    365,
    isAdmin,
    {}
  };
  // defaultAccount.username = username;
  // defaultAccount.password = password;
  // defaultAccount.dateCreate = {
  //   TimeS.Day(),
  //   TimeS.Month(),
  //   TimeS.Year(),
  //   TimeS.Hour(),
  //   TimeS.Minute()
  // };
  // defaultAccount.mobileToken = generateWebToken(365);
  // defaultAccount.session.push_back(generateWebToken(30));
  // defaultAccount.dayExp_mainSession = 30;
  // defaultAccount.dayExp_mobileSession = 365;


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


#endif
