// =====================================================
// ส่ง RemoteData ออกไปที่เครื่องบิน
//   - ไม่ใช้ checksum แล้ว → ส่ง struct ตรง ๆ
//   - debug = true จะพิมพ์ข้อมูล address และเนื้อ packet ออก Serial
// =====================================================
void sent_Remote_Packet(RemoteData &sent_Packet, bool debug)
{
  // ส่ง struct ทั้งก้อนออกไป
  bool ok = radio_Sent.write(&sent_Packet, remoteData_Size);

  if (debug)
  {
    Serial.print("[TX] ");
    Serial.print(ok ? "OK   " : "FAIL ");
    Serial.print("to addr = \"");

    // แสดง address เป็นตัวอักษร (เช่น "REM01")
    for (int i = 0; i < 5; i++) {
      Serial.print(sent_From_Plane_Addrase[i], HEX);
    }

    Serial.print("\" [");

    // แสดง address แบบ HEX (ดูง่ายเวลา debug หลายท่อ)
    for (int i = 0; i < 5; i++) {
      if (i > 0) Serial.print(' ');
      Serial.print(sent_From_Remote_Addrase[i], HEX);
    }
    Serial.println("]");

    Serial.print("      roll=");   Serial.print(sent_Packet.roll);
    Serial.print(" pitch=");      Serial.print(sent_Packet.pitch);
    Serial.print(" yaw=");        Serial.print(sent_Packet.yaw);
    Serial.print(" thrust=");     Serial.println(sent_Packet.thrust);
  }
}


// =====================================================
// รับ PlaneData จากเครื่องบิน
//   - ถ้าไม่มี packet → return false
//   - ถ้ามี packet → อ่านล่าสุดเก็บใส่ receive_Packet แล้ว return true
//   - เวอร์ชันนี้ "ไม่ตรวจ checksum" แล้ว
// =====================================================
bool receive_Plane_Packet(PlaneData &receive_Packet, bool debug)
{
  // ถ้าไม่มี packet เข้ามาเลย → ไม่ต้องอ่าน
  if (!radio_Receive.available()) {
    return false;
  }

  // ถ้ามี packet ต่อคิวอยู่หลายอัน ให้อ่านจนหมด
  while (radio_Receive.available())
  {
    radio_Receive.read(&receive_Packet, planeData_Size);
  }

  if (debug)
  {
    Serial.print("[RX] Plane | P=");
    Serial.print((double)receive_Packet.pressure / 100.0);
    Serial.print("hPa  H=");
    Serial.print((double)receive_Packet.height / 100.0);
    Serial.print("m  Batt=");
    Serial.print((double)receive_Packet.battery / 100.0);
    Serial.print("V  thrust=");
    Serial.println(receive_Packet.thrust);
  }

  return true;
}


// =====================================================
// setupNRF24()
//   - ตั้งค่า radio_Sent / radio_Receive
//   - กำหนดกำลังส่ง, bitrate, channel, autoAck ฯลฯ
//   - เปิดท่อส่ง/รับตาม pipeAddr
// =====================================================
void setupNRF24()
{
    SPI.begin(VSPI_SCK, VSPI_MISO, VSPI_MOSI); 
  // เริ่มต้นโมดูลทั้งสองตัว ถ้า begin() false = มองไม่เห็นชิป
  if (!radio_Sent.begin(&SPI))
  {
    Serial.println("radio_Sent.begin() FAIL");
  }
  spiHSPI.begin(HSPI_SCK, HSPI_MISO, HSPI_MOSI);

  if (!radio_Receive.begin(&spiHSPI))
  {
    Serial.println("radio_Receive.begin() FAIL");
  }

  // กำลังส่ง (ใกล้ ๆ ใช้ LOW จะประหยัดและรบกวนน้อย)
  radio_Sent.setPALevel(RF24_PA_LOW);
  radio_Receive.setPALevel(RF24_PA_LOW);

  // bitrate ต่ำลง (250kbps) สัญญาณทะลุสิ่งกีดขวางดีกว่า
  radio_Sent.setDataRate(RF24_250KBPS);
  radio_Receive.setDataRate(RF24_250KBPS);

  // ใช้คนละ channel สำหรับทางไปและทางกลับ (กันชนกันเอง)
  radio_Sent.setChannel(25);   // รีโมท → เครื่องบิน
  radio_Receive.setChannel(30); // เครื่องบิน → รีโมท

  // เปิด Dynamic Payloads (packet ไม่จำเป็นต้องเต็ม 32 byte)
  radio_Sent.enableDynamicPayloads();
  radio_Receive.enableDynamicPayloads();

  // ----- ตั้งค่าโหมดส่งของ radio_Sent -----
  radio_Sent.stopListening();                    // โหมดส่ง
  radio_Sent.openWritingPipe(sent_From_Remote_Addrase);  // ท่อที่ใช้ส่งจากรีโมทไปเครื่องบิน

  // ----- ตั้งค่าโหมดรับของ radio_Receive -----
  radio_Receive.openReadingPipe(1, sent_From_Plane_Addrase); // ท่อที่ใช้รับจากเครื่องบิน
  radio_Receive.startListening();
}
