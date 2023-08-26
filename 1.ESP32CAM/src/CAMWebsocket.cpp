#include <Arduino.h>
#include <ArduinoWebsockets.h>
#include <esp_camera.h> // ESP32CAM相关函数的库函数

const char *websockets_server_host = "192.168.0.200"; // ESP32C3的IP
const uint16_t websockets_server_port = 8888;         // 通信端口

void CAMWebsocket(void *pt)
{
    using namespace websockets;
    WebsocketsClient client;
    while (!client.connect(websockets_server_host, websockets_server_port, "/"))
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("Socket Connected!");

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(80);

    while (1)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        camera_fb_t *fb = esp_camera_fb_get();
        uint8_t *imagebuff = fb->buf;
        size_t imagelen = fb->len;

        client.sendBinary((const char *)imagebuff, imagelen); // 发送数据
        esp_camera_fb_return(fb);
        taskYIELD();
    }
}