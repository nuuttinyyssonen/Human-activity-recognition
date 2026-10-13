#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Serial.println("MPU6050 test!");

  Wire.begin();

  // Read WHO_AM_I
  Wire.beginTransmission(0x68);
  Wire.write(0x75);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 1);
  uint8_t id = Wire.read();
  Serial.println(id, HEX);

  if (id != 0x68) {
    Serial.println("MPU6050 not found");
    while (1) delay(10);
  }
  Serial.println("MPU6050 Found!");

  // Wake the chip: clear SLEEP in PWR_MGMT_1
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10); // this speeds up the simulation
}
