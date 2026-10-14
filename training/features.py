import numpy as np

def extract_features(X):
    """
    Transforms 3D windows (n_windows, window_size, n_channels) 
    into a 2D feature matrix (n_windows, n_features).
    """
    feature_list = []
    
    for win in X:
        # win shape: (window_size, n_channels), e.g., (100, 6)
        
        # 1. Basic statistical features per channel
        mean = np.mean(win, axis=0)
        std = np.std(win, axis=0)
        min_val = np.min(win, axis=0)
        max_val = np.max(win, axis=0)
        rms = np.sqrt(np.mean(win**2, axis=0))
        
        # 2. Magnitude feature (assuming first 3 channels are accelerometer x, y, z)
        # Calculates the vector magnitude at each timestep, then takes its mean and std
        accel_mag = np.sqrt(np.sum(win[:, :3]**2, axis=1))
        mag_mean = np.array([np.mean(accel_mag)])
        mag_std = np.array([np.std(accel_mag)])
        
        # Concatenate all features into one flat 1D vector for this window
        feature_vector = np.concatenate([
            mean, std, min_val, max_val, rms, mag_mean, mag_std
        ])
        
        feature_list.append(feature_vector)
        
    return np.array(feature_list)