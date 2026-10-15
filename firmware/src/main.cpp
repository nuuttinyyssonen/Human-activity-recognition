#include <Arduino.h>
#include "tree.h"

// ---- Switches ----
#define REPLAY_MODE 1  // 1 = replay recorded dataset samples, 0 = read the MPU6050
#define RUN_TESTS   0   // 1 = run the 24-window check at boot (needs test_cases.h)

#if !REPLAY_MODE
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
Adafruit_MPU6050 mpu;
#endif

#if RUN_TESTS
#include "test_cases.h"
#endif

#if REPLAY_MODE
#include "replay.h"
int replay_i = 0, win_count = 0, replay_match = 0, replay_total = 0;
#endif

const char* CLASS_NAMES[6] = {"dws", "jog", "sit", "std", "ups", "wlk"};

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

// NumPy-style population standard deviation (divides by N)
float calc_std(float data[], int n, float mean_val) {
  float sum_sq = 0;
  for (int i = 0; i < n; i++) {
    float diff = data[i] - mean_val;
    sum_sq += diff * diff;
  }
  return sqrt(sum_sq / n);
}

void extract_features(float win[WINDOW_SIZE][N_CHANNELS], float features[NUM_FEATURES]) {
  int f_idx = 0;

  float means[N_CHANNELS];
  float stds[N_CHANNELS];
  float mins[N_CHANNELS];
  float maxs[N_CHANNELS];
  float rmss[N_CHANNELS];

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

#if RUN_TESTS
void run_tests() {
  static float work[WINDOW_SIZE][N_CHANNELS];
  static float feats[NUM_FEATURES];
  int match = 0, correct = 0;

  for (int t = 0; t < N_TESTS; t++) {
    memcpy(work, TEST_WINDOWS[t], sizeof(work));
    extract_features(work, feats);
    int pred = classify(feats);

    bool same = (pred == PY_PRED[t]);
    if (same) match++;
    if (pred == TRUE_LABEL[t]) correct++;

    Serial.print(t);
    Serial.print(": true=");   Serial.print(CLASS_NAMES[TRUE_LABEL[t]]);
    Serial.print(" python=");  Serial.print(CLASS_NAMES[PY_PRED[t]]);
    Serial.print(" esp32=");   Serial.print(CLASS_NAMES[pred]);
    Serial.println(same ? "  OK" : "  MISMATCH");
  }
  Serial.print("Matches Python: "); Serial.print(match);
  Serial.print("/"); Serial.println(N_TESTS);
  Serial.print("Matches true label: "); Serial.print(correct);
  Serial.print("/"); Serial.println(N_TESTS);
}
#endif

void setup() {
  Serial.begin(115200);

#if RUN_TESTS
  run_tests();
#endif

#if !REPLAY_MODE
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found");
    while (1) delay(10);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_1000_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  mpu.setSampleRateDivisor(19);
#endif

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

#if REPLAY_MODE
    // Replay finished: print the summary and stop
    if (replay_i >= N_SEG * SEG_LEN) {
      Serial.print("Replay done. Matches Python: ");
      Serial.print(replay_match);
      Serial.print("/");
      Serial.println(replay_total);
      while (1) delay(1000);
    }
    // Start of a new recording: clear the buffer so windows never cross recordings
    if (replay_i % SEG_LEN == 0) {
      buffer_index = 0;
      win_count = 0;
    }
    for (int c = 0; c < N_CHANNELS; c++) {
      window_buffer[buffer_index][c] = REPLAY[replay_i][c];
    }
    replay_i++;
#else
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    window_buffer[buffer_index][0] = a.acceleration.x / 9.81;
    window_buffer[buffer_index][1] = a.acceleration.y / 9.81;
    window_buffer[buffer_index][2] = a.acceleration.z / 9.81;
    window_buffer[buffer_index][3] = g.gyro.x;
    window_buffer[buffer_index][4] = g.gyro.y;
    window_buffer[buffer_index][5] = g.gyro.z;
#endif
    buffer_index++;

    if (buffer_index >= WINDOW_SIZE) {
      float features[NUM_FEATURES];

      unsigned long t0 = micros();
      extract_features(window_buffer, features);
      int predicted_class = classify(features);
      unsigned long t1 = micros();

      Serial.print("-> Predicted Activity: ");
      Serial.print(CLASS_NAMES[predicted_class]);
      Serial.print("  (compute us: ");
      Serial.print(t1 - t0);
      Serial.print(")");

#if REPLAY_MODE
      int seg = (replay_i - 1) / SEG_LEN;
      bool ok = (predicted_class == EXPECTED[seg][win_count]);
      Serial.print("  segment "); Serial.print(seg);
      Serial.print(" window ");   Serial.print(win_count);
      Serial.print(ok ? "  OK" : "  MISMATCH");
      win_count++;
      replay_total++;
      if (ok) replay_match++;
#endif
      Serial.println();

      for (int i = 0; i < STEP; i++) {
        for (int c = 0; c < N_CHANNELS; c++) {
          window_buffer[i][c] = window_buffer[i + STEP][c];
        }
      }
      buffer_index = STEP;
    }
  }
}