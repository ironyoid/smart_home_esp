#ifndef TIME_CLASS_H
#define TIME_CLASS_H
#include "defines.h"

class Time_t
{
private:
    static uint8_t time_upload_flag;
    static uint16_t year;
    static uint8_t month;
    static uint8_t day;
    static uint8_t day_of_week_t;
    static uint16_t week_minutes;
    static uint8_t hours;
    static uint8_t minutes;
    static uint8_t seconds;
    static uint16_t milliseconds;
    static uint16_t day_minutes;
    static uint32_t timestamp;
    static OneWire ds;
    static NTPClient timeClient;
    static WiFiUDP ntpUDP;
    static TIME_STATUS write();
    static uint32_t convert_raw_ds1904(byte *d);
    static void date2timestamp();     
    static void day_of_week();
    static void timestamp2date();
    static void date2minutes();
    Time_t() {};

public:
    
    static void write_net();
    static TIME_STATUS update();
    static void initialize(uint8_t m_pin, uint32_t offset);
    static uint16_t get_year();
    static uint16_t get_week_minutes();
    static uint16_t get_day_minutes();
    static uint8_t get_month();
    static uint8_t get_day();
    static uint8_t get_day_of_week();
    static uint8_t get_hours();
    static uint8_t get_minutes();
    static uint8_t get_seconds();
    static void get_timestamp(uint32_t *u_timestamp);
    static void get_timestamp(char *u_timestamp);
    static void routine();
};

#endif