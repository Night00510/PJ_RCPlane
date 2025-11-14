#include <Arduino.h>   // ปกติ include ให้อยู่แล้ว แต่ใส่ไว้ไม่เสียหาย
#include <Ticker.h>
#include <SPI.h>
#include <RF24.h>
#include <stdint.h>    // สำหรับ int8_t, uint8_t

#define CE_PIN 4
#define CSN_PIN 5

#define XPIN_ROLL       34 // ADC input only (ADC1)
#define YPIN_PITCH      35 // ADC input only (ADC1)
#define SETCENTERPIN    0  // BOOT button on most ESP32 dev boards (active LOW)

// กำหนดค่าเริ่มต้น (กันพลาดก่อนคาลิเบรต)
int center_X_Roll  = 2048;
int center_Y_Pitch = 2048;

const int  AVG_N        = 32;    // จำนวนครั้งอ่านแล้วเฉลี่ย
const int  deadzone     = 400;   // ตั้งเดดโซน
const unsigned int longPress_ms = 2000; // กดค้าง 2 วินาทีเพื่อเริ่ม Re-Calibrate

// nRF24L01
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "NODE1";  // pipe ที่ใช้สื่อสาร

// แพ็กเกจข้อมูลที่ส่ง (-1,0,1)
struct RcPacket
{
  int8_t  roll;     // -1,0,1
  int8_t  pitch;    // -1,0,1
  uint8_t checksum; // เช็คข้อมูลเพี้ยนง่าย ๆ
};

// ตัวแปรสำหรับจับเวลาการกดค้าง
unsigned long buttonPressStartTime = 0;
bool isCalibrating = false;  // สถานะว่ากำลัง Calibrate (กันกดซ้ำหลายรอบ)

// settimer
Ticker timer_Docalibrate;

// ===== ฟังก์ชันช่วย =====

// อ่าน ADC เฉลี่ยเพื่อลด noise
int readAvg(int pin)
{
  long sum = 0;
  for (int i = 0; i < AVG_N; i++) {
    sum += analogRead(pin);
  }
  return (int)(sum / AVG_N);
}

// Calibrate (ถูกเรียกโดย Ticker หลังจากหน่วง 2 วินาที)
void doCalibrate()
{
  center_X_Roll  = readAvg(XPIN_ROLL);
  center_Y_Pitch = readAvg(YPIN_PITCH);

  Serial.printf(
    "\n*** CALIBRATION COMPLETED *** Roll center: %d Pitch center: %d\n",
    center_X_Roll, center_Y_Pitch
  );

  // รีเซ็ตสถานะ
  buttonPressStartTime = 0;
  isCalibrating        = false;
}

// จำลอง Dpad จอยสติก → -1,0,1
int digitalAxis(int raw, int center)
{
  if (raw > center + deadzone) return 1;
  if (raw < center - deadzone) return -1;
  return 0;
}

// ทำ checksum ง่าย ๆ
uint8_t doCheckSum(const RcPacket &p)
{
  // XOR พอรู้ว่าข้อมูลเสียหายไหม
  return (uint8_t)(p.roll ^ p.pitch ^ 0x5A);
}

// ===== setup / loop =====
void setup()
{
  pinMode(SETCENTERPIN, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("--- System Start ---");
  delay(500);

  // ตั้งค่า ADC ของ ESP32 ให้เต็มช่วง
  analogReadResolution(12);        // 0–4095
  analogSetAttenuation(ADC_11db);  // รองรับ ~0–3.3V

  // เริ่ม nRF24L01
  if (!radio.begin())
  {
    Serial.println("ERROR: ไม่เจอโมดูล nRF24L01 ! REBOOT ตรวจสาย/ไฟ 3.3V อีกครั้ง");
    while (1)
    {
      delay(1000);
    }
  }

  // ตั้ง RF24
  radio.setChannel(100);              // หนี WiFi นิดนึง
  radio.setDataRate(RF24_250KBPS);    // เพื่อระยะ 500m+ และเสถียร
  radio.setPALevel(RF24_PA_HIGH);     // หรือ MAX ได้ถ้าไฟนิ่ง
  radio.openWritingPipe(address);
  radio.stopListening();
  Serial.println("nRF24L01 Ready (TX mode).");

  Serial.println("Starting automatic calibration... Please keep joystick centered.");
  doCalibrate();  // Auto-calibrate ตอนเปิดเครื่องครั้งแรก
}

void loop()
{
  int buttonState = digitalRead(SETCENTERPIN);
  unsigned long currentTime = millis();

  // ปุ่มถูกกด (Active LOW)
  if (buttonState == LOW) {
    // เพิ่งเริ่มกดครั้งแรก
    if (buttonPressStartTime == 0) {
      buttonPressStartTime = currentTime;
      Serial.println("Button pressed. Hold for 2s to start Re-Calibrate...");
    }

    // เช็กว่ากดค้างครบ 2 วิ แล้วและยังไม่ได้สั่ง Calibrate
    if (!isCalibrating && (currentTime - buttonPressStartTime >= longPress_ms)) {
      isCalibrating = true; // กันไม่ให้สั่งซ้ำ
      Serial.println(
        "\n== Re-Calibrate Initiated ==\r\n"
        "*** PLEASE KEEP JOYSTICK CENTERED *** Calibrate in 2 seconds..."
      );

      // หน่วงเวลาอีก 2 วินาที ก่อนอ่านค่า center ใหม่
      timer_Docalibrate.once_ms(2000, doCalibrate);
    }
  }
  else {
    // ปุ่มถูกปล่อย
    // ถ้ายังไม่ถึงจุดที่สั่ง Re-Calibrate ก็รีเซ็ตจับเวลาไป
    if (!isCalibrating) {
      buttonPressStartTime = 0;
    }
    // ถ้า isCalibrating = true แปลว่าตั้ง Ticker ไว้แล้ว
    // ปล่อยปุ่มได้ แต่ให้มือและจอยอยู่ตรงกลางรอคาลิเบรต
  }

  // ใช้ D-Pad
  int dPadX_Roll  = digitalAxis(readAvg(XPIN_ROLL),  center_X_Roll);
  int dPadY_Pitch = digitalAxis(readAvg(YPIN_PITCH), center_Y_Pitch);

  // ส่งผ่าน nRF24L01
  RcPacket all_Sender_Packet;
  all_Sender_Packet.roll     = (int8_t)dPadX_Roll;
  all_Sender_Packet.pitch    = (int8_t)dPadY_Pitch;
  all_Sender_Packet.checksum = doCheckSum(all_Sender_Packet);

  bool ok_Radio = radio.write(&all_Sender_Packet, sizeof(all_Sender_Packet));
  if (!ok_Radio)
  {
    Serial.println("nRF24: ส่งไม่สำเร็จ (write fail)");
  }

  // Debug ดูค่าบน Serial
  Serial.printf("R: %2d | P: %2d\r\n", dPadX_Roll, dPadY_Pitch);

  delay(50);  // ~20Hz
}
