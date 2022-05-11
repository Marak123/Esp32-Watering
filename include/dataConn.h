#ifndef _DATA_CONNECTION_H_
#define _DATA_CONNECTION_H_
#pragma once

#include <logS.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ESP32_FTPClient.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <SPIFFS.h>


bool SD_CARD_AVILABLE = true;

// Inicjacja SD_CARD
void initSD(){
    if(SD_CARD_AVILABLE)
        try{
            int cRel = 10;
            while(cRel > 0){
                if (SD.begin(5)) break;
                cRel--;
                if(cRel == 0){
                    SD_CARD_AVILABLE = false;
                    Serial.println("SD_CARD Mount Failed");
                }
            }
        }catch(...){
            SD_CARD_AVILABLE = false;
            Serial.println("SD_CARD Mount Failed");
        }
}

// Inicjacja SPIFFS
void initSPIFFS(){
    if (!SPIFFS.begin(true, "/data", 100))
        Serial.println("SPIFFS Mount Failed");
};

//Zapis plikow na serwer FTP
class FTP
{
    private:
    ESP32_FTPClient *_ftp;
    bool _canConnected = true;
    bool _intiConnection = false;

    public:
    FTP(){};
    ~FTP(){};

    bool isInitConn(){
        return _intiConnection;
    }

    void initConnection(char *ftp_server, char *ftp_user, char *ftp_password, uint16_t port=21, uint16_t timeout=5000, uint8_t verbose=1)
    {
        static ESP32_FTPClient ftp(ftp_server, port, ftp_user, ftp_password, timeout, verbose);
        _ftp = &ftp;
        _intiConnection = true;
    };

    bool canConn(){ return _canConnected; };

    bool Connect()
    {
        // Serial.println("Connecting to FTP server...");
        try{
            if(!isInitConn() && (String)dataFtp.ip_address != "" && (String)dataFtp.username != "" && (String)dataFtp.password != ""){
                initConnection(dataFtp.ip_address, dataFtp.username, dataFtp.password, dataFtp.port);
            }else return false;

            _ftp->OpenConnection();
            return _ftp->isConnected();
            return false;
        }catch(...){
            Serial.println("FTP connection failed");
            // rpLog.err("FTP connection failed");
            return false;
        }
    }

    void CloseConnection(){ _ftp->CloseConnection(); }

    bool appendFile(String FileName, String message)
    {
        if(!Connect()) return false;
        Serial.println("FTP is connected");

        char FN[FileName.length()];
        char MS[message.length()];

        FileName.toCharArray(FN, FileName.length());
        message.toCharArray(MS, message.length());

        if (!_ftp->isConnected())
        {
            Serial.println("Połączenie z serwerem FTP nie jest otwarte.");
            return false;
        }
        try
        {
            _ftp->InitFile("Type A");
            _ftp->AppendFile(FN);
            _ftp->Write(MS);
            _ftp->CloseFile();
            CloseConnection();
            return true;
        }
        catch (String error)
        {
            Serial.print("Wystąpił problem: ");
            Serial.println(error);
            // rpLog.err("Wystąpił problem: " + error);
            return false;
        }

    }
} ftpClient;

#endif // _DATA_CONNECTION_H_