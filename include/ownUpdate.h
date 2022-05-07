#ifndef _OWN_UPDATES_H
#define _OWN_UPDATES_H
#pragma once

// #include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <logS.h>

#define CURRENT_VERSION "0.0.1"
#define DOMAIN_SERVER "raw.githubusercontent.com"
#define URL_VERSION "https://raw.githubusercontent.com/Marak123/test/main/version.json"
#define REPOSITORY "https://raw.githubusercontent.com/Marak123/test/main/"
#define NAME_FILE_DESCRIPTION "/Readme.md"

const char github_cert [] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIEvjCCA6agAwIBAgIQBtjZBNVYQ0b2ii+nVCJ+xDANBgkqhkiG9w0BAQsFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0yMTA0MTQwMDAwMDBaFw0zMTA0MTMyMzU5NTlaME8xCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxKTAnBgNVBAMTIERpZ2lDZXJ0IFRMUyBS
U0EgU0hBMjU2IDIwMjAgQ0ExMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC
AQEAwUuzZUdwvN1PWNvsnO3DZuUfMRNUrUpmRh8sCuxkB+Uu3Ny5CiDt3+PE0J6a
qXodgojlEVbbHp9YwlHnLDQNLtKS4VbL8Xlfs7uHyiUDe5pSQWYQYE9XE0nw6Ddn
g9/n00tnTCJRpt8OmRDtV1F0JuJ9x8piLhMbfyOIJVNvwTRYAIuE//i+p1hJInuW
raKImxW8oHzf6VGo1bDtN+I2tIJLYrVJmuzHZ9bjPvXj1hJeRPG/cUJ9WIQDgLGB
Afr5yjK7tI4nhyfFK3TUqNaX3sNk+crOU6JWvHgXjkkDKa77SU+kFbnO8lwZV21r
eacroicgE7XQPUDTITAHk+qZ9QIDAQABo4IBgjCCAX4wEgYDVR0TAQH/BAgwBgEB
/wIBADAdBgNVHQ4EFgQUt2ui6qiqhIx56rTaD5iyxZV2ufQwHwYDVR0jBBgwFoAU
A95QNVbRTLtm8KPiGxvDl7I90VUwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQG
CCsGAQUFBwMBBggrBgEFBQcDAjB2BggrBgEFBQcBAQRqMGgwJAYIKwYBBQUHMAGG
GGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBABggrBgEFBQcwAoY0aHR0cDovL2Nh
Y2VydHMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0R2xvYmFsUm9vdENBLmNydDBCBgNV
HR8EOzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRH
bG9iYWxSb290Q0EuY3JsMD0GA1UdIAQ2MDQwCwYJYIZIAYb9bAIBMAcGBWeBDAEB
MAgGBmeBDAECATAIBgZngQwBAgIwCAYGZ4EMAQIDMA0GCSqGSIb3DQEBCwUAA4IB
AQCAMs5eC91uWg0Kr+HWhMvAjvqFcO3aXbMM9yt1QP6FCvrzMXi3cEsaiVi6gL3z
ax3pfs8LulicWdSQ0/1s/dCYbbdxglvPbQtaCdB73sRD2Cqk3p5BJl+7j5nL3a7h
qG+fh/50tx8bIKuxT8b1Z11dmzzp/2n3YWzW2fP9NsarA4h20ksudYbj/NhVfSbC
EXffPgK2fPOre3qGNm+499iTcc+G33Mw+nur7SpZyEKEOxEXGlLzyQ4UfaJbcme6
ce1XR2bFuAJKZTRei9AqPCCcUZlM51Ke92sRKw2Sfh3oius2FkOH6ipjv3U/697E
A7sKPPcw7+uvTPyLNhBzPvOk
-----END CERTIFICATE-----
)CERT";

class UpdateSoftware
{
    private:
    String comunicatVer = "";

    struct s_olderVersion
    {
        String versionFamily;
        std::vector<String> version;
    };
    struct s_Version
    {
        String lastVersion;
        std::vector<s_olderVersion> olderVersion;
    } Version;

    String httpRequestUpdate()
    {
        WiFiClientSecure client;
        client.setCACert(github_cert);

        if (!client.connect(DOMAIN_SERVER, 443))
        {
            rpLog.err("Failed to connect to the server to check the list of the latest versions. Connection failed!!");
            return "";
        }
        client.print(String("GET ") + URL_VERSION + " HTTP/1.1\r\n" +
            "Host: " + DOMAIN_SERVER + "\r\n" +
            "Cache-Control: no-cache\r\n" +
            "User-Agent: Awesome-Octocat-App\r\n" +
            "Authorization: token " + "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" + "\r\n" +
            "Connection: close\r\n\r\n");

        unsigned long timeout = millis();
        while (client.available() == 0) {
            delay(1);
            if (millis() - timeout > 5000) {
                rpLog.err("Client Timeout!. Update");
                client.stop();
                return "";
            }
        }

        long contentLength = 0;
        bool isValidContentType = false;
        while (client.available()) {
            delay(1);
            String line = client.readStringUntil('\n');
            line.trim();

            if(!line.length()) break;
            if (line.startsWith("HTTP/1.1")) {
                if (line.indexOf("200") < 0) {
                    rpLog.log("Got a non 200 status code from server. " + line);
                    break;
                }
            }
            if (line.startsWith("Content-Length: ")) {
                contentLength = atol((line.substring(strlen(String("Content-Length: ").c_str()))).c_str());
            }
            if (line.startsWith("Content-Type: ")) {
                String contentType = line.substring(strlen(String("Content-Type: ").c_str()));
                if (contentType == "application/octet-stream") {
                    isValidContentType = true;
                }
            }
        }
        if (!contentLength && !isValidContentType){
            rpLog.log("There was no content in the response");
            client.flush();
            return "";
        }

        String payload = "";
        for(int i=0; i < contentLength; i++){
            delay(1);
            int c = client.read();
            if(c == -1){
                i--;
                continue;
            }
            payload += (char)c;
        }

        client.flush();
        return payload;
    }

    public:

    String getUpdateCommunicat(){
        return comunicatVer;
    }

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

        JsonArray older_version = versionDoc["version"].createNestedArray("older");
        for(s_olderVersion old : Version.olderVersion)
            for(String ver : old.version) older_version.add(ver);

        String payload;
        serializeJson(versionDoc, payload);
        return payload;
    }

    String checkUpdate(){
        String sensorReadings = this->httpRequestUpdate();
        Version = convertFromJSON(sensorReadings);
        comunicatVer = showComunnicat(Version.lastVersion != CURRENT_VERSION);
        if(comunicatVer.length() == 0){
            rpLog.err("Error in the communication with the server. Lenght of the response is 0");
            Serial.println("Communicat is empty. Communicat: " + comunicatVer);
        }
        return comunicatVer;
    }
} updateSoft;

#endif // UPDATES_H