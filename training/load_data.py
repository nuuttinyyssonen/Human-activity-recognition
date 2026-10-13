import pandas as pd
df = pd.read_csv("../data/sit_5/sub_1.csv", index_col=0)
print(df.shape)
print(df.head())