#ifndef MAIN_FUN_
#define MAIN_FUN_
#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "struct.h"
#include "variables.h"
#include "ownTime.h"

 bool isRune = true;

StaticJsonDocument<JSON_OBJECT_SIZE(20)> inLoopActionPerform()
{
    bool change = false;
    int sizeArray = varPins.actionList.size();
    try{
        for (int i = 0; i < sizeArray; i++)
            if (varPins.actionList[i].time.hours == TimeS.Hour() && varPins.actionList[i].time.minute <= TimeS.Minute() && varPins.actionList[i].time.day == TimeS.Day() && varPins.actionList[i].time.month == TimeS.Month() && varPins.actionList[i].time.year == TimeS.Year())
            {
                for (int j = 0; j < varPins.pins.size(); j++)
                    if (varPins.pins[j].idPin == varPins.actionList[i].idPin && varPins.pins[j].nrPin == varPins.actionList[i].nrPin){
                        digitalWrite(varPins.actionList[i].nrPin, varPins.actionList[i].action);
                        StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonTemp;
                        jsonTemp["what"] = "changePowerPin";
                        jsonTemp["pinID"] = varPins.pins[j].nrPin;
                        jsonTemp["action"] = varPins.actionList[i].action;
                        web_serv.notifyClients(jsonTemp);
                        rpLog.log("Wykonano akcje \"" + String(varPins.actionList[i].action) + "\" na pinie \"" + String(varPins.actionList[i].nrPin) + "\"");
                    }
                varPins.actionList.erase(varPins.actionList.begin() + i);

                sizeArray--;
                i--;
                change = true;
            }
    }catch(...){
        rpLog.err("Wystapil blad w \"inLoopActionPerform\" przy sprawdzaniu listy akcji do wykonania");
    }

    int startWeek = 0;
    int endWeek = 0;

    int wDay = TimeS.timeinfo.tm_wday;

    if(TimeS.Day() < wDay) startWeek = ownTime::dayINmonth(TimeS.Month()-1, TimeS.Year()) - (wDay - TimeS.Day());
    else startWeek = TimeS.Day() - wDay;

    if(ownTime::dayINmonth(TimeS.Month(), TimeS.Year()) < TimeS.Day() + wDay) endWeek = (wDay + TimeS.Day()) - ownTime::dayINmonth(TimeS.Month(), TimeS.Year());
    else endWeek = TimeS.Day() + wDay;

    for(int AW = 0; AW < varPins.actionWeek.size(); AW++){
        if((varPins.actionWeek[AW].nrDayInWeek[0] != startWeek || varPins.actionWeek[AW].nrDayInWeek[1] != endWeek) || wDay == 7 || (varPins.actionWeek[AW].nrDayInWeek[0] == -1 || varPins.actionWeek[AW].nrDayInWeek[1] == -1)){
            for(int wd : varPins.actionWeek[AW].arrayWeek){
                struct s_date da;
                if((wd + 1) > wDay) da = ownTime::addDates({ (wd + 1)- wDay });
                else if(wDay == 7) da = ownTime::addDates({ wd + 1});
                else continue;

                struct s_actionList act = {{da.day, da.month, da.year, varPins.actionWeek[AW].time.hour, varPins.actionWeek[AW].time.minute, varPins.actionWeek[AW].time.second}, varPins.actionWeek[AW].action, varPins.actionWeek[AW].idPin,  varPins.actionWeek[AW].nrPin};
                if(!actionExist(act)){
                    varPins.actionList.push_back(act);
                    change = true;
                }
            }
            varPins.actionWeek[AW].weekCount++;
            varPins.actionWeek[AW].nrDayInWeek[0] = startWeek;
            varPins.actionWeek[AW].nrDayInWeek[1] = endWeek;
        }
    }

    if (change)
    {
        change = false;
        StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
        jsonRet["what"] = "action-done";
        jsonRet["arrayListActivity"] = arrCreator::listActionElm(true);
        return jsonRet;
    }
    StaticJsonDocument<JSON_OBJECT_SIZE(20)> jsonRet;
    jsonRet["what"] = "none";
    return jsonRet;
}

bool initTempSensor()
{
    for (int i = 0; i < varPins.tempData.size(); i++){
        try{
            varPins.tempData[i].handleRead.begin();
            rpLog.log("Czujniki temperatury zainicjowane");
        }catch(String err){
            Serial.print("Wystapil problem z inicjacja czujnikow temperatury");
            rpLog.log("Wystapil problem z inicjacja czujnikow temperatury");
        }
    }
    return true;
}

bool readTemperature()
{
    float temp = 0;
    for (int i = 0; i < varPins.tempData.size(); i++)
    {
        temp = varPins.tempData[i].handleRead.readTemperature();
        if (!isnan(temp)){
            varPins.tempData[i].curlTemp = temp;
            temperatureData.push_back(temp);
            countLoopReadTemperature++;
        }
        else varPins.tempData[i].curlTemp = -1;
    }
    if(countLoopReadTemperature == 20){
        //calculation of the average temperature
        float averageTemp = 0;
        for(int i = 0; i < temperatureData.size(); i++){
            averageTemp += temperatureData[i];
        }
    }
    return true;
}


#endif