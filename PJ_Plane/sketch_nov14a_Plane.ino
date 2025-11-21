  #include <SPI.h>
  #include <RF24.h>
  #include <MPU6050_tockn.h>
  #include <Adafruit_Sensor.h>
  #include <Adafruit_BME280.h>
  #include <DHT.h>
  #include <BH1750.h>
  #include "Data_Stuct.h"   // <- แก้ชื่อไฟล์ให้ตรงกับจริง

  #define CE_TX  4    // ขาส่ง
  #define CSN_TX 5    // ขาส่ง
  #define CE_RX  16   // ขารับ
  #define CSN_RX 17   // ขารับ

  #define DHT_PIN 2
  #define DHT_TYPE DHT22

  #define GUVA_PIN 32
  #define BAT_PIN 33
  #define R1 47.0   // 47kΩ
  #define R2 10.0   // 10kΩ

  // ========= nRF24 =====================
  enum PipeID { REMODE = 0, PLANE = 1 };
  const byte pipeAddr[][6] = { "REM01" , "PLN01" };

  RF24 radio_Sent(CE_TX, CSN_TX);
  RF24 radio_Receive(CE_RX, CSN_RX);
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

  RemoteData rxData; // ตัวแปรเก็บข้อมูลที่รับมา
  PlaneData txData;  // ตัวแปรเก็บข้อมูลที่จะส่ง

  void setup()
  {
    // ---------------- Serial ----------------
    Serial.begin(115200);              // เปิด Serial สำหรับ debug ดูค่าต่าง ๆ

    // ---------------- I2C / SPI ----------------
    Wire.begin();                       // เริ่มต้นระบบ I2C (ค่า default ESP32: SDA=21, SCL=22)
    SPI.begin();                        // เริ่มต้น SPI (ใช้กับ nRF24L01)

    // ---------------- Sensors Init ----------------
    // BME280 (อุณหภูมิ, ความดัน, ความสูง)
    if(!bme.begin(0x76)) {             // ที่อยู่ I2C ของ BME280 ของคุณคือ 0x76
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
    analogSetPinAttenuation(GUVA_PIN, ADC_11db);  // ให้ ADC อ่านได้ช่วง ~0–3.3V
    analogSetPinAttenuation(BAT_PIN, ADC_11db);
    analogReadResolution(12);                     // ตั้ง ADC เป็น 12-bit (0–4095)

    // ---------------- Radio Receive ----------------
    radio_Receive.begin();              // เริ่มต้นโมดูลฝั่งรับ
    radio_Receive.setPALevel(RF24_PA_MAX);   // กำลังรับ/ส่งสูงสุด
    radio_Receive.setDataRate(RF24_250KBPS); // ลด data rate ให้ไกลขึ้น
    radio_Receive.setChannel(100);           // เลี่ยงชน WiFi

    // เปิดรับจาก pipe ของรีโมท "REM01"
    radio_Receive.openReadingPipe(0, pipeAddr[REMODE]);
    radio_Receive.startListening();     // เข้าสู่โหมดรับ

    // ---------------- Radio Send ----------------
    radio_Sent.begin();                // เริ่มต้นโมดูลฝั่งส่ง
    radio_Sent.setPALevel(RF24_PA_MAX);      // กำลังส่งสูงสุด
    radio_Sent.setDataRate(RF24_250KBPS);    // ลด data rate เพื่อเพิ่มระยะ
    radio_Sent.setChannel(100);              // ต้องตรงกับฝั่งรับ
    radio_Sent.setRetries(5, 5);             // ตั้ง retry เวลาแพ็กเก็ตหลุด setRetries( delay, count ) รอ 5 × 250µs = 1250µs จะลองใหม่ สูงสุด 5 ครั้ง

    // ---------------- System Ready ----------------
    Serial.println("System Ready...");
  }

  long timer1 = 0;
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
      sent_Plane_Data(txData, pipeAddr[PLANE]);
    }
  }

