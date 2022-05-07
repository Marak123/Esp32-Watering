#ifndef VARS_H
#define VARS_H
#pragma once

#include "struct.h"

const int output_pin[] = {2, 4, 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33};
const int input_pin[] = {2, 4, 5, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33, 34, 35, 36, 39};

// struct s_wifi dataWifi;// = {
//     "SAGEM_1A85",
//     "63D34E7A"
// }; //Wifi dane konfiguracyjne

// struct s_ftpServer dataFtp; //Serwer ftp dane konfiguracyjne

//Konta użytkownikow
std::vector<struct s_AccountSchema> Accounts;
std::vector<float> temperatureData; //Temperatur z ostatnich 10min w petli, analizuje i włącza lub wyłącza ogrzewanie i chłodzenie
int countLoopReadTemperature = 0; //Licznik petli odczytu temperatur

// s_pin pin1 = {12, "Pin 12", "power", 0};
// s_pin pin2 = {13, "Pin 13", "power", 1};
// s_pin pin3 = {14, "Pin 14", "power", 2};
// s_pin pin4 = {15, "Pin 15", "power", 3};

// s_pin pin5 = {4, "Pin 4", "temperature", 4};
// s_pin pin6 = {17, "Pin 17", "heat", 5};
// s_pin pin7 = {18, "Pin 18", "air", 6};

// s_powerPin pow1 = {0, false, 0};
// s_powerPin pow2 = {1, true, 1};
// s_powerPin pow3 = {2, false, 2};
// s_powerPin pow4 = {3, true, 3};

// s_tempPin temp1 = {16, 5, 17, 6, 18, 7, 0};

// DHT dht(4, DHT11);
// s_tempData tempData1 = {0, 4, 6, false, 7, true, 22.5, 24.0, dht};

// s_actionList action1 = {{20, 12, 2021, 18, 54}, true, 3, 15};

// s_actionWeek actionWeek = {{2, 3, 6}, {18, 39, 0}, true, 1, 13, 0};

// s_Var varPins = {
//     {pin1, pin2, pin3, pin4, pin5, pin6, pin7},
//     {pow1, pow2, pow3, pow4},
//     {temp1},
//     {tempData1},
//     {action1},
//     {actionWeek}
// };

// s_Var varPins;

#endif