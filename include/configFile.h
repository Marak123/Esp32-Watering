#ifndef CONFIGFILE
#define CONFIGFILE

#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <algorithm>

#include "struct.h"
#include "variables.h"
#include "function.h"

class confFile
{
private:
    String pathFile = "/configFile.json";

public:
    confFile(String pathFile)
    {
        Serial.println("SSS");
        this->pathFile = "/configFile.json";
    }

    bool loadConfig()
    {
        File configFile = SPIFFS.open(this->pathFile, "r");
        if (!configFile)
        {
            Serial.println("Błąd przy ładowaniu pliku konfiguracyjnego.");
            deepSleep("Moduł został wyłączony. Uruchomi się ponownie po 60 sekundach lub zresetuj go recznie za pomocą przycisku na płytce", 60);
            return false;
        }

        size_t sizeFile = configFile.size();
        char *buf = new char[sizeFile];
        configFile.readBytes(buf, sizeFile);
        const int sizeFileconst = JSON_OBJECT_SIZE(300);

        Serial.print("File: ");
        Serial.println(sizeFileconst);

        StaticJsonDocument<sizeFileconst> json;
        StaticJsonDocument<sizeFileconst> wifi;
        StaticJsonDocument<sizeFileconst> ftp;
        DeserializationError err = deserializeJson(json, buf);

        if (err)
        {
            delete buf;
            Serial.println("Error: " + String(err.c_str()));
            Serial.println("Błąd przy konwersji danych z pliku do typu JSON");
            deepSleep("Moduł został wyłączony. Uruchomi się ponownie po 60 sekundach lub zresetuj go recznie za pomocą przycisku na płytce.");
            return false;
        }

        Serial.print("Old ssid: ");
        char ss = json["WIFI"]["SSID"];
        Serial.println(ss);

        DeserializationError errWifi = deserializeJson(wifi, json["WIFI"]);
        if(errWifi){
            delete buf;
            Serial.println("Error: " + String(errWifi.c_str()));
            Serial.println("Błąd przy konwersji danych z pliku do typu JSON");
            deepSleep("Moduł został wyłączony. Uruchomi się ponownie po 60 sekundach lub zresetuj go recznie za pomocą przycisku na płytce.");
            return false;
        }

        // strcpy(dataWifi.SSID, wifi["SSID"]);
        // strcpy(dataWifi.password, wifi["PASSWORD"]);
        // strcpy(dataWifi.localIP, wifi["LOCAL_IP"]);
        // strcpy(dataWifi.gateway, wifi["GETEWAY"]);
        // strcpy(dataWifi.subnet, wifi["SUBNET"]);

        // strcpy(dataFtp.addrIP, json["FTP_SERVER"]["IP_ADDRESS"]);
        // strcpy(dataFtp.username, json["FTP_SERVER"]["USERNAME"]);
        // strcpy(dataFtp.password, json["FTP_SERVER"]["PASSWORD"]);
        // dataWifi.SSID = json["WIFI"]["SSID"];
        // dataWifi.password = json["WIFI"]["PASSWORD"];
        // dataWifi.localIP = json["WIFI"]["LOCAL_IP"];
        // dataWifi.password = json["WIFI"]["GETEWAY"];
        // dataWifi.password = json["WIFI"]["SUBNET"];

        // dataFtp.addrIP = json["FTP_SERVER"]["IP_ADDRESS"];
        // dataFtp.username = json["FTP_SERVER"]["USERNAME"];
        // dataFtp.password = json["FTP_SERVER"]["PASSWORD"];

        Serial.print("New ssid: ");
        // char sk = *dataWifi.SSID;
        Serial.println(dataWifi.SSID);

        struct s_pin pin;
        struct s_powerPin powerPin;
        struct s_tempPin tempPin;
        struct s_tempData tempData;

        int idPin = 0;
        int idPower = 0;
        int idTemp = 0;

        JsonArray arrayPowerPins = json["PINS"]["POWER_PINS"].to<JsonArray>();
        for (int i = 0; i < arrayPowerPins.size(); i++)
        {
            if (arrayPowerPins[i].size() == 3)
            {
                pin.nrPin = arrayPowerPins[i][0];
                pin.namePin = arrayPowerPins[i][1].as<String>();
                pin.actionPin = "power";
                pin.idPin = idPin;
                varPins.pins.push_back(pin);

                powerPin.idPin = idPin;
                powerPin.power = arrayPowerPins[i][2];
                powerPin.idPower = idPower;
                idPin++;
                idPower++;
            }
        }

        JsonArray arrayTempPins = json["PINS"]["TEMP_PINS"].to<JsonArray>();
        for (int i = 0; i < arrayTempPins.size(); i++)
        {
            if (arrayTempPins[i].size() == 2)
            {
                pin.nrPin = arrayTempPins[i][0];
                pin.namePin = arrayTempPins[i][1].as<String>();
                pin.actionPin = "temperature";
                pin.idPin = idPin;
                varPins.pins.push_back(pin);
                idPin++;
            }
        }

        JsonArray arrayAirPins = json["PINS"]["AIR_PINS"].to<JsonArray>();
        for (int i = 0; i < arrayAirPins.size(); i++)
        {
            if (arrayAirPins[i].size() == 3)
            {
                pin.nrPin = arrayAirPins[i][0];
                pin.namePin = arrayAirPins[i][1].as<String>();
                pin.actionPin = "air";
                pin.idPin = idPin;
                varPins.pins.push_back(pin);
                idPin++;
            }
        }

        JsonArray arrayHeatPins = json["PINS"]["HEAT_PINS"].to<JsonArray>();
        for (int i = 0; i < arrayHeatPins.size(); i++)
        {
            if (arrayHeatPins[i].size() == 3)
                {
                    pin.nrPin = arrayHeatPins[i][0];
                    pin.namePin = arrayHeatPins[i][1].as<String>();
                    pin.actionPin = "heat";
                    pin.idPin = idPin;
                    varPins.pins.push_back(pin);
                    idPin++;
                }
        }

        for (int i = 0; i < arrayTempPins.size(); i++)
        {
            if (arrayTempPins[i].size() == 2)
            {
                tempPin.nrPinTemp = arrayTempPins[i][0];
                for (int j = 0; j < varPins.pins.size(); j++)
                {
                    if (varPins.pins[j].nrPin == arrayTempPins[i][0])
                    {
                        tempPin.idPinTemp = varPins.pins[j].idPin;
                        break;
                    }
                }

                for (int i = 0; i < arrayAirPins.size(); i++)
                {
                    if (arrayTempPins[i][0] == arrayAirPins[i][2])
                    {
                        tempPin.nrPinAir = arrayAirPins[i][0];
                        for (int j = 0; j < varPins.pins.size(); j++)
                        {
                            if (varPins.pins[j].nrPin == arrayAirPins[i][0])
                            {
                                tempPin.idPinAir = varPins.pins[j].idPin;
                                break;
                            }
                        }
                    }
                }

                for (int i = 0; i < arrayHeatPins.size(); i++)
                {
                    if (arrayTempPins[i][0] == arrayHeatPins[i][2])
                    {
                        tempPin.nrPinHeat = arrayHeatPins[i][0];
                        for (int j = 0; j < varPins.pins.size(); j++)
                        {
                            if (varPins.pins[j].nrPin == arrayHeatPins[i][0])
                            {
                                tempPin.idPinHeat = varPins.pins[j].idPin;
                                break;
                            }
                        }
                    }
                }

                tempPin.idTemp = idTemp;
                idTemp++;
            }
        }

        JsonArray arrayTempData = json["PINS"]["TEMP_DATA"].to<JsonArray>();

        for (int i = 0; i < arrayTempData.size(); i++)
        {
            if (arrayTempData[i].size() == 2)
            {
                for (int j = 0; j < varPins.tempPins.size(); j++)
                {
                    if (arrayTempData[i][0] == varPins.tempPins[j].nrPinTemp)
                    {
                        tempData.idTemp = varPins.tempPins[j].idTemp;
                        tempData.idPinHeat = varPins.tempPins[j].idPinHeat;
                        tempData.idPinAir = varPins.tempPins[j].idPinAir;
                        tempData.primTemp = arrayTempData[i][1];
                        break;
                    }
                }
            }
        }

        delete buf;

        configFile.close();
        return true;
    }
};

#endif