  #ifndef LAMP_H
  #define LAMP_H
  #include "defines.h"

class Lamp
{
    protected:
            uint16_t lamp_on_time;
            uint16_t lamp_off_time;
            uint16_t lamp_on_time_wkn;
            uint16_t lamp_off_time_wkn;
            uint8_t internal_state;
            uint8_t external_state;
            uint8_t real_state;
            uint8_t pin;

    public:
            void set_external_state(uint8_t state);
            char get_real_state();
            char get_external_state();

    protected:
    Lamp(uint8_t m_pin, uint16_t lp_on, uint16_t lp_off, 
            uint16_t lp_on_wkn, uint16_t lp_off_wkn, uint8_t m_internal_state, uint8_t m_external_state);
    Lamp(uint8_t m_pin, uint16_t lp_on, uint16_t lp_off, uint8_t m_internal_state, uint8_t m_external_state);
};

class WireLamp : public Lamp
{
    private:

    public:
    void routine();
    WireLamp(uint8_t m_pin, uint16_t lp_on, uint16_t lp_off, 
            uint16_t lp_on_wkn, uint16_t lp_off_wkn, uint8_t m_internal_state, uint8_t m_external_state) 
            : Lamp(m_pin, lp_on, lp_off, lp_on_wkn, lp_off_wkn, m_internal_state, m_external_state) {}
    WireLamp(uint8_t m_pin, uint16_t lp_on, uint16_t lp_off, uint8_t m_internal_state, uint8_t m_external_state)
            : Lamp(m_pin, lp_on, lp_off, m_internal_state, m_external_state) {}
};

class WirelessLamp : public Lamp
{
    private:
    RCSwitch mySwitch; 
    uint8_t tx_flag;
    public:
    void routine();
    WirelessLamp(uint8_t m_pin, uint16_t lp_on, uint16_t lp_off, 
            uint16_t lp_on_wkn, uint16_t lp_off_wkn, uint8_t m_internal_state, uint8_t m_external_state);
    WirelessLamp(uint8_t m_pin, uint16_t lp_on, uint16_t lp_off, uint8_t m_internal_state, uint8_t m_external_state);
};


#endif