#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Timing constants
const unsigned long INTERVAL_US = 20000; // 20 ms -> 50 Hz
unsigned long next_due_time = 0;
unsigned long last_read_time = 0;

void setup() {
  Serial.begin(115200);
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found");
    while (1) delay(10);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_1000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  mpu.setSampleRateDivisor(19);

  // Initialize timing markers
  unsigned long now = micros();
  last_read_time = now;
  next_due_time = now + INTERVAL_US;
}

void loop() {
  unsigned long now = micros();
  if((long) (now - next_due_time) >= 0) {
    unsigned long real_interval = now - last_read_time;
    last_read_time = now;

    // Advance due time without drift
    next_due_time += INTERVAL_US;

    // 1. Read sensor data
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Convert accel from m/s^2 to g
    float ax = a.acceleration.x / 9.81;
    float ay = a.acceleration.y / 9.81;
    float az = a.acceleration.z / 9.81;
    
    // Gyro is already in rad/s
    float gx = g.gyro.x;
    float gy = g.gyro.y;
    float gz = g.gyro.z;

    // Print comma-separated line
    Serial.print(ax); Serial.print(",");
    Serial.print(ay); Serial.print(",");
    Serial.print(az); Serial.print(",");
    Serial.print(gx); Serial.print(",");
    Serial.print(gy); Serial.print(",");
    Serial.print(gz); Serial.print(",");
    Serial.println(real_interval);
  }
}