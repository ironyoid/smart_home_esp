  #ifndef MQTT_H
  #define MQTT_H
  #include "defines.h"
  #include <PubSubClient.h>
class MQTT
{
    private:
    static WiFiClient wifi_client;
    static PubSubClient client;
    static char *id;
    static char *ip;
    static uint16_t port;
    static char tx_buf[256];
    static uint8_t tx_buf_length;
    static void rx_callback(char* topic, byte* payload, unsigned int length);
    MQTT() {};
    public:
    static void initialize(char *ip, char *id, uint16_t port);
    static void add(uint8_t data);
    static void add(uint16_t data);
    static void add(float data);
    static void add(char *data, uint8_t len);
    static void add(uint8_t *data, uint8_t len);
    static void connect(const char *topic);
    static uint8_t send(const char *data, uint8_t length, const char *topic);
    static uint8_t send(uint8_t data, const char *topic);
};

#endif