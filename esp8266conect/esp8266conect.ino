#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// --- CẤU HÌNH WIFI ---
const char* ssid = "Core intel 7 109999h"; 
const char* password = "nigggako";

ESP8266WebServer server(80);

void setup() {
  // Tốc độ baud 9600 để khớp với Arduino Uno trên xe
  Serial.begin(9600); 
  
  WiFi.begin(ssid, password);
  
  // In dấu chấm trong khi chờ kết nối để bạn biết mạch không bị treo
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  }

  // --- ĐOẠN CODE QUAN TRỌNG ĐỂ XEM IP ---
  Serial.println(""); // Xuống dòng cho sạch
  Serial.println("DA KET NOI WIFI THANH CONG!");
  Serial.print("DIA CHI IP CUA XE: ");
  Serial.println(WiFi.localIP()); // Dòng này sẽ hiện IP bạn cần
  Serial.println("---------------------------");

  // Các đường dẫn nhận lệnh từ Python/OpenCV
  server.on("/F", []() { Serial.print('F'); server.send(200, "text/plain", "Tien"); });
  server.on("/B", []() { Serial.print('B'); server.send(200, "text/plain", "Lui"); });
  server.on("/L", []() { Serial.print('L'); server.send(200, "text/plain", "Trai"); });
  server.on("/R", []() { Serial.print('R'); server.send(200, "text/plain", "Phai"); });
  server.on("/S", []() { Serial.print('S'); server.send(200, "text/plain", "Dung"); });

  server.begin();
}

void loop() {
  server.handleClient(); 
}