void Control_Surfaces()
{
  switch (rxData.takeoff_landing)
  {
    case -1: // กดปีกขึ้นทั้งสองข้าง
      roll_Left_Servo.write(WING_UP);
      roll_Right_Servo.write(WING_UP);
      break;

    case 1:  // กดปีกลงทั้งสองข้าง
      roll_Left_Servo.write(WING_DOWN);
      roll_Right_Servo.write(WING_DOWN);
      break;

    default:  // ใช้ค่า roll/pitch/yaw ปกติ
    {
      // --------- ROLL ---------
      switch (rxData.roll)
      {
        case -1: // เอียงซ้าย
          roll_Left_Servo.write(WING_UP);
          roll_Right_Servo.write(WING_DOWN);
          break;

        case 1:  // เอียงขวา
          roll_Left_Servo.write(WING_DOWN);
          roll_Right_Servo.write(WING_UP);
          break;

        default: // ตรง
          roll_Left_Servo.write(WING_CENTER);
          roll_Right_Servo.write(WING_CENTER);
          break;
      }

      // --------- PITCH ---------
      switch (rxData.pitch)
      {
        case 1:  // หัวขึ้น
          pitch_Servo.write(WING_DOWN);
          break;

        case -1: // หัวลง
          pitch_Servo.write(WING_UP);
          break;

        default: // กลาง
          pitch_Servo.write(WING_CENTER);
          break;
      }

      // --------- YAW ---------
      switch (rxData.yaw)
      {
        case 1:  // หันซ้าย
          yaw_Servo.write(WING_UP);
          break;

        case -1: // หันขวา
          yaw_Servo.write(WING_DOWN);
          break;

        default: // กลาง
          yaw_Servo.write(WING_CENTER);
          break;
      }
    }
    break;
  }

  // --------- THRUST ---------
  uint thrust = map(rxData.thrust, 0, 100, THRUST_MIN, THRUST_MAX);
  thrust_BLDC.writeMicroseconds(thrust);
}
