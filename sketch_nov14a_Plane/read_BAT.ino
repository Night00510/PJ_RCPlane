float readBatteryVoltage()
{
  // อ่านค่าเป็น mV จาก ADC
  int mv = analogReadMilliVolts(BAT_PIN);

  // แปลงเป็นโวลต์ที่ขา ADC เห็น
  float v_adc = mv / 1000.0f;

  // ย้อนสูตรวงจรแบ่งแรงดัน
  float v_bat = v_adc * ((R1 + R2) / R2);

  // clamp กันค่าหลุด
  if (v_bat < 0.0f)  v_bat = 0.0f;
  if (v_bat > 20.0f) v_bat = 20.0f;

  return v_bat;   // โวลต์จริง เช่น 11.45V
}
