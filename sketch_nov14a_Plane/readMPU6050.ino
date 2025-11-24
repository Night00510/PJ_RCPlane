// ----- wrapper ฟังก์ชันเอาไว้ใช้กับ avgRead() -----

// อ่านมุม roll จาก MPU6050 (อัปเดต + คืนค่าเป็น float)
float readRoll() {
  mpu.update();               // อ่านค่าจากเซนเซอร์ + คำนวณมุมใหม่
  return mpu.getAngleX();     // คืนค่ามุม roll (องศา)
}

// อ่านมุม pitch
float readPitch() {
  mpu.update();
  return mpu.getAngleY();
}

// อ่านมุม yaw
float readYaw() {
  mpu.update();
  return mpu.getAngleZ();
}


// ฟังก์ชันอ่าน MPU6050 แล้วคืนค่าเป็น struct MPU_Data
MPU_Data read_MPU(bool debug)
{
    MPU_Data data;

    // อ่านหลายครั้งแล้วเฉลี่ยเพื่อลด noise
    // 16 ตรงนี้ปรับได้ตามใจ ถ้าอยากให้ไวขึ้นก็ลดลง เช่น 8
    data.roll  = avgRead(readRoll,  16);
    data.pitch = avgRead(readPitch, 16);
    data.yaw   = avgRead(readYaw,   16);

    if (debug)
    {
        Serial.print(F("[MPU] Roll: "));
        Serial.print(data.roll, 2);

        Serial.print(F(" | Pitch: "));
        Serial.print(data.pitch, 2);

        Serial.print(F(" | Yaw: "));
        Serial.print(data.yaw, 2);

        Serial.println(F(" deg"));
    }

    return data;
}
