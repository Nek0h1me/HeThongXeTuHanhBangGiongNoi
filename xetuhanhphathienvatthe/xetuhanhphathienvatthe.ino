#include <SoftwareSerial.h>

// Nối chân TX của ESP8266 vào chân 2 của Arduino Uno
SoftwareSerial espSerial(2, 3); 

// Cấu hình các chân điều khiển L298 
const int ENA = 11; 
const int IN1 = 8;  
const int IN2 = 7;  

const int ENB = 6;  
const int IN3 = 5;  
const int IN4 = 4;  

// --- CẤU HÌNH CHÂN CẢM BIẾN HC-SR04 ---
const int TRIG_PIN = 9;   // Chân phát xung
const int ECHO_PIN = 10;  // Chân nhận xung
const int SAFETY_DISTANCE = 10; // Khoảng cách an toàn được nâng lên 20cm cho phù hợp với thông báo

void setup() {
  // Cài đặt chân xuất tín hiệu cho động cơ
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  
  // Cài đặt chân cho cảm biến siêu âm
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // ÉP XE DỪNG 
  driveRobot(0, 0); 
  
  // Khởi động giao tiếp Serial để debug và espSerial để nhận lệnh từ Wi-Fi
  Serial.begin(9600);
  espSerial.begin(9600); 
  
  Serial.println("--- XE TU HANH: SAN SANG NHAN LENH WIFI HO TRO HC-SR04 ---");
}

void loop() {
  // Đo khoảng cách tới vật cản trước khi xử lý lệnh di chuyển từ Python
  long distance = readDistance();

  // Cơ chế tự động bảo vệ: Nếu phát hiện vật thể nguy hiểm trước mặt
  if (distance > 0 && distance < SAFETY_DISTANCE) {
    Serial.print(" PHÁT HIỆN VẬT THỂ TRƯỚC MẶT: ");
    Serial.print(distance);
    Serial.println(" cm. Tự động lùi thẳng an toàn!");

    // TRẠNG THÁI 1: Phanh khẩn cấp để dập tắt quán tính tiến của phương tiện
    driveRobot(0, 0);       
    delay(300);             // Ngắt trễ ngắn giúp xe đứng im ổn định phần cơ khí
    
    // TRẠNG THÁI 2: Kích hoạt dòng điện ngược chiều để cả xe lùi thẳng tịnh tiến ra xa
    driveRobot(-180, -180); 
    delay(400);            // Duy trì hành trình lùi thẳng trong khoảng thời gian một phẩy năm giây
    
    // TRẠNG THÁI 3: Dừng xe cố định tại chỗ sau khi tạo khoảng cách an toàn thành công
    driveRobot(0, 0);       
    
    return;                 // Bỏ qua luồng đọc mã Wi-Fi phía dưới để cô lập hoàn toàn lệnh di chuyển lỗi
  }

  // Luồng lắng nghe tín hiệu từ ESP8266 khi không gian phía trước trống thoáng
  if (espSerial.available()) {
    char command = espSerial.read();
    
    Serial.print("Nhan duoc lenh tu Wi-Fi: ");
    Serial.println(command); 
    
    // Xử lý các lệnh điều khiển hướng từ ứng dụng Python máy tính
    switch(command) {
      case 'F': 
        driveRobot(200, 200);   // Phát lệnh tiến lên
        delay(500);            
        driveRobot(0, 0);       
        break;   
        
      case 'B': 
        driveRobot(-200, -200); // Phát lệnh lùi lại
        delay(500);
        driveRobot(0, 0);
        break; 
        
      case 'L': 
        driveRobot(-180, 180);  // Phát lệnh xoay trái tại chỗ
        delay(200);             
        driveRobot(0, 0);
        break;  
        
      case 'R': 
        driveRobot(180, -180);  // Phát lệnh xoay phải tại chỗ
        delay(200);
        driveRobot(0, 0);
        break;  
        
      case 'S': 
        driveRobot(0, 0);       // Chủ động dừng xe
        break;       
    }
  }
}

// Hàm đo khoảng cách trả về giá trị cm từ cảm biến siêu âm HC-SR04
long readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // Cơ chế ngắt để tránh treo luồng hệ thống
  if (duration == 0) {
    return -1; 
  }
  return duration * 0.034 / 2; 
}

// Hàm bóc tách tín hiệu điều khiển động cơ vi sai
void driveRobot(int speedLeft, int speedRight) {
  // Thiết lập trạng thái cầu H cho cụm bánh mạn TRÁI
  if (speedLeft > 0) { 
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); 
  } else if (speedLeft < 0) { 
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); 
  } else { 
    digitalWrite(IN1, LOW); digitalWrite(IN2, LOW); 
  }
  analogWrite(ENA, abs(speedLeft));

  // Thiết lập trạng thái cầu H cho cụm bánh mạn PHẢI
  if (speedRight > 0) { 
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); 
  } else if (speedRight < 0) { 
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); 
  } else { 
    digitalWrite(IN3, LOW); digitalWrite(IN4, LOW); 
  }
  analogWrite(ENB, abs(speedRight));
}