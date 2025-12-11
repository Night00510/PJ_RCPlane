#include <stddef.h>
#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H
//
// ===================== Plane → Remote =====================
//   ข้อมูลที่ "เครื่องบิน" ส่งกลับมาหารีโมท
//   - ทุกค่าที่เป็น float ให้คูณ 100 ก่อนเก็บ (ลดขนาด packet)
//   - ฝั่งรีโมทค่อยหาร 100.0 กลับ เวลาเอาไปใช้ / ส่งเข้า Blynk
//   - struct นี้ออกแบบให้ขนาดรวมไม่เกิน 32 bytes (ข้อจำกัดของ nRF24)
//
struct PlaneData {
  // ---- ค่า 32-bit (อยู่ด้านบนเพื่อลด padding) ----
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

  uint8_t  thrust;      // คันเร่งของเครื่องบิน 0–100 (%)
};

//
// ===================== Remote → Plane =====================
//   ข้อมูลจากรีโมทส่งไปที่เครื่องบิน
//
struct RemoteData {
  int8_t  roll;         // แกน ROLL  จากจอยสติ๊ก    -1 → 0 → +1
  int8_t  pitch;        // แกน PITCH จากจอยสติ๊ก    -1 → 0 → +1
  int8_t  yaw;          // แกน YAW   จากจอยสติ๊ก    -1 → 0 → +1
  int8_t takeoff_landing; // -1 = land, 0 = hold, +1 = takeoff
  uint8_t thrust;       // คันเร่ง 0–100 (%)

};

//
// ===================== Utilities =====================
//
//   ใช้ sizeof() ตรง ๆ ไม่ต้องกลัวลืมแก้จำนวน byte
//
constexpr size_t planeData_Size  = sizeof(PlaneData);
constexpr size_t remoteData_Size = sizeof(RemoteData);

//
// ===================== Function Prototypes =====================
//
//   ประกาศ prototype ไว้ให้ไฟล์อื่นเรียกได้
//
int  read_Avg_ADC(int pin);
void doCalibrate();
int  digitalAxis(int raw, int center);

void setupNRF24();
void sent_Remote_Packet(RemoteData &sent_Packet, bool debug = false);
bool receive_Plane_Packet(PlaneData &receive_Packet, bool debug = false);
void sent_to_Blynk();   // อยู่ใน Blynk.ino
void randomPlane(PlaneData &p);
void randomRemote(RemoteData &d);

#endif
