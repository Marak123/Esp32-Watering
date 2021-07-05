#ifndef VARIABLES
#define VARIABLES

#include "variablesSchemat.h"

const char *ssid = "SAGEM_1A85";    //Nazwa Routera WIFI
const char *password = "63D34E7A";   //Haslo do Router WIFI

char ftp_serverIP[] = "192.168.1.12";  //IP Serwera ftp
char ftp_username[] = "esp32";  //Nazwa użytkownika klienta ftp
char ftp_password[] = "haslo";  //Hasło użytkownika klienta ftp

//Konta użytkownikow
std::vector<AccountSchema> Accounts;

//Struktora zmiennych odnoszaca sie do uzywanych pinow i jakie nazwy maja nosic
Variables var = {
    {{"13", "pin 13"}, {"12", "pin 12"}, {"14", "pin 14"}, {"27", "pin 27"}, {"26", "pin 26"}, {"25", "pin 25"}, {"33", "pin 33"}, {"32", "pin 32"}},  //Uzywane piny. Formula {numer uzywanego pinu, nazwa jaki ma byc wyswietlana przy tym pinie(dowolna, aby nie za dluga)}
    {false, false, false, false, false, false, false, false}     ///Status pinu na poczatku. false - wylaczony(bez napiecia),  true - wlaczony(z napieciem).    ///UWAGA!! Ilosc stanow(true lub false) musi byc taka sama jak ilosc uzywanych pinow
};

#endif