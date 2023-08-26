#include <Arduino.h>
#include <WebServer.h>
#include <esp_camera.h> // ESP32CAM相关函数的库函数

WebServer server(80); // 网络服务器标准http端口号为80

const char HEADER[] = "HTTP/1.1 200 OK\r\n"
                      "Access-Control-Allow-Origin: *\r\n"                                                    // 允许跨域访问
                      "Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n"; // multipart/x-mixed-replace表示响应体由多部份组成，通常用于实时或流式传输

const char BOUNDARY[] = "\r\n--123456789000000000000987654321\r\n"; // 边界标识符，每部分以这个标识符进行分割

// Content-Type: image/jpeg\r\n，表示响应体中的内容类型为图像
// Content-Length: 表示响应体中的数据长度，长度部分留空
// 在实际发送响应时，需要在这个字符串后面动态添加实际图像数据的长度，以便客户端可以正确读取响应体的长度。
const char CTNTTYPE[] = "Content-Type: image/jpeg\r\nContent-Length: ";

const int hdrLen = strlen(HEADER);
const int bdrLen = strlen(BOUNDARY);
const int cntLen = strlen(CTNTTYPE);

// 处理视频流数据
void HandleJpegStream()
{
    char buf[32]; // 存储数据长度的字符串
    short len;    // 存储图像的长度

    WiFiClient client = server.client();
    client.write(HEADER, hdrLen);
    client.write(BOUNDARY, bdrLen);

    while (1)
    {
        if (!client.connected())
            break;

        camera_fb_t *fb = esp_camera_fb_get(); // 获取图像帧
        len = fb->len;                         // 缓冲区的长度(字节)
        uint8_t *Imagebuf = fb->buf;           // 图像数据帧

        sprintf(buf, "%d\r\n\r\n", len); // 将图像数据的长度len格式化为字符串buf

        // 向客户端发送数据
        client.write(CTNTTYPE, cntLen); // 发送数据类型
        client.write(buf, strlen(buf)); // 在HTTP协议中，接收方需要事先知道即将接收的数据的长度，以便正确解析和处理响应。
        client.write(Imagebuf, len);    // 将实际图像数据写入客户端
        client.write(BOUNDARY, bdrLen); // 标记每个图像部分的结束，使得客户端能够正确解析多部分的响应体
        esp_camera_fb_return(fb);
    }
}

void HandleNotFound()
{
    String message = "Server is running!\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    server.send(200, "text / plain", message);
}

void CAMWebServer(void *pt)
{
    server.on("/video", HandleJpegStream);
    server.onNotFound(HandleNotFound);
    server.begin();

    TickType_t xLastWakeTime = xTaskGetTickCount();  // 获取当前时间
    const TickType_t xFrequency = pdMS_TO_TICKS(80); // 毫秒转换为内部时间单位 TickType_t

    while (1)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        server.handleClient();
        taskYIELD();
    }
}