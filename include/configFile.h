#ifndef CONFFILE_H
#define CONFFILE_H

#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <function.h>

IPAddress splitIPaddress(char in[16]){
    String oct[4];
    int dots = 0;
    for(int i=0; i < 16; i++)
        if(dots > 3) break;
        else if(in[i] >= '0' && in[i] <= '9'){
            if(dots == 0) oct[0] += in[i];
            else if(dots == 1) oct[1] += in[i];
            else if(dots == 2) oct[2] += in[i];
            else if(dots == 3) oct[3] += in[i];
        }
        else if(in[i] == '.') dots++;
    IPAddress ip(oct[0].toInt(), oct[1].toInt(), oct[2].toInt(), oct[3].toInt());
    return ip;
}

void printAllData(){
        //Print all data with dataWifi struct
        Serial.print("Wifi config: { ");
        Serial.print("SSID: ");
        Serial.print(dataWifi.ssid);
        Serial.print("; PASS: ");
        Serial.print(dataWifi.password);
        Serial.print("; IP: ");
        Serial.print(dataWifi.local_ip.toString());
        Serial.print("; GATEWAY: ");
        Serial.print(dataWifi.gateway.toString());
        Serial.print("; SUBNET: ");
        Serial.print(dataWifi.subnet.toString());
        Serial.println(" }");


        //Print FTP_SERVER config with structure
        Serial.print("FTP_SERVER config: { ");
        Serial.print("IP_ADDRESS: ");
        Serial.print(dataFtp.ip_address);
        Serial.print("; PORT: ");
        Serial.print(dataFtp.port);
        Serial.print("; USERNAME: ");
        Serial.print(dataFtp.username);
        Serial.print("; PASSWORD: ");
        Serial.print(dataFtp.password);
        Serial.println(" }");

        //Print all data with account struct
        Serial.println("Account config: { ");
        for(s_AccountSchema account : Accounts){
            Serial.print("      [ USERNAME: ");
            Serial.print(account.username);
            Serial.print("; PASSWORD: ");
            Serial.print(account.password);
            Serial.println("; ]");
        }
        Serial.println(" }");

        //Print all data with varPins.pins structure
        Serial.println("PINS config: {");
        for(s_pin pin : varPins.pins){
            Serial.print("      { PIN_NR: ");
            Serial.print(pin.nrPin);
            Serial.print("; PIN_ACTION: ");
            Serial.print(pin.actionPin);
            Serial.print("; PIN_NAME: ");
            Serial.print(pin.namePin);
            Serial.print("; PIN_ID: ");
            Serial.print(pin.idPin);
            Serial.println("; }");
        }
        Serial.println("}");

        //Print all data with varPins.powerPins structure
        Serial.println("POWER_PINS config: {");
        for(s_powerPin powerPin : varPins.powerPins){
            Serial.print("      { PIN_ID: ");
            Serial.print(powerPin.idPin);
            Serial.print("; PIN_STATUS: ");
            Serial.print(powerPin.power);
            Serial.print("; POWER_ID: ");
            Serial.print(powerPin.idPower);
            Serial.println("; }");
        }
        Serial.println("}");

        //Print all data with varPins.tempPins structure
        Serial.println("TEMP_PINS config: {");
        for(s_tempPin tempPin : varPins.tempPins){
            Serial.print("      { TEMP_NR: ");
            Serial.print(tempPin.nrPinTemp);
            Serial.print("; TEMP_ID: ");
            Serial.print(tempPin.idPinTemp);
            Serial.print("; AIR_NR: ");
            Serial.print(tempPin.nrPinAir);
            Serial.print("; AIR_ID: ");
            Serial.print(tempPin.idPinAir);
            Serial.print("; HEAT_NR: ");
            Serial.print(tempPin.nrPinHeat);
            Serial.print("; HEAT_ID: ");
            Serial.print(tempPin.idPinHeat);
            Serial.print("; ID_TEMPERATURE: ");
            Serial.print(tempPin.idTemp);
            Serial.println("; }");
        }
        Serial.println("}");

        //Print all data with varPins.tempData structure
        Serial.println("TEMP_DATA config: {");
        for(s_tempData tempData : varPins.tempData){
            Serial.print("      { TEMPERATURE_DATA_ID: ");
            Serial.print(tempData.idTemp);
            Serial.print("; TEMP_ID: ");
            Serial.print(tempData.idPinTemp);
            Serial.print("; HEAT_ID: ");
            Serial.print(tempData.idPinHeat);
            Serial.print("; HEAT_POWER: ");
            Serial.print(tempData.powerHeat);
            Serial.print("; AIR_ID: ");
            Serial.print(tempData.idPinAir);
            Serial.print("; AIR_POWER: ");
            Serial.print(tempData.powerAir);
            Serial.print("; PRIM_TEMP: ");
            Serial.print(tempData.primTemp);
            Serial.print("; CURL_TEMP: ");
            Serial.print(tempData.curlTemp);
            Serial.println("; }");
        }
        Serial.println("}");

        //Print all data with varPins.actionList structure
        Serial.println("ACTION_LIST: {");
        for(s_actionList actionList : varPins.actionList){
            Serial.print("      { TIME: ");
            Serial.print(actionList.time.day);
            Serial.print(".");
            Serial.print(actionList.time.month);
            Serial.print(".");
            Serial.print(actionList.time.year);
            Serial.print(" ");
            Serial.print(actionList.time.hours);
            Serial.print(":");
            Serial.print(actionList.time.minute);
            Serial.print("; ACTION: ");
            Serial.print(actionList.action);
            Serial.print("; ID_PIN: ");
            Serial.print(actionList.idPin);
            Serial.print("; NR_PIN: ");
            Serial.print(actionList.nrPin);
            Serial.println("; }");
        }
        Serial.println("}");

        //Print all data with varPins.actionWeek structure
        Serial.println("ACTION_WEEK: {");
        for(s_actionWeek actionWeek : varPins.actionWeek){
            Serial.print("      { ARRAY_WEEK: [");
            for(int i : actionWeek.arrayWeek){
                Serial.print(i);
                Serial.print(", ");
            }
            Serial.print("]; TIME: ");
            Serial.print(actionWeek.time.hour);
            Serial.print(":");
            Serial.print(actionWeek.time.minute);
            Serial.print("; ACTION: ");
            Serial.print(actionWeek.action);
            Serial.print("; ID_PIN: ");
            Serial.print(actionWeek.idPin);
            Serial.print("; NR_PIN: ");
            Serial.print(actionWeek.nrPin);
            Serial.print("; WEEK_COUNT: ");
            Serial.print(actionWeek.weekCount);
            Serial.println("; }");
        }
        Serial.println("}\n\n\n");
}

class config
{
    private:
    String configFilePath = "/configFile.json";
    File configFile;
    StaticJsonDocument<JSON_OBJECT_SIZE(200)> configObject;
    public:
    config(){};
    ~config(){};
    bool begin(String filePath="/configFile.json"){
        try{
            configFilePath = filePath;
            configFile = SPIFFS.open(configFilePath);

            if(!configFile){
                Serial.println("Error with open config file");
                return false;
            }

            String configFileContent = "";
            while(configFile.available()) configFileContent += char(configFile.read());
            deserializeJson(configObject, configFileContent);

            // Serial.print("Object size: ");
            // Serial.println(measureJson(configObject));

            // Serial.println("Config file JSON Fresh: ");
            // serializeJsonPretty(configObject, Serial);
            // Serial.println("\n\n");
        }catch(...){
            Serial.println("Error with read config file");
            return false;
        }
        try{
            configFile.close();
        }catch(...){
            return false;
        }
        return true;
    };
    bool loadConfig(){
        // Serial.println("Old config:");
        // printAllData();
        // Serial.println("\n\n");
        try{
            if(configObject.containsKey("WIFI")){
                if(configObject["WIFI"].containsKey("SSID"))
                    dataWifi.ssid = configObject["WIFI"]["SSID"].as<const char*>();
                if(configObject["WIFI"].containsKey("PASSWORD"))
                    dataWifi.password = configObject["WIFI"]["PASSWORD"].as<const char*>();
                if(configObject["WIFI"].containsKey("LOCAL_IP"))
                    if(sizeof(configObject["WIFI"]["LOCAL_IP"]) > 0){
                        char ipChar[16];
                        configObject["WIFI"]["LOCAL_IP"].as<String>().toCharArray(ipChar, 16);
                        dataWifi.local_ip = splitIPaddress(ipChar);
                    }
                if(configObject["WIFI"].containsKey("GATEWAY"))
                    if(sizeof(configObject["WIFI"]["GATEWAY"]) > 4){
                        char ipChar[16];
                        configObject["WIFI"]["GATEWAY"].as<String>().toCharArray(ipChar, 16);
                        dataWifi.gateway = splitIPaddress(ipChar);
                    }
                if(configObject["WIFI"].containsKey("SUBNET"))
                    if(sizeof(configObject["WIFI"]["SUBNET"]) > 4){
                        char ipChar[16];
                        configObject["WIFI"]["SUBNET"].as<String>().toCharArray(ipChar, 16);
                        dataWifi.subnet = splitIPaddress(ipChar);
                    }
                if(configObject["WIFI"].containsKey("HOSTNAME"))
                    if(sizeof(configObject["WIFI"]["HOSTNAME"]) > 0){
                        dataWifi.hostname = configObject["WIFI"]["HOSTNAME"].as<String>();
                    }

                initWiFi();
            }
            if(configObject.containsKey("FTP_SERVER")){
                if(configObject["FTP_SERVER"].containsKey("IP_ADDRESS"))
                    dataFtp.ip_address = ToChar(configObject["FTP_SERVER"]["IP_ADDRESS"].as<String>());
                if(configObject["FTP_SERVER"].containsKey("PORT"))
                    dataFtp.port = configObject["FTP_SERVER"]["PORT"].as<uint16_t>();
                if(configObject["FTP_SERVER"].containsKey("USERNAME"))
                    dataFtp.username = ToChar(configObject["FTP_SERVER"]["USERNAME"].as<String>());
                if(configObject["FTP_SERVER"].containsKey("PASSWORD"))
                    dataFtp.password = ToChar(configObject["FTP_SERVER"]["PASSWORD"].as<String>());
            }

            if(configObject.containsKey("PINS")){
                if(configObject["PINS"].containsKey("POWER_PINS")){
                    JsonArray power_pins = configObject["PINS"]["POWER_PINS"];
                    for(JsonVariant i : power_pins){
                        if(i.size() != 3) continue;
                        if(!(i[0].as<int>() >= 0 && i[1].as<String>().length() > 0 && (i[2].as<int>() == 0 || i[2].as<int>() == 1))) continue;
                        s_pin pin = {i[0].as<int>(), i[1].as<String>(), "power",  static_cast<int>(varPins.pins.size())};
                        s_powerPin powerPin = {static_cast<int>(varPins.pins.size()), i[2].as<int>(), static_cast<int>(varPins.powerPins.size())};
                        varPins.pins.push_back(pin);
                        varPins.powerPins.push_back(powerPin);
                    }
                }

                if(configObject["PINS"].containsKey("TEMP_PINS")){
                    JsonArray temp_pins = configObject["PINS"]["TEMP_PINS"];
                    for(JsonVariant i : temp_pins){
                        if(i.size() != 2) continue;
                        if(!(i[0].as<int>() >= 0 && i[1].as<String>().length() > 0)) continue;
                        s_pin pin = {i[0].as<int>(), i[1].as<String>(), "temperature", static_cast<int>(varPins.pins.size())};
                        s_tempPin tempPin = {i[0].as<int>(), static_cast<int>(varPins.pins.size())};
                        varPins.pins.push_back(pin);
                        varPins.tempPins.push_back(tempPin);
                    }

                    if(configObject["PINS"].containsKey("AIR_PINS")){
                        JsonArray air_pins = configObject["PINS"]["AIR_PINS"];
                        for(JsonVariant i : air_pins){
                            if(i.size() != 3) continue;
                            if(!(i[0].as<int>() >= 0 && i[1].as<String>().length() > 0 && i[2].as<int>() >= 0)) continue;
                            s_pin pin = {i[0].as<int>(), i[1].as<String>(), "air", static_cast<int>(varPins.pins.size())};
                            for(int l=0; l < varPins.tempPins.size(); l++)
                                if(varPins.tempPins[l].nrPinTemp == i[2].as<int>()){
                                    varPins.tempPins[l].nrPinAir = i[0].as<int>();
                                    varPins.tempPins[l].idPinAir = varPins.pins.size();
                                }
                            varPins.pins.push_back(pin);
                        }
                    }

                    if(configObject["PINS"].containsKey("HEAT_PINS")){
                        JsonArray heat_pins = configObject["PINS"]["HEAT_PINS"];
                        for(JsonVariant i : heat_pins){
                            if(i.size() != 3) continue;
                            if(!(i[0].as<int>() >= 0 && i[1].as<String>().length() > 0 && i[2].as<int>() >= 0)) continue;
                            s_pin pin = {i[0].as<int>(), i[1].as<String>(), "heat", static_cast<int>(varPins.pins.size())};
                            for(int l=0; l < varPins.tempPins.size(); l++)
                                if(varPins.tempPins[l].nrPinTemp == i[2].as<int>()){
                                    varPins.tempPins[l].nrPinHeat = i[0].as<int>();
                                    varPins.tempPins[l].idPinHeat = varPins.pins.size();
                                }
                            varPins.pins.push_back(pin);
                        }
                    }

                    if(configObject["PINS"].containsKey("TEMP_DATA")){
                        JsonArray tempData = configObject["PINS"]["TEMP_DATA"];
                        for(JsonVariant i : tempData){
                            if(i.size() != 2) continue;
                            if(!(i[0].as<int>() >= 0 && i[1].as<int>() >= 0)) continue;

                            DHT initTer(i[0].as<int>(), DHT11);
                            s_tempData data = {static_cast<int>(varPins.tempData.size()), 0, 0, false, 0, false, 0.0, 0.0, initTer};

                            for(s_pin iPin : varPins.pins)
                                if(iPin.nrPin == i[0].as<int>() && iPin.actionPin == "temperature")
                                    data.idPinTemp = iPin.idPin;
                            for(s_tempPin iTempPin : varPins.tempPins)
                                if(iTempPin.nrPinTemp == i[0].as<int>()){
                                    data.idPinHeat = iTempPin.idPinHeat;
                                    data.idPinAir = iTempPin.idPinAir;
                                }
                            data.primTemp = i[1].as<int>();

                            varPins.tempData.push_back(data);
                        }
                    }
                }
            }
            if(configObject.containsKey("ACCOUNTS")){
                JsonArray accounts = configObject["ACCOUNTS"];
                for(JsonVariant i : accounts){
                    if(i.size() < 2) continue;
                    if(i.containsKey("USERNAME") && i.containsKey("PASSWORD")){
                        s_AccountSchema account;

                        if(!i["USERNAME"].isNull()) account.username = i["USERNAME"].as<String>();
                        else continue;
                        if(!i["PASSWORD"].isNull()) account.password = i["PASSWORD"].as<String>();
                        else continue;

                        bool exists = false;
                        for(s_AccountSchema iAccount : Accounts)
                            if(iAccount.username == account.username) exists = true;

                        if(exists) continue;

                        if(i.containsKey("DATA_CREATE")){
                            if(i["DATA_CREATE"].isNull())
                                account.dateCreate = ownTime::addDates({});
                            else{
                                account.dateCreate.day = i["DATA_CREATE"]["DAY"].as<int>();
                                account.dateCreate.month = i["DATA_CREATE"]["MONTH"].as<int>();
                                account.dateCreate.year = i["DATA_CREATE"]["YEAR"].as<int>();
                                account.dateCreate.hours = i["DATA_CREATE"]["HOUR"].as<int>();
                                account.dateCreate.minute = i["DATA_CREATE"]["MINUTE"].as<int>();
                            }
                        }else account.dateCreate = ownTime::addDates({});
                        if(i.containsKey("MOBILE_TOKEN")){
                            if(i["MOBILE_TOKEN"].isNull() || !i["MOBILE_TOKEN"].containsKey("TOKEN") || i["MOBILE_TOKEN"]["TOKEN"].isNull()) continue;
                            if(i["MOBILE_TOKEN"].containsKey("EXP_DATE")){
                                account.mobileToken.expDate.day = i["MOBILE_TOKEN"]["EXP_DATE"]["DAY"].as<int>();
                                account.mobileToken.expDate.month = i["MOBILE_TOKEN"]["EXP_DATE"]["MONTH"].as<int>();
                                account.mobileToken.expDate.year = i["MOBILE_TOKEN"]["EXP_DATE"]["YEAR"].as<int>();
                                account.mobileToken.expDate.hours = i["MOBILE_TOKEN"]["EXP_DATE"]["HOUR"].as<int>();
                                account.mobileToken.expDate.minute = i["MOBILE_TOKEN"]["EXP_DATE"]["MINUTE"].as<int>();
                            }else account.mobileToken.expDate = ownTime::addDates({i.containsKey("DAY_EXP_MOBILE_TOKEN") ? i["DAY_EXP_MOBILE_TOKEN"].as<int>() : 365});
                            account.mobileToken.token = i["MOBILE_TOKEN"]["TOKEN"].as<String>();
                        }
                        if(i.containsKey("SESSION")){
                            JsonArray sess = i["SESSION"];
                            if(sess.size() > 0){
                                for(JsonVariant ses : sess){
                                    if(!ses.size() == 0 || !ses.containsKey("TOKEN") || ses["TOKEN"].isNull()) continue;
                                    s_session sessionACC;
                                    if(ses.containsKey("EXP_DATE")){
                                        sessionACC.expDate.day = ses["EXP_DATE"]["DAY"].as<int>();
                                        sessionACC.expDate.month = ses["EXP_DATE"]["MONTH"].as<int>();
                                        sessionACC.expDate.year = ses["EXP_DATE"]["YEAR"].as<int>();
                                        sessionACC.expDate.hours = ses["EXP_DATE"]["HOUR"].as<int>();
                                        sessionACC.expDate.minute = ses["EXP_DATE"]["MINUTE"].as<int>();
                                    }else sessionACC.expDate = ownTime::addDates({i.containsKey("DAY_EXP_MAIN_SESSION") ? i["DAY_EXP_MAIN_SESSION"].as<int>() : 30});
                                    sessionACC.token = ses["TOKEN"].as<String>();
                                }
                            }
                        }
                        if(i.containsKey("RIGHTS")){
                            if(i["RIGHTS"].isNull()) continue;
                            if(i["RIGHTS"].as<String>() == "ADMIN") account.isAdmin = true;
                            else account.isAdmin = false;
                        }
                        Accounts.push_back(account);
                    }
                }
            }
        }catch(...){
            Serial.println("Error reading config file");
            return false;
        }

        // Serial.println("Config loaded: ");
        // printAllData();
        // Serial.println("\n\n");

        return true;
    }
    bool saveWifi_Config(){
        try{
            configObject["WIFI"]["SSID"] = dataWifi.ssid;
            configObject["WIFI"]["PASSWORD"] = dataWifi.password;
            configObject["WIFI"]["LOCAL_IP"] = dataWifi.local_ip.toString();
            configObject["WIFI"]["GATEWAY"] = dataWifi.gateway.toString();
            configObject["WIFI"]["SUBNET"] = dataWifi.subnet.toString();
        }catch(...){
            Serial.println("Error saving \"WIFI\" config");
            return false;
        }
        return true;
    }
    bool saveFTP_Config(){
        try{
        configObject["FTP_SERVER"]["IP_ADDRESS"] = dataFtp.ip_address;
        configObject["FTP_SERVER"]["PORT"] = dataFtp.port;
        configObject["FTP_SERVER"]["USERNAME"] = dataFtp.username;
        configObject["FTP_SERVER"]["PASSWORD"] = dataFtp.password;
        }catch(...){
            Serial.println("Error saving \"FTP\" config");
            return false;
        }
        return true;
    }
    bool savePin_Config(){
        try{
            JsonArray powerPins = configObject["PINS"].createNestedArray("POWER_PINS");
            JsonArray tempPins = configObject["PINS"].createNestedArray("TEMP_PINS");
            JsonArray airPins = configObject["PINS"].createNestedArray("AIR_PINS");
            JsonArray heatPins = configObject["PINS"].createNestedArray("HEAT_PINS");
            for(s_pin pin : varPins.pins){
                if(pin.actionPin == "power"){
                    JsonArray pinArray = powerPins.createNestedArray();
                    pinArray.add(pin.nrPin);
                    pinArray.add(pin.namePin);
                    pinArray.add(false);
                }
                else if(pin.actionPin == "temperature") {
                    JsonArray pinArray = tempPins.createNestedArray();
                    pinArray.add(pin.nrPin);
                    pinArray.add(pin.namePin);
                }
                else if(pin.actionPin == "air"){
                    JsonArray pinArray = airPins.createNestedArray();
                    pinArray.add(pin.nrPin);
                    pinArray.add(pin.namePin);
                    for(s_tempPin tempPin : varPins.tempPins)
                        if(tempPin.nrPinAir == pin.nrPin)
                            pinArray.add(tempPin.nrPinTemp);
                }
                else if(pin.actionPin == "heat"){
                    JsonArray pinArray = heatPins.createNestedArray();
                    pinArray.add(pin.nrPin);
                    pinArray.add(pin.namePin);
                    for(s_tempPin tempPin : varPins.tempPins)
                        if(tempPin.nrPinHeat == pin.nrPin)
                            pinArray.add(tempPin.nrPinTemp);
                }
            }

            JsonArray tempData = configObject["PINS"].createNestedArray("TEMP_DATA");
            for(s_tempData data : varPins.tempData){
                JsonArray tempDataArray = tempData.createNestedArray();
                for(s_tempPin tempPin : varPins.tempPins)
                    if(tempPin.idPinTemp == data.idPinTemp)
                        tempDataArray.add(tempPin.nrPinTemp);
                tempDataArray.add(data.primTemp);
            }
        }catch(...){
            Serial.println("Error saving \"PIN\" config");
            return false;
        }
        return true;
    }
    bool saveAccount_Config(){
        try{
            JsonArray users = configObject.createNestedArray("ACCOUNTS");
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
        }catch(...){
            Serial.println("Error saving \"ACCOUNT\" config");
            return false;
        }

        return true;
    }
    bool saveConfigFile(){
        try{
            configFile = SPIFFS.open("/test.json", "w");
            if(!configFile){
                Serial.println("Failed to open config file for writing");
                return false;
            }
            // serializeJson(configObject, configFile);
            Serial.println("Save is done");
            configFile.close();
        }catch(...){
            Serial.println("Error saving config file");
            return false;
        }
        return true;
    }

    bool saveConfig(){
        saveWifi_Config();
        saveFTP_Config();
        savePin_Config();
        saveAccount_Config();
        saveConfigFile();

        // int usedBytes = SPIFFS.usedBytes();
        // int totalBytes = SPIFFS.totalBytes();
        // int result = usedBytes * 100 / totalBytes;
        // Serial.print("SPIFFS used: ");
        // Serial.print(result);
        // Serial.println("%");


        // Serial.println("Config saved: ");
        // serializeJsonPretty(configObject, Serial);
        return true;
    }

    bool validWifi_Config(){
        if(dataWifi.ssid.length() == 0)
            return false;
        if((dataWifi.local_ip[0] < 1 || dataWifi.local_ip[0] > 255) || (dataWifi.local_ip[1] < 1 || dataWifi.local_ip[1] > 255) || (dataWifi.local_ip[2] < 1 || dataWifi.local_ip[2] > 255) || (dataWifi.local_ip[3] < 1 || dataWifi.local_ip[3] > 255))
            return false;
        if((dataWifi.gateway[0] < 1 || dataWifi.gateway[0] > 255) || (dataWifi.gateway[1] < 1 || dataWifi.gateway[1] > 255) || (dataWifi.gateway[2] < 1 || dataWifi.gateway[2] > 255) || (dataWifi.gateway[3] < 1 || dataWifi.gateway[3] > 255))
            return false;
        if((dataWifi.subnet[0] < 1 || dataWifi.subnet[0] > 255) || (dataWifi.subnet[1] < 1 || dataWifi.subnet[1] > 255) || (dataWifi.subnet[2] < 1 || dataWifi.subnet[2] > 255) || (dataWifi.subnet[3] < 1 || dataWifi.subnet[3] > 255))
            return false;
        return true;
    }
    bool validFTP_Config(){
        int ip[4];
        ip[0] = String(dataFtp.ip_address).substring(0, String(dataFtp.ip_address).indexOf(".")).toInt();
        String(dataFtp.ip_address).remove(0, String(dataFtp.ip_address).indexOf(".") + 1);
        ip[1] = String(dataFtp.ip_address).substring(0, String(dataFtp.ip_address).indexOf(".")).toInt();
        String(dataFtp.ip_address).remove(0, String(dataFtp.ip_address).indexOf(".") + 1);
        ip[2] = String(dataFtp.ip_address).substring(0, String(dataFtp.ip_address).indexOf(".")).toInt();
        String(dataFtp.ip_address).remove(0, String(dataFtp.ip_address).indexOf(".") + 1);
        ip[3] = String(dataFtp.ip_address).substring(0, String(dataFtp.ip_address).indexOf(".")).toInt();
        if(ip[0] < 1 || ip[0] > 255 || ip[1] < 1 || ip[1] > 255 || ip[2] < 1 || ip[2] > 255 || ip[3] < 1 || ip[3] > 255)
            return false;
        if(String(dataFtp.username).length() == 0)
            return false;
        if(dataFtp.port > 1 || dataFtp.port < 65535)
            dataFtp.port = 21;
        return true;
    }
    bool validPin_Config(){

        return true;
    }
    bool validAccount_Config(){
        try{
            for(s_AccountSchema acc : Accounts){
                // if(acc.username.length() == 0) Accounts.erase(std::remove(Accounts.begin(), Accounts.end(), acc), Accounts.end());
                if(acc.dateCreate.day == 0 || acc.dateCreate.month == 0 || acc.dateCreate.year == 0 || acc.dateCreate.hours == 0 || acc.dateCreate.minute == 0)
                    acc.dateCreate = TimeS.Today();
                if(acc.dayExp_mainSession == 0) acc.dayExp_mainSession = 30;
                if(acc.dayExp_mobileSession == 0) acc.dayExp_mobileSession = 365;
            }
        }catch(...){
            Serial.println("Wystąpił błąd podczas sprawdzania kont");
            return false;
        }
        return true;
    }
    bool validData(){
        if(!validWifi_Config()){
            Serial.println("Wifi config is not valid");
            return false;
        }
        if(!validFTP_Config()){
            Serial.println("FTP config is not valid");
            return false;
        }
        if(!validPin_Config()){
            Serial.println("PIN config is not valid");
            return false;
        }
        if(!validAccount_Config()){
            Serial.println("ACCOUNT config is not valid");
            return false;
        }
        return true;
    }
};

#endif //CONFFILE_H