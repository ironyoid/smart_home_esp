

#include "defines.h"
#include "water_class.h"
#include "time_class.h"
#include "sensors_class.h"
#include "mqtt_class.h"
#include "lamp.h"
#include "ir_devices.h"
#include "ledlenta_class.h"

String cantainer_str[] = {"CONTEINER_IS_EMPTY", "CONTEINER_IS_FULL", "CONTEINER_ERR"};
const char *from_list[] = {"/smart_home/temp/from/","/smart_home/hum/from/", "/smart_home/time/from/", 
                           "/smart_home/wireless_lamp/from/", "/smart_home/ir_lamp/from/", 
                           "/smart_home/wire_lamp/real_state/from/", "/smart_home/led_lenta/from/", 
                           "/smart_home/music/from/", "/smart_home/water_state/from/",
                           "/smart_home/pump_state/from/", "/smart_home/water_schd/from/", "/smart_home/wire_lamp/time/from/",
                           "/smart_home/wire_lamp/ex_state/from/"};


String time_str[] = {"DS1904_OK", "DS1904_ERR"};
String sensors_str[] = {"SENSORS_OK", "SENSORS_ERR"};
uint8_t test_bf[5] = {1, 2, 34, 4, 125};
nvs_handle my_handle;
OneWire ds(4);
SemaphoreHandle_t uart_mutex;
QueueHandle_t uart2_queue;
intr_handle_t handle_console;
WireLamp      wire_lamp(LAMP, LAMP_ON_TIME, LAMP_OFF_TIME, LAMP_ON_TIME_WKN, LAMP_OFF_TIME_WKN, 0, 0);
WirelessLamp  wireless_lamp(RF_PIN, LAMP_ON_TIME, LAMP_OFF_TIME, LAMP_ON_TIME_WKN, LAMP_OFF_TIME_WKN, 0, 0);
Water         pump_1(ADC_PIN1, ADC_PIN2, PALLETE_ADC, PUMP_UP, PUMP_DOWN, 
                    PUMP_ACCESS, PUMP_ADC1_TRESH, PUMP_ADC2_TRESH, PALLETE_TRESH);
Sensors       dht_1(DHTPIN, DHTesp::DHT_MODEL_t::DHT22);
LEDLenta      lenta_1(0, 0, 0, 0, eLED_MODE_LAST);
irLamp        ir_lamp(IR_PIN, 0);
irMusic       music_ir(IR_PIN, 0);

void uart_task(void *parameter)
{
  while (1)
  {
    xSemaphoreTake(uart_mutex, portMAX_DELAY);
    char led_lenta_str[32];
    lenta_1.GetLED(led_lenta_str);
    uart_write_bytes(EX_UART_NUM, led_lenta_str, strlen(led_lenta_str));
  }
}

void time_task(void *parm)
{
  static uint8_t tougle_sw = 0;
  while (1)
  {
    Time_t::routine();
    delay(1000);
  }
}
void water_task(void *parameter)
{
  while (1)
  {
    pump_1.routine();
    delay(5000);
  }
}
void th_task(void *parameter)
{
  while (1)
  {
    dht_1.routine();
    delay(1000);
  }
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  if(len == 0) {
    return;
  }
  payload[len] = '\0';
  Serial.printf("Topic: %s Payload: %s Size: %d\n", topic, payload, len);

  if (!strcmp("/smart_home/wireless_lamp/to/", topic))
  {
    int st = atoi(payload);
    Serial.printf("Value = %d\n", st);
    wireless_lamp.set_external_state((uint8_t)st);
  }
  if (!strcmp("/smart_home/ir_lamp/to/", topic))
  {
    int st = atoi(payload);
    Serial.printf("Value = %d\n", st);
    if(st == 1) 
    {
      ir_lamp.turn_on();
    } 
    else if(st != 1)
    {
      ir_lamp.turn_off();
    }
  }
  if (!strcmp("/smart_home/music/to/", topic))
  {
    int st = atoi(payload);
    Serial.printf("Value = %d\n", st);
    if(st == 1) 
    {
      music_ir.turn_on();
    } 
    else if(st != 1)
    {
      music_ir.turn_off();
    }
  }
  if (!strcmp("/smart_home/wire_lamp/ex_state/to/", topic))
  {
    
    int st = payload[0] - '0';
    Serial.printf("WIRE LAMP STATE %d\n", st);
    wire_lamp.set_external_state((uint8_t)st);
  }
  if (!strcmp("/smart_home/wire_lamp/time/to/", topic))
  {
    Serial.printf("WIRE LAMP TIME \n");
    wire_lamp.set_time(payload);
  }
  if (!strcmp("/smart_home/led_lenta/to/", topic))
  {
    lenta_1.SetLED(payload);
    xSemaphoreGive(uart_mutex);
  }
  if (!strcmp("/smart_home/water_schd/to/", topic))
  {
    pump_1.set_mliters(payload);
  }
}

void mqtt_t_task(void *parameter)
{
  while (1)
  {
    char temp[5], hum[5], timestamp[9];
    char wr_l[2] = {0};
    char ir_l[2] = {0};
    char w_l[2]  = {0};
    char m_l[2]  = {0};
    char water_l[2]  = {0};
    char pump_l[2]  = {0};
    char led_lenta_str[32] = {0};
    char schd[128] = {0};
    char lamp_time[64] = {0};
    char ex_state[2] = {0};

    dht_1.get_temperature(temp);
    dht_1.get_humidity(hum);
    Time_t::get_timestamp(timestamp);
    wr_l[0] = wireless_lamp.get_real_state();
    ir_l[0] = ir_lamp.get_real_state();
    w_l[0]  = wire_lamp.get_real_state();
    m_l[0] = music_ir.get_real_state();
    water_l[0] = pump_1.get_container_state() + '0';
    pump_l[0] = (char)pump_1.get_pump_state() + '0';
    Serial.printf("water state: %d pump state %d\n", water_l[0], pump_l[0]);
    lenta_1.GetLED(led_lenta_str);
    pump_1.get_mliters(schd);
    wire_lamp.get_time(lamp_time);
    ex_state[0] = wire_lamp.get_external_state();

    MQTT::Publish(from_list[0], temp);
    MQTT::Publish(from_list[1], hum);
    MQTT::Publish(from_list[2], timestamp);
    MQTT::Publish(from_list[3], wr_l);
    MQTT::Publish(from_list[4], ir_l);
    MQTT::Publish(from_list[5], w_l);
    MQTT::Publish(from_list[6], led_lenta_str);
    MQTT::Publish(from_list[7], m_l);
    MQTT::Publish(from_list[8], water_l);
    MQTT::Publish(from_list[9], pump_l);
    MQTT::Publish(from_list[10], schd);
    MQTT::Publish(from_list[11], lamp_time);
    MQTT::Publish(from_list[12], ex_state);
    // MQTT::connect(topic_list[6]);
     //MQTT::send(led_lenta_str,strlen(led_lenta_str), topic_list[6]);
    delay(5000);
  }
}

void setup()
{
  Serial.begin(115200);

  Schedule test_schedule[4];
  test_schedule[0].minutes = 1400;
  test_schedule[0].mliters = MLITERS;
  uart_mutex = xSemaphoreCreateMutex();
  //pump_1.set_mliters("");
  pump_1.init();
  pump_1.set_test_mliters(MLITERS_TEST);
  pump_1.set_deadtime(DEADTIME);
  CONTAINER_STATUS water_test = pump_1.water_tester();
  TIME_STATUS time_test = Time_t::update();
  SENSORS_STATUS dht_test = dht_1.test();
  Serial.println("Water pump status: " + cantainer_str[water_test]);
  Serial.println("DS1904 status: " + time_str[time_test]);
  Serial.println("DHT_22 status: " + sensors_str[dht_test]);
  uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
  ESP_ERROR_CHECK(uart_param_config(EX_UART_NUM, &uart_config));
  ESP_ERROR_CHECK(uart_set_pin(EX_UART_NUM, TXD2, RXD2, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  ESP_ERROR_CHECK(uart_driver_install(EX_UART_NUM, BUF_SIZE_U, 0, 0, NULL, 0));
  ESP_ERROR_CHECK(uart_isr_free(EX_UART_NUM));
  nvs_flash_init();
  MQTT::Init(onMqttMessage);

  //MQTT::initialize("192.168.0.100", "IoT_1", 1883);
  xTaskCreate(th_task, "th_task", 10000, NULL, 1, NULL);
  xTaskCreate(time_task, "TimeWorkflow", 4096, NULL, 1, NULL);
  xTaskCreate(water_task, "water_task", 4096, NULL, 1, NULL);
  xTaskCreate(uart_task, "uart_task", 4096, NULL, 15, NULL);
//  xTaskCreate(mqtt_t_task, "mqtt_t_task", 4096, NULL, 2, NULL);
  xTaskCreate(mqtt_t_task, "mqtt_r_task", 4096, NULL, 2, NULL);
}

void loop()
{
  wire_lamp.routine();
  wireless_lamp.routine();
  delay(1000);
}
