#ifndef FUNCTION
#define FUNCTION

#include <SPIFFS.h>
#include <time.h>

#include "variablesSchemat.h"
#include "variables.h"

const int Month_30[4] = {4, 6, 9, 11};
const int Month_31[7] = {1, 3, 5, 7, 8, 10, 12};
const String Month_Word[12] = {"Styczeń", "Luty", "Marzec", "Kwiecień", "Maj", "Czerwiec", "Lipiec", "Sierpień", "Wrzesień", "Październik", "Listopad", "Grudzień"};

///Tworzenie tablicy dla clienta JavaScript
String createJSArray(int lengthStringArray, Variables *var)
{
  String arrayForJS = "var elements = [";
  for (int i = 0; i < lengthStringArray; i++)
  {
    arrayForJS += "[\'" + var->pins[i][0] + "\', " + var->pinsPower[i] + ", \'" + var->pins[i][1] + "\']";
    if (i != lengthStringArray - 1)
      arrayForJS += ", ";
  }
  return arrayForJS + "]";
}

///Inicjacja pinow
void initialPins(std::vector<std::vector<String>> array)
{
  for (int i = 0; i < array.size(); i++)
    pinMode(array[i][0].toInt(), OUTPUT);
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
  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
}

//Klasa odpowiedzialna za dostarczanie czasu
class Time
{
public:
  struct tm timeinfo;
  Time(const char *ntpServer, const long gmtOffset_sec, const int daylightOffset_sec)
  {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  }

  void getTime()
  {
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Błąd pobierania czasu.");
      return;
    }
  }

  int Hour()
  {
    char timeHour[3];
    strftime(timeHour, 3, "%H", &timeinfo);
    return atoi(timeHour);
  }
  int Minute()
  {
    char timeHour[3];
    strftime(timeHour, 3, "%M", &timeinfo);
    return atoi(timeHour);
  }
  int Second()
  {
    char timeHour[3];
    strftime(timeHour, 3, "%S", &timeinfo);
    return atoi(timeHour);
  }
  int Day()
  {
    char timeHour[3];
    strftime(timeHour, 3, "%d", &timeinfo);
    return atoi(timeHour);
  }
  String weekDay()
  {
    char timeHour[10];
    strftime(timeHour, 10, "%d", &timeinfo);
    return timeHour;
  }
  String Month()
  {
    char timeHour[10];
    strftime(timeHour, 10, "%B", &timeinfo);
    return timeHour;
  }
  int nrMonth()
  {
    String month[] = {"January", "February", "March", "April", "May", "	June", "July", "August", "September", "October", "November", "December"};
    char timeHour[10];
    strftime(timeHour, 10, "%B", &timeinfo);
    for (int i = 0; i < 12; i++)
      if (String(timeHour) == month[i])
        return i + 1;
    return -1;
  }
  int Year()
  {
    char timeHour[4];
    strftime(timeHour, 4, "%B", &timeinfo);
    return atoi(timeHour);
  }
};

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

//Porownywanie daty ze struktury DATE
bool compareDate(DATE minimalDate, DATE maximalDate)
{
  if ((minimalDate.Day > maximalDate.Day && minimalDate.Month > maximalDate.Month) || minimalDate.Year > maximalDate.Year)
    return false;
  return true;
}

//Generowanie tokenu użytkownika
String generateWebToken()
{
  srand(time(NULL));
  String wordToken = "1234567890poiuytrewqasdfghjklmnbvcxz!@#$%&*?LPOIUYTREWQASDFGHJKMNBVCXZ";
  String token = "";
  int wordLength = wordToken.length();

  int randomNumber = 0;
  const int maxNb = rand() % 40 + 40;
  for (int i = 0; i < maxNb; i++)
  {
    randomNumber = rand() % wordLength;
    token += wordToken[randomNumber];
  }
}

//Tworzenie konta użytkownika
AccountSchema createAccount(Time *t, String username, String password)
{
  struct DATE createDate;
  createDate.Day = t->Day();
  createDate.Month = t->nrMonth();
  createDate.Year = t->Year();

  struct AccountSchema defaultAccount;
  defaultAccount.username = username;
  defaultAccount.password = password;
  defaultAccount.dateCreate = createDate;
  return defaultAccount;
}

#endif
