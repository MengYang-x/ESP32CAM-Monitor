#include <WiFi.h>
#include "LCD.h"

const char *DeviceName = "ESP32C3";
const char *ssid = "613专属";
const char *password = "613613613";

void ConnectWifi();

TaskHandle_t taskHandle; // 计算task的空间大小

void setup()
{
  Serial.begin(115200);
  ConnectWifi();
  vTaskDelay(500);
  
  xTaskCreate(Video, "", 1024 * 2, NULL, 1, &taskHandle);

  int waterMark = uxTaskGetStackHighWaterMark(taskHandle);
  Serial.print(F("Task Free Memory: ")); // 任务剩余空间
  Serial.print(waterMark);
  Serial.println(F(" Bytes"));
  vTaskDelete(NULL);
}

void loop()
{
}

void ConnectWifi()
{
  WiFi.hostname(DeviceName);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    vTaskDelay(500);
    Serial.print(F("."));
  }

  Serial.println(F(""));
  Serial.println(F("WiFi Connected!"));
  Serial.print("IP:");
  Serial.println(WiFi.localIP());
}