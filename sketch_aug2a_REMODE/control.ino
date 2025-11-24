// อ่าน ADC เฉลี่ยเพื่อลด noise
int read_Avg_ADC(int pin)
{
  long sum = 0;
  for (int i = 0; i < AVG_N; i++) {
    sum += analogRead(pin);
  }
  return (int)(sum / AVG_N);
}

// จำลอง Dpad จอยสติก → -1,0,1
int digitalAxis(int raw, int center)
{
  if (raw > center + deadzone) return 1;
  if (raw < center - deadzone) return -1;
  return 0;
}

void doCalibrate()
{
  center_X_Roll  = read_Avg_ADC(XPIN_ROLL); // ตั้งศูนย์กลาง
  center_Y_Pitch = read_Avg_ADC(YPIN_PITCH);
  center_Z_Yaw   = read_Avg_ADC(ZPIN_YAW);
  base_Thrucst = read_Avg_ADC(PIN_THRUST);
}


