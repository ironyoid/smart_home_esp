  #ifndef SENSORS_CLASS_H
  #define SENSORS_CLASS_H
  #include "defines.h"

class Sensors
{
    protected:
    float humidity;
    float temperature;
    char temperature_str[5];
    char humidity_str[5];
    DHTesp dht;
    public:
    Sensors(uint8_t pin, DHTesp::DHT_MODEL_t type);
    void get_humidity(float *hum);
    void get_humidity(char *buf);
    void get_temperature(float *temp);
    void get_temperature(char *buf);
    void routine();
    SENSORS_STATUS test();
};

#endif