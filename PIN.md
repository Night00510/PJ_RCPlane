# nRF24L0 PA LNA Pin
- nRF ตัวส่ง
  - CE → pin 4
  - CSN → pin 5
  - SCK → SCK (GPIO18)
  - MOSI → MOSI (GPIO23)
  - MISO → MISO (GPIO19)
  - VCC → 3.3V
  - GND → GND
- nRF ตัวรับ
  - CE → pin 16
  - CSN → pin 17
  - SCK → SCK (ต่อรวมกับตัวส่ง)
  - MOSI → MOSI (ต่อรวมกับตัวส่ง)
  - MISO → MISO (ต่อรวมกับตัวส่ง)
  - VCC → 3.3V
  - GND → GND

# DHT 22
- OUT → 2
- VCC → 3.3V
- GND → GND

# BME280
- ค่า default ESP32: SDA=21, SCL=22
- Vin → 3.3V
- GND → GND

# GUVA
- SIG → 32
- Vin → 3.3V
- GND → GND

# Readbat battery
เป็นวงจรแบ่งแรงดัน
- Bat Pin → 33
- R1 47kΩ
- R2 10kΩ

# Servo
- ROLL_LEFT_SERVO_PIN   12   // Servo ปีกช้าย
- ROLL_RIGHT_SERVO_PIN  13   // Servo ปีกขวา
- PITCH_SERVO_PIN       14   // Servo pitch
- YAW_SERVO_PIN         25
- THRUST_PIN            26
   
