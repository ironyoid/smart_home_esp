#ifndef LED_H
#define LED_H
#include "defines.h"
class LEDLenta
{
protected:
    led_lenta_t led_lenta;
public:
    LEDLenta(uint8_t r, uint8_t g, uint8_t b, uint8_t bright, led_lenta_mode_t mode);
    void SetLED(const char *str);
    void SetLED(led_lenta_t data);
    void GetLED(char *str);
    void GetLED(led_lenta_t *data);
protected:
};

#endif