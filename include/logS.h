#ifndef _LOG_S_H_
#define _LOG_S_H_
#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <SPIFFS.h>
#include <vector>
#include <HTTPClient.h>

#include "dataConn.h"
#include "struct.h"

String httpRequest(const char *serverName){
    HTTPClient http;
    bool beginHTTP = http.begin(serverName);
    Serial.println("HTTP GET Request done");
    if(!beginHTTP){
        Serial.println("HTTPClient failed to connect");
        return "";
    }
    int httpResponseCode = http.GET();
    Serial.println("Downloading version.json");

    if(httpResponseCode <= 0 || httpResponseCode != HTTP_CODE_OK){
        Serial.println("Error on HTTP request");
        return "";
    }

    String payload = http.getString();
    http.end();
    return payload;
}

class raportLogs{
    private:

    String errFilePath = "/raportFile.txt";
    String logFilePath = "/logFile.txt";
    String infoUserPath = "userInfo";

    struct s_buffer_log{
        String type;
        String message;
        unsigned long millis;
        String path;
        String ip;
        String user_agent;
        String platform;
        String browser;
        String url;
    };
    std::vector<s_buffer_log> buffer_log;

    bool writeAppend_SD(String path, String msg){
        if(!SD_CARD_AVILABLE) return false;
        File file = SD.open(path, FILE_APPEND);
        if(!file) return false;
        file.println(msg);
        file.close();
        return true;
    }

    void writeData(String path, String msg){
        writeAppend_SD(path, msg);
        ftpClient.appendFile(path, msg);
    }

    public:

    raportLogs(){
        if(!SD_CARD_AVILABLE) return;
        // if(!SD.exists(infoUserPath)) SD.mkdir(infoUserPath);
    };//ftpClient.initConnection(dataFtp.ip_address, dataFtp.username, dataFtp.password, dataFtp.port); };
    ~raportLogs(){};

    void setErrFile(String filePath){ errFilePath = filePath; }
    void setLogFile(String filePath){ logFilePath = filePath; }
    void setUserInfoDir(String dir){ infoUserPath = dir; }



    String make_msg(String w="err", String msg="", String ip="", String user_agent="", String platform="", String browser="", String url=""){
        String message = "";
        if(w == "err") message = " Error: " + msg;
        else if(w == "info") message = " Info: " + msg;
        else if(w == "warn") message = " Warning: " + msg;
        else if(w == "log") message = " Login: " + msg;

        if(ip.length() > 0 && user_agent.length() > 0 && platform.length() > 0 && browser.length() > 0){
            message += " { ";
            if(ip.length() > 0) message += "  |  IP_INFO: " + httpRequest(String("http://ip-api.com/json/" + ip + "?fields=66846719").c_str());
            if(user_agent.length() > 0) message += "  |  User-Agent: " + user_agent;
            if(platform.length() > 0) message += "  |  Platform: " + platform;
            if(browser.length() > 0) message += "  |  Browser: " + browser;
            if(url.length() > 0) message += "  |  URL: " + url;
            message += " }";
        }

        return message;
    }

    String msgData(){
        return "{ " + String(TimeS.Day()) + "/" + String(TimeS.Month()) + "/" + String(TimeS.Year()) + " " + String(TimeS.Hour()) + ":" + String(TimeS.Minute()) + ":" + String(TimeS.Second()) + " }";
    }

    bool unloadBuffer(){
        if(WiFi.status() != WL_CONNECTED) return false;
        for(struct s_buffer_log i : buffer_log){
            s_date date = ownTime::getDate(i.millis);
            String msg = "{ " + String(date.day) + "/" + String(date.month) + "/" + String(date.year) + " " + String(date.hours) + ":" + String(date.minute) + ":" + String(date.second) + " } " + make_msg(i.type, i.message, i.ip, i.user_agent, i.platform, i.browser);
            writeData(i.path, msg);
        }
        buffer_log.clear();
        return true;
    }

    void err(String err, String ip="", String user_agent="", String platform="", String browser="", String url=""){
        if(err == "") return;

        if(WiFi.status() != WL_CONNECTED) {
            buffer_log.push_back({"err", err, millis(), errFilePath, ip, user_agent, platform, browser, url});
            return;
        }

        String msg = msgData() + make_msg("err", err, ip, user_agent, platform, browser, url);
        writeData(errFilePath, msg);
    }

    void log(String log, String ip="", String user_agent="", String platform="", String browser="", String url=""){
        if(log == "") return;

        if(WiFi.status() != WL_CONNECTED) {
            buffer_log.push_back({"log", log, millis(), logFilePath, ip, user_agent, platform, browser, url});
            return;
        }

        String msg = msgData() + make_msg("log", log, ip, user_agent, platform, browser, url);
        writeData(logFilePath, msg);
    }

    void user_info(String user, String info, String ip="", String user_agent="", String platform="", String browser="", String url=""){
        if(info == "" && user.length() == 0) return;

        if(WiFi.status() != WL_CONNECTED) {
            buffer_log.push_back({"info", info, millis(), infoUserPath + user + ".txt", ip, user_agent, platform, browser, url});
            return;
        }

        String msg = msgData() + make_msg("info", info, ip, user_agent, platform, browser, url);
        writeData("/" + infoUserPath + "/" + user + ".txt", msg);
    }

    void big_error(){
        pinMode(2, OUTPUT);
        while(true){
            Serial.println("Big error! Nie można zaladowac pliku konfiguracyjnego. Sprawdz czy SD karta jest podlaczona.");
            digitalWrite(2, HIGH);
            delay(500);
            digitalWrite(2, LOW);
            delay(500);
        }
    }
} rpLog;

#endif