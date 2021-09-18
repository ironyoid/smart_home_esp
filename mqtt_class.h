#ifndef MQTT_H
#define MQTT_H
#include "defines.h"
#include "AsyncMqttClient.h"
#include "AsyncTCP.h"

class MQTT
{
    private:
    static AsyncMqttClient mqttClient;
    static char *id;
    static char *ip;
    static uint16_t port;
    static char tx_buf[256];
    static uint8_t tx_buf_length;
    static TimerHandle_t mqttReconnectTimer;
    static TimerHandle_t wifiReconnectTimer;
    static void WiFiEvent(WiFiEvent_t event);
    static void timersInit(void);
    static void connectToMqtt(void);
    static void connectToWiFi(void);
    static void onMqttConnect(bool sessionPresent);
    static void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    static void onMqttSubscribe(uint16_t packetId, uint8_t qos);
    static void onMqttUnsubscribe(uint16_t packetId);
    static void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, 
                              size_t len, size_t index, size_t total);
    MQTT() {};
    public:
    static uint16_t Publish(const char *topic, const char *data);
    static void Init(void (*callback)(char*, char*, 
                AsyncMqttClientMessageProperties, size_t, size_t, size_t));
    static void onMqttPublish(uint16_t packetId);                          
};

#endif