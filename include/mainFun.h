#ifndef MAIN_FUN_
#define MAIN_FUN_

 bool isRune = true;

void print(s_date dateAction){
    Serial.print(dateAction.day);
    Serial.print("-");
    Serial.print(dateAction.month);
    Serial.print("-");
    Serial.println(dateAction.year);
}

void printActionList(){
    Serial.print("Action list: Lenght=");
    Serial.print(varPins.actionList.size());
    for(int i=0; i < varPins.actionList.size(); i++){
        Serial.print("     Date=");
        Serial.print(varPins.actionList[i].time.day);
        Serial.print("-");
        Serial.print(varPins.actionList[i].time.month);
        Serial.print("-");
        Serial.print(varPins.actionList[i].time.year);
        Serial.print("     Time=");
        Serial.print(varPins.actionList[i].time.hours);
        Serial.print(":");
        Serial.print(varPins.actionList[i].time.minute);
        Serial.print("     Pin=");
        Serial.println(varPins.actionList[i].nrPin);
    }
}

StaticJsonDocument<JSON_OBJECT_SIZE(20)> inLoopActionPerform()
{
    bool change = false;
    int sizeArray = varPins.actionList.size();
    for (int i = 0; i < sizeArray; i++)
        if (varPins.actionList[i].time.hours <= TimeS.Hour() && varPins.actionList[i].time.minute <= TimeS.Minute() && varPins.actionList[i].time.day <= TimeS.Day() && varPins.actionList[i].time.month <= TimeS.Month() && varPins.actionList[i].time.year <= TimeS.Year())
        {
            for (int j = 0; j < varPins.pins.size(); j++)
                if (varPins.pins[j].idPin == varPins.actionList[i].idPin && varPins.pins[j].nrPin == varPins.actionList[i].nrPin)
                    digitalWrite(varPins.actionList[i].nrPin, varPins.actionList[i].action);
            varPins.actionList.erase(varPins.actionList.begin() + i);
            sizeArray--;
            i--;
            change = true;
        }

    for(int AW = 0; AW < varPins.actionWeek.size(); AW++)
        if(isRune || varPins.actionWeek[AW].weekCount == 0){
            isRune = false;
            if(TimeS.nrWeekDay() == 1){
                for (int aW = 0; aW < varPins.actionWeek.size(); aW++)
                    for (int wD = 0; wD < varPins.actionWeek[aW].arrayWeek.size(); wD++){
                        s_date dateAction = ownTime::addDates({varPins.actionWeek[aW].arrayWeek[wD] - 1});
                        dateAction.hours = varPins.actionWeek[aW].time.hour;
                        dateAction.minute = varPins.actionWeek[aW].time.minute;

                        varPins.actionList.push_back({dateAction, varPins.actionWeek[aW].action, varPins.actionWeek[aW].idPin,  varPins.actionWeek[aW].nrPin});
                        varPins.actionWeek[aW].weekCount++;
                    }
            }else{
                int dayAdd = 0;
                bool isOnList = false;
                for (int aW = 0; aW < varPins.actionWeek.size(); aW++){
                    for (int wD = 0; wD < varPins.actionWeek[aW].arrayWeek.size(); wD++){
                        if(varPins.actionWeek[aW].arrayWeek[wD] > TimeS.nrWeekDay()) dayAdd = varPins.actionWeek[aW].arrayWeek[wD] - TimeS.nrWeekDay();
                        else if(varPins.actionWeek[aW].arrayWeek[wD] == TimeS.nrWeekDay())
                            if(varPins.actionWeek[aW].time.hour > TimeS.Hour() || (varPins.actionWeek[aW].time.hour == TimeS.Hour() && varPins.actionWeek[aW].time.minute > TimeS.Minute())) dayAdd = 0;
                            else continue;
                        else continue;

                        s_date dateAction = ownTime::addDates({23, 11, 2021}, {dayAdd});
                        dateAction.hours = varPins.actionWeek[aW].time.hour;
                        dateAction.minute = varPins.actionWeek[aW].time.minute;

                        for(int aL = 0; aL < varPins.actionList.size(); aL++)
                            if(dateAction.day == varPins.actionList[aL].time.day && dateAction.month == varPins.actionList[aL].time.month && dateAction.year == varPins.actionList[aL].time.year && dateAction.hours == varPins.actionList[aL].time.hours && dateAction.minute == varPins.actionList[aL].time.minute){
                                isOnList = true;
                                break;
                            }
                        if(isOnList){
                            dayAdd = 0;
                            isOnList = false;
                            continue;
                        }

                        varPins.actionList.push_back({dateAction, varPins.actionWeek[aW].action, varPins.actionWeek[aW].idPin,  varPins.actionWeek[aW].nrPin});
                        change = true;
                    }
                    varPins.actionWeek[aW].weekCount++;
                }
            }
        }

    if((TimeS.nrWeekDay() == 1 && TimeS.Hour() == 0 && TimeS.Minute() == 0)){
        for (int aW = 0; aW < varPins.actionWeek.size(); aW++){
            for (int wD = 0; wD < varPins.actionWeek[aW].arrayWeek.size(); wD++){
                s_date dateAction = ownTime::addDates({varPins.actionWeek[aW].arrayWeek[wD] - 1});

                varPins.actionList.push_back({dateAction, varPins.actionWeek[aW].action, varPins.actionWeek[aW].idPin,  varPins.actionWeek[aW].nrPin});
                change = true;
            }
            varPins.actionWeek[aW].weekCount++;
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
        }catch(String err){
            Serial.print("Wystapil problem z inicjacja temperatury");
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