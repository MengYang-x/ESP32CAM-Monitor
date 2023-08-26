#include <Arduino.h>
#include "ConnectWiFi.h"
#include "CAM_Init.h"
#include "CAMWebserver.h"
#include "CAMWebsocket.h"

TaskHandle_t taskHandle; // 计算task的空间大小

void setup()
{
  Serial.begin(115200);
  InitCamera();
  ConnectWifi();

  xTaskCreatePinnedToCore(CAMWebServer, "", 1024 * 5, NULL, 2, &taskHandle, 1); // 核心1，网页显示图像
  xTaskCreatePinnedToCore(CAMWebsocket, "", 1024 * 5, NULL, 2, NULL, 0);        // 核心0，把图像数据发送给ESP32C3，进行TFT显示

  int waterMark = uxTaskGetStackHighWaterMark(taskHandle);
  Serial.print(F("Task Free Memory: ")); // 任务剩余空间
  Serial.print(waterMark);
  Serial.println(F(" Bytes"));
  vTaskDelete(NULL);
}

void loop()
{
}
