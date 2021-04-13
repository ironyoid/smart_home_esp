#include "mqtt_class.h"
#define DEBUG
char *MQTT::id;
char *MQTT::ip;
uint16_t MQTT::port;
char MQTT::tx_buf[256];
uint8_t MQTT::tx_buf_length;
WiFiClient MQTT::wifi_client;
PubSubClient MQTT::client(wifi_client);

void MQTT::initialize(char *ip, char *mqtt_id, uint16_t port)
{
    client.setServer(ip, port);
    client.setCallback(rx_callback);
    id = mqtt_id;
    tx_buf_length = 0;
    memset(tx_buf, 0, sizeof(tx_buf));
}

void MQTT::rx_callback(char *topic, byte *payload, unsigned int length)
{
#ifdef DEBUG
    Serial.print("Message received: ");
    Serial.println(topic);
    Serial.print("Payload: ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
#endif
}

void MQTT::connect(const char *topic)
{
    while (!client.connected())
    {
#ifdef DEBUG
        Serial.print("MQTT connecting ...");
#endif
        if (client.connect(id))
        {
#ifdef DEBUG
            Serial.println("Connected");
            client.subscribe(topic);
#endif
        }
        else
        {
#ifdef DEBUG
            Serial.print("failed, status code =");
            Serial.print(client.state());
            Serial.println("try again in 5 seconds");
#endif
            /* Wait 5 seconds before retrying */
            delay(5000);
        }
    }
}

void MQTT::add(uint8_t data)
{

    int i = sprintf(&tx_buf[tx_buf_length], "%d", data);
    if ((i > 0) && ((i + tx_buf_length) < 255))
    {
        tx_buf_length += i;
    }
}

void MQTT::add(uint16_t data)
{
    int i = sprintf(&tx_buf[tx_buf_length], "%d", data);
    if ((i > 0) && ((i + tx_buf_length) < 255))
    {
        tx_buf_length += i;
    }
}

void MQTT::add(float data)
{
    int i = sprintf(&tx_buf[tx_buf_length], "%.1lf", data);
    if ((i > 0) && ((i + tx_buf_length) < 255))
    {
        tx_buf_length += i;
    }
}

void MQTT::add(char *data, uint8_t len)
{
    if ((tx_buf_length + len) <= 255)
    {
        memcpy(&tx_buf[tx_buf_length], data, len);
    }
}

void MQTT::add(uint8_t *data, uint8_t len)
{
    char buf_tmp[3];
    uint8_t len_tmp = 0;
    for(uint8_t i = 0; i < len; i++)
    {
        len_tmp = sprintf(buf_tmp, "%d", data[i]);
        if((tx_buf_length + len_tmp) <= 255)
        {
            memcpy(&tx_buf[tx_buf_length], buf_tmp, len_tmp);
            tx_buf_length += len_tmp;
        }
        else 
        {
            break;
        }
    }
}
uint8_t MQTT::send(const char *data, uint8_t length, const char *topic)
{
   if(client.connected())
   {
#ifdef DEBUG
        Serial.print("tx_buf: ");
        Serial.println((char *)data);
#endif
        client.publish(topic, (const char *)data, (unsigned int)length);
        tx_buf_length = 0;
   }
}

uint8_t MQTT::send(uint8_t data, const char *topic)
{
   if(client.connected())
   {
        client.publish(topic, &data, 1);
        tx_buf_length = 0;
   }
}