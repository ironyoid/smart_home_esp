#include "ledlenta_class.h"
#define DEBUG

LEDLenta::LEDLenta(uint8_t r, uint8_t g, uint8_t b, uint8_t bright, led_lenta_mode_t mode)
{
    led_lenta.r = r;
    led_lenta.g = g;
    led_lenta.b = b;
    led_lenta.brightness = bright;
    led_lenta.mode = mode;
}

void LEDLenta::SetLED(const char *str)
{
    sscanf(str, "%d,%d,%d,%d,%d\n", &led_lenta.r, &led_lenta.g, &led_lenta.b, &led_lenta.brightness, &led_lenta.mode);
}
void LEDLenta::SetLED(led_lenta_t data)
{
    memcpy(&led_lenta, &data, sizeof(led_lenta_t));
}
void LEDLenta::GetLED(char *str) 
{
   sprintf(str, "%d,%d,%d,%d,%d\n", led_lenta.r, led_lenta.g, led_lenta.b, led_lenta.brightness, led_lenta.mode);
}
void LEDLenta::GetLED(led_lenta_t *data)
{
   memcpy(data, &led_lenta, sizeof(led_lenta_t));
}