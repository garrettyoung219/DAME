import sys
import numpy as np
from scipy.io import wavfile
import os

def remove_pops_normalize_and_trim(input_wav, output_wav):
    try:
        if not os.path.exists(input_wav):
            raise FileNotFoundError(f"Input file {input_wav} not found")

        sample_rate, data = wavfile.read(input_wav)

        if len(data.shape) > 1:
            data = np.mean(data, axis=1).astype(np.int16)  # Convert to mono

        if len(data) == 0 or np.all(data == 0):
            raise ValueError("Audio file is empty or silent")

        # Step 1: Pop Removal (Detecting Sudden Spikes)
        pop_threshold = np.max(np.abs(data)) * 0.8  # Use absolute max
        for i in range(1, len(data) - 1):
            if abs(data[i] - data[i - 1]) > pop_threshold:
                data[i] = (data[i - 1] + data[i + 1]) // 2  # Smooth out spike

        # Step 2: Apply a Smooth Fade-Out to Last 50ms
        fade_duration = int(sample_rate * 0.05)  # 50ms fade-out
        fade_start = max(len(data) - fade_duration, 0)
        for i in range(fade_start, len(data)):
            fade_factor = (len(data) - i) / fade_duration  # Linear fade
            data[i] = int(data[i] * fade_factor)

        # Step 3: Trim the Last 10ms (Optional, only if pop persists)
        trim_duration = int(sample_rate * 0.01)  # 10ms
        data = data[:-trim_duration]  # Remove last 10ms of samples

        # Step 4: RMS Normalization
        rms = np.sqrt(np.mean(data.astype(np.float32) ** 2))
        if np.isnan(rms) or rms == 0:
            raise ValueError("Invalid RMS value. Audio may be silent or corrupted.")

        target_rms = 12000  # Target volume level
        gain = target_rms / rms if rms > 0 else 1
        data = np.clip(data * gain, -32768, 32767).astype(np.int16)

        # Save the processed file
        wavfile.write(output_wav, sample_rate, data)
        print("Processing complete!")

    except Exception as e:
        print(f"Error processing audio: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python process_audio.py input.wav output.wav")
        sys.exit(1)

    input_wav = sys.argv[1]
    output_wav = sys.argv[2]
    remove_pops_normalize_and_trim(input_wav, output_wav)
