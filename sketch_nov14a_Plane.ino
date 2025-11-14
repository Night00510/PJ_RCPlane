#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

#define CE_PIN 4
#define CSN_PIN 5

//ตัวแปล RF24L0
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "NODE1"; // Pipe address ต้องตรงกับฝั่งส่ง

struct RcPacket // โครงสร้างแพ็กเกจต้องเหมือนฝั่งส่งเป๊ะ
{
  int8_t roll; // -1,0,1
  int8_t pitch; // -1,0,1
  uint8_t checksum; // ใช้ XOR เช็กข้อมูลเพี้ยน
};

// ฟังก์ชัน checksum แบบเดียวกับฝั่งส่ง


void setup()
{
  Serial.being(115200);
  delay(500);

  if(!radio.begin())
  {
    Serial.println("nRF24 inti FAIL");
    whle(1) delay(1000);
  }

  radio.setPALevel(RF24_PA_HING);
  radio.
}

