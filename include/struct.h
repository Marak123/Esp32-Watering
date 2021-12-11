#ifndef STRUCT_H
#define STRUCT_H

#include <vector>
#include <cstdint>
#include <DHT.h>

struct s_date
{
    int day;
    int month;
    int year;
    int hours;
    int minute;
};

struct s_hour{
    int hour;
    int minute;
    int second;
};

struct s_session
{
    struct s_date expDate;
    String token;
};

struct s_AccountSchema
{
    String username;
    String password;
    struct s_date dateCreate;
    struct s_session mobileToken;
    std::vector<struct s_session> session;
    int dayExp_mainSession;
    int dayExp_mobileSession;
};

struct s_pin
{
    int nrPin;
    String namePin;
    String actionPin;
    int idPin;
};

struct s_powerPin
{
    int idPin;
    bool power;
    int idPower;
};

struct s_tempPin
{
    int nrPinTemp;
    int idPinTemp;
    int nrPinHeat;
    int idPinHeat;
    int nrPinAir;
    int idPinAir;
    int idTemp;
};

struct s_tempData
{
    int idTemp;
    int idPinTemp;
    int idPinHeat;
    bool powerHeat;
    int idPinAir;
    bool powerAir;
    float curlTemp;
    float primTemp;
    DHT handleRead;
};

struct s_actionList
{
    struct s_date time;
    bool action;
    int idPin;
    int nrPin;
};

struct s_actionWeek{
    std::vector<int> arrayWeek;
    struct s_hour time;
    bool action;
    int idPin;
    int nrPin;
    int weekCount;
};

struct s_Var
{
    std::vector<struct s_pin> pins;
    std::vector<struct s_powerPin> powerPins;
    std::vector<struct s_tempPin> tempPins;
    std::vector<struct s_tempData> tempData;
    std::vector<struct s_actionList> actionList;
    std::vector<struct s_actionWeek> actionWeek;
};

struct s_wifi
{
    char* SSID;
    char* password;
    char localIP[4];
    char gateway[4];
    char subnet[4];
};

struct s_ftpServer
{
    char addrIP[];
    char username[];
    char password[];
};

#endif