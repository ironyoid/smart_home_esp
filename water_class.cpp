#include "water_class.h"
#include "time_class.h"
//#define DEBUG
Water::Water(uint8_t adc_1_pin, uint8_t adc_2_pin, uint8_t adc_pallet_pin, uint8_t pump_up_pin, uint8_t pump_down_pin,
             uint8_t pump_access_pin, uint16_t adc_1_tresh, uint16_t adc_2_tresh, uint16_t adc_pallet_tresh)
{
    this->adc_1_pin = adc_1_pin;
    this->adc_2_pin = adc_2_pin;
    this->adc_pallet_pin = adc_pallet_pin;
    this->pump_up_pin = pump_up_pin;
    this->pump_down_pin = pump_down_pin;
    this->pump_access_pin = pump_access_pin;
    this->adc_1_tresh = adc_1_tresh;
    this->adc_2_tresh = adc_2_tresh;
    this->adc_pallet_tresh = adc_pallet_tresh;
    this->test_mliters = 0;
    memset(water_schedule, 0, sizeof(Schedule) * 4);
    water_flag = 0;
    pinMode(pump_access_pin, OUTPUT);
    pinMode(pump_up_pin, OUTPUT);
    pinMode(pump_down_pin, OUTPUT);
    pinMode(adc_1_pin, INPUT);
    pinMode(adc_2_pin, INPUT);
    pinMode(adc_pallet_pin, INPUT);
    digitalWrite(pump_access_pin, HIGH);
    digitalWrite(pump_up_pin, LOW);
    digitalWrite(pump_down_pin, LOW);
    water_tester();

}

void Water::routine()
{

    uint16_t week_minutes = Time_t::get_week_minutes();
    uint32_t timestamp;
    Time_t::get_timestamp(&timestamp);
    uint16_t shedule_minutes = 0;
    uint32_t mliters = 0;
    for(uint8_t i = 0; i < 4; i++)
    {
        if((week_minutes == water_schedule[i].minutes) && (water_schedule[i].mliters != 0))
        {
            shedule_minutes = water_schedule[i].minutes;
            mliters = water_schedule[i].mliters;
            break;
        }
    }
    if(!shedule_minutes)
    {
        water_flag = 0;
    }
    if ((shedule_minutes != 0) && (mliters != 0) && (water_flag == 0))
    {

        uint32_t loc_time = 0;
        nvs_open("storage", NVS_READWRITE, &my_handle);
        nvs_get_u32(my_handle, "whater_time", &loc_time);
        nvs_close(my_handle);
 #ifdef DEBUG
        Serial.print("timestamp - loc_time: ");
        Serial.println(timestamp - loc_time);
        Serial.print("deadtime:  ");
        Serial.println(deadtime);
#endif
        if ((timestamp - loc_time) > (deadtime * 60))
        {
#ifdef DEBUG
            Serial.print("is pallet full: ");
            Serial.println(Water::is_pallet_full());
#endif
            if (!Water::is_pallet_full())
            {
                if (water_state == CONTEINER_IS_FULL)
                {
                    nvs_open("storage", NVS_READWRITE, &my_handle);
                    nvs_set_u32(my_handle, "whater_time", timestamp);
                    nvs_commit(my_handle);
                    nvs_close(my_handle);
                    Water::watering(mliters);
                    Water::pump_whater_down();
#ifdef DEBUG
                    Serial.println("Watering succsess...");
#endif
                    water_flag = 1;
                }
                 Water::water_tester();
             }
        }
    }
}

PUMP_STATUS Water::pump_whater_down()
{
    uint16_t adc1, adc2;
    uint8_t counter = 0;
    adc1 = analogRead(adc_1_pin);
    adc2 = analogRead(adc_2_pin);
    if ((adc1 < adc_1_tresh) && (adc2 < adc_2_tresh))
    {
#ifdef DEBUG
        Serial.println("NO WHATER");
#endif
        pump_state = PUMP_NO_WATER;
        return pump_state;
    }
    else
    {
        digitalWrite(pump_access_pin, LOW);
        digitalWrite(pump_up_pin, LOW);
        digitalWrite(pump_down_pin, HIGH);
        while (((adc1 > adc_1_tresh) || (adc2 > adc_2_tresh)) && (counter < 5))
        {
#ifdef DEBUG
            Serial.print("adc1: ");
            Serial.println(adc1);
            Serial.print("adc2: ");
            Serial.println(adc2);
            Serial.println(counter);
#endif
            delay(5000);
            adc1 = analogRead(adc_1_pin);
            adc2 = analogRead(adc_2_pin);
            counter++;
        }
        digitalWrite(pump_access_pin, HIGH);
        digitalWrite(pump_up_pin, LOW);
        digitalWrite(pump_down_pin, LOW);
        adc1 = analogRead(adc_1_pin);
        adc2 = analogRead(adc_2_pin);
        if ((adc1 < adc_1_tresh) && (adc2 < adc_2_tresh))
        {
#ifdef DEBUG
            Serial.println("PUMP_SUCCSES");
#endif 
            pump_state = PUMP_SUCCESS;
            return pump_state;
        }
        else
        {
            if (counter >= 5)
            {
#ifdef DEBUG
                Serial.println("TIME IS OVER");
#endif 
                pump_state = PUMP_TIMEOUT;
                return pump_state;
            }
            else
            {
#ifdef DEBUG
                Serial.println("WTF_ERR");
#endif 
                pump_state = PUMP_ERR;
            }

            return pump_state;
        }
    }
}
CONTAINER_STATUS Water::water_tester()
{
    uint16_t conteiner_adc = analog_read_fit(adc_1_pin, 10);

    if (conteiner_adc > adc_1_tresh)
    {
        pump_whater_down();
    }

    if (conteiner_adc <= adc_1_tresh)
    {
        digitalWrite(pump_access_pin, LOW);     // качаем вверх
        digitalWrite(pump_up_pin, HIGH);
        digitalWrite(pump_down_pin, LOW);
        delay(test_mliters);
        digitalWrite(pump_access_pin, HIGH);   // выключаем
        digitalWrite(pump_up_pin, LOW);
        digitalWrite(pump_down_pin, LOW);

        conteiner_adc = analog_read_fit(adc_1_pin, 10);

        if (conteiner_adc > adc_1_tresh)
        {
            water_state = CONTEINER_IS_FULL;
            pump_state = pump_whater_down();
        }
        else
        {
            pump_state = pump_whater_down();
            water_state = CONTEINER_IS_EMPTY;
        }
    }
    else
        water_state = CONTEINER_ERR;
#ifdef DEBUG
    Serial.print("water_state:  ");
    Serial.print(water_state);
    Serial.print("  pump_state:  ");
    Serial.println(pump_state);
#endif
    return water_state;
}

void Water::watering(uint32_t mliters)
{
    digitalWrite(pump_access_pin, LOW);
    digitalWrite(pump_up_pin, HIGH);
    digitalWrite(pump_down_pin, LOW);
    delay(mliters);
    digitalWrite(pump_access_pin, HIGH);
    digitalWrite(pump_up_pin, LOW);
    digitalWrite(pump_down_pin, LOW);
}

uint8_t Water::is_pallet_full()
{
    if(analog_read_fit(adc_pallet_pin, 10) > adc_pallet_tresh) return 0;
    else return 1;
}
uint32_t Water::analog_read_fit(uint8_t ch, uint8_t par)
{
    uint32_t mes = 0;

    for (uint8_t i = 0; i < par; i++)
    {
        mes += analogRead(ch);
    }

    return mes / par;
}

void Water::set_mliters(Schedule *water_schedule)
{
    this->water_schedule[0] = water_schedule[0];
    this->water_schedule[1] = water_schedule[1];
    this->water_schedule[2] = water_schedule[2];
    this->water_schedule[3] = water_schedule[3];
}

void Water::get_mliters(Schedule *water_schedule)
{
    water_schedule[0] = this->water_schedule[0];
    water_schedule[1] = this->water_schedule[1];
    water_schedule[2] = this->water_schedule[2];
    water_schedule[3] = this->water_schedule[3];
}

void Water::set_test_mliters(uint32_t mliters)
{
    if (mliters > 0)
        this->test_mliters = mliters;
}
uint16_t Water::get_deadtime()
{
    return deadtime;
}
void Water::set_deadtime(uint16_t deadtime)
{
    this->deadtime = deadtime;
}

CONTAINER_STATUS Water::get_container_state()
{
    return water_state;
}

PUMP_STATUS Water::get_pump_state()
{
    return pump_state;
}
