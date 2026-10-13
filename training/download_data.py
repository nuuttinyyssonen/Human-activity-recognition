import kagglehub

# Download latest version
path = kagglehub.dataset_download("malekzadeh/motionsense-dataset")

print("Path to dataset files:", path)