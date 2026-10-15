#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Timing constants
const unsigned long INTERVAL_US = 20000; // 20 ms -> 50 Hz
unsigned long next_due_time = 0;
unsigned long last_read_time = 0;

// Window constants
const int WINDOW_SIZE = 100;
const int STEP = 50;
const int N_CHANNELS = 6;
const int NUM_FEATURES = 32;

float window_buffer[WINDOW_SIZE][N_CHANNELS];
int buffer_index = 0;

// Helper function: NumPy-style population standard deviation (divides by N)
float calc_std(float data[], int n, float mean_val) {
  float sum_sq = 0;
  for (int i = 0; i < n; i++) {
    float diff = data[i] - mean_val;
    sum_sq += diff * diff;
  }
  return sqrt(sum_sq / n);
}

// Main feature extraction function for a single window buffer
void extract_features(float win[WINDOW_SIZE][N_CHANNELS], float features[NUM_FEATURES]) {
  int f_idx = 0;

  float means[N_CHANNELS];
  float stds[N_CHANNELS];
  float mins[N_CHANNELS];
  float maxs[N_CHANNELS];
  float rmss[N_CHANNELS];

  // Compute basic statistical features per channel (axis=0 equivalent)
  for (int c = 0; c < N_CHANNELS; c++) {
    float sum = 0;
    float rms_sum = 0;
    float mn = win[0][c];
    float mx = win[0][c];

    for (int i = 0; i < WINDOW_SIZE; i++) {
      float val = win[i][c];
      sum += val;
      rms_sum += (val * val);
      if (val < mn) mn = val;
      if (val > mx) mx = val;
    }

    means[c] = sum / WINDOW_SIZE;

    float col_data[WINDOW_SIZE];
    for (int i = 0; i < WINDOW_SIZE; i++) {
      col_data[i] = win[i][c];
    }
    stds[c] = calc_std(col_data, WINDOW_SIZE, means[c]);

    mins[c] = mn;
    maxs[c] = mx;
    rmss[c] = sqrt(rms_sum / WINDOW_SIZE);
  }

  // Same order as Python: [mean, std, min, max, rms]
  for (int c = 0; c < N_CHANNELS; c++) features[f_idx++] = means[c];
  for (int c = 0; c < N_CHANNELS; c++) features[f_idx++] = stds[c];
  for (int c = 0; c < N_CHANNELS; c++) features[f_idx++] = mins[c];
  for (int c = 0; c < N_CHANNELS; c++) features[f_idx++] = maxs[c];
  for (int c = 0; c < N_CHANNELS; c++) features[f_idx++] = rmss[c];

  // Accelerometer magnitude features
  float accel_mag[WINDOW_SIZE];
  float mag_sum = 0;
  for (int i = 0; i < WINDOW_SIZE; i++) {
    float ax = win[i][0];
    float ay = win[i][1];
    float az = win[i][2];
    accel_mag[i] = sqrt((ax * ax) + (ay * ay) + (az * az));
    mag_sum += accel_mag[i];
  }

  float mag_mean = mag_sum / WINDOW_SIZE;
  float mag_std = calc_std(accel_mag, WINDOW_SIZE, mag_mean);

  features[f_idx++] = mag_mean;
  features[f_idx++] = mag_std;
}

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

  unsigned long now = micros();
  last_read_time = now;
  next_due_time = now + INTERVAL_US;
}

void loop() {
  unsigned long now = micros();
  if ((long)(now - next_due_time) >= 0) {

    if (now - next_due_time > INTERVAL_US) {
      next_due_time = now + INTERVAL_US;
      last_read_time = now;
      return;
    }

    last_read_time = now;
    next_due_time += INTERVAL_US;

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    float ax = a.acceleration.x / 9.81;
    float ay = a.acceleration.y / 9.81;
    float az = a.acceleration.z / 9.81;
    float gx = g.gyro.x;
    float gy = g.gyro.y;
    float gz = g.gyro.z;

    window_buffer[buffer_index][0] = ax;
    window_buffer[buffer_index][1] = ay;
    window_buffer[buffer_index][2] = az;
    window_buffer[buffer_index][3] = gx;
    window_buffer[buffer_index][4] = gy;
    window_buffer[buffer_index][5] = gz;
    buffer_index++;

    if (buffer_index >= WINDOW_SIZE) {
      Serial.println("--- Window Full! Ready for Feature Extraction ---");

      for (int i = 0; i < STEP; i++) {
        for (int c = 0; c < N_CHANNELS; c++) {
          window_buffer[i][c] = window_buffer[i + STEP][c];
        }
      }
      buffer_index = STEP;
    }
  }
}