#include "sensors_class.h"
//#define DEBUG
Sensors::Sensors(uint8_t pin, DHTesp::DHT_MODEL_t type)
{
    dht.setup(pin, type);
    humidity = 0.0;
    temperature = 0.0;
    for (uint8_t i = 0; i < 5; i++)
    {
        temperature_str[i] = humidity_str[i] = 0.0;
    }
}

void Sensors::get_humidity(float *hum)
{
    *hum = humidity;
}

void Sensors::get_humidity(char *buf)
{
    for (uint8_t i = 0; i < 5; i++)
    {
        buf[i] = humidity_str[i];
    }
}

void Sensors::get_temperature(float *temp)
{
    *temp = temperature;
}

void Sensors::get_temperature(char *buf)
{
    for (uint8_t i = 0; i < 5; i++)
    {
        buf[i] = temperature_str[i];
    }
}

void Sensors::routine()
{
    TempAndHumidity sensor_data = dht.getTempAndHumidity();
    humidity = sensor_data.humidity;
    temperature = sensor_data.temperature;
    if (humidity != humidity)
    {
#ifdef DEBUG
        Serial.print("Hum NULL ");
#endif
        humidity = 0.0;
    }
    if (temperature != temperature)
    {
        temperature = 0.0;
 #ifdef DEBUG
          Serial.print("temperature NULL ");
#endif      
    }
    sprintf(humidity_str, "%.1lf", humidity);
    sprintf(temperature_str, "%.1lf", temperature);
    //sprintf(humidity_str, "%.1lf", 16.82);
    //sprintf(temperature_str, "%.1lf", 72.99);
 #ifdef DEBUG
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("  Humidity: ");
    Serial.println(humidity);
#endif
}

SENSORS_STATUS Sensors::test()
{
    TempAndHumidity sensor_data = dht.getTempAndHumidity();
    if (sensor_data.humidity != sensor_data.humidity)
    {
        humidity = 0.0;
        return SENSORS_ERR;
    }
    if (sensor_data.temperature != sensor_data.temperature)
    {
        temperature = 0.0;
        return SENSORS_ERR;
    }
    return SENSORS_OK;
}