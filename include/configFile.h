#ifndef CONFFILE_H
#define CONFFILE_H
#pragma once

#include <SPIFFS.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Arduino.h>

#include "logS.h"
#include "struct.h"
#include "dataConn.h"

bool _expireAction = false;

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
        if(timeout_counter >= 10) ESP.restart();
    }
    Serial.print("Connected to the WiFi network. \nWith IP: " + WiFi.localIP().toString() + "\nGateway: " + WiFi.gatewayIP().toString() + "\nSubnet: " + WiFi.subnetMask().toString() + "\nHostname: " + String(WiFi.getHostname()) + "\n\n");
    TimeS.getTimeAndSetTimezone();
    rpLog.unloadBuffer();
    rpLog.log("Connected to the WiFi network. With IP: " + WiFi.localIP().toString() + " Gateway: " + WiFi.gatewayIP().toString() + " Subnet: " + WiFi.subnetMask().toString() + " Hostname: " + String(WiFi.getHostname()) );
}

IPAddress splitIPaddress(String str){
    int indF = str.indexOf('.'),
        indS = str.indexOf('.', indF+1),
        indT = str.indexOf('.', indS+1),
        indFo = str.indexOf('.', indT+1);

    return (str.substring(0, indF).toInt(), str.substring(indF+1, indS).toInt(), str.substring(indS+1, indT).toInt(), str.substring(indT+1, indFo).toInt());
}

namespace load{
    void wifi(JsonObject object){
        s_wifi wifi;
        try{

            if(!object.containsKey("SSID") && !object.containsKey("PASSWORD")) return;
            wifi.ssid = object["SSID"].as<String>();
            wifi.password = object["PASSWORD"].as<String>();

            if(!object.containsKey("LOCAL_IP") && !object.containsKey("GATEWAY") && !object.containsKey("SUBNET") && !object.containsKey("HOSTNAME")) return;

            if(object["LOCAL_IP"].size() >= 7)
                wifi.local_ip.fromString(object["LOCAL_IP"].as<String>());
            if(object["GATEWAY"].size() >= 7)
                wifi.gateway.fromString(object["GATEWAY"].as<String>());
            if(object["SUBNET"].size() >= 7)
                wifi.subnet.fromString(object["SUBNET"].as<String>());
            if(object["HOSTNAME"].size() > 0)
                wifi.hostname = object["HOSTNAME"].as<String>();

            dataWifi = wifi;

            if(wifi.ssid.length() == 0) deepSleep("WiFi SSID jest pusty więc nie można kontynuować. Przechodę w tryb uśpienia i się nie uruchomię.");
            initWiFi();
        }catch(...){
            // error
        }

    }
    void ftp(JsonObject object){
        struct s_ftpServer ftp;
        try{
            if(!object.containsKey("IP_ADDRESS") || !object.containsKey("USERNAME") || !object.containsKey("PASSWORD")) return;

            dataFtp.ip_address = ToChar(object["IP_ADDRESS"].as<String>());
            dataFtp.username = ToChar(object["USERNAME"].as<String>());
            dataFtp.password = ToChar(object["PASSWORD"].as<String>());

            if(object.containsKey("PORT"))
                dataFtp.port = object["PORT"].as<uint16_t>();

            dataFtp = ftp;
            ftpClient.initConnection(dataFtp.ip_address, dataFtp.username, dataFtp.password, dataFtp.port);
        }catch(...){
            // error
        }
    }

    void powerPins(JsonArray object){
        std::vector<struct s_pin> pins;
        std::vector<struct s_powerPin> powerPins;
        try{
            for(JsonVariant i : object){
                bool _continue = true;
                if(i.size() < 2) continue;
                for(int p : output_pin) if(i[0].as<int>() == p){ _continue = false; break; }
                for(struct s_pin p : varPins.pins) if(i[0].as<int>() == p.nrPin){ _continue = true; break; }
                for(struct s_pin p : pins) if(i[0].as<int>() == p.nrPin){ _continue = true; break; }
                if(i[1].as<String>().length() < 0 || _continue) continue;

                int idPin = varPins.pins.size() + pins.size();
                pins.push_back({i[0].as<int>(), (i.size() > 1 ? i[1].as<String>() : "Pin "+i[0].as<String>()), "power",  idPin});
                powerPins.push_back({idPin, (i.size() > 2 ? i[2].as<bool>() : false), static_cast<int>(varPins.powerPins.size() + powerPins.size())});
            }

            varPins.pins.insert(varPins.pins.end(), pins.begin(), pins.end());
            varPins.powerPins.insert(varPins.powerPins.end(), powerPins.begin(), powerPins.end());
        }catch (...) {
            // error
        }
    }

    void tempPins(JsonArray object){
        std::vector<struct s_pin> pins;
        std::vector<struct s_tempPin> tempPins;
        try{
            for(JsonVariant i : object){
                bool _continue = true;
                if(i.size() != 2) continue;
                for(int p : input_pin) if(i[0].as<int>() == p){ _continue = false; break; }
                for(struct s_pin p : varPins.pins) if(i[0].as<int>() == p.nrPin){ _continue = true; break; }
                for(struct s_pin p : pins) if(i[0].as<int>() == p.nrPin){ _continue = true; break; }
                if(!(i[0].as<int>() >= 0 && i[1].as<String>().length() > 0) || _continue) continue;

                pins.push_back({i[0].as<int>(), i[1].as<String>(), "temperature", static_cast<int>(varPins.pins.size() + pins.size())});
                tempPins.push_back({i[0].as<int>(), static_cast<int>(varPins.pins.size() + tempPins.size())});
            }

            varPins.pins.insert(varPins.pins.end(), pins.begin(), pins.end());
            varPins.tempPins.insert(varPins.tempPins.end(), tempPins.begin(), tempPins.end());
        }catch (...) {
            // error
        }
    }

    void airPins(JsonArray object){
        std::vector<struct s_pin> pins;
        try{
            for(JsonVariant i : object){
                bool _continue = false;
                if(i.size() != 3) continue;
                for(struct s_pin p : varPins.pins) if(i[0].as<int>() == p.nrPin){ _continue = true; break; }
                for(struct s_pin p : pins) if(i[0].as<int>() == p.nrPin){ _continue = true; break; }
                if(!(i[0].as<int>() >= 0 && i[1].as<String>().length() > 0 && i[2].as<int>() >= 0) || _continue) continue;

                int idPin = varPins.pins.size() + pins.size();
                for(int l=0; l < varPins.tempPins.size(); l++)
                    if(varPins.tempPins[l].nrPinTemp == i[2].as<int>()){
                        varPins.tempPins[l].nrPinAir = i[0].as<int>();
                        varPins.tempPins[l].idPinAir = idPin;
                    }
                pins.push_back({i[0].as<int>(), (i.size() > 1 ? i[1].as<String>() : "Pin "+i[0].as<String>()), "air",  idPin});
            }

            varPins.pins.insert(varPins.pins.end(), pins.begin(), pins.end());
        }catch (...) {
            // error
        }
    }

    void heatPins(JsonArray object){
        std::vector<struct s_pin> pins;
        try{
            for(JsonVariant i : object){
                bool _continue = false;
                if(i.size() != 3) continue;
                for(struct s_pin p : varPins.pins) if(i[0].as<int>() == p.nrPin){ _continue = true; break; }
                for(struct s_pin p : pins) if(i[0].as<int>() == p.nrPin){ _continue = true; break; }
                if(!(i[0].as<int>() >= 0 && i[1].as<String>().length() > 0 && i[2].as<int>() >= 0) || _continue) continue;

                int idPin = varPins.pins.size() + pins.size();
                for(int l=0; l < varPins.tempPins.size(); l++)
                    if(varPins.tempPins[l].nrPinTemp == i[2].as<int>()){
                        varPins.tempPins[l].nrPinHeat = i[0].as<int>();
                        varPins.tempPins[l].idPinHeat = varPins.pins.size();
                    }
                pins.push_back({i[0].as<int>(), (i.size() > 1 ? i[1].as<String>() : "Pin "+i[0].as<String>()), "heat",  idPin});
            }

            varPins.pins.insert(varPins.pins.end(), pins.begin(), pins.end());
        }catch (...) {
            // error
        }
    }

    void tempData(JsonArray object){
        std::vector<s_tempData> tempData;
        try{
            for(JsonVariant i : object){
                if(i.size() != 2) continue;
                if(!(i[0].as<int>() >= 0 && i[1].as<int>() >= 0)) continue;

                DHT initTer(i[0].as<int>(), DHT11);
                initTer.begin();

                s_tempData data = {static_cast<int>(varPins.tempData.size() + tempData.size()), 0, 0, false, 0, false, 0.0, i[1].as<float>(), initTer};

                for(s_pin iPin : varPins.pins)
                    if(iPin.nrPin == i[0].as<int>() && iPin.actionPin == "temperature")
                        data.idPinTemp = iPin.idPin;
                for(s_tempPin iTempPin : varPins.tempPins)
                    if(iTempPin.nrPinTemp == i[0].as<int>()){
                        data.idPinHeat = iTempPin.idPinHeat;
                        data.idPinAir = iTempPin.idPinAir;
                    }

                tempData.push_back(data);
            }

            varPins.tempData.insert(varPins.tempData.end(), tempData.begin(), tempData.end());
        }catch (...) {
            // error
        }
    }

    void accounts(JsonArray object){
        std::vector<s_AccountSchema> accounts;
        try{
            for(JsonVariant i : object){
                if(i.size() < 2) continue;
                if(!i.containsKey("USERNAME") && !i.containsKey("PASSWORD")) continue;

                s_AccountSchema account;

                if(!i["USERNAME"].isNull() && !i["PASSWORD"].isNull()){
                    account.username = i["USERNAME"].as<String>();
                    account.password = i["PASSWORD"].as<String>();
                }
                bool _exist = false;
                for(s_AccountSchema iA : accounts) if(iA.username == account.username) _exist = true;
                if(_exist) continue;

                if(!i.containsKey("DATA_CREATE") || i["DATA_CREATE"].isNull()) account.dateCreate = ownTime::addDates({});
                else account.dateCreate = {i["DATA_CREATE"]["DAY"].as<int>(), i["DATA_CREATE"]["MONTH"].as<int>(), i["DATA_CREATE"]["YEAR"].as<int>(), i["DATA_CREATE"]["HOUR"].as<int>(), i["DATA_CREATE"]["MINUTE"].as<int>()};

                if(i.containsKey("EXP_MAINSESSION")) account.dayExp_mainSession = i["EXP_MAIN_SESSION"].as<int>();
                else account.dayExp_mainSession = 30;
                if(i.containsKey("EXP_MOBILESESSION")) account.dayExp_mobileSession = i["EXP_MOBILE_SESSION"].as<int>();
                else account.dayExp_mobileSession = 365;

                if(i.containsKey("MOBILE_TOKEN")){
                    if(!i["MOBILE_TOKEN"].isNull() || i["MOBILE_TOKEN"].containsKey("TOKEN") || !i["MOBILE_TOKEN"]["TOKEN"].isNull()){
                        account.mobileToken.token = i["MOBILE_TOKEN"]["TOKEN"].as<String>();
                        if(i["MOBILE_TOKEN"].containsKey("EXP_DATE")) account.mobileToken.expDate = {i["MOBILE_TOKEN"]["EXP_DATE"]["DAY"].as<int>(), i["MOBILE_TOKEN"]["EXP_DATE"]["MONTH"].as<int>(), i["MOBILE_TOKEN"]["EXP_DATE"]["YEAR"].as<int>(), i["MOBILE_TOKEN"]["EXP_DATE"]["HOUR"].as<int>(), i["MOBILE_TOKEN"]["EXP_DATE"]["MINUTE"].as<int>()};
                        else account.mobileToken.expDate = ownTime::addDates({i.containsKey("DAY_EXP_MOBILE_TOKEN") ? i["DAY_EXP_MOBILE_TOKEN"].as<int>() : 365});
                    }
                }else{
                    account.mobileToken = generateWebToken(account.dayExp_mobileSession, 0, 0, "mobile", {accounts, Accounts});
                }

                if(i.containsKey("SESSION")){
                    JsonArray sess = i["SESSION"];
                    if(sess.size() > 0){
                        for(JsonVariant ses : sess){
                            if(ses.size() == 0 || !ses.containsKey("TOKEN") || ses["TOKEN"].isNull()) continue;

                            s_session sessionACC;
                            sessionACC.token = ses["TOKEN"].as<String>();
                            if(ses.containsKey("EXP_DATE")) sessionACC.expDate = {ses["EXP_DATE"]["DAY"].as<int>(), ses["EXP_DATE"]["MONTH"].as<int>(), ses["EXP_DATE"]["YEAR"].as<int>(), ses["EXP_DATE"]["HOUR"].as<int>(), ses["EXP_DATE"]["MINUTE"].as<int>()};
                            else sessionACC.expDate = ownTime::addDates({i.containsKey("DAY_EXP_MAIN_SESSION") ? i["DAY_EXP_MAIN_SESSION"].as<int>() : 30});
                            account.session.push_back(sessionACC);
                        }
                    }
                }
                if(i.containsKey("RIGHTS") || !i["RIGHTS"].isNull()) account.isAdmin = i["RIGHTS"].as<String>() == "ADMIN";
                accounts.push_back(account);
            }
            Accounts = accounts;
        }catch (...) {
            // error
        }
    }

    void actionList(JsonObject object){
        try{
            std::vector<struct s_actionList> actionList;
            std::vector<struct s_actionWeek> actionWeek;

            for(JsonVariant obj : object["LISTS"].as<JsonArray>()){
                if(!obj.containsKey("NR_PIN") && obj.containsKey("ACTION") && !obj.containsKey("TIMNE") && !obj["TIME"].containsKey("DAY") && !obj["TIME"].containsKey("MONTH") && !obj["TIME"].containsKey("YEAR") && !obj["TIME"].containsKey("HOUR") && !obj["TIME"].containsKey("MINUTE")) continue;
                struct s_actionList al;
                al.time.day = obj["TIME"]["DAY"].as<int>();
                al.time.month = obj["TIME"]["MONTH"].as<int>();
                al.time.year = obj["TIME"]["YEAR"].as<int>();
                al.time.hours = obj["TIME"]["HOUR"].as<int>();
                al.time.minute = obj["TIME"]["MINUTE"].as<int>();
                al.time.second = obj["TIME"]["SECOND"].as<int>();

                al.action = obj["ACTION"].as<bool>();
                if(obj["NR_PIN"].isNull()) continue;
                al.nrPin = obj["NR_PIN"].as<int>();
                for(s_pin s : varPins.pins)
                    if(s.nrPin == al.nrPin)
                        al.idPin = s.idPin;
                if(validAction(al)) actionList.push_back(al);
                else _expireAction = true;
            }

            for(JsonVariant obj : object["WEEK_LISTS"].as<JsonArray>()){
                if(!obj.containsKey("NR_PIN") && obj.containsKey("ACTION") && !obj.containsKey("TIMNE") && !obj["TIME"].containsKey("DAY") && !obj["TIME"].containsKey("MONTH") && !obj["TIME"].containsKey("YEAR") && !obj["TIME"].containsKey("HOUR") && !obj["TIME"].containsKey("MINUTE") && !obj.containsKey("WEEK_DAY")) continue;
                struct s_actionWeek al;
                for(JsonVariant i : obj["WEEK_DAY"].as<JsonArray>())
                    al.arrayWeek.push_back(i.as<int>());
                al.time.hour = obj["TIME"]["HOUR"].as<int>();
                al.time.minute = obj["TIME"]["MINUTE"].as<int>();
                al.time.second = obj["TIME"]["SECOND"].as<int>();
                al.action = obj["ACTION"].as<bool>();
                if(obj["NR_PIN"].isNull()) continue;
                al.nrPin = obj["NR_PIN"].as<int>();
                for(s_pin s : varPins.pins)
                    if(s.nrPin == al.nrPin)
                        al.idPin = s.idPin;
                al.weekCount = obj["WEEK_COUNT"].as<int>();
                if(validAction(al)) actionWeek.push_back(al);
                else _expireAction = true;
            }

            varPins.actionList = actionList;
            varPins.actionWeek = actionWeek;
        }catch(...){
            return;
        }
    }
}

namespace save{
    StaticJsonDocument<JSON_OBJECT_SIZE(100)> wifi(){
        StaticJsonDocument<JSON_OBJECT_SIZE(100)> object;
        object["SSID"] = dataWifi.ssid;
        object["PASSWORD"] = dataWifi.password;
        object["LOCAL_IP"] = dataWifi.local_ip.toString();
        object["GATEWAY"] = dataWifi.gateway.toString();
        object["SUBNET"] = dataWifi.subnet.toString();
        return object;
    }

    StaticJsonDocument<JSON_OBJECT_SIZE(100)> ftp(){

        StaticJsonDocument<JSON_OBJECT_SIZE(100)> object;
        object["IP_ADDRESS"] = dataFtp.ip_address;
        object["PORT"] = dataFtp.port;
        object["USERNAME"] = dataFtp.username;
        object["PASSWORD"] = dataFtp.password;
        return object;
    }

    StaticJsonDocument<JSON_OBJECT_SIZE(100)> powerPins(){
        try{
            StaticJsonDocument<JSON_OBJECT_SIZE(100)> doc;
            JsonArray powerPins = doc.createNestedArray();
            for(s_pin pin : varPins.pins){
                if(pin.actionPin == "power"){
                    JsonArray pinArray = powerPins.createNestedArray();
                    pinArray.add(pin.nrPin);
                    pinArray.add(pin.namePin);
                    pinArray.add(false);
                }
            }
            return powerPins;
        }catch(...){
            return JsonArray();
        }
    }

    StaticJsonDocument<JSON_OBJECT_SIZE(100)> tempPins(){
        try{
            StaticJsonDocument<JSON_OBJECT_SIZE(100)> doc;
            JsonArray tempPins = doc.createNestedArray();
            for(s_pin pin : varPins.pins){
                if(pin.actionPin == "temperature") {
                    JsonArray pinArray = tempPins.createNestedArray();
                    pinArray.add(pin.nrPin);
                    pinArray.add(pin.namePin);
                }
            }
            return tempPins;
        }catch(...){
            return JsonArray();
        }
    }

    StaticJsonDocument<JSON_OBJECT_SIZE(100)> airPins(){
        try{
            StaticJsonDocument<JSON_OBJECT_SIZE(100)> doc;
            JsonArray airPins = doc.createNestedArray();
            for(s_pin pin : varPins.pins){
                if(pin.actionPin == "air"){
                    JsonArray pinArray = airPins.createNestedArray();
                    pinArray.add(pin.nrPin);
                    pinArray.add(pin.namePin);
                    for(s_tempPin tempPin : varPins.tempPins)
                        if(tempPin.nrPinAir == pin.nrPin)
                            pinArray.add(tempPin.nrPinTemp);
                }
            }
            return airPins;
        }catch(...){
            return JsonArray();
        }
    }

    StaticJsonDocument<JSON_OBJECT_SIZE(100)> heatPins(){
        try{
            StaticJsonDocument<JSON_OBJECT_SIZE(100)> doc;
            JsonArray heatPins = doc.createNestedArray();
            for(s_pin pin : varPins.pins){
                if(pin.actionPin == "heat"){
                    JsonArray pinArray = heatPins.createNestedArray();
                    pinArray.add(pin.nrPin);
                    pinArray.add(pin.namePin);
                    for(s_tempPin tempPin : varPins.tempPins)
                        if(tempPin.nrPinHeat == pin.nrPin)
                            pinArray.add(tempPin.nrPinTemp);
                }
            }
            return heatPins;
        }catch(...){
            return JsonArray();
        }
    }

    StaticJsonDocument<JSON_OBJECT_SIZE(100)> tempData(){
        try{
            StaticJsonDocument<JSON_OBJECT_SIZE(100)> doc;
            JsonArray tempData = doc.createNestedArray();
            for(s_tempData data : varPins.tempData){
                JsonArray tempDataArray = tempData.createNestedArray();
                for(s_tempPin tempPin : varPins.tempPins)
                    if(tempPin.idPinTemp == data.idPinTemp)
                        tempDataArray.add(tempPin.nrPinTemp);
                tempDataArray.add(data.primTemp);
            }
            return tempData;
        }catch(...){
            return JsonArray();
        }
    }

    StaticJsonDocument<JSON_OBJECT_SIZE(100)> accounts(){
        try{
            StaticJsonDocument<JSON_OBJECT_SIZE(100)> doc;
            JsonArray users = doc.createNestedArray();
            for(s_AccountSchema acc : Accounts){
                JsonObject user = users.createNestedObject();
                user["USERNAME"] = acc.username;
                user["PASSWORD"] = acc.password;
                user["DATA_CREATE"]["DAY"] = acc.dateCreate.day;
                user["DATA_CREATE"]["MONTH"] = acc.dateCreate.month;
                user["DATA_CREATE"]["YEAR"] = acc.dateCreate.year;
                user["DATA_CREATE"]["HOUR"] = acc.dateCreate.hours;
                user["DATA_CREATE"]["MINUTE"] = acc.dateCreate.minute;
                user["MOBILE_TOKEN"]["EXP_DATE"]["DAY"] = acc.mobileToken.expDate.day;
                user["MOBILE_TOKEN"]["EXP_DATE"]["MONTH"] = acc.mobileToken.expDate.month;
                user["MOBILE_TOKEN"]["EXP_DATE"]["YEAR"] = acc.mobileToken.expDate.year;
                user["MOBILE_TOKEN"]["EXP_DATE"]["HOUR"] = acc.mobileToken.expDate.hours;
                user["MOBILE_TOKEN"]["EXP_DATE"]["MINUTE"] = acc.mobileToken.expDate.minute;
                user["MOBILE_TOKEN"]["TOKEN"] = acc.mobileToken.token;
                JsonArray sessionArray = user.createNestedArray("SESSION");
                for(s_session session : acc.session){
                    JsonObject sessionObj = sessionArray.createNestedObject();
                    sessionObj["EXP_DATE"]["DAY"] = session.expDate.day;
                    sessionObj["EXP_DATE"]["MONTH"] = session.expDate.minute;
                    sessionObj["EXP_DATE"]["YEAR"] = session.expDate.year;
                    sessionObj["EXP_DATE"]["HOUR"] = session.expDate.hours;
                    sessionObj["EXP_DATE"]["MINUTE"] = session.expDate.minute;
                    sessionObj["TOKEN"] = session.token;
                }
                user["DAY_EXP_MAIN_SESSION"] = acc.dayExp_mainSession;
                user["DAY_EXP_MOBILE_TOKEN"] = acc.dayExp_mobileSession;
            }

            return users;
        }catch(...){
            return JsonArray();
        }
    }

    StaticJsonDocument<JSON_OBJECT_SIZE(100)> actionLists(){
        try{
            StaticJsonDocument<JSON_OBJECT_SIZE(100)> doc;
            JsonArray actionLists = doc.createNestedArray("LISTS");
            for(s_actionList act : varPins.actionList){
                JsonObject obj = actionLists.createNestedObject();
                JsonObject time = obj.createNestedObject("TIME");
                time["DAY"] = act.time.day;
                time["MONTH"] = act.time.month;
                time["YEAR"] = act.time.year;
                time["HOUR"] = act.time.hours;
                time["MINUTE"] = act.time.minute;
                obj["ACTION"] = act.action;
                obj["NR_PIN"] = act.nrPin;
            }

            JsonArray weekAction = doc.createNestedArray("WEEK_LISTS");
            for(s_actionWeek act : varPins.actionWeek){
                JsonObject obj = weekAction.createNestedObject();
                JsonArray weekDay = obj.createNestedArray("WEEK_DAY");
                for(int i : act.arrayWeek) weekDay.add(i);
                JsonObject timeObj = obj.createNestedObject("TIME");
                timeObj["HOUR"] = act.time.hour;
                timeObj["MINUTE"] = act.time.minute;

                obj["ACTION"] = act.action;
                obj["NR_PIN"] = act.nrPin;
                obj["WEEK_COUNT"] = act.weekCount;
            }

            return doc;
        }catch(...){
            return JsonArray();
        }
    }
}

class config
{
    private:

    bool _configLoaded = false;
    const String configFilePath = "/configFile.json";
    const String factoryConfigFilePath = "/factoryConf.json";
    StaticJsonDocument<JSON_OBJECT_SIZE(600)> configObject;

    bool firstRun(){
        if(configObject["FIRST_RUN"].as<bool>()){
            Serial.println("First run");
            configObject["FIRST_RUN"] = false;
            File configFile = SPIFFS.open(factoryConfigFilePath, "w");
            if(!configFile) return false;
            serializeJson(configObject, configFile);
            configFile.close();
            write_to_spiffs();
            return true;
        }
        return false;
    }

    bool recoverFactoryConfig(){
        File configFile = SPIFFS.open(factoryConfigFilePath, "r");
        if(!configFile){
            return false;
        }
        deserializeJson(configObject, configFile);
        configFile.close();
        return true;
    }

    bool load_from_spiffs(){
        File configFile = SPIFFS.open(configFilePath, "r");
        if(!configFile){
            Serial.println("Failed to open config file");
            rpLog.err("File to open config file from SPIFFS");
            return false;
        }

        DeserializationError error = deserializeJson(configObject, configFile);
        if(error){
            Serial.printf("Failed to read file, error: %s\n", error.c_str());
            rpLog.err("Failed to read config file from SPIFFS" + String(error.c_str()));
            return false;
        }
        configFile.close();
        // serializeJsonPretty(configObject, Serial);
        rpLog.log("Succesfully loaded config file from SPIFFS");
        if(firstRun()) rpLog.log("Succesfully loaded factory config to SPIFFS");
        if(!configObject["USE_SD_CARD"].as<bool>()) SD_CARD_AVILABLE = false;
        else initSD();
        return true;
    }

    bool load_from_sd(){
        if(!SD_CARD_AVILABLE) return false;
        File configFile = SD.open(configFilePath, "r");
        if(!configFile){
            Serial.println("Failed to open config file");
            rpLog.err("File to open config file from SD");
            return false;
        }

        DeserializationError error = deserializeJson(configObject, configFile);
        if(error){
            Serial.printf("Failed to read file, error: %s\n", error.c_str());
            rpLog.err("Failed to read config file from SD" + String(error.c_str()));
            return false;
        }
        configFile.close();
        rpLog.log("Succesfully loaded config file from SD");
        if(firstRun()) rpLog.log("Succesfully loaded factory config to SPIFFS");
        return true;
    }

    bool write_to_spiffs(){
        File configFile = SPIFFS.open(configFilePath, "w");
        if(!configFile){
            Serial.println("Failed to open config file");
            rpLog.err("File to open config file from SPIFFS");
            return false;
        }

        serializeJson(configObject, configFile);
        configFile.close();
        rpLog.log("Config file save to SPIFFS");
        return true;
    }

    bool write_to_sd(){
        if(!SD_CARD_AVILABLE) return false;
        File configFile = SD.open(configFilePath, "w");
        if(!configFile){
            Serial.println("Failed to open config file");
            rpLog.err("File to open config file from SD");
            return false;
        }

        serializeJson(configObject, configFile);
        configFile.close();
        rpLog.log("Config file save to SD");
        return true;
    }

    public:

    config(){};
    ~config(){};

    String spiffsSize(){
        int usedBytes = SPIFFS.usedBytes();
        int totalBytes = SPIFFS.totalBytes();
        int result = usedBytes * 100 / totalBytes;
        return String("SPIFFS used: ") + String(result) + String("%");
    }

    bool isLoaded(){ return _configLoaded; }

    bool factoryResetConfig(){
        if(!recoverFactoryConfig()){
            rpLog.err("Failed to recover factory config");
            Serial.println("Failed to recover factory config");
            return false;
        }
        if(!write_to_spiffs()){
            rpLog.err("Failed to write factory config to SPIFFS");
            return false;
        }
        if(!write_to_sd()){
            rpLog.err("Failed to write factory config to SD");
            return false;
        }
        rpLog.log("Succesfully recovered factory config");
        return true;
    }

    void load_configuration(){
        if(load_from_spiffs()){
            Serial.println("Config file loaded from SPIFFS");
            rpLog.log("Config file loaded from SPIFFS");
        }else if(load_from_sd()){
            Serial.println("Config file loaded from SD card");
            rpLog.log("Config file loaded from SD card");
        }else{
            Serial.println("Config file not found");
            rpLog.err("Config file not found");
            rpLog.big_error();
            return;
        }

        load::wifi(configObject["WIFI"]);
        load::ftp(configObject["FTP"]);
        load::accounts(configObject["ACCOUNTS"]);
        load::powerPins(configObject["PINS"]["POWER_PINS"]);
        load::tempPins(configObject["PINS"]["TEMP_PINS"]);
        load::airPins(configObject["PINS"]["AIR_PINS"]);
        load::heatPins(configObject["PINS"]["HEAT_PINS"]);
        load::tempData(configObject["PINS"]["TEMP_DATA"]);
        load::actionList(configObject["ACTIONS"]);
        // make_log();

        if(_expireAction) save_action();
        _configLoaded = true;
        rpLog.log("Configuration loaded");
    }

    void save_config(){
        if(!write_to_spiffs()){
            Serial.println("Failed save config file to SPIFFS");
            rpLog.log("Failed save config file to SPIFFS");
        }
        else if(!write_to_sd()){
            Serial.println("Failed save config file to SD");
            rpLog.log("Failed save config file to SD");
        }
        else{
            Serial.println("Config file not saved");
            rpLog.log("Config file not saved");
        }
    }

    void save_complete_config(){
        save::wifi();
        save::ftp();
        save::accounts();
        save::powerPins();
        save::tempPins();
        save::airPins();
        save::heatPins();
        save::tempData();

        save_config();
    }

    void save_wifi(){
        configObject["WIFI"] = save::wifi();
        save_config();
    }

    void save_ftp(){
        configObject["FTP_SERVER"] = save::ftp();
        save_config();
    }

    void save_accounts(){
        configObject["ACCOUNTS"] = save::accounts();
        save_config();
    }

    void save_powerPins(){
        configObject["PINS"]["POWER_PINS"] = save::powerPins();
        save_config();
    }

    void save_tempPins(){
        configObject["PINS"]["TEMP_PINS"] = save::tempPins();
        save_config();
    }

    void save_airPins(){
        configObject["PINS"]["AIR_PINS"] = save::airPins();
        save_config();
    }

    void save_heatPins(){
        configObject["PINS"]["HEAT_PINS"] = save::heatPins();
        save_config();
    }

    void save_tempData(){
        configObject["PINS"]["TEMP_DATA"] = save::tempData();
        save_config();
    }

    void save_action(){
        configObject["ACTIONS"] = save::actionLists();
        save_config();
    }

    void save_allPins(){
        save::powerPins();
        save::tempPins();
        save::airPins();
        save::heatPins();
        save_config();
    }

} configFile;

#endif //CONFFILE_H