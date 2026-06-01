import requests
import speech_recognition as sr
import time

# --- CẤU HÌNH WIFI XE ---
# Địa chỉ IP đã xác nhận từ Serial Monitor của bạn
CAR_IP = "http://172.20.10.3" 

def send_to_car(command_char):
    """Gửi ký tự điều khiển tới Web Server trên ESP8266"""
    try:
        # Tăng timeout lên 2s để xử lý độ trễ mạng 
        url = f"{CAR_IP}/{command_char}"
        
        response = requests.get(url, timeout=2.0)
        
        if response.status_code == 200:
            print(f"✅ Xe phản hồi: {response.text}")
        else:
            print(f"❌ Lỗi phản hồi từ xe: {response.status_code}")
    except requests.exceptions.Timeout:
        print("⚠️ Mạng lag! Lệnh đã gửi nhưng xe chưa kịp trả lời.")
    except Exception as e:
        print(f"❌ Lỗi kết nối Wi-Fi: {e}")

# Khởi tạo bộ nhận diện giọng nói
recognizer = sr.Recognizer()
mic = sr.Microphone()

print(" Hệ thống xe tự hành nói bằng wi-fi !")
print("Hãy nói: Tiến, Lùi, Trái, Phải, Dừng (hoặc 'Thoát' để dừng chương trình)")

with mic as source:
    # Lọc nhiễu môi trường
    recognizer.adjust_for_ambient_noise(source, duration=1) 
    
    while True:
        print("\n>>> Đang nghe lệnh...")
        try:
            # Nghe âm thanh
            audio = recognizer.listen(source, timeout=5, phrase_time_limit=3)
            
            # Chuyển âm thanh thành văn bản qua Google Speech API
            text = recognizer.recognize_google(audio, language="vi-VN").lower()
            print(f" Bạn nói: {text}")

            # Logic phân tích từ khóa và gửi lệnh
            if "tiến" in text:
                print(" Lệnh: TIẾN")
                send_to_car('F')
            elif "lùi" in text:
                print(" Lệnh: LÙI")
                send_to_car('B')
            elif "trái" in text:
                print("⬅ Lệnh: TRÁI")
                send_to_car('L')
            elif "phải" in text:
                print(" Lệnh: PHẢI")
                send_to_car('R')
            elif "dừng" in text:
                print(" Lệnh: DỪNG")
                send_to_car('S')
            elif "thoát" in text:
                print(" Đang tắt hệ thống...")
                break
            else:
                print(" Không hiểu lệnh này, hãy thử lại.")
                
        except sr.WaitTimeoutError:
            pass # Không nghe thấy gì thì tiếp tục vòng lặp
        except sr.UnknownValueError:
            print(" Không nhận diện được âm thanh.")
        except Exception as e:
            print(f" Lỗi hệ thống: {e}")