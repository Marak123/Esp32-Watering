#ifndef STRUCT_H
#define STRUCT_H
#pragma once

#include <vector>
#include <cstdint>
#include <DHT.h>

// Struktura daty
struct s_date
{
    int day; //Dzień
    int month; //Miesiąc
    int year; //Rok
    int hours; //Godzina
    int minute; //Minuta
    int second; //Sekunda
};

// Struktura czasu
struct s_hour{
    int hour; //godzina
    int minute; //minuta
    int second; //sekunda
};

// Struktura przechowujaca dane o sesjach uzytkownikow
struct s_session
{
    struct s_date expDate; //Data wygasniecia sesji
    String token; //Token sesji
};

// Struktura przechowywujaca dane uzytkownika
struct s_AccountSchema
{
    // s_AccountSchema() : isAdmin(false) {}
    String username; //Nazwa uzytkownika
    String password; //Haslo
    struct s_date dateCreate; //Data utworzenia konta
    struct s_session mobileToken; //Token mobilny
    std::vector<struct s_session> session; //Sesje
    int dayExp_mainSession; //Czas wygasania sesji przegladarkowych
    int dayExp_mobileSession; //Czas wygasania sesji mobilnej
    bool isAdmin; //Czy jest to konto z uprawnieniami administratora
    std::vector<struct s_session> adminSession; //Sesje administratora
};

// Struktura przechowywujaca dane o pinie
struct s_pin
{
    int nrPin; // numer pinu na plytce esp32
    String namePin; //Spersanizowane nazwa pinu przez uzytkownika
    String actionPin; //Rodzaj akcji do wykonywania na pinie
    int idPin; //Podsawowy identyfikator pinu
};

// Struktura przechowywuje dane o pinach napieciowych, wlaczanie czegosc(pompy, chlodzenie, ogrzewanie)
struct s_powerPin
{
    int idPin; //Indetyfikator pinu ze struktury s_pin
    bool power; //Stan pinu
    int idPower; //Podstawowy Indetyfikator stanu pinu
};

// Struktura przechowywuje dane o pinach sluzacych do regulacji temperatury
struct s_tempPin
{
    int nrPinTemp; //Numer pinu z czujnikiem temeratury ze struktury s_pin
    int idPinTemp; //Identyfikator pinu z czujnikiem temeratury ze struktury s_pin
    int nrPinHeat; //Numer pinu z grzalka ze struktury s_pin
    int idPinHeat; //Identyfikator pinu z grzalka ze struktury s_pin
    int nrPinAir; //Numer pinu z wiatrakiem ze struktury s_pin
    int idPinAir; //Identyfikator pinu z wiatrakiem ze struktury s_pin
    int idTemp; //Podstawowy Indetyfikator konfiguracji sprzetowej zarzadzajacej temperatura
};

// Struktura przechowywuje dane do zarzadzania temperatura
struct s_tempData
{
    int idTemp; //Podstawowy Indetyfikator przechowywanych danych do zarzadzania temperatura
    int idPinTemp; //Identyfikator pinu z czujnikiem temperatury ze struktury s_pin
    int idPinHeat; //Identyfikator pinu z grzalka ze struktury s_pin
    bool powerHeat; //Stan grzalki
    int idPinAir; //Identyfikator pinu z wiatrakiem ze struktury s_pin
    bool powerAir; //Stan wiatraki
    float curlTemp; //Temperatura z czujnika
    float primTemp; //Utrzymywana temperatura
    DHT handleRead; //Obiekt do odczytu temperatury z czujnika
};

// Struktura przechowywuje dane zaplanowanej akcji do wynonania
struct s_actionList
{
    struct s_date time; //Czas wykonania akcji
    bool action; //Rodzaj akcji wlacz/wylacz
    int idPin; //Obcy indetyfikator pinu ze struktury s_pin
    int nrPin; //Numer pinu ze struktury s_pin
};

// Struktura przechowywuje dane do wynowywania akcji co tydzien
struct s_actionWeek{
    std::vector<int> arrayWeek; //Tablica z dniem tygodnia
    struct s_hour time; //Czas wykonania akcji
    bool action; //Rodzaj akcji wlacz/wylacz
    int idPin; //Obcy indetyfikator pinu ze struktury s_pin
    int nrPin; //Numer pinu ze struktury s_pin
    int weekCount; //Liczba tygodni z wykonana akcja
    int nrDayInWeek[2]; //Numery dni początku i końca tygodnia wykonanej już akcji
};

// Struktura przechowywujaca dane o skonfigurowanych pinach oraz akcjach zaplanowanych przez uzytkownika
struct s_Var
{
    std::vector<struct s_pin> pins; //Tablica przechowujaca dane o pinach
    std::vector<struct s_powerPin> powerPins; //Tablica przechowujaca dane o pinach zes stanem pinow (wlacz/wylacz)
    std::vector<struct s_tempPin> tempPins; //Tablica przechowujaca dane o pinach do zarzadzania temperatura
    std::vector<struct s_tempData> tempData; //Tablica przechowujaca dane do zarzadzania temperatura
    std::vector<struct s_actionList> actionList; //Tablica przechowujaca dane o akcjach zaplanowanych przez uzytkownika
    std::vector<struct s_actionWeek> actionWeek; //Tablica przechowujaca dane o cotygodniowych akcjach zaplanowanych przez uzytkownika
} varPins;

// Struktura przechowujaca dane do laczenia sie z siecia wifi
struct s_wifi
{
    String ssid; //Nazwa sieci wifi
    String password; //Haslo do sieci wifi
    IPAddress local_ip; //Adres IP lokalny statyczny
    IPAddress gateway; //Adres IP bramy domyslnej
    IPAddress subnet; //Adres IP maski podsieci
    String hostname; //Nazwa hosta
} dataWifi;

// Struktora przechowywujaca dane do laczenia sie ze zdanlnym serwerem ftp
struct s_ftpServer
{
    s_ftpServer() : port(21){}
    char *ip_address; //Adres IP serwera ftp
    char *username; //Nazwa uzytkownika
    char *password; //Haslo uzytkownika
    uint16_t port; //Port serwera ftp
} dataFtp;

#endif