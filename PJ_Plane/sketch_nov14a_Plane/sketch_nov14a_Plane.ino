//************ Not the final version In actual use **************//

//************ ไม่ใช่เวอร์ชั่นสุดท้ายที่ใช้จริง **************//
#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <stdint.h>

#define CE_PIN 4
#define CSN_PIN 5

// ตัวแปร RF24L01
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "NODE1"; // Pipe address ต้องตรงกับฝั่งส่ง

// โครงสร้างแพ็กเกจต้องเหมือนฝั่งส่งเป๊ะ
struct RcPacket
{
  int8_t  roll;     // -1,0,1
  int8_t  pitch;    // -1,0,1
  uint8_t checksum; // ใช้ XOR เช็กข้อมูลเพี้ยน
};

// ฟังก์ชัน checksum แบบเดียวกับฝั่งส่ง
uint8_t doCheckSum(const RcPacket &p)
{
  return (uint8_t)(p.roll ^ p.pitch ^ 0x5A);
}

void setup()
{
  Serial.begin(115200);      // แก้ from being → begin
  delay(500);

  Serial.println();
  Serial.println("--- RX start ---");

  if (!radio.begin())
  {
    Serial.println("nRF24 init FAIL");
    while (1) {
      delay(1000);
    }
  }

  radio.setChannel(100);            // ต้องตรงกับฝั่งส่ง
  radio.setDataRate(RF24_250KBPS);  // ต้องตรงกับฝั่งส่ง
  radio.setPALevel(RF24_PA_HIGH);   // แก้ RF24_PA_HING → RF24_PA_HIGH

  radio.openReadingPipe(1, address);
  radio.startListening();

  Serial.println("nRF24L01 Ready (RX mode). Listening...");
}

void loop()
{
  if (radio.available())
  {
    RcPacket rx;   // ใช้ชื่อ rx ให้ตรงกับข้างล่าง

    // ถ้ามีหลาย frame ค้าง ให้ดูอันล่าสุด
    while (radio.available())
    {
      radio.read(&rx, sizeof(rx));
    }

    // เช็ก checksum ก่อนใช้ข้อมูล
    uint8_t calc = doCheckSum(rx);  // ต้องส่งพารามิเตอร์เข้าไป
    if (calc != rx.checksum)
    {
      Serial.print("Checksum ERROR! recv=");
      Serial.print(rx.checksum, HEX);
      Serial.print("  calc=");
      Serial.println(calc, HEX);
      // return;  // ทิ้งแพ็กนี้ไป
    }

    // ถ้า checksum ผ่าน แปลว่าข้อมูลโอเค
    Serial.print("OK  Roll = ");
    Serial.print(rx.roll);
    Serial.print("  Pitch = ");
    Serial.println(rx.pitch);
  }

  delay(5);
}
