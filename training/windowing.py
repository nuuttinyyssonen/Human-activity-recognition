from load_data import build_dataframe, CHANNELS
import numpy as np

def create_windows(df, window_size, step):
    X, y, subjects = [], [], []
    for key, group in df.groupby(["subject", "trial", "activity"]):
        subject, trial, activity = key
        data = group[CHANNELS].to_numpy()
        for start in range(0, len(data) - window_size + 1, step):
            window = data[start : start + window_size]
            X.append(window)
            y.append(activity)
            subjects.append(subject)
    return np.array(X), np.array(y), np.array(subjects)


if __name__ == "__main__":      
    df = build_dataframe("../data")
    X, y, subjects = create_windows(df, 100, 50)

    print(X.shape, y.shape, subjects.shape)
    print(np.unique(y, return_counts=True))