# Human Activity Recognition

## Firmware
Live version for ESP 32 and MPU 6050 is currently in wokwi

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

## AI use log:

- **Firmware:** Claude suggested reading the MPU6050 registers directly instead of using a library, and pointed me to the register map document, with a link to a copy of it. I used it to look up the addresses: 0x68, 0x75, and 0x6B.
- **data formatting** (`features.py`): Claude suggested and helped me to write function turns each window (a 100×6 block of numbers) into one row of 32 numbers. Each window of 600 raw numbers is too big and too noisy for a classifier (or an ESP32) to use directly, so the function boils it down to 32 summary numbers that capture how strong, how variable, and how oriented the motion is.
- **Training** (`train.py`): Claude suggested and helped to write depth sweep that compares different max depth values to get best possible accuracy.
**Tree export and test cases:** With Claude's guidance I was able to write `export_model.py`. Claude wrote me `test_cases.py`, which generates the `test_cases.h` header, and the C++ `run_tests()` function that compares the firmware's predictions with Python's on 24 windows. The firmware matched Python on all 24.
- **C++ features:** I wrote `extract_features` in C++ with Claude's guidance.
- Other use cases for AI: some debugging and validation for correctness was also made with AI.