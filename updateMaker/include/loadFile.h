#ifndef _LOADFILE_H_
#define _LOADFILE_H_

#include <ArduinoJson.h>
#include <SPIFFS.h>

bool LOAD_FILE_ERROR = false;

struct s_updateConfig{
    String version;
    bool spiffs;
    bool software;
    String url_soft;
    String url_spiffs;
    String host;
};

// Struktura przechowujaca dane do laczenia sie z siecia wifi
struct s_wifi
{
    String ssid; //Nazwa sieci wifi
    String password; //Haslo do sieci wifi
    IPAddress local_ip; //Adres IP lokalny statyczny
    IPAddress gateway; //Adres IP bramy domyslnej
    IPAddress subnet; //Adres IP maski podsieci
    String hostname; //Nazwa hosta
};

class loadFileOwn{
    private:
        String load(String fileName){
            if(!SPIFFS.exists(fileName)){
                Serial.println("File " + fileName + " not found");
                LOAD_FILE_ERROR = true;
                return "";
            }
            Serial.println("File " + fileName + " found");
            File file = SPIFFS.open(fileName, "r");
            if(!file){
                Serial.println("Failed to open file for reading");
                return "";
            }
            String content = file.readString();
            file.close();
            return content;
        }
    public:
        struct s_wifi loadWifi(){
            String Strjson = load("/configFile.json");
            StaticJsonDocument<JSON_OBJECT_SIZE(200)> json;
            DeserializationError error = deserializeJson(json, Strjson);
            if(error){
                Serial.println(F("Failed to parse json file"));
                return {};
            }

            struct s_wifi dataWifi;
            if(json["WIFI"].containsKey("SSID")) dataWifi.ssid = json["WIFI"]["SSID"].as<String>();
            if(json["WIFI"].containsKey("PASSWORD")) dataWifi.password = json["WIFI"]["PASSWORD"].as<String>();
            if(json["WIFI"].containsKey("LOCAL_IP")) dataWifi.local_ip.fromString(json["WIFI"]["LOCAL_IP"].as<String>());
            if(json["WIFI"].containsKey("GATEWAY")) dataWifi.gateway.fromString(json["WIFI"]["GATEWAY"].as<String>());
            if(json["WIFI"].containsKey("SUBNET")) dataWifi.subnet.fromString(json["WIFI"]["SUBNET"].as<String>());
            if(json["WIFI"].containsKey("HOSTNAME")) dataWifi.hostname = json["WIFI"]["HOSTNAME"].as<String>();

            return dataWifi;
        }

        s_updateConfig loadUpdateConfig(){
            String Strjson = load("/updateConfig.json");
            StaticJsonDocument<JSON_OBJECT_SIZE(200)> json;
            DeserializationError error = deserializeJson(json, Strjson);
            if(error){
                Serial.println(F("Failed to parse json file"));
                return {};
            }
            if(json["updateAvilable"].as<bool>() == false){
                Serial.println("Update aviable: false");
                return s_updateConfig();
            }
            s_updateConfig dataUpdateConfig;
            dataUpdateConfig.version = json["updateVersion"].as<String>();
            dataUpdateConfig.spiffs = json["SPIFFS"].as<bool>();
            dataUpdateConfig.software = json["SOFTWARE"].as<bool>();
            if(json.containsKey("URL_SOFT")) dataUpdateConfig.url_soft = json["URL_SOFT"].as<String>();
            if(json.containsKey("URL_SPIFFS")) dataUpdateConfig.url_spiffs = json["URL_SPIFFS"].as<String>();
            dataUpdateConfig.host = json["HOST"].as<String>();

            // Serial.println("Update file: {");
            // // serializeJsonPretty(dataUpdateConfig, Serial);
            // Serial.println("   Version: " + dataUpdateConfig.version);
            // Serial.println("   SPIFFS: " + String(dataUpdateConfig.spiffs));
            // Serial.println("   Software: " + String(dataUpdateConfig.software));
            // Serial.println("   URL_SOFT: " + dataUpdateConfig.url_soft);
            // Serial.println("   URL_SPIFFS: " + dataUpdateConfig.url_spiffs);
            // Serial.println("   HOST: " + dataUpdateConfig.host);
            // Serial.println("}");
            return dataUpdateConfig;
        }
} lf;

#endif