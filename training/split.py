from windowing import create_windows
from load_data import build_dataframe
from sklearn.model_selection import GroupShuffleSplit
import numpy as np

df = build_dataframe("../data")
X, y, subjects = create_windows(df, 100, 50)

# Initialize the splitter (e.g., 80% train, 20% test)
gss = GroupShuffleSplit(n_splits=1, train_size=0.8, random_state=42)

# Get the train and test indices based on the subjects array
train_idx, test_idx = next(gss.split(X, y, groups=subjects))

# Create your final train and test sets
X_train, y_train = X[train_idx], y[train_idx]
X_test, y_test = X[test_idx], y[test_idx]

print(f"Train set windows: {X_train.shape}, Test set windows: {X_test.shape}")
print(f"Unique train subjects: {np.unique(subjects[train_idx])}")
print(f"Unique test subjects: {np.unique(subjects[test_idx])}")
print(np.unique(y_test, return_counts=True))