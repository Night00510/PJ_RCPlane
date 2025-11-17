#include <Wire.h>
#include <MPU6050_tockn.h>
#include "Data_Stuct.h"

MPU6050 mpu(Wire);

void setupMPU()
{
    mpu.begin();            // เริ่มสื่อสารกับ MPU6050
    delay(100);

    mpu.calcGyroOffsets();  // คาลิเบรตตอนนิ่ง
}

MPU_Data readMPU()
{
    MPU_Data data;

    mpu.update();
    data.roll  = mpu.getAngleX();
    data.pitch = mpu.getAngleY();
    data.yaw   = mpu.getAngleZ();

    return data;
}
