#include "time_class.h"
//#define DEBUG
uint16_t Time_t::year;
uint8_t Time_t::month;
uint8_t Time_t::day;
uint8_t Time_t::day_of_week_t;
uint16_t Time_t::week_minutes;
uint8_t Time_t::hours;
uint8_t Time_t::minutes;
uint8_t Time_t::seconds;
uint16_t Time_t::milliseconds;
uint32_t Time_t::timestamp;
uint16_t Time_t::day_minutes;
uint8_t Time_t::time_upload_flag;
WiFiUDP Time_t::ntpUDP;
NTPClient Time_t::timeClient(ntpUDP);
OneWire Time_t::ds;
void Time_t::initialize(uint8_t m_pin, uint32_t offset)
{
    ds = OneWire(m_pin);
    timeClient.begin();
    timeClient.setTimeOffset(offset);
}

TIME_STATUS Time_t::write()
{
    byte i;
    long a;
    byte b;
    byte present = 0;
    byte type_s;
    byte data[12];
    byte addr[8];
    String arduinoTime;
    ds.reset_search();
    if (!ds.search(addr))
    {
        ds.reset_search();
        return DS1904_ERR;
    }

    data[0] = 0x0C; //Set control byte
    data[1] = timestamp & 0xFF;
    a = timestamp << 16;
    data[2] = a >> 24;
    a = timestamp << 8;
    data[3] = a >> 24;
    data[4] = timestamp >> 24;
    //Now write to the DS1904...
    ds.reset();
    ds.select(addr);
    ds.write(0x99); // 0x66=Read; 0x99=Write
    delay(20);      // maybe 750ms is enough, maybe not
    for (i = 0; i < 5; i++)
    {
        ds.write(data[i]);
    }
    ds.reset();
    return DS1904_OK;
}

void Time_t::write_net()
{

    if (timeClient.update())
    {
        // timeClient.forceUpdate();
        timestamp = timeClient.getEpochTime();
#ifdef DEBUG
        Serial.println("TIME");
        Serial.println(timestamp);
#endif
        write();
    }
}

uint32_t Time_t::convert_raw_ds1904(byte *d)
{
    uint32_t a, b;
    a = d[4];
    a = a << 24;
    b = d[3];
    b = b << 16;
    a = a | b;
    b = d[2];
    b = b << 8;
    a = a | b;
    a = a | d[1];
    return a;
    //return (d[4] * 16777216) + (d[3] * 65536) + (d[2] * 256) + d[1];
}

TIME_STATUS Time_t::update()
{
    byte i;
    byte present = 0;
    byte type_s;
    byte data[12];
    byte addr[8];
    String arduinoTime;
    ds.reset_search();

    if (!ds.search(addr))
    {
        ds.reset_search();
        return DS1904_ERR;
    }

    ds.reset();
    ds.select(addr);
    ds.write(0x66); // 0x66=Read; 0x99=Write

    delay(20);
    for (i = 0; i < 5; i++)
    { // we need 5 bytes
        data[i] = ds.read();
    }
    timestamp = convert_raw_ds1904(&data[0]);
    timestamp2date();
    day_of_week();
    date2minutes();  
    return DS1904_OK;
}

void Time_t::date2timestamp()
{
    uint8_t y;
    uint8_t m;
    uint8_t d;
    uint32_t t;

    //Year
    y = year;
    //Month of year
    m = month;
    //Day of month
    d = day;

    //January and February are counted as months 13 and 14 of the previous year
    if (m <= 2)
    {
        m += 12;
        y -= 1;
    }

    //Convert years to days
    t = (365 * y) + (y / 4) - (y / 100) + (y / 400);
    //Convert months to days
    t += (30 * m) + (3 * (m + 1) / 5) + d;
    //Unix time starts on January 1st, 1970
    t -= 719561;
    //Convert days to seconds
    t *= 86400;
    //Add hours, minutes and seconds
    t += (3600 * hours) + (60 * minutes) + seconds;
    timestamp = t;
}

void Time_t::day_of_week()
{
    uint8_t h;
    uint8_t j;
    uint8_t k;
    uint8_t m = month;
    uint16_t y = year;
    uint8_t d = day;
    //January and February are counted as months 13 and 14 of the previous year
    if (m <= 2)
    {
        m += 12;
        y -= 1;
    }

    //J is the century
    j = y / 100;
    //K the year of the century
    k = y % 100;

    //Compute H using Zeller's congruence
    h = d + (26 * (m + 1) / 10) + k + (k / 4) + (5 * j) + (j / 4);

    //Return the day of the week
    day_of_week_t = ((h + 5) % 7) + 1;
}

void Time_t::timestamp2date()
{
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f;
    uint32_t t = timestamp;
    //Negative Unix time values are not supported
    if (t < 1)
        t = 0;

    //Clear milliseconds
    milliseconds = 0;

    //Retrieve hours, minutes and seconds
    seconds = t % 60;
    t /= 60;
    minutes = t % 60;
    t /= 60;
    hours = t % 24;
    t /= 24;

    //Convert Unix time to date
    a = (uint32_t)((4 * t + 102032) / 146097 + 15);
    b = (uint32_t)(t + 2442113 + a - (a / 4));
    c = (20 * b - 2442) / 7305;
    d = b - 365 * c - (c / 4);
    e = d * 1000 / 30601;
    f = d - e * 30 - e * 601 / 1000;

    //January and February are counted as months 13 and 14 of the previous year
    if (e <= 13)
    {
        c -= 4716;
        e -= 1;
    }
    else
    {
        c -= 4715;
        e -= 13;
    }

    //Retrieve year, month and day
    year = c;
    month = e;
    day = f;

}

void Time_t::date2minutes()
{
  week_minutes = hours * 60 + minutes + (day_of_week_t - 1) * 1440;
  day_minutes = hours * 60 + minutes;
}

void Time_t::routine()
{
        update();


    if ((get_hours() > 22) && (get_minutes() > 58) && (time_upload_flag == 0))
    {
      time_upload_flag = 1;
      Time_t::write_net();
    }

    if ((Time_t::get_hours()== 0) && (Time_t::get_minutes() == 0) && (time_upload_flag == 1))
    {
      time_upload_flag = 0;
    }
 #ifdef DEBUG
    Serial.print("Hours: ");
    Serial.print(Time_t::get_hours());
    Serial.print(" Minutes: ");
    Serial.print(Time_t::get_minutes());
    Serial.print("  Seconds: ");
    Serial.print(Time_t::get_seconds());
    Serial.print("  DayOfWeek: ");
    Serial.print(Time_t::get_day_of_week());
    Serial.print("  week minutes: ");
    Serial.print(Time_t::get_week_minutes());
    Serial.println();
    Serial.print("Water pot sensor: ");
    Serial.println(analogRead(PALLETE_ADC));
    Serial.print("Conteiner sensor: ");
    Serial.println(analogRead(ADC_PIN1));
    Serial.print("Flower sensor: ");
    Serial.println(analogRead(ADC_PIN2));
    Serial.println();
#endif
}
uint16_t Time_t::get_day_minutes() {return day_minutes;}
uint16_t Time_t::get_year() { return year; }
uint16_t Time_t::get_week_minutes() { return week_minutes; }
uint8_t Time_t::get_month() { return month; }
uint8_t Time_t::get_day() { return day; }
uint8_t Time_t::get_day_of_week() { return day_of_week_t; }
uint8_t Time_t::get_hours() { return hours; }
uint8_t Time_t::get_minutes() { return minutes; }
uint8_t Time_t::get_seconds() { return seconds; }
void Time_t::get_timestamp(uint32_t *u_timestamp) 
{ 
    *u_timestamp = timestamp; 
}

void Time_t::get_timestamp(char *u_timestamp) 
{ 
    sprintf(u_timestamp, "%d", timestamp);
}