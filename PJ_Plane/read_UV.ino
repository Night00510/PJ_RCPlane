GUVA_BH1750 read_UV_BH1750(byte pin, bool debug)
{
    GUVA_BH1750 data;

    // อ่านค่า ADC เป็น mV 
    int mv = analogReadMilliVolts(pin);
    float V = mv / 1000.0f;

    // ===== GUVA Conversion =====
    // Datasheet โดยประมาณ:
    // 1.0V ≈ UV Index 10 ≈ ~250 µW/cm²
    data.uv_Index  = V * 10.0f;
    data.uv_uW_cm2 = V * 250.0f;

    // ===== BH1750 =====
    data.lux = light_Meter.readLightLevel();

    // ----- clamp ป้องกันค่าเพี้ยน -----
    if (data.uv_Index < 0) data.uv_Index = 0;
    if (data.uv_uW_cm2 < 0) data.uv_uW_cm2 = 0;

    if (debug)
    {
        Serial.println(F("=== GUVA + BH1750 ==="));

        Serial.print(F("ADC: "));
        Serial.print(mv);
        Serial.print(F(" mV | V = "));
        Serial.print(V, 3);
        Serial.println(F(" V"));

        Serial.print(F("UV Index: "));
        Serial.print(data.uv_Index, 2);
        Serial.print(F("  | Irradiance: "));
        Serial.print(data.uv_uW_cm2, 2);
        Serial.println(F(" uW/cm^2"));

        Serial.print(F("Light: "));
        Serial.print(data.lux, 2);
        Serial.println(F(" lux"));

        Serial.println(F("======================"));
    }

    return data;
}
