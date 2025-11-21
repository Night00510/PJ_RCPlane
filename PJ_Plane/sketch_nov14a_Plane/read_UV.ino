float readUV_Index(byte pin, bool debug)
{
  float V = analogReadMilliVolts(pin) / 1000.0;  // แปลง mV → V
  float uvIndex = V * 10.0;                      // สูตร GUVA-S12SD

  if (debug) {
    Serial.print("[UV] Raw mV: ");
    Serial.print(analogReadMilliVolts(pin));

    Serial.print(" | Voltage: ");
    Serial.print(V, 3);
    Serial.print(" V");

    Serial.print(" | UV Index: ");
    Serial.println(uvIndex, 2);
  }

  return uvIndex;
}

