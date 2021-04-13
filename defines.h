#ifndef DEFINES_H
#define DEFINES_H
#include <IRremoteInt.h>
#include <IRremote.h>
#include <DHTesp.h>
#include "Ticker.h"
#include <RCSwitch.h>
#include "driver/uart.h"
#include <OneWire.h>
#include <Time.h>
#include <TimeLib.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "EEPROM.h"
#include "nvs_flash.h"
#include "nvs.h"
/*Тут у нас UART для ардуины*/
#define EX_UART_NUM       UART_NUM_2 
#define BUF_SIZE_U        (1024 * 2)
#define RD_BUF_SIZE       (1024)
#define RXD2              16
#define TXD2              17

#define LAMP              14  // Лампа
#define PUMP_ACCESS       27  // включение реле помпы(желтый провод)
#define ADC_PIN1          34  // Датчик потока у контейнера
#define ADC_PIN2          35  // Датчик потока у цветка
#define PUMP_UP           25  // Воду качаем наверх
#define PUMP_DOWN         26  // Воду качаем вниз (зеленый провод)
#define RF_PIN            21

#define PALLETE_ADC        32   // датчик в поддоне 
#define LAMP_ON_TIME      1080 // 7.25
#define LAMP_OFF_TIME     1260 //21.00
#define LAMP_ON_TIME_WKN  1080 // 10.00
#define LAMP_OFF_TIME_WKN 1260 // 21.00
#define LAMP_ON           digitalWrite(LAMP, LOW)
#define LAMP_OFF          digitalWrite(LAMP, HIGH)
#define DEADTIME          1440
#define EEPROM_SIZE       2
#define PUMP_ON           digitalWrite(PUMP, LOW)
#define PUMP_OFF          digitalWrite(PUMP, HIGH)
#define DHTPIN            15
#define DHTTYPE           DHT22
#define BUF_SIZE          24
#define BUF_TIME          1800
#define MLITERS           70000
#define MLITERS_TEST      3500
//#define WATERING_PERIOD 60
#define PUMP_ADC1_TRESH   1000
#define PUMP_ADC2_TRESH   400
#define PALLETE_TRESH     3000
#define IR_PIN 13           // IR transmmit pin is here
#define MQTT_IP           "192.168.0.10" 
#define MQTT_ID           "SmartHome"
#define MQTT_PORT         1883
typedef enum
{
    PUMP_NO_WATER,
    PUMP_SUCCESS,
    PUMP_TIMEOUT,
    PUMP_ERR
} PUMP_STATUS;

typedef enum
{
    CONTEINER_IS_EMPTY,
    CONTEINER_IS_FULL,
    CONTEINER_ERR
} CONTAINER_STATUS;

typedef enum
{
    DS1904_OK,
    DS1904_ERR
} TIME_STATUS;

typedef enum
{
    SENSORS_OK,
    SENSORS_ERR
} SENSORS_STATUS;

struct Schedule
{
    uint16_t minutes;
    uint32_t mliters; // Сколько воды льем, по факту это просто задержка
};
extern nvs_handle my_handle;

#endif