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
    data.temp_C = isnan(dht_temp)  ? 0.0 : dht_temp;
    data.humid  = isnan(dht_humid) ? 0.0 : dht_humid;

    // -----------------------
    // อ่านจาก BME280
    // -----------------------
    float p_hPa = bme.readPressure() / 100.0;   // จาก Pa → hPa
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

        Serial.print("Temp: ");
        Serial.print(data.temp_C, 2); //แบบทศนิยม 2 ตำแหน่ง
        Serial.print(" C | ");

        Serial.print("Humid: ");
        Serial.print(data.humid, 2); // แบบทศนิยม 2 ตำแหน่ง
        Serial.println(" %");

        Serial.print("Pressure: ");
        Serial.print(data.pressure, 2); // แบบทศนิยม 2 ตำแหน่ง
        Serial.println(" hPa");

        Serial.print("Altitude: ");
        Serial.print(data.height, 2);// แบบทศนิยม 2 ตำแหน่ง
        Serial.println(" m");

        Serial.println("============================");
    }
    
    return data;
}