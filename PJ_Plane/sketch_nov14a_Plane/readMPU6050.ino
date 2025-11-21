// extern MPU6050 mpu;

// ฟังก์ชันอ่าน MPU6050 แล้ว return struct MPU_Data
MPU_Data readMPU(bool debug)
{
    MPU_Data data;

    // อ่านข้อมูลทั้งหมดจากเซนเซอร์ MPU6050 และคำนวณมุม
    mpu.update(); 

    // เก็บค่ามุมเข้าใน Struct MPU_Data
    data.roll  = mpu.getAngleX();
    data.pitch = mpu.getAngleY();
    data.yaw   = mpu.getAngleZ();

    if (debug)
    {  
        // แสดงผลค่ามุมออกทาง Serial Monitor
        Serial.print("Roll: ");
        Serial.print(data.roll);
        
        Serial.print(" | Pitch: ");
        Serial.print(data.pitch);
        
        Serial.print(" | Yaw: ");
        Serial.print(data.yaw);
        
        Serial.println(); // ขึ้นบรรทัดใหม่หลังแสดงค่าทั้งหมด
    }

    return data;
}

