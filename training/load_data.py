import pandas as pd
import os

CHANNELS = [
    "acc.x", "acc.y", "acc.z",
    "rotationRate.x", "rotationRate.y", "rotationRate.z",
]
LABELS = ["activity", "trial", "subject"]

def load_dataframe(directory, folder_name, file_name):
    df = pd.read_csv(f"{directory}/{folder_name}/{file_name}", index_col=0)

    df["acc.x"] = df["gravity.x"] + df["userAcceleration.x"]
    df["acc.y"] = df["gravity.y"] + df["userAcceleration.y"]
    df["acc.z"] = df["gravity.z"] + df["userAcceleration.z"]

    activity, trial = folder_name.split("_")
    subject = file_name.removesuffix(".csv").split("_")[1]

    df["activity"] = activity
    df["trial"] = int(trial)
    df["subject"] = int(subject)

    return df[CHANNELS + LABELS]

def build_dataframe(directory):
    folders = os.listdir(directory)
    dfs = []
    for i in folders:
        folder = os.listdir(f"{directory}/{i}")
        for file in folder:
            dataframe = load_dataframe(directory, i, file)
            dfs.append(dataframe)

    df = pd.concat(dfs, ignore_index=True)
    return df

if __name__ == "__main__":
    df = build_dataframe("../data")
    print(df.shape)   # (N, 9)