#include <stddef.h>
#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <Arduino.h>
#include <stdint.h>

//
// ===================== Plane → Remote =====================
//
// หมายเหตุการสเกลค่า:
//  - ค่าที่เป็น float จากเซนเซอร์ ให้คูณ 100 ก่อนเก็บ ถ้าไม่ได้ระบุไว้เป็นอย่างอื่น
//  - ฝั่งรีโมทให้หารกลับด้วย 100.0 (หรือ 10.0 ตามคอมเมนต์ของแต่ละตัวแปร)
//  - struct นี้ออกแบบให้ขนาดรวม (sizeof(PlaneData)) ไม่เกิน 32 bytes สำหรับ nRF24
//
struct PlaneData {
  // ---- ค่า 32-bit (จัดไว้ด้านบนเพื่อลด padding) ----
  uint32_t pressure;    // ความดันอากาศ (hPa) ×100   เช่น 1013.25 hPa → 101325
  uint32_t uv_uW_cm2;   // ความเข้มรังสี UV (µW/cm²) ×100
  uint32_t lux;         // ความสว่าง (lux) ×100      เช่น 1234.56 lux → 123456

  // ---- ค่า 16-bit ----
  int16_t  pitch;       // มุม pitch (องศา) ×100      เช่น -12.34° → -1234
  int16_t  roll;        // มุม roll  (องศา) ×100
  int16_t  yaw;         // มุม yaw   (องศา) ×100

  int16_t  temp_C;      // อุณหภูมิ (°C) ×100         เช่น 25.37°C → 2537
  uint16_t humid;       // ความชื้นสัมพัทธ์ (%) ×100  เช่น 55.12% → 5512

  uint16_t uv_Index;    // UV Index ×100               เช่น 5.23 → 523
  uint16_t battery;     // แรงดันแบตเตอรี่ (V) ×100   เช่น 11.45V → 1145

  int16_t  height;      // ความสูง (เมตร) ×100        เช่น 123.45m → 12345
                        // ช่วงที่ปลอดภัย ≈ -327.68m ถึง +327.67m

  uint8_t  thrust;
  uint8_t  checksum;    // checksum XOR ของข้อมูลทั้งหมด (ยกเว้นตัว checksum เอง)
};

// ขนาดจริงดูจาก: planeData_Size = sizeof(PlaneData)
// (ถูกออกแบบให้ไม่เกิน 32 bytes สำหรับ nRF24L01)


//
// ===================== Remote → Plane =====================
// ข้อมูลจาก "รีโมท" ส่งไปที่เครื่องบิน
//
struct RemoteData {
  int8_t roll;         // จากจอยสติ๊ก    -1 → 0 → +1
  int8_t pitch;        // จากจอยสติ๊ก    -1 → 0 → +1
  int8_t yaw;          // จากจอยสติ๊ก    -1 → 0 → +1
  uint8_t thrust;       // คันเร่ง          0 → 100

  uint8_t checksum;     // checksum XOR ของข้อมูลทั้งหมด (ยกเว้น checksum เอง)
};

//
// ===================== Raw sensor data =====================
// ใช้ภายในเครื่องบินก่อนสเกล / ส่งวิทยุ
//

// // ข้อมูลมุมจาก MPU6050 (ยังเป็น float เต็ม ๆ)
// struct MPU_Data {
//   float pitch;    // ° 
//   float roll;     // °
//   float yaw;      // °
// };

// // ข้อมูล UV + แสงจาก GUVA + BH1750 (ยังเป็น float เต็ม ๆ)
// struct GUVA_BH1750 {
//   float uv_uW_cm2;  // µW/cm²   (ค่าจริงจากสูตรคำนวณ)
//   float uv_Index;   // UV Index
//   float lux;        // luxs
// };

// // ข้อมูลจาก BME280 + DHT (ยังไม่สเกล เป็น float เต็ม ๆ)
// struct BMP280_DHT_Data {
//   float temp_C;     // °C
//   float temp_F;     // °F (ถ้าไม่ใช้จริง จะตัดทิ้งก็ได้)
//   float pressure;   // hPa
//   float height;     // meters
//   float humid;      // %RH
// };


//
// ===================== Utilities =====================
//

// constexpr เป็น const ที่คำนวณตั้งแต่ตอน compile
constexpr size_t planeData_Size  = sizeof(PlaneData);   // ใช้เวลา radio.write / checksum
constexpr size_t remoteData_Size = sizeof(RemoteData);  // ใช้เวลา radio.read / checksum

//
// ===================== Function Prototypes =====================
//
int read_Avg_ADC(int pin);
void doCalibrate();
int digitalAxis(int raw, int center);
void sent_Remote_Packet(RemoteData &sent_Packet, const byte *addr, bool debug = false);
bool receive_Plane_Packet(PlaneData &receive_Packet, bool debug = false);
void setupNRF24();
void sent_to_Blynk();

#endif