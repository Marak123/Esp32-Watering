#ifndef TIME_H
#define TIME_H

#include <time.h>
#include "struct.h"

namespace ownTime
{
    //Klasa odpowiedzialna za dostarczanie czasu
    class ownTime
    {
    public:
        struct tm timeinfo;
        ownTime(const char *ntpServer = "pool.ntp.org", const long gmtOffset_sec = 3600, const int daylightOffset_sec = 3600)
        {
            configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        }

        void getTime()
        {
            if (!getLocalTime(&timeinfo))
            {
                Serial.println("Błąd pobierania czasu.");
                return;
            }
        }

        int Hour()
        {
            getTime();
            char timeHour[3];
            strftime(timeHour, 3, "%H", &timeinfo);
            return atoi(timeHour);
        }
        int Minute()
        {
          getTime();
          char timeMinute[4];
          strftime(timeMinute, 4, "%M", &timeinfo);
          return atoi(timeMinute);
        }
        int Second()
        {
          getTime();
          char timeSecond[3];
          strftime(timeSecond, 3, "%S", &timeinfo);
          return atoi(timeSecond);
        }
        int Day()
        {
          getTime();
          char timeDay[3];
          strftime(timeDay, 3, "%d", &timeinfo);
          return atoi(timeDay);
        }
        String weekDay()
        {
          getTime();
          char timeWeekDay[10];
          strftime(timeWeekDay, 10, "%A", &timeinfo);
          return timeWeekDay;
        }
        int nrWeekDay(){
          String day = weekDay();
          String weekDay[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
          for(int i=0; i < 7; i++){
            if(weekDay[i] == day){
              return i + 1;
            }
          }
          return -1;
        }
        String nameMonth()
        {
          getTime();
          char timeMonth[10];
          strftime(timeMonth, 10, "%B", &timeinfo);
          return timeMonth;
        }
        int Month()
        {
          getTime();
          String month[] = {"January", "February", "March", "April", "May", "	June", "July", "August", "September", "October", "November", "December"};
          String monthName = nameMonth();
          for (int i = 0; i < 12; i++) if (monthName == month[i]) return i + 1;
          return -1;
        }
        int Year()
        {
          getTime();
          char timeYear[6];
          strftime(timeYear, 6, "%Y", &timeinfo);
          return atoi(timeYear);
        }
    };

  const int Month_30[4] = {4, 6, 9, 11};
  const int Month_31[7] = {1, 3, 5, 7, 8, 10, 12};
  const String Month_Word[12] = {"Styczeń", "Luty", "Marzec", "Kwiecień", "Maj", "Czerwiec", "Lipiec", "Sierpień", "Wrzesień", "Październik", "Listopad", "Grudzień"};

  //Sprawdzanie ilosci dni w miesiacu
  int dayINmonth(s_date date){
    for (int i = 0; i < sizeof(Month_30) / sizeof(Month_30[0]); i++)
      if (date.month == Month_30[i]) return 30;
    for (int i = 0; i < sizeof(Month_31) / sizeof(Month_31[0]); i++)
      if (date.month == Month_31[i]) return 31;
    return -1;
  }
  int dayINmonth(int month)
  {
    for (int i = 0; i < sizeof(Month_30) / sizeof(Month_30[0]); i++)
      if (month == Month_30[i]) return 30;
    for (int i = 0; i < sizeof(Month_31) / sizeof(Month_31[0]); i++)
      if (month == Month_31[i]) return 31;
    return -1;
  }

  //Sprawdzanie czy dany dzien jest ostatnim dniem w miesiacu
  bool lastDay_inMonth(s_date date)
  {
    int kt = dayINmonth(date.month);
    if (!kt) if (date.day == 30) return true;
    if (kt) if (date.day == 31) return true;
    return false;
  }

  bool lastDay_inMonth(int day, int month)
  {
    int kt = dayINmonth(month);
    if (!kt)
      if (day == 30)
        return true;
    if (kt)
      if (day == 31)
        return true;
    return false;
  }

  //Walidator daty
  bool validDate(s_date date)
  {
    if (date.month > 12) return false;
    if (date.day > dayINmonth(date.month)) return false;
    if(date.hours > 23) return false;
    if(date.minute > 59) return false;
    return true;
  }

  //Porownywanie daty ze struktury DATE
  bool compareDate(s_date tokenData, s_date today)
  {
    if ((tokenData.day > today.day && tokenData.month > today.month) || tokenData.year > today.year)
      return false;
    return true;
  };

  s_hour convertHours(String hours){
    String hour = "";
    String minute = "";
    String second = "";

    char har;
    bool mt = 0;
    bool sc = 0;
    for (int i = 0; i < hours.length(); i++)
    {
      har = hours[i];
      if(har == ':'){
        if(mt && !sc) sc = 1;
        if(!mt && !sc) mt = 1;
      }
      if(isDigit(har)){
        if(!mt && !sc) hour += har;
        if(mt && !sc) minute += har;
        if(mt && sc) second += har;
      }
    }
    return s_hour {
      hour.toInt(),
      minute.toInt(),
      second.toInt()
    };
  }
  s_date convertDate(String hours){
    String day = "";
    String month = "";
    String year = "";

    char har;
    bool mt = 0;
    bool sc = 0;
    for (int i = 0; i < hours.length(); i++)
    {
      har = hours[i];
      if(har == '/'){
        if(mt && !sc) sc = 1;
        if(!mt && !sc) mt = 1;
      }
      if(isDigit(har)){
        if(!mt && !sc) day += har;
        if(mt && !sc) month += har;
        if(mt && sc) year += har;
      }
    }
    return s_date {
      day.toInt(),
      month.toInt(),
      year.toInt(),
      0,
      0
    };
  }

  //Dodawanie jednej daty do drugiej
  s_date addDates(s_date primaryDate, s_date addDate)
  {
    int day = primaryDate.day;
    int month = primaryDate.month;
    int year = primaryDate.year;
    int hours = primaryDate.hours + addDate.hours;
    int minute = primaryDate.minute + addDate.minute;

    if(minute > 59){
      minute -= 59;
      hours += 1;
    }
    if(hours > 23){
      hours -= 23;
      day += 1;
    }
    if (day > dayINmonth(month))
    {
      day -= dayINmonth(month);
      month += 1;
    }
    if(month > 12){
      month = 1;
      year += 1;
    }

    return s_date{
      day,
      month,
      year,
      hours,
      minute
    };
  }
  s_date addDates(s_date addDate)
  {
    ownTime tm;
    tm.getTime();

    int day = tm.Day();
    int month = tm.Month();
    int year = tm.Year();
    int hours = tm.Hour();
    int minute = tm.Minute();

    if(minute > 59){
      minute -= 59;
      hours += 1;
    }
    if(hours > 23){
      hours -= 23;
      day += 1;
    }
    if (day > dayINmonth(month))
    {
      day -= dayINmonth(month);
      month += 1;
    }
    if(month > 12){
      month = 1;
      year += 1;
    }

    return s_date{
      day,
      month,
      year,
      hours,
      minute
    };
  }
  s_date addHours(String date)
  {
    ownTime tm;
    tm.getTime();
    s_hour hr = convertHours(date);
    int day = tm.Day();
    int month = tm.Month();
    int year = tm.Year();
    int hours = tm.Hour();
    int minute = tm.Minute();
    hours += hr.hour;
    minute += hr.minute;

    if(minute > 59){
      minute -= 60;
      hours += 1;
    }
    if(hours > 23){
      hours -= 24;
      day += 1;
    }
    if (day > dayINmonth(month))
    {
      day -= dayINmonth(month);
      month += 1;
    }
    if(month > 12){
      month = 1;
      year += 1;
    }
    return s_date{
      day,
      month,
      year,
      hours,
      minute
    };
  }

};

ownTime::ownTime TimeS;

#endif