#include <SPI.h>
#include <RF24.h>
#include "Data_Stuct.h"   // <- แก้ชื่อไฟล์ให้ตรงกับจริง

// ========= nRF24 =====================
#define CE_TX  4    // ขาส่ง
#define CSN_TX 5    // ขาส่ง
#define CE_RX  16   // ขารับ
#define CSN_RX 17   // ขารับ

enum PipeID { REMODE = 0, PLANE = 1 };
const byte pipeAddr[][6] = { "REM01" , "PLN01" };

RF24 radio_Sent(CE_TX, CSN_TX);
RF24 radio_Receive(CE_RX, CSN_RX);
// ====================================

// ตัวแปรเก็บข้อมูลที่รับมา
RemoteData rxData;

void setup()
{
  Serial.begin(115200);
  SPI.begin();   // สำคัญมากสำหรับ ESP32

  // --------- ฝั่งรับ ----------
  radio_Receive.begin();
  radio_Receive.setPALevel(RF24_PA_LOW);
  radio_Receive.setDataRate(RF24_1MBPS);
  radio_Receive.setChannel(100);

  // รับจากรีโมทที่ส่งด้วย "REM01"
  radio_Receive.openReadingPipe(0, pipeAddr[REMODE]);
  radio_Receive.startListening();

  Serial.println("Receiver Ready...");
}

void loop()
{
  receive_Remote_Packet(rxData, 1);

  delay(500);
}
