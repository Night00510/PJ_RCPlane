void Control_Surfaces()

{
  switch (rxData.roll)
  {
    case -1: // เอียงช้าย
      roll_Left_Servo.write(WING_UP);
      roll_Right_Servo.write(WING_DOWN);
      break;
    case 1: // เอียงขวา
      roll_Left_Servo.write(WING_DOWN);
      roll_Right_Servo.write(WING_UP);
      break;
    default: // ตรง
      roll_Left_Servo.write(WING_CENTER);
      roll_Right_Servo.write(WING_CENTER);
      break;
  }

  switch (rxData.pitch)
  {
    case -1: // หันหัวลง
      pitch_Servo.write(WING_DOWN);
      break;
    case 1: // หันหัวขึ้น
      pitch_Servo.write(WING_UP);
      break;
    default: // หันหัวตรง
      pitch_Servo.write(WING_CENTER);
      break;
  }

  switch (rxData.yaw)
  {
    case -1: // หันหัวช้าย
      yaw_Servo.write(WING_UP);
      break;
    case 1: // หันหัวขวา
      yaw_Servo.write(WING_DOWN);
      break;
    default: // หันหัวตรง
      yaw_Servo.write(WING_CENTER);
      break;
  }

  thrust_BLDC.writeMicroseconds(rxData.thrust);
}