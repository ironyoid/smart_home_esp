  #ifndef WATER_CLASS_H
  #define WATER_CLASS_H
  #include "defines.h"

#define PALLET_TRESH        3000                            // значение АЦП при котором считаем что поддон пустой


class Water
{
    private:

    Schedule water_schedule[4];
    uint32_t water_adc;                                  // значение АЦП воды в поддоне        
    uint8_t water_flag;            
    uint32_t test_mliters; 
    uint8_t adc_1_pin;
    uint8_t adc_2_pin;
    uint8_t adc_pallet_pin;
    uint8_t pump_up_pin;
    uint8_t pump_down_pin;
    uint8_t pump_access_pin;
    uint16_t adc_1_tresh;
    uint16_t adc_2_tresh;
    uint16_t adc_pallet_tresh;  
    uint16_t deadtime; 
    PUMP_STATUS pump_state;
    CONTAINER_STATUS water_state;
    void watering(uint32_t mliters);
    uint32_t analog_read_fit(uint8_t ch, uint8_t par);
    PUMP_STATUS pump_whater_down();
    /**
     * @brief Water
    * @param adc_1_pin пин для АЦП у контейнера
    * @param adc_2_pin пин для АЦП у горшка
    * @param adc_pallet_pin   пин датчика поддона
    * @param pump_up_pin пин врашения помпы вверх
    * @param pump_down_pin пин врашения помпы вниз
    * @param pump_access_pin пин разрешения работы помпы
    * @param adc_1_tresh    граница срабатывания первого АЦП
    * @param adc_2_tresh    граница срабатывания второго АЦП
    * @param adc_pallet_tresh граница срабатывания АЦП поддона
    * @param mliters          миллилитры
    */
   
    public:
    Water(uint8_t adc_1_pin, uint8_t adc_2_pin, uint8_t adc_pallet_pin, uint8_t pump_up_pin, uint8_t pump_down_pin, 
    uint8_t pump_access_pin, uint16_t adc_1_tresh, uint16_t adc_2_tresh, uint16_t adc_pallet_tresh);
    //PUMP_STATUS pump_whater_up();
    void init(void);
    CONTAINER_STATUS water_tester();
    CONTAINER_STATUS get_container_state();
    PUMP_STATUS get_pump_state();
    uint8_t is_pallet_full();
    void get_mliters(char *str);
    void set_mliters(const char *str);
    void set_test_mliters(uint32_t mliters);
    uint16_t get_deadtime();
    void set_deadtime(uint16_t deadtime);
    void routine();
};

#endif