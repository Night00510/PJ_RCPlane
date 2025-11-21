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
    Serial.begin(115200);
    SPI.begin();   // สำคัญมากสำหรับ ESP32

    bme.begin(0x76);
    dht.begin();

    // --------- ฝั่งรับ ----------
    radio_Receive.begin();
    radio_Receive.setPALevel(RF24_PA_LOW);
    radio_Receive.setDataRate(RF24_1MBPS);
    radio_Receive.setChannel(100);

    // รับจากรีโมทที่ส่งด้วย "REM01"
    radio_Receive.openReadingPipe(0, pipeAddr[REMODE]);
    radio_Receive.startListening();

    Serial.println("Receiver Ready...");
  }

  void loop()
  {
    receive_Remote_Packet(rxData, 1);

    delay(500);
  }
