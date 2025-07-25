import sys
import numpy as np
import matplotlib.pyplot as plt

def load_data(file_path):
    try:
        theta, count = np.loadtxt(file_path, delimiter=",", unpack=True)
        return theta, count
    except Exception as e:
        print(f"Error loading file '{file_path}': {e}")
        sys.exit(1)

def plot_scattering(theta, count, title="Scattering Histogram", label="Sample Count"):
    plt.figure()
    plt.plot(theta, count, label=label)
    plt.xlabel("Scattering Angle (deg)")
    plt.ylabel(label)
    plt.title(title)
    plt.xticks(np.arange(0, 181, 10))
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.show()

def main():
    if len(sys.argv) != 2:
        print("Usage: python visualize_histogram.py <csv_file>")
        sys.exit(1)

    file = sys.argv[1]
    theta, count = load_data(file)

    # Optional: apply transformation
    # cos_theta = np.cos(np.radians(theta))
    # tan_theta = np.tan(np.radians(theta))

    plot_scattering(theta, count, title="Scattering Histogram at 0.38 μm")

if __name__ == "__main__":
    main()

