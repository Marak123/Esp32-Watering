#ifndef UPDATES_H
#define UPDATES_H

#include <HTTPClient.h>
#include <ArduinoJson.h>

#define CURRENT_VERSION "0.0.1"
#define URL_VERSION "https://raw.githubusercontent.com/Marak123/test/main/version.json"
#define REPOSITORY "https://raw.githubusercontent.com/Marak123/test/main/"
#define NAME_FILE_DESCRIPTION "/Readme.md"


String httpGETRequest(const char *serverName)
{
    HTTPClient http;
    bool beginHTTP = http.begin(URL_VERSION);
    if(!beginHTTP){
        Serial.println("HTTPClient failed to connect");
        return "";
    }
    int httpResponseCode = http.GET();

    if(httpResponseCode <= 0 || httpResponseCode != HTTP_CODE_OK){
        Serial.println("Error on HTTP request");
        return "";
    }

    String payload = http.getString();
    http.end();
    return payload;
}

class UpdateSoftware
{
    private:

    struct s_olderVersion
    {
        String versionFamily;
        std::vector<String> version;
    };
    struct s_Version
    {
        String lastVersion;
        std::vector<s_olderVersion> olderVersion;
    };

    s_Version Version;

    public:


    s_Version convertFromJSON(String payload){
        StaticJsonDocument<JSON_OBJECT_SIZE(50)> versionDoc;
        DeserializationError error = deserializeJson(versionDoc, payload);
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            return{"None",{}};
        }
        String last_version = versionDoc["last_version"].as<String>();
        JsonArray older_version = versionDoc["older"].as<JsonArray>();

        std::vector<s_olderVersion> older;
        for(JsonVariant i : older_version){
            JsonArray ol = i["version"].as<JsonArray>();
            s_olderVersion older_ver = {i["group"].as<String>(), {}};
            for(JsonVariant j : ol) older_ver.version.push_back(j.as<String>());
            older.push_back(older_ver);
        }

        return{
            last_version,
            older
        };
    }

    String showComunnicat(bool isNewVersion){
        if(isNewVersion) return "{\"status\":\"success\", \"update\":true, \"version\":\"" + Version.lastVersion + "\", \"description_url\":\"" + REPOSITORY + Version.lastVersion + NAME_FILE_DESCRIPTION + "\"}";
        else return "{\"status\":\"success\", \"update\":false}";
    }

    String getVersion(){
        StaticJsonDocument <JSON_OBJECT_SIZE(50)> versionDoc;
        versionDoc["url_pattern_start"] = REPOSITORY;
        versionDoc["url_pattern_end"] = NAME_FILE_DESCRIPTION;
        versionDoc["version"]["current"] = CURRENT_VERSION;
        versionDoc["version"]["last_new"] = Version.lastVersion;

        JsonArray older_version = versionDoc.createNestedArray("older");
        for(s_olderVersion old : Version.olderVersion)
            for(String ver : old.version) older_version.add(ver);
        versionDoc["version"]["older"] = older_version;

        String payload;
        serializeJson(versionDoc, payload);
        return payload;
    }

    String checkUpdate(){
        String sensorReadings = httpGETRequest(URL_VERSION);
        Version = convertFromJSON(sensorReadings);
        Serial.println(showComunnicat(true));
        if(Version.lastVersion != CURRENT_VERSION) return showComunnicat(true);
        return showComunnicat(false);
    }
};

// Create update object to public use
UpdateSoftware updateSoft;

#endif // UPDATES_H