// ใช้ radio_Sent / radio_Receive ที่ประกาศอยู่ในไฟล์หลัก (ไม่สร้างซ้ำ)
// extern RF24 radio_Sent;
// extern RF24 radio_Receive;

// ตาราง address ของแต่ละ pipe (0 = REMOTE, 1 = PLANE)
// extern const byte pipeAddr[][6];


// ============================================
// ฟังก์ชันคำนวณ checksum แบบ XOR ทีละ byte
// ============================================
byte doCheckSum(const byte *buf, size_t len)
{
  byte sum = 0;

  for (size_t i = 0; i < len; i++)
  {
    sum ^= buf[i];
  }

  return sum;
}


// =====================================================
// ฟังก์ชันส่ง PlaneData → ส่งไปที่รีโมทอย่างเดียว
// debug = true เพื่อเปิดแสดงผลทาง Serial
// =====================================================
void sent_Remote_Packet(RemoteData &sent_Packet, const byte *addr, bool debug)
{
  const byte *buf = (const byte*)&sent_Packet;

  // คำนวณ checksum
  sent_Packet.checksum = doCheckSum(buf, remoteData_Size - 1);

  // เข้าโหมดส่ง
  radio_Sent.stopListening();

  // ใช้ address ที่ส่งเข้ามา
  radio_Sent.openWritingPipe(addr);

  // ส่ง packet ออกไป
  bool ok = radio_Sent.write(&sent_Packet, remoteData_Size);

  // debug runtime เปิด-ปิดได้ตอนเรียกฟังก์ชัน
  if (debug)
  {
    Serial.print("Send ");
    Serial.print(ok ? "OK" : "FAIL");
    Serial.print(" to addr = \"");

    // แสดง address เป็นตัวอักษร (เช่น "REM01")
    for (int i = 0; i < 5; i++) {
      Serial.write(addr[i]);
    }

    Serial.print("\" [");

    // แสดง address แบบ HEX ด้วย
    for (int i = 0; i < 5; i++) {
      if (i > 0) Serial.print(' ');
      Serial.print(addr[i], HEX);
    }

    Serial.println("]");
  }
}



// =====================================================
// รับ RemoteData ← รับจากรีโมททางเดียว
// debug = true เพื่อเปิดแสดงผลทาง Serial
// =====================================================
bool receive_Plane_Packet(PlaneData &receive_Packet, bool debug)
{
  // ถ้าไม่มี packet เข้ามาเลย → ออก
  if (!radio_Receive.available()) {
    return false;
  }

  while (radio_Receive.available()) //ถ้ามี packet ค้างในชิปจะอ่าน packet
  {
    // อ่านข้อมูลออกจาก RX FIFO 
    radio_Receive.read(&receive_Packet, planeData_Size);
  }
  // แปลง struct เป็น byte*
  const byte *buf = (const byte *)&receive_Packet;

  // คำนวณ checksum ใหม่
  byte checksum = doCheckSum(buf, planeData_Size - 1);

  // ตรวจว่า checksum ตรงไหม
  if (checksum != receive_Packet.checksum)
  {
    if (debug)
    {
      Serial.print("Bad checksum, calc = ");
      Serial.print(checksum);
      Serial.print(" pkt = ");
      Serial.println(receive_Packet.checksum);
    }

    return false;
  }

  if (debug)
  {
    Serial.print("[RX] OK | Roll=");
    Serial.print(receive_Packet.roll);
    Serial.print(" Pitch=");
    Serial.print(receive_Packet.pitch);
    Serial.print(" Yaw=");
    Serial.print(receive_Packet.yaw);
    Serial.print(" Thrust=");
    Serial.println(receive_Packet.thrust);
  }

  return true;  // ใช้ได้
}

void setupNRF24()
{
  // เริ่มต้นโมดูลทั้งสองตัว
  if (!radio_Sent.begin())
  {
    Serial.println(F("radio_Sent.begin() FAIL"));
  }

  if (!radio_Receive.begin())
  {
    Serial.println(F("radio_Receive.begin() FAIL"));
  }

  // ตั้งกำลังส่ง (ลอง LOW / MEDIUM ถ้าใกล้ ๆ พอ)
  radio_Sent.setPALevel(RF24_PA_HIGH);
  radio_Receive.setPALevel(RF24_PA_HIGH);

  // ตั้ง bitrate 
  radio_Sent.setDataRate(RF24_250KBPS);
  radio_Receive.setDataRate(RF24_250KBPS);

  // ตั้งช่อง (2.4GHz + channel/100) เช่น 2.508 GHz
  radio_Sent.setChannel(30);
  radio_Receive.setChannel(25);

  // Auto-Ack และ retries เวลาส่งไม่ติด
  radio_Sent.setAutoAck(false);
  radio_Sent.setRetries(0, 0);
  radio_Receive.setAutoAck(true);
  radio_Receive.setRetries(5, 1); // หน่วง, จำนวนครั้ง retry       /// delay = หน่วงเวลาก่อน แต่ละ การ retry (หน่วยเป็น step ~250µs)

  //ปกติ nRF24 จะส่งข้อมูลได้สูงสุด 32 bytes ต่อแพ็กเก็ต แต่ถ้าเปิด Dynamic Payloads → มันจะส่งข้อมูล เท่าที่ใช้จริง โดยไม่ต้องเต็ม 32 byte ทุกครั้ง ถ้าเกินจะตัดส่วนเกินออก ต้องแบ่ง packet เอง
  radio_Sent.enableDynamicPayloads();
  radio_Receive.enableDynamicPayloads();

  //mode ส่ง
  radio_Sent.stopListening();
  radio_Sent.openWritingPipe(pipeAddr[REMOTE]);//ส่งจาก remote

  //mode รับ
  radio_Receive.openReadingPipe(1, pipeAddr[PLANE]); //รับจากเครื่องบิน
  radio_Receive.startListening();
}