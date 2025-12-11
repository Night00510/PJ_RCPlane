// -------------------------
// debug helper (optional)
// -------------------------
static void printRemoteData(const RemoteData &d) {
  Serial.print("Remote roll=");
  Serial.print(d.roll);
  Serial.print(" pitch=");
  Serial.print(d.pitch);
  Serial.print(" yaw=");
  Serial.print(d.yaw);
  Serial.print(" thrust=");
  Serial.println(d.thrust);
}

static void printPlaneData(const PlaneData &p) {
  Serial.print("Plane P=");
  Serial.print(p.pressure);
  Serial.print(" uv=");
  Serial.print(p.uv_uW_cm2);
  Serial.print(" lux=");
  Serial.print(p.lux);
  Serial.print(" T=");
  Serial.print(p.temp_C);
  Serial.print(" H=");
  Serial.print(p.humid);
  Serial.print(" h=");
  Serial.print(p.height);
  Serial.print(" batt=");
  Serial.print(p.battery);
  Serial.print(" thr=");
  Serial.println(p.thrust);
}

// =====================================
// ส่งข้อมูลจาก Plane → Remote
// =====================================
void sent_Plane_Data(PlaneData &sent_Packet, bool debug)
{
  // เขียน struct ทั้งก้อนตามขนาดจริง
  bool ok = radio_Sent.write(&sent_Packet, planeData_Size);

  if (debug) {
    Serial.print("[NRF] TX Plane: ");
    Serial.println(ok ? "OK" : "FAILED");
    printPlaneData(sent_Packet);
  }
}

// =====================================
// รับข้อมูลจาก Remote → Plane
// =====================================
bool receive_Remote_Packet(RemoteData &receive_Packet, bool debug)
{
  if (!radio_Receive.available()) {
    return false;   // ยังไม่มี packet เข้ามา
  }

  // ใช้ dynamic payload size
  uint8_t len = radio_Receive.getDynamicPayloadSize();

  if (len != remoteData_Size) {
    // ขนาดไม่ตรงที่คาดไว้ → อ่านทิ้งกันค้าง
    uint8_t dump[32];
    if (len == 0 || len > sizeof(dump)) {
      len = sizeof(dump); 
    }
    radio_Receive.read(dump, len);

    if (debug) {
      Serial.print("[NRF] RX size mismatch: ");
      Serial.println(len);
    }
    return false;
  }

  // อ่าน struct RemoteData ทั้งก้อน
  radio_Receive.read(&receive_Packet, remoteData_Size);

  if (debug) {
    Serial.println("[NRF] RX Remote OK");
    printRemoteData(receive_Packet);
  }

  return true;
}

// =====================================
// ตั้งค่า nRF24 ทั้งคู่ (TX + RX)
// =====================================
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

  // ความถี่ channel
  radio_Sent.setChannel(30);
  radio_Receive.setChannel(25);

  // ความเร็วข้อมูล
  radio_Sent.setDataRate(RF24_250KBPS);
  radio_Receive.setDataRate(RF24_250KBPS);

  // กำลังส่ง
  radio_Sent.setPALevel(RF24_PA_LOW);
  radio_Receive.setPALevel(RF24_PA_LOW);

  // เปิดใช้ dynamic payloads
  radio_Sent.enableDynamicPayloads();
  radio_Receive.enableDynamicPayloads();

  // ---------- ตั้งท่อส่ง/รับ ----------
  // ส่งจากเครื่องบิน → รีโมท
  radio_Sent.stopListening();
  radio_Sent.openWritingPipe(sent_From_Plane_Addrase);

  // รับจากรีโมท → เครื่องบิน
  radio_Receive.openReadingPipe(1, sent_From_Remote_Addrase);
  radio_Receive.startListening();

}
