#include "lamp.h"
#include "time_class.h"
#define DEBUG
Lamp::Lamp(uint8_t m_pin, uint16_t lp_on, uint16_t lp_off, 
            uint16_t lp_on_wkn, uint16_t lp_off_wkn, uint8_t m_internal_state = 0, uint8_t m_external_state = 0)
{
    pin = m_pin;
    internal_state = m_internal_state;
    external_state = m_external_state;
    lamp_on_time = lp_on;
    lamp_off_time = lp_off;
    lamp_on_time_wkn = lp_on_wkn;
    lamp_off_time_wkn= lp_off_wkn;
    pinMode(pin, OUTPUT);
}
Lamp::Lamp(uint8_t m_pin, uint16_t lp_on, uint16_t lp_off, uint8_t m_internal_state = 0, uint8_t m_external_state = 0)
{
    pin = m_pin;
    internal_state = m_internal_state;
    external_state = m_external_state;
    lamp_on_time = lp_on;
    lamp_off_time = lp_off;
    lamp_on_time_wkn = lp_on;
    lamp_off_time_wkn= lp_off;
    pinMode(pin, OUTPUT);
}

void Lamp::Init(void)
{
    nvs_open("storage", NVS_READWRITE, &my_handle);
    nvs_get_u32(my_handle, "time_on", &lamp_on_time);
    nvs_get_u32(my_handle, "time_off", &lamp_off_time);
    nvs_get_u32(my_handle, "time_on_wkn", &lamp_on_time_wkn);
    nvs_get_u32(my_handle, "time_off_wkn", &lamp_off_time_wkn);
    nvs_commit(my_handle);
    nvs_close(my_handle);
}

WirelessLamp::WirelessLamp(uint8_t m_pin, uint16_t lp_on, uint16_t lp_off,
                           uint16_t lp_on_wkn, uint16_t lp_off_wkn, uint8_t m_internal_state, uint8_t m_external_state)
    : Lamp(m_pin, lp_on, lp_off, lp_on_wkn, lp_off_wkn, m_internal_state, m_external_state)
{
    pinMode(m_pin, OUTPUT);
    mySwitch = RCSwitch();
    mySwitch.enableTransmit(pin);
}

WirelessLamp::WirelessLamp(uint8_t m_pin, uint16_t lp_on, uint16_t lp_off, uint8_t m_internal_state, uint8_t m_external_state)
    : Lamp(m_pin, lp_on, lp_off, m_internal_state , m_external_state)
{
    pinMode(m_pin, OUTPUT);
    mySwitch = RCSwitch();
    mySwitch.enableTransmit(pin);
}

void WireLamp::routine()
{
#ifdef DEBUG
   Serial.print("lamp_on_time: ");
    Serial.print(lamp_on_time);
    Serial.print("  lamp_off_time: ");
    Serial.print(lamp_off_time); 
    Serial.print("lamp_on_time_wkn: ");
    Serial.print(lamp_on_time_wkn);
    Serial.print("  lamp_off_time_wkn: ");
    Serial.print(lamp_off_time_wkn); 
    Serial.print("  internal_state: ");
    Serial.print(internal_state); 
    Serial.print("  external_state: ");
    Serial.print(external_state); 
    Serial.print("  day minutes: ");
    Serial.println(Time_t::get_day_minutes()); 
#endif
if ((Time_t::get_day_of_week() > 0) && (Time_t::get_day_of_week() < 6))
  {
    if ((Time_t::get_day_minutes() > lamp_on_time) && (Time_t::get_day_minutes() < lamp_off_time))
    {
      internal_state = 1;
    }
    else
    {
      Serial.println("TURN OFF 1");   
      internal_state = 0;
    }
  }
  else
  {
    if ((Time_t::get_day_minutes() > lamp_on_time_wkn) && (Time_t::get_day_minutes() < lamp_off_time_wkn))
    {
      internal_state = 1;
    }
    else
    {
        Serial.println("TURN OFF 2");  
        internal_state = 0;
    }
  }
  if (internal_state && external_state)
  {
      digitalWrite(pin, LOW); // ??????????????????
      real_state = 1;
  }
  else
  {
      digitalWrite(pin, HIGH); // ????????????????????
      real_state = 0;
  }
}

void WirelessLamp::routine()
{
    if ((external_state == 1) && (tx_flag == 0))
    {
        ESP_ERROR_CHECK(uart_disable_rx_intr(EX_UART_NUM));
        mySwitch.send(5584140, 24); //on
        ESP_ERROR_CHECK(uart_enable_rx_intr(EX_UART_NUM));
        tx_flag = 1;
        real_state = 1;
    }
    else if((external_state == 0) && (tx_flag == 1))
    {
        ESP_ERROR_CHECK(uart_disable_rx_intr(EX_UART_NUM));
        mySwitch.send(5584320, 24); //off
        ESP_ERROR_CHECK(uart_enable_rx_intr(EX_UART_NUM));
        tx_flag = 0;
        real_state = 0;
    }
}

void Lamp::set_external_state(uint8_t state)
{
    if ((state == 0) || (state == 1))
    {
        Serial.printf("TURN OFF 3 EX %d", state);   
        this->external_state = state;
    }
}
char Lamp::get_external_state()
{
    return '0' + this->external_state;
}
char Lamp::get_real_state()
{
    return '0' + this->real_state;
}

void WireLamp::get_time(char *str)
{
    sprintf(str, "%d,%d;%d,%d;", lamp_on_time, lamp_off_time, lamp_on_time_wkn, lamp_off_time_wkn);
}

void WireLamp::set_time(const char *str)
{
    sscanf(str, "%d,%d;%d,%d", &lamp_on_time, &lamp_off_time, &lamp_on_time_wkn, &lamp_off_time_wkn);
    nvs_open("storage", NVS_READWRITE, &my_handle);
    nvs_set_u32(my_handle, "time_on", lamp_on_time);
    nvs_set_u32(my_handle, "time_off", lamp_off_time);
    nvs_set_u32(my_handle, "time_on_wkn", lamp_on_time_wkn);
    nvs_set_u32(my_handle, "time_off_wkn", lamp_off_time_wkn);
    nvs_commit(my_handle);
    nvs_close(my_handle);
}