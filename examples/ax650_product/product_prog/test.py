import matplotlib.pyplot as plt
import numpy as np

# 假设 audio 是 [样本数，4] 的 NumPy 数组, sr 是采样率
# 这里示例随机生成
sr = 16000
duration = 2  # 秒
num_channels = 4
num_samples = sr * duration
audio = np.random.randn(num_samples, num_channels)  # 示例音频

time = np.arange(num_samples) / sr  # x轴：时间(s)

fig, axs = plt.subplots(num_channels, 1, figsize=(12, 8), sharex=True)

for i in range(num_channels):
    axs[i].plot(time, audio[:, i])
    axs[i].set_title(f'Channel {i+1}')
    axs[i].set_ylabel('Amplitude')

axs[-1].set_xlabel('Time (s)')
plt.tight_layout()
plt.show()