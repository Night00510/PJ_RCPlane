#define BLYNK_TEMPLATE_ID   "TMPL63iUfewUj"
#define BLYNK_TEMPLATE_NAME "PJ RCPlane"
#define BLYNK_AUTH_TOKEN    "bk5L47VYcYUsof06JqM3myUNWNp-CoEm"

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "Data_Struct.h"

// ---------- ตั้งค่า WiFi / Blynk ----------

const char ssid[] = "AP_net";
const char pass[] = "apao.3994";

// ---------- ขา SPI และขา nRF24 สองตัว ----------
// #define SPI_SCK   18
// #define SPI_MISO  19
// #define SPI_MOSI  23

#define VSPI_SCK   18
#define VSPI_MISO  19
#define VSPI_MOSI  23
#define CE1  16
#define CSN1 17

#define HSPI_SCK   14
#define HSPI_MISO  12
#define HSPI_MOSI  13
#define CE2  4
#define CSN2 5

SPIClass spiHSPI(HSPI);
// #define PIN_SW_LAND     12
// #define PIN_SW_TAKEOFF  14

RF24 radio_Sent(CE1, CSN1);
RF24 radio_Receive(CE2, CSN2);

// ---------- address ของแต่ละท่อ ----------
const byte sent_From_Plane_Addrase[] = "REM01";
const byte sent_From_Remote_Addrase[] = "PLN01";

// ---------- ขา Joystick / Throttle ----------
#define XPIN_ROLL    34   // ADC1-only
#define YPIN_PITCH   35
#define ZPIN_YAW     32
#define PIN_THRUST   33

// จำนวนครั้งอ่านแล้วเฉลี่ย (ลด noise)
#define AVG_N        32

// เดดโซนของแกนจอย (หน่วยเป็นค่าดิบ ADC)
#define DEADZONE     400

// ---------- ค่าศูนย์/พื้นหลังคาลิเบรต ----------
int center_X_Roll  = 2048;
int center_Y_Pitch = 2048;
int center_Z_Yaw   = 2048;
int base_Thrucst   = 0;     // ค่า ADC ตอนคันเร่งอยู่ "ต่ำสุด"

// ---------- ตัวแปร packet ----------
RemoteData txData;   // ส่งจากรีโมท → เครื่องบิน
PlaneData  rxData;   // รับจากเครื่องบิน → รีโมท (ส่งต่อ Blynk)

// =====================================================
// อ่าน ADC เฉลี่ย AVG_N ครั้งเพื่อลด noise
// =====================================================
int read_Avg_ADC(int pin)
{
  long sum = 0;
  for (int i = 0; i < AVG_N; i++) {
    sum += analogRead(pin);
  }
  return (int)(sum / AVG_N);
}

// =====================================================
// แปลงค่า ADC ของแกนจอยเป็น -1, 0, +1 ด้วยเดดโซน
//   raw    = ค่าที่อ่านได้
//   center = ค่าศูนย์ (หลังคาลิเบรต)
// =====================================================
int digitalAxis(int raw, int center)
{
  if (raw > center + DEADZONE) return  1;
  if (raw < center - DEADZONE) return -1;
  return 0;
}

// =====================================================
// doCalibrate()
//   - ให้ผู้ใช้วางรีโมทนิ่ง ๆ จอยอยู่ศูนย์ คันเร่งลงสุด
//   - อ่านค่า ADC หลายครั้งเพื่อหา "ค่าศูนย์" และ "พื้นคันเร่ง"
// =====================================================
void doCalibrate()
{
  Serial.println();
  Serial.println(F("=== CALIBRATE JOYSTICK & THROTTLE ==="));
  Serial.println(F("วางรีโมทนิ่ง ๆ บนพื้น"));
  Serial.println(F("- จอย ROLL / PITCH / YAW ให้อยู่ตรงกลาง"));
  Serial.println(F("- คันเร่ง (THRUST) ดันลงสุด"));
  Serial.println(F("เริ่มคาลิเบรตใน 3 วินาที..."));
  delay(3000);

  const int N = 200;
  long sumX = 0;
  long sumY = 0;
  long sumZ = 0;
  long sumT = 0;

  for (int i = 0; i < N; i++) {
    sumX += read_Avg_ADC(XPIN_ROLL);
    sumY += read_Avg_ADC(YPIN_PITCH);
    sumZ += read_Avg_ADC(ZPIN_YAW);
    sumT += read_Avg_ADC(PIN_THRUST);
    delay(5);
  }

  center_Y_Pitch = (int)(sumY / N);
  center_X_Roll  = (int)(sumX / N);
  center_Z_Yaw   = (int)(sumZ / N);
  base_Thrucst   = (int)(sumT / N);

  Serial.println("=== CALIBRATE DONE ===");
  Serial.print("center_X_Roll  = "); Serial.println(center_X_Roll);
  Serial.print("center_Y_Pitch = "); Serial.println(center_Y_Pitch);
  Serial.print("center_Z_Yaw   = "); Serial.println(center_Z_Yaw);
  Serial.print("base_Thrucst   = "); Serial.println(base_Thrucst);
  Serial.println();
}

// =====================================================
// setup()
//   - ตั้ง Serial, WiFi/Blynk, nRF24
//   - ทำการคาลิเบรตครั้งแรก
// =====================================================
void setup()
{
  Serial.begin(115200);
  delay(500);
  Serial.println(F("--- Remote System Start ---"));

  // ตั้งโหมดขา Joystick
  pinMode(XPIN_ROLL,   INPUT);
  pinMode(YPIN_PITCH,  INPUT);
  pinMode(ZPIN_YAW,    INPUT);
  pinMode(PIN_THRUST,  INPUT);
  // pinMode(PIN_SW_LAND, INPUT);
  // pinMode(PIN_SW_TAKEOFF, INPUT);

  // เริ่ม Blynk (ถ้าเน็ตยังไม่พร้อมอาจดีเลย์นานนิดหน่อย)
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // เริ่มต้น nRF24 สองตัว
  setupNRF24();

  // คาลิเบรตศูนย์/พื้น
  doCalibrate();
}

// ใช้ timer1 จำกัดความถี่การส่ง/รับ (ทุก ~100ms)
unsigned long timer1 = 0;

// =====================================================
// loop()
//   - ทุก ๆ ~100ms: อ่านจอย → แปลง → ส่ง packet ไปเครื่องบิน
//   - ถ้ามี PlaneData กลับมา → ส่งเข้า Blynk
// =====================================================
void loop()
{
  Blynk.run();

  // --- ส่ง RemoteData ไปเครื่องบินทุก 100ms ---
  if (millis() - timer1 > 100)
  {
    timer1 = millis();

    // 1) อ่านค่าดิบจากจอยสติ๊ก / คันเร่ง (ค่าเฉลี่ยลด noise)
    int rawX = read_Avg_ADC(XPIN_ROLL);
    int rawY = read_Avg_ADC(YPIN_PITCH);
    int rawZ = read_Avg_ADC(ZPIN_YAW);
    int rawT = read_Avg_ADC(PIN_THRUST);
    // int swLand    = digitalRead(PIN_SW_LAND);     // 0 หรือ 1
    // int swTakeoff = digitalRead(PIN_SW_TAKEOFF);  // 0 หรือ 1

    // ให้ค่าตามที่คุณต้องการ
    // if (swLand == 1 && swTakeoff == 0) {
    //     txData.takeoff_landing = -1;     // LAND
    // }
    // else if (swLand == 0 && swTakeoff == 1) {
    //     txData.takeoff_landing = 1;     // TAKEOFF
    // }
    // else {
    //     txData.takeoff_landing = 0;      // NONE / CENTER
    // }
    
    // 2) แปลงแกนจอยเป็น -1,0,+1 ด้วยเดดโซน
    txData.roll  = (int8_t)digitalAxis(rawX, center_X_Roll);
    txData.pitch = (int8_t)digitalAxis(rawY, center_Y_Pitch);
    txData.yaw   = (int8_t)digitalAxis(rawZ, center_Z_Yaw);

    // 3) คำนวณคันเร่งจาก "พื้น" ขึ้นมา 1000 step แล้ว map เป็น 0–100
    //
    //    diff = ส่วนที่สูงกว่าพื้น (base_Thrucst)
    //    - ถ้าต่ำกว่าพื้น → 0
    //    - ถ้าเกินพื้น+1000 → ตันที่ 1000
    //    - จากนั้น map 0..1000 → 0..100 (%)
    //
    int diff = rawT - base_Thrucst;
    if (diff < 0)    diff = 0;
    if (diff > 1000) diff = 1000;

    int tPercent = map(diff, 0, 1000, 0, 100);
    txData.thrust = (uint8_t)tPercent;

    // 4) ส่ง packet ออกไป (เปิด debug true ช่วงเทสต์)
    sent_Remote_Packet(txData);
  }

  // --- รับ PlaneData จากเครื่องบิน ถ้ามี ---
  if (receive_Plane_Packet(rxData))
  {
    // ถ้ามี telemetry กลับมา → ส่งเข้า Blynk ให้ dashboard แสดงผล
    sent_to_Blynk();
    debugPlaneData(rxData);
  }
}
