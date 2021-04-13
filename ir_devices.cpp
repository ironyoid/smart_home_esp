#include "ir_devices.h"

IrDevices::IrDevices(uint8_t m_pin, uint8_t real_state) 
{
    ir_lamp = IRsend(m_pin);
    //ir_lamp.sendNEC(0x1FE48B7, 32);     // Выключение лампы
    //ir_lamp.sendNEC(0xFF85EA15, 32);     // Музыка
    this->real_state = real_state;
}

char IrDevices::get_real_state()
{
    return '0' + real_state;
}

void IrDevices::set_topic(String topic)
{
    this->topic = topic;
}

String IrDevices::get_topic()
{
    return this->topic;
}

 irLamp::irLamp(uint8_t m_pin, uint8_t real_state) : IrDevices(m_pin, real_state)
 {

 }

 irMusic::irMusic(uint8_t m_pin, uint8_t real_state) : IrDevices(m_pin, real_state)
 {

 }

 void  irLamp::turn_off()
{
    if(real_state)
    {
        for(uint8_t i = 0; i < 3; i++)
        {
            ir_lamp.sendNEC(0x1FE48B7, 32); 
        }
        real_state = 0;
    }
}

 void  irLamp::turn_on()
{
    if(!real_state)
    {
        for(uint8_t i = 0; i < 3; i++)
        {
            ir_lamp.sendNEC(0x1FE48B7, 32); 
        }
        real_state = 1;
    }
}

 void  irMusic::turn_off()
{
    if(real_state)
    {
        for(uint8_t i = 0; i < 3; i++)
        {
            ir_lamp.sendNEC(0xFF85EA15, 32); 
        }
        real_state = 0;
    }
}

 void  irMusic::turn_on()
{
    if(!real_state)
    {
        for(uint8_t i = 0; i < 3; i++)
        {
            ir_lamp.sendNEC(0xFF85EA15, 32); 
        }
        real_state = 1;
    }
}
