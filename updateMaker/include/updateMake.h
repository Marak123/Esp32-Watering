#ifndef _UPDATE_MAKER_H_
#define _UPDATE_MAKER_H_

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <loadFile.h>
// #include "certs_github.h"
// #include "mbedtls/base64.h"
#include "SD.h"
// #include <base64/base64.h>
#include <base64.h>

const char github_cert [] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIIEFzCCAv+gAwIBAgIQB/LzXIeod6967+lHmTUlvTANBgkqhkiG9w0BAQwFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0yMTA0MTQwMDAwMDBaFw0zMTA0MTMyMzU5NTlaMFYxCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxMDAuBgNVBAMTJ0RpZ2lDZXJ0IFRMUyBI
eWJyaWQgRUNDIFNIQTM4NCAyMDIwIENBMTB2MBAGByqGSM49AgEGBSuBBAAiA2IA
BMEbxppbmNmkKaDp1AS12+umsmxVwP/tmMZJLwYnUcu/cMEFesOxnYeJuq20ExfJ
qLSDyLiQ0cx0NTY8g3KwtdD3ImnI8YDEe0CPz2iHJlw5ifFNkU3aiYvkA8ND5b8v
c6OCAYIwggF+MBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFAq8CCkXjKU5
bXoOzjPHLrPt+8N6MB8GA1UdIwQYMBaAFAPeUDVW0Uy7ZvCj4hsbw5eyPdFVMA4G
A1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwdgYI
KwYBBQUHAQEEajBoMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5j
b20wQAYIKwYBBQUHMAKGNGh0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9EaWdp
Q2VydEdsb2JhbFJvb3RDQS5jcnQwQgYDVR0fBDswOTA3oDWgM4YxaHR0cDovL2Ny
bDMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0R2xvYmFsUm9vdENBLmNybDA9BgNVHSAE
NjA0MAsGCWCGSAGG/WwCATAHBgVngQwBATAIBgZngQwBAgEwCAYGZ4EMAQICMAgG
BmeBDAECAzANBgkqhkiG9w0BAQwFAAOCAQEAR1mBf9QbH7Bx9phdGLqYR5iwfnYr
6v8ai6wms0KNMeZK6BnQ79oU59cUkqGS8qcuLa/7Hfb7U7CKP/zYFgrpsC62pQsY
kDUmotr2qLcy/JUjS8ZFucTP5Hzu5sn4kL1y45nDHQsFfGqXbbKrAjbYwrwsAZI/
BKOLdRHHuSm8EdCGupK8JvllyDfNJvaGEwwEqonleLHBTnm8dqMLUeTF0J5q/hos
Vq4GNiejcxwIfZMy0MJEGdqN9A57HSgDKwmKdsp33Id6rHtSJlWncg+d0ohP/rEh
xRqhqjn1VtvChMQ1H3Dau0bwhr9kAMQ+959GG50jBbl9s08PqUU643QwmA==
-----END CERTIFICATE-----
)CERT";

const char* github_host = "api.github.com";
const uint16_t github_port = 443;
String github_token = "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";

class updateMakeOwn{
    private:
        s_updateConfig updateConfig ;
        StaticJsonDocument<JSON_OBJECT_SIZE(200)> configFileSAVE;

        bool loadConfigFile(){
            File file = SPIFFS.open("/configFile.json", "r");
            if(!file){
                Serial.println("Failed to open file for reading");
                return false;
            }
            String content = file.readString();
            file.close();
            DeserializationError error = deserializeJson(configFileSAVE, content);
            if(error){
                Serial.println(F("Failed to parse json file"));
                return false;
            }
            return true;
        }

        bool saveConfigFile(){
            File file = SPIFFS.open("/configFile.json", "w");
            if(!file){
                Serial.println("Failed to open file for writing");
                return false;
            }
            serializeJson(configFileSAVE, file);
            file.close();
            return true;
        }

        void update(String url, String command="U_FLASH"){
            long contentLength = 0;
            bool isValidContentType = false;

            WiFiClientSecure client;
            client.setCACert(github_cert);

            Serial.println("\n\nConnecting to: " + String(github_host));
            Serial.println("Requesting URL: " + url + "\n\n");

            if(!client.connect(github_host, github_port)){
                Serial.println("Failed to connect to server");
                return;
            }
            client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + github_host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "User-Agent: Awesome-Octocat-App\r\n" +
                 "Authorization: token " + github_token + "\r\n" +
                 "Connection: close\r\n\r\n");

            unsigned long timeout = millis();
            while (client.available() == 0) {
                if (millis() - timeout > 5000) {
                    Serial.println("Client Timeout!");
                    client.stop();
                    return;
                }
            }

            while (client.available()) {
                String line = client.readStringUntil('\n');
                Serial.println(line);
                line.trim();

                if(!line.length()) break;
                if (line.startsWith("HTTP/1.1")) {
                    if (line.indexOf("200") < 0) {
                        Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
                        break;
                    }
                }
                if (line.startsWith("Content-Length: ")) {
                    contentLength = atol((line.substring(strlen(String("Content-Length: ").c_str()))).c_str());
                    Serial.println("Got " + String(contentLength) + " bytes from server");
                }
                if (line.startsWith("Content-Type: ")) {
                    String contentType = line.substring(strlen(String("Content-Type: ").c_str()));
                    Serial.println("Got " + contentType + " payload.");
                    if (contentType == "application/octet-stream") {
                        isValidContentType = true;
                    }
                }
            }

            if (!contentLength && !isValidContentType){
                Serial.println("There was no content in the response");
                client.flush();
                return;
            }

            File fs = SD.open("/updates.txt", FILE_APPEND);
            File spif = SPIFFS.open("/updates_bin_firmware.bin", FILE_APPEND);

            String base = "";
            size_t size = 200;
            bool startFind = false;
            bool endFind = false;
            int before = 0;
            for(int i=0; i<contentLength; i++){
                int c = client.read();
                if(c == -1){
                    i--;
                    continue;
                }
                String ch = "";
                ch += (char)before;
                ch += (char)c;
                before = c;
                if(ch == "\\n"){
                    base = base.substring(0, base.length()-1);
                    continue;
                }
                base += (char)c;
                if(!startFind){
                    if(base.indexOf("\",\"content\":\"") != -1){
                        startFind = true;
                        Serial.println("Find string: " + base);
                        base = "";
                    }
                    continue;
                }
                if(base.indexOf("\"") != -1){
                    endFind = true;
                    Serial.println("Find string: " + base);
                    base = base.substring(0, base.length()-1);
                    Serial.println("Find string: " + base);
                }
                if((base.length() == 200 && startFind) || endFind){
                    char output[size];
                    int sizeDec = b64_decode(output, (char*)base.c_str(), base.length());
                    fs.write((const uint8_t*)output, sizeDec);
                    // fs.write((const uint8_t*)base.c_str(), base.length());
                    spif.write((const uint8_t*)output, sizeDec);
                    Serial.println("Działa");
                    base = "";

                    if(endFind){
                        Serial.println("Writing end :)");
                        break;
                    }
                }
            }

            fs.print("\n\n\n\n\n'ENDING TESTING UPDATE WRITE'\n\n\n\n\n");
            fs.close();

            // Print file from spif variable
            while(spif.available()){
                Serial.write(spif.read());
            }

            int fileSize = spif.size();
            bool canBegin;// = Update.begin(contentLength, U_SPIFFS);
            // if(command == "U_FLASH") canBegin = Update.begin(contentLength);
            // else if(command == "U_SPIFFS") canBegin = Update.begin(contentLength, U_SPIFFS);
            
            if(command == "U_FLASH") canBegin = Update.begin(fileSize);
            else if(command == "U_SPIFFS") canBegin = Update.begin(fileSize, U_SPIFFS);

            if (!canBegin){
                Serial.println("Not enough space to begin OTA");
                client.flush();
            }

            Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");



            // size_t written = Update.writeStream(client);
            size_t written = Update.writeStream(spif);

            if (written == contentLength) Serial.println("Written : " + String(written) + " successfully");
            else Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );

            if (Update.end()) {
                Serial.println("OTA done!");
                if (Update.isFinished()) {
                    Serial.println("Update successfully completed.");
                }else Serial.println("Update not finished? Something went wrong!");
            } else Serial.println("Error Occurred. Error #: " + String(Update.getError()));
        }
        void loadConf(){
            this->updateConfig = lf.loadUpdateConfig();
        }
    public:
        void spiffsUpdate(){
            Serial.println("Starting SPIFFS update");
            this->loadConf();
            if(!updateConfig.spiffs && updateConfig.url_spiffs.length() > 0 && updateConfig.host.length() > 0){
                Serial.println("SPIFFS update error");
                return;
            }

            File fl1 = SPIFFS.open("/configFile.json", "r");
            if(!fl1){
                Serial.println("Failed to open file for reading");
                return;
            }
            String content1 = fl1.readString();
            fl1.close();
            // Serial.println("Przed update file: {\n" + content1 + "\n}");

            loadConfigFile();
            this->update(updateConfig.url_spiffs, "U_SPIFFS");
            saveConfigFile();

            File fl = SPIFFS.open("/configFile.json", "r");
            if(!fl){
                Serial.println("Failed to open file for reading");
                return;
            }
            String content = fl.readString();
            fl.close();
            // Serial.println("Po update file: {\n" + content + "\n}");
        }
        void softwareUpdate(){
            Serial.println("Starting software update");
            this->loadConf();
            if(!updateConfig.software && updateConfig.url_soft.length() > 0 && updateConfig.host.length() > 0){
                Serial.println("Software update error");
                return;
            }
            this->update(updateConfig.url_soft);
        }
} um;

#endif