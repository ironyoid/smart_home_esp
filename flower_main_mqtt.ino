#include "defines.h"
#include "water_class.h"
#include "time_class.h"
#include "sensors_class.h"
#include "mqtt_class.h"
#include "lamp.h"
#include "ir_devices.h"
const char *ssid = "TRAP_HATA";
const char *password = "LwwA9SRuH";
String cantainer_str[] = {"CONTEINER_IS_EMPTY", "CONTEINER_IS_FULL", "CONTEINER_ERR"};
char *topic_list[] = {"/smart_home/temp/from","/smart_home/hum/from", "/smart_home/time/from", "/smart_home/wireless_lamp/from",
                      "/smart_home/ir_lamp/from", "/smart_home/wire_lamp/from"};
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

irLamp        ir_lamp(IR_PIN, 0);
irMusic        music_ir(IR_PIN, 0);
static void IRAM_ATTR uart_intr_handle(void *arg)
{
  static portBASE_TYPE xHigherPriorityTaskWoken;
  uint16_t rx_fifo_len, status;
  uint16_t i;
  uint8_t rxbuf[256];
  status = UART2.int_st.val;             // read UART interrupt Status
                                         //Serial.println("INSIDE ISR");
  rx_fifo_len = UART2.status.rxfifo_cnt; // read number of bytes in UART buffer

  while (rx_fifo_len)
  {
    rxbuf[i++] = UART2.fifo.rw_byte; // read all bytes
    rx_fifo_len--;
  }

  if (rxbuf[0] == 0x0A)
  {
    xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(uart_mutex, &xHigherPriorityTaskWoken);
  }
  uart_clear_intr_status(EX_UART_NUM, UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR);
}

void uart_task(void *parameter)
{
  while (1)
  {
    //if(uart_flag == 1)
    //{
    //xSemaphoreTake(uart_mutex, portMAX_DELAY);
    // color_convert(buf_rt);
   // const char *ret = (const char *)buf_rt;
   // uart_write_bytes(EX_UART_NUM, ret, strlen(ret));
   // uart_flag = 0;
    //}
    delay(1000);
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

void mqtt_task(void *parameter)
{
  while (1)
  {
    char temp[5], hum[5], timestamp[9];
    dht_1.get_temperature(temp);
    dht_1.get_humidity(hum);
    Time_t::get_timestamp(timestamp);

    MQTT::connect(topic_list[0]);
    MQTT::send(temp, 5, topic_list[0]);
    MQTT::connect(topic_list[1]);
    MQTT::send(hum, 5, topic_list[1]);
    MQTT::connect(topic_list[2]);
    MQTT::send(timestamp, 10, topic_list[2]);
    MQTT::connect(topic_list[3]);
    MQTT::send(wireless_lamp.get_real_state(), topic_list[3]);
    MQTT::connect(topic_list[4]);
    MQTT::send(ir_lamp.get_real_state(), topic_list[4]);
    MQTT::connect(topic_list[5]);
    MQTT::send(wire_lamp.get_real_state(), topic_list[5]);
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
  pump_1.set_mliters(test_schedule);
  pump_1.set_test_mliters(MLITERS_TEST);
  pump_1.set_deadtime(DEADTIME);
  CONTAINER_STATUS water_test = pump_1.water_tester();
  TIME_STATUS time_test =Time_t::update();
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
  ESP_ERROR_CHECK(uart_isr_register(EX_UART_NUM, uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &handle_console));
  ESP_ERROR_CHECK(uart_enable_rx_intr(EX_UART_NUM));

  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Time_t::initialize(4, 10080);
  MQTT::initialize("192.168.0.10", "IoT_1", 1883);
  xTaskCreate(th_task, "th_task", 10000, NULL, 1, NULL);
  xTaskCreate(time_task, "TimeWorkflow", 4096, NULL, 1, NULL);
  xTaskCreate(water_task, "water_task", 4096, NULL, 1, NULL);
  xTaskCreate(uart_task, "uart_task", 4096, NULL, 1, NULL);
  xTaskCreate(mqtt_task, "mqtt_task", 4096, NULL, 2, NULL);
}

void loop()
{
  wire_lamp.routine();
  wireless_lamp.routine();
  delay(1000);
}
