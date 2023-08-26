#include <TJpg_Decoder.h>
#include <TFT_eSPI.h>
#include <ArduinoWebsockets.h>

TFT_eSPI tft = TFT_eSPI();

using namespace websockets;
WebsocketsServer server;
WebsocketsClient client;

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    if (y >= tft.height())
        return 0;
    tft.pushImage(x, y, w, h, bitmap);
    return 1;
}

void Video(void *pt)
{
    tft.begin();
    tft.setRotation(1);
    tft.fillScreen(TFT_RED);
    tft.setSwapBytes(true);          // We need to swap the colour bytes (endianess)
    TJpgDec.setJpgScale(1);          // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
    TJpgDec.setCallback(tft_output); // 图片解码

    server.listen(8888); // 监听数据
    while (1)
    {
        if (server.poll())
        {
            client = server.accept();
        }

        if (client.available())
        {
            client.poll(); // 持续监听

            WebsocketsMessage msg = client.readBlocking(); // 接收数据

            uint32_t t = millis();

            uint16_t w = 0, h = 0; // Get the width and height in pixels of the jpeg if you wish
            TJpgDec.getJpgSize(&w, &h, (const uint8_t *)msg.c_str(), msg.length());
            Serial.print("Width = ");
            Serial.print(w);
            Serial.print(", height = ");
            Serial.println(h);

            TJpgDec.drawJpg(0, 0, (const uint8_t *)msg.c_str(), msg.length()); // Draw the image, top left at 0,0

            // How much time did rendering take (ESP8266 80MHz 271ms, 160MHz 157ms, ESP32 SPI 120ms, 8bit parallel 105ms
            t = millis() - t;
            Serial.print(t);
            Serial.println(" ms");
        }
    }
}