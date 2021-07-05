#ifndef VARIABLES_SCHEMATE
#define VARIABLES_SCHEMATE

#include <vector>
#include "function.h"

struct DATE{
  int Day = 100000;
  int Month = 100000;
  int Year = 100000;
};

struct SESSION
{
  DATE expirationDates;
  String token = "m3c8974gtr92843mht8hm2x398m9x438xhrc29834,gth97534mhrt";
  bool session = false;
};

struct AccountSchema
{
  String username;
  String password;
  DATE dateCreate;
  struct SESSION sess;
};

struct withDelayTemp
{
  int hour;
  int minute;
  std::vector<int> pins;
  int turn;
};

struct planeLaunchTemp
{
  int day;
  int month;
  int year;
  int hour;
  int minute;
  std::vector<int> pin;
  int turn;
};

struct Variables
{
  std::vector<std::vector<String>> pins;
  std::vector<bool> pinsPower;
  std::vector<withDelayTemp> withDelay;
  std::vector<planeLaunchTemp> planeLaunch;
};

#endif