#ifndef VARS_H
#define VARS_H

#include "struct.h"

struct s_wifi dataWifi = {
    "SAGEM_1A85",
    "63D34E7A"
}; //Wifi dane konfiguracyjne

struct s_ftpServer dataFtp; //Serwer ftp dane konfiguracyjne

//Konta użytkownikow
std::vector<struct s_AccountSchema> Accounts;

s_pin pin1 = {12, "Pin 12", "power", 0};
s_pin pin2 = {13, "Pin 13", "power", 1};
s_pin pin3 = {14, "Pin 14", "power", 2};
s_pin pin4 = {15, "Pin 15", "power", 3};

s_pin pin5 = {16, "Pin 16", "temperature", 4};
s_pin pin6 = {17, "Pin 17", "heat", 5};
s_pin pin7 = {18, "Pin 18", "air", 6};

s_powerPin pow1 = {0, false, 0};
s_powerPin pow2 = {1, true, 1};
s_powerPin pow3 = {2, false, 2};
s_powerPin pow4 = {3, true, 3};

s_tempPin temp1 = {16, 5, 17, 6, 18, 7, 0};

s_tempData tempData1 = {0, 6, false, 7, true, 22.5, 24.0};

s_actionList action1 = {{20, 12, 2021, 18, 54}, true, 3, 15};

s_Var varPins = {
    {pin1, pin2, pin3, pin4, pin5, pin6, pin7},
    {pow1, pow2, pow3, pow4},
    {temp1},
    {tempData1},
    {action1}
};

#endif