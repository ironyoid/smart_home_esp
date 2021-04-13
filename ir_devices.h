#ifndef IR_DEVICES_H
#define IR_DEVICES_H
#include "defines.h"

class IrDevices 
{
    protected:
    uint8_t real_state;
    IRsend ir_lamp;
    String topic;
    public:
    IrDevices(uint8_t m_pin, uint8_t real_state);
    char get_real_state();
    void set_topic(String topic);
    String get_topic();
    void turn_on();
};

class irLamp : public IrDevices
{
    public:
    void turn_on();
    void turn_off();
    irLamp (uint8_t m_pin, uint8_t real_state);
};

class irMusic : public IrDevices
{
    public:
    void turn_on();
    void turn_off();
    irMusic (uint8_t m_pin, uint8_t real_state);
};

#endif