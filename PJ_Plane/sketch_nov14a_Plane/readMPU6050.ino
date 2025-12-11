float norm360(float a)
{
    if (a >= 360.0f) a -= 360.0;
    if (a < 0.0f)    a += 360.0;
    return a;
}

MPU_Data read_MPU(bool debug)
{
    MPU_Data data;

    // อ่านหลายครั้งแล้วเฉลี่ยเพื่อลด noise
    const int N = 16;

    float sumRoll  = 0.0;
    float sumPitch = 0.0;
    float sumYaw   = 0.0;

    for (int i = 0; i < N; ++i)
    {
        mpu.update();

        // อ่านค่าจาก MPU
        float r = mpu.getAngleX();  // roll (deg)
        float p = mpu.getAngleY();  // pitch (deg)
        float y = mpu.getAngleZ();  // yaw (deg)

        sumRoll  += r;
        sumPitch += p;
        sumYaw   += y;

        delay(2);  // หน่วงเล็กน้อย
    }

    // หาค่าเฉลี่ย แล้วค่อย normalize เป็นช่วง 0–360
    data.roll  = norm360(sumRoll  / N);
    data.pitch = norm360(sumPitch / N);
    data.yaw   = norm360(sumYaw   / N);

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
