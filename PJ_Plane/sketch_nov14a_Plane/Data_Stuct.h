#ifndef STRUCTS_H //ถ้ายังไม่ #define STRUCTS_H ให้ run ต่อ
#define STRUCTS_H 

#include <Arduino.h>

// ข้อมูลจาก "เครื่องบิน" → ส่งไปรีโมท
struct PlaneData {
  int16_t pitch;       //ต้องเอาค่าจริง (float) ×100 แล้วเก็บลงตัวแปร int16_t
  int16_t roll;        //ต้องเอาค่าจริง (float) ×100 แล้วเก็บลงตัวแปร int16_t
  int16_t yaw;         // ต้องเอาค่าจริง (float) ×100 แล้วเก็บลงตัวแปร int16_t

  int16_t temp;        // ต้องเอาค่าจริง (float) ×10 แล้วเก็บลงตัวแปร int16_t
  int16_t humid;       // ต้องเอาค่าจริง (float) ×10 แล้วเก็บลงตัวแปร int16_t
  int16_t pressure;    // ต้องเอาค่าจริง (float) ×10 แล้วเก็บลงตัวแปร int16_t

  int16_t uv;          // UV GUVA เซนเซอร์ดิบ
  int16_t battery;     // แบตเตอรี่โวลต์ 
  int16_t height;      // ความสูงเมตร
  uint8_t checksum;    // เช็คข้อมูลเพี้ยนไหม
};

// ขนาดทั้งหมด = 9 ค่า × 2 bytes = 18 bytes


// ข้อมูลจาก "รีโมท" → ส่งไปเครื่องบิน
struct RemoteData {
  int16_t roll;        // จากจอยสติ๊ก    -1000 → +1000
  int16_t pitch;       // จากจอยสติ๊ก    -1000 → +1000
  int16_t yaw;         // จากจอยสติ๊ก    -1000 → +1000
  int16_t thrust;      // คันเร่ง          0 → 1000

  uint8_t checksum;    // เช็คข้อมูลเพี้ยนไหม
};

// ขนาด = 4 × 2 = 8 bytes

#endif //ถ้าใช้ไฟล์นี้แล้ว 1 ครั้ง #ifndef STRUCTS_H จะเป็นเท็จ คอมไพล์จะ ข้ามไฟล์นี้ทั้งไฟล์ทันที
