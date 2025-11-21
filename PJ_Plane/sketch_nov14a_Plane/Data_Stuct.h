#include <stddef.h>
#ifndef DATA_STRUCT_H //ถ้ายังไม่ #define DATA_STRUCT_H ให้ run ต่อ
#define DATA_STRUCT_H

#include <Arduino.h>
#include <stdint.h>

// ข้อมูลจาก "เครื่องบิน" → ส่งไปรีโมท
struct PlaneData {
  int16_t pitch;       // ต้องเอาค่าจริง (float) ×100 แล้วเก็บลงตัวแปร int16_t
  int16_t roll;        // ต้องเอาค่าจริง (float) ×100 แล้วเก็บลงตัวแปร int16_t
  int16_t yaw;         // ต้องเอาค่าจริง (float) ×100 แล้วเก็บลงตัวแปร int16_t

  int16_t temp_C;      // ต้องเอาค่าจริง (float) ×100 แล้วเก็บลงตัวแปร int16_t
  int16_t temp_F;      // ต้องเอาค่าจริง (float) ×100 แล้วเก็บลงตัวแปร int16_t
  int16_t humid;       // ต้องเอาค่าจริง (float) ×100 แล้วเก็บลงตัวแปร int16_t
  int16_t pressure;    // ต้องเอาค่าจริง (float) ×100 แล้วเก็บลงตัวแปร int16_t

  int16_t uv;          // UV GUVA เซนเซอร์ดิบ (ค่าจาก ADC)
  int16_t battery;     // แบตเตอรี่โวลต์ ×100 หรือ ×100 ก็ได้
  int16_t height;      // ความสูงเมตร ×100 (ถ้าต้องการละเอียด)
  byte checksum;   // เช็คข้อมูลเพี้ยนไหม
};
// ขนาด (ไม่คิด padding) =  10 × 2 + 1 bytes = 21 bytes


// ข้อมูลจาก "รีโมท" → ส่งไปเครื่องบิน
struct RemoteData {
  int16_t roll;        // จากจอยสติ๊ก    -1000 → +1000
  int16_t pitch;       // จากจอยสติ๊ก    -1000 → +1000
  int16_t yaw;         // จากจอยสติ๊ก    -1000 → +1000
  int16_t thrust;      // คันเร่ง          0 → 1000

  byte checksum;    // เช็คข้อมูลเพี้ยนไหม
};
// ขนาด (ไม่คิด padding) = 4×2 + 1 = 9 bytes

// raw data ของ MPU (ถ้าจะเก็บดิบ ๆ)
struct MPU_Data {
  float   pitch;     
  float   roll;
  float   yaw;
};

// raw data ของ GUVA
struct GUVA {
  uint16_t uv_raw;   // ADC raw = 0–4095
};

// raw data ของ BMP280
struct BMP280_DHT_Data {
  float temp_C;       // °C
  float temp_F;       // °F
  float pressure;     // hPa
  float height;       // meters
  float humid;        // %
};

typedef float (*ReadFunc)();   // สร้าง “ชนิดข้อมูลใหม่” ชื่อ ReadFunc สำหรับฟังก์ชันที่คืนค่าเป็น float และ ไม่รับพารามิเตอร์

constexpr size_t planeData_Size = sizeof(PlaneData); // นับ checksum ด้วย
constexpr size_t remodeData_Size = sizeof(RemoteData); // นับ checksum ด้วย 
                                                      //constexpr เป็นconstที่สร้างตั้งแต่ตอน compile
void setupMPU();    // ประกาศฟังก์ชันเริ่มต้นเซนเซอร์MPU
MPU_Data readMPU(bool debug = false); // ฟังก์ชันอ่านค่ามุม
bool receive_Remote_Packet(RemoteData &receive_Packet, bool debug = false);
void sent_Plane_Data(PlaneData &sent_Packet, const byte *addr, bool debug = false);
BMP280_DHT_Data readBME_DHT(bool read_SEALEVEL = false, bool debug = false);
float readUV_Index(byte pin, bool debug = false);
float readBH1750(bool debug = false);

#endif //ถ้าใช้ไฟล์นี้แล้ว 1 ครั้ง #ifndef STRUCTS_H จะเป็นเท็จ คอมไพล์จะข้าม code จนถึง #endif ทันที
