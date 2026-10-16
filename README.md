# Human Activity Recognition

# Running the project
1. First the dataset is needed from https://www.kaggle.com/datasets/malekzadeh/motionsense-dataset or you can use the download_data.py
2. Put the dataset in project root in folder called data.
3. packages that needs to be installed pandas, numpy, scikit-learn
4. You can try to run load_data.py to see that the data works.
5. Run train.py from /training
6. Make sure /firmware/include contains replay.h, test_cases.h and tree.h

One thing to note!:
main.cpp might give error for "#include errors detected. Please update your includePath. Syntax errors for this file will not be reported until included files are found.". To work around this: Select PlatformIO from VScode and "Pick a folder". Select the firmware folder and it opens only firmware folder in vs code. This allows you to build from platfomio.ini and compile from diagram.json

## Firmware
- Current Deployment: Fully functional local implementation (migrated to PlatformIO/VS Code for optimal local compilation and testing).
- Validation Modes: Supports both real-time MPU6050 sensor polling (at a strict 50 Hz using a jitter-resistant micros() scheduler) and offline replay testing (replay.h) to verify C++ and Python pipeline parity.

## Data
- MotionSense, 50 Hz, 24 subjects, 6 activities (iPhone 6s, front pocket)
- Channels: acc (gravity + userAcceleration, g) and rotationRate (rad/s)
- Source: https://github.com/mmalekzadeh/motion-sense

## Pipeline contract
- Window: 100 samples (2 s), step 50 (50% overlap), no window crosses a recording
- Split: by subject (19 train / 5 test, seed 42)
- Features: 32 (mean, std, min, max, RMS per channel + accel magnitude mean/std)

## Results (decision tree)

### Depth sweep (5-fold cross-validation by subject, training set only)
| max_depth | Train macro F1 | Val macro F1 |
|-----------|----------------|--------------|
| 2         | 0.440          | 0.427        |
| 3         | 0.622          | 0.604        |
| 4         | 0.734          | 0.713        |
| 6         | 0.862          | 0.799        |
| 8         | 0.922          | **0.823**    |
| 12        | 0.972          | 0.803        |
| None      | 1.000          | 0.766        |

Validation F1 peaks at depth 8, then falls while training F1 keeps rising to 1.0. That is overfitting: deeper trees memorize the training subjects. Depth 8 was selected.

### Test set (5 held-out subjects, evaluated once)
| Model   | Test macro F1 | Test accuracy |
|---------|---------------|---------------|
| depth 8 | 0.84          | 0.89          |

- Per class: sit and std are about 1.00, and jog is 0.90 F1.
- Weak: dws recall 0.50, ups recall 0.61.
- Validation (0.82) and test (0.84) agree, which suggests no leakage.

## Replay mode log on main.cpp
-> Predicted Activity: dws  (compute us: 2384)  segment 0 window 0  OK
-> Predicted Activity: dws  (compute us: 2385)  segment 0 window 1  OK
-> Predicted Activity: jog  (compute us: 2383)  segment 1 window 0  OK
-> Predicted Activity: jog  (compute us: 2385)  segment 1 window 1  OK
-> Predicted Activity: sit  (compute us: 2382)  segment 2 window 0  OK
-> Predicted Activity: sit  (compute us: 2389)  segment 2 window 1  OK
-> Predicted Activity: std  (compute us: 2382)  segment 3 window 0  OK
-> Predicted Activity: std  (compute us: 2387)  segment 3 window 1  OK
-> Predicted Activity: wlk  (compute us: 2385)  segment 4 window 0  OK
-> Predicted Activity: dws  (compute us: 2385)  segment 4 window 1  OK
-> Predicted Activity: wlk  (compute us: 2389)  segment 5 window 0  OK
-> Predicted Activity: wlk  (compute us: 2383)  segment 5 window 1  OK
Replay done. Matches Python: 12/12

## AI use log:

- **Firmware Timing & MPU6050 Integration:** Used AI guidance to implement an overflow-safe micros() timing scheduler running at a precise 50 Hz loop interval with lag-rejection guardrails, combined with the Adafruit MPU6050 driver configured for optimal sensor range and filter bandwidth.
- **data formatting** (`features.py`): Claude suggested and helped me to write function turns each window (a 100×6 block of numbers) into one row of 32 numbers. Each window of 600 raw numbers is too big and too noisy for a classifier (or an ESP32) to use directly, so the function boils it down to 32 summary numbers that capture how strong, how variable, and how oriented the motion is.
- **Training** (`train.py`): Claude suggested and helped to write depth sweep that compares different max depth values to get best possible accuracy.
- **Tree export:** With Claude's guidance I was able to write `export_model.py`.
- **Test cases:** Test cases turned out to be the most diffuclt part in the project. To simulate real example data to test firmware for sitting, walking, joggin, etc. AI was able to help me with writing these scripts.
- **C++ features:** I wrote `extract_features` in C++ with Claude's guidance.
