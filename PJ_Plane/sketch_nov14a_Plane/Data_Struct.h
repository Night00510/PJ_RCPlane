#include <stddef.h>
#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <Arduino.h>
#include <stdint.h>

//
// ===================== Plane → Remote =====================
// ข้อมูลจาก "เครื่องบิน" ส่งไปที่รีโมท
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

  uint8_t  thrust;      // คันเร่ง (0–100 หรือ map ตามที่กำหนด)
};

// ขนาดจริงดูจาก: planeData_Size = sizeof(PlaneData)
// (ออกแบบให้ไม่เกิน 32 bytes สำหรับ nRF24L01)


//
// ===================== Remote → Plane =====================
// ข้อมูลจาก "รีโมท" ส่งไปที่เครื่องบิน
//
struct RemoteData {
  int8_t  roll;         // แกน ROLL  จากจอยสติ๊ก    -1 → 0 → +1
  int8_t  pitch;        // แกน PITCH จากจอยสติ๊ก    -1 → 0 → +1
  int8_t  yaw;          // แกน YAW   จากจอยสติ๊ก    -1 → 0 → +1
  int8_t takeoff_landing; // -1 = land, 0 = hold, +1 = takeoff
  uint8_t thrust;       // คันเร่ง 0–100 (%)

};


//
// ===================== Raw sensor data =====================
// ใช้ภายในเครื่องบินก่อนสเกล / ส่งวิทยุ
//

// ข้อมูลมุมจาก MPU6050 (ยังเป็น float เต็ม ๆ)
struct MPU_Data {
  float pitch;    // °
  float roll;     // °
  float yaw;      // °
};

// ข้อมูล UV + แสงจาก GUVA + BH1750 (ยังเป็น float เต็ม ๆ)
struct GUVA_BH1750 {
  float uv_uW_cm2;  // µW/cm²   (ค่าจริงจากสูตรคำนวณ)
  float uv_Index;   // UV Index
  float lux;        // lux
};

// ข้อมูลจาก BME280 + DHT (ยังไม่สเกล เป็น float เต็ม ๆ)
struct BMP280_DHT_Data {
  float temp_C;     // °C
  float temp_F;     // °F (ถ้าไม่ใช้จริง จะตัดทิ้งก็ได้)
  float pressure;   // hPa
  float height;     // meters
  float humid;      // %RH
};


//
// ===================== Utilities =====================
//

// constexpr เป็น const ที่คำนวณตั้งแต่ตอน compile
constexpr size_t planeData_Size  = sizeof(PlaneData);   // ใช้เวลา radio.write
constexpr size_t remoteData_Size = sizeof(RemoteData);  // ใช้เวลา radio.read

//
// ===================== Function Prototypes =====================
//

// อ่านค่าจากเซนเซอร์
MPU_Data read_MPU(bool debug = false); // MPU6050
BMP280_DHT_Data readBME_DHT(bool read_SEALEVEL = false, bool debug = false); // BME280 + DHT
GUVA_BH1750 read_UV_BH1750(byte pin, bool debug = false); // GUVA + BH1750
float readBatteryVoltage(bool = false); // แรงดันแบต

// ส่ง / รับ packet ผ่าน nRF24
bool receive_Remote_Packet(RemoteData &receive_Packet, bool debug = false);
void sent_Plane_Data(PlaneData &sent_Packet, bool debug = false);

void Control_Surfaces();  // คุม servo ตาม rxData
void setupNRF24(); // ตั้งค่า nRF24 ทั้ง TX/RX

#endif
