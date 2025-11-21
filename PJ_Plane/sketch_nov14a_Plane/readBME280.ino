extern Adafruit_BME280 bme;
extern DHT dht;

// กำหนดค่าความกดอากาศ ณ ระดับน้ำทะเล (hPa)
#define SEALEVELPRESSURE_HPA (1013.25)

// -----------------------------------------------------------
// ฟังก์ชันอ่านค่าเซนเซอร์รวม (คืนค่าเป็น Struct)
// -----------------------------------------------------------
BMP280_DHT_Data readBME_DHT(bool read_SEALEVEL, bool debug) 
{
    BMP280_DHT_Data data;

    // A. อ่านค่าจาก BME280/BMP280
    float temp_C = bme.readTemperature();
    data.temp = temp_C;
    data.pressure = bme.readPressure() / 100.00F;
    if (read_SEALEVEL)
    {
      
    }
    else
    {
      data.height = bme.readAltitude(SEALEVELPRESSURE_HPA);
    }

    // B. อ่านค่าจาก DHT22 (สำหรับความชื้น)
    float dht_humid = dht.readHumidity();

    if (isnan(dht_humid)) {
        data.humid = 0.0; // ค่า Error
    } else {
        data.humid = dht_humid;
    }

    if (debug)
    {
        Serial.println(F("--- BME280/DHT22 Data ---"));
        
        Serial.print(F("Temp: "));
        Serial.print(data.temp);
        Serial.print(F(" C | Press: "));
        Serial.print(data.pressure);
        Serial.print(F(" hPa"));

        Serial.print(F(" | Humid: "));
        Serial.print(data.humid);
        Serial.print(F(" % | Alt: "));
        Serial.print(data.height);
        Serial.println(F(" m"));
        Serial.println(F("-------------------------"));
    }
    
    // คืนค่า Struct ที่มีข้อมูลทั้งหมด
    return data;
}