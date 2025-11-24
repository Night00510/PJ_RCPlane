// extern Adafruit_BME280 bme;
// extern DHT dht;

#define SEALEVELPRESSURE (1013.25f)

// เก็บค่าความดันอ้างอิงสำหรับคำนวณความสูงสัมพัทธ์
static float basePressure_hPa = NAN;

// ฟังก์ชันอ่านค่าจาก BME280 + DHT22
BMP280_DHT_Data readBME_DHT(bool read_SEALEVEL, bool debug) 
{
    BMP280_DHT_Data data;

    // -----------------------
    // อ่านจาก DHT22
    // -----------------------
    float dht_temp  = dht.readTemperature();   // °C
    float dht_humid = dht.readHumidity();      // %

    // ถ้าอ่านไม่ได้ ให้เป็น 0
    data.temp_C = isnan(dht_temp)  ? 0.0f : dht_temp;
    data.temp_F = isnan(dht_temp)  ? 0.0f : (dht_temp * 1.8f + 32.0f); // แปลง °F
    data.humid  = isnan(dht_humid) ? 0.0f : dht_humid;

    // -----------------------
    // อ่านจาก BME280
    // -----------------------
    float p_hPa = bme.readPressure() / 100.0f;   // จาก Pa → hPa
    data.pressure = p_hPa;

    // ถ้ายังไม่เคยตั้ง basePressure ให้ตั้งจากค่าครั้งแรก
    if (isnan(basePressure_hPa)) 
    {
        basePressure_hPa = p_hPa;   // ใช้ความดันตอน start เป็นระดับ 0 เมตร
    }

    // เลือกว่าจะใช้ sea level จริง หรือ altitude เทียบจาก position ปัจจุบัน
    if (read_SEALEVEL)
        data.height = bme.readAltitude(SEALEVELPRESSURE); 
    else
        data.height = bme.readAltitude(basePressure_hPa);

    // -----------------------
    // DEBUG PRINT
    // -----------------------
    if (debug)
    {
        Serial.println(F("=== BME280 + DHT22 Data ==="));

        Serial.print(F("Temp: "));
        Serial.print(data.temp_C, 2);
        Serial.print(F(" C | "));
        Serial.print(data.temp_F, 2);
        Serial.println(F(" F"));

        Serial.print(F("Humid: "));
        Serial.print(data.humid, 2);
        Serial.println(F(" %"));

        Serial.print(F("Pressure: "));
        Serial.print(data.pressure, 2);
        Serial.println(F(" hPa"));

        Serial.print(F("Altitude: "));
        Serial.print(data.height, 2);
        Serial.println(F(" m"));

        Serial.println(F("============================"));
    }
    
    return data;
}