float readBH1750(bool debug)
{
  float lux = light_Meter.readLightLevel();

  if (debug) {
    Serial.print("[BH1750] Light: ");
    Serial.print(lux, 2);
    Serial.println(" lux");
  }

  return lux;
}