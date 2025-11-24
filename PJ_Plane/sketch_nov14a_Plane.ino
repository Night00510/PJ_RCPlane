  #include <SPI.h>
  #include <RF24.h>
  #include <MPU6050_tockn.h>
  #include <Adafruit_Sensor.h>
  #include <Adafruit_BME280.h>
  #include <DHT.h>
  #include <BH1750.h>
  #include <ESP32Servo.h>
  #include "Data_Struct.h"

  #define CE_TX  4    // ขาส่ง
  #define CSN_TX 5    // ขาส่ง
  #define CE_RX  16   // ขารับ
  #define CSN_RX 17   // ขารับ

  #define DHT_PIN  2
  #define DHT_TYPE DHT22

  #define GUVA_PIN 32
  #define BAT_PIN  33
  #define R1       47.0   // 47kΩ
  #define R2       10.0   // 10kΩ

  #define ROLL_LEFT_SERVO_PIN   12   // Servo ปีกช้าย
  #define ROLL_RIGHT_SERVO_PIN  13   // Servo ปีกขวา
  #define PITCH_SERVO_PIN       14   // Servo pitch
  #define YAW_SERVO_PIN         25   // Servo yaw
  #define THRUST_PIN            26   // BLDE thust

  #define WING_UP               140  // servo angle
  #define WING_DOWN             40
  #define WING_CENTER           90

  // ========= nRF24 =====================
  enum PipeID { REMODE = 0, PLANE = 1 };
  const byte pipeAddr[][6] = { "REM01" , "PLN01" };

  RF24 radio_Sent(CE_TX, CSN_TX);
  RF24 radio_Receive(CE_RX, CSN_RX);
  RemoteData rxData; // ตัวแปรเก็บข้อมูลที่รับมา
  PlaneData txData;  // ตัวแปรเก็บข้อมูลที่จะส่ง
  // ========= nRF24 =====================

  //===========MPU6050===================
  MPU6050 mpu(Wire);
  //===========MPU6050===================

  //===========BME280 DHT22====================
  Adafruit_BME280 bme;
  DHT dht(DHT_PIN, DHT_TYPE);
  //===========BME280 DHT22====================

  //===========GUVA-S12SD====================
  uint uv_Level = 0;
  //===========GUVA-S12SD====================

  //===========BH1750====================
  BH1750 light_Meter;
  //===========BH1750====================

  //===========SERVO====================
  Servo roll_Left_Servo;
  Servo roll_Right_Servo;
  Servo pitch_Servo;
  Servo yaw_Servo;
  Servo thrust_BLDC;
  //===========SERVO====================

  void setup()
  {
    pinMode(GUVA_PIN, INPUT);
    pinMode(BAT_PIN, INPUT);
    // ---------------- Serial ----------------
    Serial.begin(115200);              // เปิด Serial สำหรับ debug ดูค่าต่าง ๆ

    // ---------------- I2C / SPI ----------------
    Wire.begin();                       // เริ่มต้นระบบ I2C (ค่า default ESP32: SDA=21, SCL=22)
    SPI.begin();                        // เริ่มต้น SPI (ใช้กับ nRF24L01)

    // ---------------- Sensors Init ----------------
    // BME280 (อุณหภูมิ, ความดัน, ความสูง)
    if(!bme.begin(0x76)) {             // ที่อยู่ I2C ของ BME280 คือ 0x76
      Serial.println("BME280 not found!");
    }

    // DHT22 (อุณหภูมิ + ความชื้น)
    dht.begin();                        // เริ่มต้นเซ็นเซอร์ DHT

    // BH1750 (วัดความสว่าง lux)
    light_Meter.begin();                // ค่า default = CONTINUOUS_HIGH_RES_MODE

    // MPU6050 (IMU)
    mpu.begin();                        // เริ่มต้น MPU6050
    mpu.calcGyroOffsets(true);         // คาลิเบรต gyro ตอนเครื่องอยู่นิ่ง



    // ---------------- ADC สำหรับ GUVA ----------------
  analogReadResolution(12);        //  ตั้งADC 12 bit(0–4095)
  analogSetAttenuation(ADC_11db);  // รองรับ ~0–3.3V

    // ---------------- Servo-------- ----------------
    // --------- SERVO ATTACH ----------
    roll_Left_Servo.attach(ROLL_LEFT_SERVO_PIN);
    roll_Right_Servo.attach(ROLL_RIGHT_SERVO_PIN);
    pitch_Servo.attach(PITCH_SERVO_PIN);
    yaw_Servo.attach(YAW_SERVO_PIN);
    thrust_BLDC.attach(THRUST_PIN, 1000, 2000);  // ESC BLDC ช่วง 1000–2000 µs

    // ให้ทุกตัวอยู่กลาง 90 องศา
    roll_Left_Servo.write(WING_CENTER);
    roll_Right_Servo.write(WING_CENTER);
    pitch_Servo.write(WING_CENTER);
    yaw_Servo.write(WING_CENTER);

    // เริ่มต้น thrust ต่ำสุด (เพื่อ arm ESC ให้ปลอดภัย)
    thrust_BLDC.writeMicroseconds(0);
    delay(3000);  // รอ ESC ติ๊ด ๆ จบก่อน

    setupNRF24(); //เปิดnRF24

    // ---------------- System Ready ----------------
    Serial.println("System Ready...");
  }

  long timer1 = 0;//ใช้ส่งทุก 2 วินาที
  void loop()
  {
    if (millis() - timer1 > 2000)
    {
      timer1 = millis();

      // ================= MPU6050 =================
     MPU_Data input_MPU = read_MPU();

      // มุม (°) → ×100 เก็บใน int16_t
      // ตัวอย่าง: 12.34° → 1234
      txData.pitch = (int16_t)(input_MPU.pitch * 100);
      txData.roll  = (int16_t)(input_MPU.roll  * 100);
      txData.yaw   = (int16_t)(input_MPU.yaw   * 100);

      // ================= BME280 + DHT22 =================
      BMP280_DHT_Data input_BMP280_DHT22 = readBME_DHT();

      // อุณหภูมิ Celsius → ×100 → int16_t
      // เช่น 26.37°C → 2637
      txData.temp_C = (int16_t)(input_BMP280_DHT22.temp_C * 100);

      // ความชื้น (%) → ×100 → uint16_t
      // เช่น 55.32% → 5532
      txData.humid = (uint16_t)(input_BMP280_DHT22.humid * 100);

      // ความดัน (hPa) → ×100 → uint32_t
      // เช่น 1013.25 → 101325
      txData.pressure = (uint32_t)(input_BMP280_DHT22.pressure * 100);

      // ความสูง (เมตร) → ×100 → int16_t
     // เช่น 123.45 m → 12345
      txData.height = (int16_t)(input_BMP280_DHT22.height * 100);

      // ================= GUVA + BH1750 =================
      GUVA_BH1750 input_GUVA_BH1750 = read_UV_BH1750(GUVA_PIN);

      // ความเข้ม UV (µW/cm²) → ×100 → uint32_t
      txData.uv_uW_cm2 = (uint32_t)(input_GUVA_BH1750.uv_uW_cm2 * 100);

      // UV Index → ×100 → uint16_t
      // เช่น 5.23 → 523
      txData.uv_Index = (uint16_t)(input_GUVA_BH1750.uv_Index * 100);

      // ความสว่าง (lux) → ×100 → uint32_t
      // เช่น 1234.56 lux → 123456
      txData.lux = (uint32_t)(input_GUVA_BH1750.lux * 100);

      float plane_bat = readBatteryVoltage();         
      txData.battery = (uint16_t)(plane_bat * 100);   


      // ================= ส่งข้อมูล =================
      // ส่งที่ pipe "PLN01"
      sent_Plane_Data(txData);
    }

    if (receive_Remote_Packet(rxData))
    {
        Control_Surfaces();
    }
  }

