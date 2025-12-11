void debugRemoteData(const RemoteData &d)
{
  Serial.print("[Remote TX] roll=");
  Serial.print(d.roll);
  Serial.print(" pitch=");
  Serial.print(d.pitch);
  Serial.print(" yaw=");
  Serial.print(d.yaw);
  Serial.print(" takeoff=");
  Serial.print(d.takeoff_landing);
  Serial.print(" thrust=");
  Serial.println(d.thrust);
}

void debugPlaneData(const PlaneData &p)
{

  Serial.print("[Plane RX] P=");
  Serial.print(p.pressure);           // hPa ×100
  Serial.print(" UV=");
  Serial.print(p.uv_uW_cm2);          // uW/cm2 ×100
  Serial.print(" Lux=");
  Serial.print(p.lux);                // lux ×100

  Serial.print(" Pitch=");
  Serial.print(p.pitch / 100.0);
  Serial.print(" Roll=");
  Serial.print(p.roll / 100.0);
  Serial.print(" Yaw=");
  Serial.print(p.yaw / 100.0);

  Serial.print(" Temp=");
  Serial.print(p.temp_C / 100.0);
  Serial.print("C Humid=");
  Serial.print(p.humid / 100.0);
  
  Serial.print(" UVindex=");
  Serial.print(p.uv_Index / 100.0);

  Serial.print(" Batt=");
  Serial.print(p.battery / 100.0);
  Serial.print("V");

  Serial.print(" Height=");
  Serial.print(p.height / 100.0);
  Serial.print("m");

  Serial.print(" Thrust=");
  Serial.println(p.thrust);
}

void randomPlane(PlaneData &p)
{
  // ความดัน — ปกติ 900–1100 hPa → ×100
  p.pressure = random(90000, 110000 + 1);

  // UV (uW/cm2 ×100) สมมุติ 0–3000
  p.uv_uW_cm2 = random(0, 300000 + 1);

  // lux ×100 — ปกติ indoor 0–200k
  p.lux = random(0, 20000000 + 1);

  // pitch / roll / yaw = -45.00° ถึง +45.00°
  p.pitch = random(-4500, 4500 + 1);
  p.roll  = random(-4500, 4500 + 1);
  p.yaw   = random(-18000, 18000 + 1); // yaw หมุนรอบได้มากกว่า

  // temp_C ×100 — 15°C ถึง 40°C
  p.temp_C = random(1500, 4000 + 1);

  // humidity ×100 — 20% ถึง 90%
  p.humid = random(2000, 9000 + 1);

  // UV index ×100 — 0.00 ถึง 11.00
  p.uv_Index = random(0, 1100 + 1);

  // battery (V ×100) — LiPo 3S: 10.5–12.6 V
  p.battery = random(1050, 1260 + 1);

  // height ×100 — -10.00 ถึง 500.00 m
  p.height = random(-1000, 50000 + 1);

  // thrust (0–100)
  p.thrust = random(0, 101);
}

void randomRemote(RemoteData &d)
{
  // random(-1, 2) → ให้ค่า -1,0,1
  d.roll  = random(-1, 2);
  d.pitch = random(-1, 2);
  d.yaw   = random(-1, 2);

  // -1,0,+1
  d.takeoff_landing = random(-1, 2);

  // thrust 0–100
  d.thrust = random(0, 101);
}

