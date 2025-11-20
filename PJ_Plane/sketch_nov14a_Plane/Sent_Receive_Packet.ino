#include <RF24.h>
#include "Data_Struct.h"

// ใช้ radio_Sent ที่ประกาศอยู่ในไฟล์หลัก (ไม่สร้างซ้ำ)
extern RF24 radio_Sent;

// ตาราง address ของแต่ละ pipe (0 = REMODE, 1 = PLANE)
extern const byte pipeAddr[][6];


// ============================================
// ฟังก์ชันคำนวณ checksum แบบ XOR ทีละ byte
// buf  = pointer ชี้ไปยังข้อมูลจริง
// len  = จำนวน byte ที่ต้องนำมาคิด
// ============================================
byte doCheckSum(const byte *buf, size_t len)
{
  byte sum = 0;

  // วิ่งอ่านทีละ byte จากหน่วยความจำ
  for (size_t i = 0; i < len; i++)
  {
    // XOR byte ทุกตัวเข้าด้วยกัน
    sum ^= buf[i];
  }

  // ส่งค่า checksum กลับ
  return sum;
}


// =====================================================
// ฟังก์ชันส่งข้อมูลผ่าน nRF24
// sent_Packet = struct PlaneData ที่จะส่ง
// pipeline    = pipe ที่จะใช้ส่ง (REMODE หรือ PLANE)
// =====================================================
void sent_Data(PlaneData &sent_Packet, PipeID pipeline)
{
  // แปลง struct ให้เป็น pointer byte
  // เพื่อให้ doCheckSum อ่านข้อมูลทุก byte ได้
  const byte *buf = (const byte*)&sent_Packet;

  // คำนวณ checksum จากทุก byte ยกเว้น
  // byte สุดท้าย (ช่อง checksum เอง)
  sent_Packet.checksum = doCheckSum(buf, sizeof(PlaneData) - 1);

  // เปลี่ยน nRF24 เข้าโหมดส่ง
  radio_Sent.stopListening();

  // เลือก address ของ pipe ตาม enum ที่ส่งเข้ามา
  // เช่น REMODE -> pipeAddr[0]
  //      PLANE  -> pipeAddr[1]
  radio_Sent.openWritingPipe(pipeAddr[pipeline]);

  // ส่ง packet ออกไปจริง ๆ
  bool ok = radio_Sent.write(&sent_Packet, sizeof(PlaneData));

  // ส่วน debug: แสดงผลว่าส่งสำเร็จหรือไม่
  #ifdef DEBUG_RADIO
    if (ok)
    {
      Serial.print("Send OK on pipe ");
      Serial.println(pipeline);
    }
    else
    {
      Serial.print("Send FAIL on pipe ");
      Serial.println(pipeline);
    }
  #endif
}
