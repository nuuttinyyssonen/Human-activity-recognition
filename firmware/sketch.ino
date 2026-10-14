#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Timing constants
const unsigned long INTERVAL_US = 20000; // 20 ms -> 50 Hz
unsigned long next_due_time = 0;
unsigned long last_read_time = 0;

// Window Buffer Constants
const int WINDOW_SIZE = 100;
const int STEP = 50;
const int N_CHANNELS = 6;

float window_buffer[WINDOW_SIZE][N_CHANNELS];
int buffer_index = 0;

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
   
    if (now - next_due_time > INTERVAL_US) {
      next_due_time = now + INTERVAL_US;
      last_read_time = now;
      return;
    }

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

    // Test line
    // delay(100);

    // 1. Store sample into the buffer
    window_buffer[buffer_index][0] = ax;
    window_buffer[buffer_index][1] = ay;
    window_buffer[buffer_index][2] = az;
    window_buffer[buffer_index][3] = gx;
    window_buffer[buffer_index][4] = gy;
    window_buffer[buffer_index][5] = gz;
    
    buffer_index++;

    // 2. Detect a full window (100 samples)
    if (buffer_index >= WINDOW_SIZE) {
      Serial.println("--- Window Full! Ready for Feature Extraction ---");

      // TODO: Call feature extraction and classifier here in the next step!

      // 3. Handle 50% Overlap: Shift the last 50 samples to the front
      for (int i = 0; i < STEP; i++) {
        window_buffer[i][0] = window_buffer[i + STEP][0];
        window_buffer[i][1] = window_buffer[i + STEP][1];
        window_buffer[i][2] = window_buffer[i + STEP][2];
        window_buffer[i][3] = window_buffer[i + STEP][3];
        window_buffer[i][4] = window_buffer[i + STEP][4];
        window_buffer[i][5] = window_buffer[i + STEP][5];
      }
      
      // Reset buffer index to 50 so the next incoming samples fill the second half
      buffer_index = STEP;
    }
  }
}