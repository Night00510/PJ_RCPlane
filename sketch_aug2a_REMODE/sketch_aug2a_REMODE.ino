#define BLYNK_TEMPLATE_ID "xxx"
#define BLYNK_TEMPLATE_NAME "xxx"
#define BLYNK_AUTH_TOKEN "xxx"

#include <Arduino.h>   // ปกติ include ให้อยู่แล้ว แต่ใส่ไว้ไม่เสียหาย
#include <stdint.h>    // สำหรับ int8_t, uint8_t
#include <SPI.h>
#include <RF24.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "Data_Struct.h"   

#define CE_TX  4    // ขาส่ง
#define CSN_TX 5    // ขาส่ง
#define CE_RX  16   // ขารับ
#define CSN_RX 17   // ขารับ

#define XPIN_ROLL    34 // ADC input only (ADC1)
#define YPIN_PITCH   35 
#define ZPIN_YAW     32
#define PIN_THRUST   33

#define AVG_N        32   // จำนวนครั้งอ่านแล้วเฉลี่ย
#define deadzone     400  // ตั้งเดดโซน (ใช้ใน digitalAxis)

//WiFi
const char ssid[] = "AP_net";
const char pass[] = "apao.3994";

// กำหนดค่าเริ่มต้น (กันพลาดก่อนคาลิเบรต)
int center_X_Roll  = 2048;
int center_Y_Pitch = 2048;
int center_Z_Yaw   = 2048;
int base_Thrucst   = 0;

// ========= nRF24 =====================
enum PipeID { REMOTE = 0, PLANE = 1 };
const byte pipeAddr[][6] = { "REM01" , "PLN01" };

RF24 radio_Sent(CE_TX, CSN_TX);
RF24 radio_Receive(CE_RX, CSN_RX);

RemoteData txData; // ตัวแปรเก็บข้อมูลที่จะส่ง
PlaneData  rxData; // ตัวแปรเก็บข้อมูลที่รับมา

// ถ้าฟังก์ชันอื่นๆ อยู่คนละไฟล์ .ino แต่ในโปรเจ็กต์เดียวกัน
// Arduino จะเห็นหมด ไม่จำเป็นต้องประกาศ prototype ก็ได้
// แต่บอกไว้เผื่ออ่านง่าย
// void sent_to_Blynk();  // ไว้ค่อยทำทีหลัง

// ===== setup / loop =====
void setup()
{
  pinMode(XPIN_ROLL,   INPUT);
  pinMode(YPIN_PITCH,  INPUT);
  pinMode(ZPIN_YAW,    INPUT);
  pinMode(PIN_THRUST,  INPUT);

  Serial.begin(115200);
  Serial.println("--- System Start ---");
  delay(500);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // ตั้งค่า ADC ของ ESP32 ให้เต็มช่วง
  analogReadResolution(12);        // 0–4095
  analogSetAttenuation(ADC_11db);  // รองรับ ~0–3.3V

  setupNRF24();// เปิf nRF24
  doCalibrate();  // Auto-calibrate ตอนเปิดเครื่องครั้งแรก
}

long timer1 = 0;

void loop()
{
  Blynk.run();

  if (millis() - timer1 > 100)
  {
    timer1 = millis();

    // อ่านค่าจอย
    int rawX = read_Avg_ADC(XPIN_ROLL);
    int rawY = read_Avg_ADC(YPIN_PITCH);
    int rawZ = read_Avg_ADC(ZPIN_YAW);
    int rawT = read_Avg_ADC(PIN_THRUST);

    // แปลงเป็น -1,0,+1
    txData.roll  = (int8_t)digitalAxis(rawX, center_X_Roll);
    txData.pitch = (int8_t)digitalAxis(rawY, center_Y_Pitch);
    txData.yaw   = (int8_t)digitalAxis(rawZ, center_Z_Yaw);

    // map คันเร่งจาก ADC → 0..100
    int t = map(rawT, base_Thrucst, 4095, 0, 100);
    if (t < 0)   t = 0;
    if (t > 100) t = 100;
    txData.thrust = (int8_t)t;

    // ส่ง packet ไปที่เครื่องบิน
    sent_Remote_Packet(txData, pipeAddr[REMOTE]);  // debug=false (default)
  }

  // ถ้ามีข้อมูลจากเครื่องบินส่งกลับมา
  if (receive_Plane_Packet(rxData))   // debug=false (default)
  {
    sent_to_Blynk();   
  }
}
