// extern Adafruit_BME280 bme;
// extern DHT dht;

// กำหนดค่าความกดอากาศ ณ ระดับน้ำทะเล (hPa)
#define SEALEVELPRESSURE (1013.25f)

// เก็บค่าความดัน "อ้างอิง" จากการอ่านครั้งแรก (hPa)
static float basePressure_hPa = NAN;

// ฟังก์ชันอ่านข้อมูล
BMP280_DHT_Data readBME_DHT(bool read_SEALEVEL, bool debug) 
{
    BMP280_DHT_Data data;

    // -----------------------
    // อ่านจาก DHT22
    // -----------------------
    float dht_temp  = dht.readTemperature();
    float dht_humid = dht.readHumidity();

    data.temp_C  = isnan(dht_temp)  ? 0.0f : dht_temp;
    data.temp_F  = isnan(dht_temp)  ? 0.0f : dht_temp;
    data.humid = isnan(dht_humid) ? 0.0f : dht_humid;

    // -----------------------
    // อ่านจาก BME/BMP280
    // -----------------------
    float p_hPa = bme.readPressure() / 100.0f;   // จาก Pa → hPa
    data.pressure = p_hPa;

    // ถ้ายังไม่เคยตั้ง baseline (ครั้งแรกที่เรียกฟังก์ชัน) ถ้ายังไม่เคยเก็บ basePressure_hPa 
    if (isnan(basePressure_hPa)) 
    {
        basePressure_hPa = p_hPa;   // ล็อกค่าความดันตอนนี้ไว้เป็น "ระดับ 0"
    }

    data.height = (read_SEALEVEL) ? bme.readAltitude(SEALEVELPRESSURE) : bme.readAltitude(basePressure_hPa);

    // -----------------------
    // DEBUG PRINT
    // -----------------------
    if (debug)
    {
        Serial.println(F("--- BME280 + DHT22 Data ---"));
        
        Serial.print(F("Temp (DHT): "));
        Serial.print(data.temp_C);
        Serial.print(F(" C | Temp (DHT): "));
        Serial.print(data.temp_F);
        Serial.print(F(" F | Humid (DHT): "));
        Serial.print(data.humid);
        Serial.print(F(" %"));

        Serial.print(F(" | Pressure: "));
        Serial.print(data.pressure);
        Serial.print(F(" hPa"));

        Serial.print(F(" | Altitude: "));
        Serial.print(data.height);
        Serial.println(F(" m"));
        Serial.println(F("----------------------------"));
    }
    
    return data;
}
