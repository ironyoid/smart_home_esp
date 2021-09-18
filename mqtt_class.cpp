#include "mqtt_class.h"
#include "time_class.h"
#define DEBUG

const char *ssid = "TP-Link_A900";
const char *password = "91693020";

char           *MQTT::id;
char           *MQTT::ip;
uint16_t        MQTT::port;
char            MQTT::tx_buf[256];
uint8_t         MQTT::tx_buf_length;
AsyncMqttClient MQTT::mqttClient;
TimerHandle_t   MQTT::mqttReconnectTimer;
TimerHandle_t   MQTT::wifiReconnectTimer;

void MQTT::WiFiEvent(WiFiEvent_t event) 
{
    Serial.printf("[WiFi-event] event: %d\n", event);
    switch(event) {
    case SYSTEM_EVENT_STA_GOT_IP:
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        connectToMqtt();
        Time_t::initialize(4, 10080);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        Serial.println("WiFi lost connection");
        xTimerStop(mqttReconnectTimer, 0); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
        xTimerStart(wifiReconnectTimer, 0);
        break;
    }
}

void MQTT::timersInit()
{
    mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, 
                                    reinterpret_cast<TimerCallbackFunction_t>(MQTT::connectToMqtt));
    wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, 
                                    reinterpret_cast<TimerCallbackFunction_t>(MQTT::connectToWiFi));
}

void MQTT::connectToWiFi() 
{
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
}

void MQTT::connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void MQTT::onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  mqttClient.subscribe("/smart_home/wireless_lamp/to/", 0);
  mqttClient.subscribe("/smart_home/ir_lamp/to/", 0);
  mqttClient.subscribe("/smart_home/wire_lamp/to/", 0);
  mqttClient.subscribe("/smart_home/led_lenta/to/", 0);
//   Serial.print("Subscribing at QoS 0, packetId: ");
//   Serial.println(packetIdSub);
//   mqttClient.publish("test/lol", 0, true, "test 1");
//   Serial.println("Publishing at QoS 0");
}

void MQTT::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    xTimerStart(mqttReconnectTimer, 0);
  }
}

void MQTT::onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void MQTT::onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void MQTT::onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void MQTT::Init(void (*callback)(char*, char*, 
                AsyncMqttClientMessageProperties, size_t, size_t, size_t)) 
{
  timersInit();  
  WiFi.onEvent(WiFiEvent);  
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(callback);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_IP, MQTT_PORT);
  connectToWiFi();
}

uint16_t MQTT::Publish(const char *topic, const char *data) 
{
    return mqttClient.publish(topic, 0, true, data);
}