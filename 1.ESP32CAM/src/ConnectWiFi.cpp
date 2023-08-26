#include <Arduino.h>
#include <WiFi.h>

// 配置静态IP
// IPAddress staticIP(192, 168, 212, 126); // 指定开发板的静态IP
// IPAddress gateway(192, 168, 212, 82);   // 网关，路由器的IP地址
// IPAddress subnet(255, 255, 255, 0);     // 子网掩码

const char *DeviceName = "ESP32CAM";
const char *ssid = "613专属";
const char *password = "613613613";

void ConnectWifi()
{
    WiFi.hostname(DeviceName);
    // WiFi.config(staticIP, gateway, subnet);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(500);
        Serial.print(F("."));
    }
    Serial.println(F(""));
    Serial.println(F("WiFi Connected!"));
    Serial.print("ESP32CAM IP:");
    Serial.println(WiFi.localIP());
    Serial.print("Stream Link: http://");

    Serial.print(WiFi.localIP());
    Serial.println("/video");
}