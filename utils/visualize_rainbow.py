import os
import re
import numpy as np
import matplotlib.pyplot as plt
from glob import glob

def wavelength_to_rgb(wavelength, gamma=0.8):
    if wavelength < 380 or wavelength > 780:
        return (0, 0, 0)
    if wavelength < 440:
        R, G, B = -(wavelength - 440) / (440 - 380), 0.0, 1.0
    elif wavelength < 490:
        R, G, B = 0.0, (wavelength - 440) / (490 - 440), 1.0
    elif wavelength < 510:
        R, G, B = 0.0, 1.0, -(wavelength - 510) / (510 - 490)
    elif wavelength < 580:
        R, G, B = (wavelength - 510) / (580 - 510), 1.0, 0.0
    elif wavelength < 645:
        R, G, B = 1.0, -(wavelength - 645) / (645 - 580), 0.0
    else:
        R, G, B = 1.0, 0.0, 0.0
    if wavelength < 420:
        factor = 0.3 + 0.7 * (wavelength - 380) / (420 - 380)
    elif wavelength < 701:
        factor = 1.0
    else:
        factor = 0.3 + 0.7 * (780 - wavelength) / (780 - 700)
    return (R ** gamma * factor, G ** gamma * factor, B ** gamma * factor)

def load_spectral_data(filename):
    data = openfile(filename)
    match = re.search(r'output_lambda_([0-9]+\.[0-9]+)\.dat', filename)
    wl = float(match.group(1)) if match else None
    wl = wl*1000

    if data is not None:
        x_data, y_data = data
        bins = [1000, 1000]
        heatmap, xedges, yedges = np.histogram2d(x_data, y_data, bins=bins, range=[[-2.5, 2.5], [-2.5, 2.5]])
        return heatmap, xedges, yedges, wl
    else:
        print(f"No data to plot for {filename}.")
        return None, None, None, None

def convert_heatmap_to_rgb(heatmap, wl):
    if heatmap is None:
        return None

    rgb_heatmap = np.zeros((*heatmap.shape, 3), dtype=np.float32)
    rgb_color = wavelength_to_rgb(wl)
    
    rgb_color = np.array(wavelength_to_rgb(wl), dtype=np.float32)
    rgb_heatmap = heatmap[:, :, np.newaxis] * rgb_color  # Broadcasting to apply color to each pixel

    return rgb_heatmap

def conv_heatmaps(directory):
    num_files = len(glob(os.path.join(directory, 'output_lambda_*.dat')))
    heatmap_multispectral = np.zeros((1000, 1000, 3, num_files), dtype=np.float32)
    filelist = sorted(glob(os.path.join(directory, 'output_lambda_*.dat')))

    xedges, yedges = None, None

    for idx, filename in enumerate(filelist):
        heatmap, xe, ye, wl = load_spectral_data(filename)
        if heatmap is None or wl is None:
            continue

        rgb_heatmap = convert_heatmap_to_rgb(heatmap, wl)
        # print(f"Processing {filename} with wavelength {wl} nm")
        heatmap_multispectral[:, :, :, idx] = rgb_heatmap

        if xedges is None:
            xedges, yedges = xe, ye

    rgb_image = np.sum(heatmap_multispectral, axis=3)

    rgb_image = np.sqrt(rgb_image)
    rgb_image /= np.max(rgb_image + 1e-10)

    return rgb_image, xedges, yedges

def openfile(filename):
    try:
        data = np.loadtxt(filename, delimiter=',')
        return data[:, 0], data[:, 1]
    except Exception as e:
        print(f"Error reading file {filename}: {e}")
        return None

def plot_rgb_image(rgb_image, xedges, yedges):
    extent = [xedges[0], xedges[-1], yedges[0], yedges[-1]]
    plt.figure(figsize=(10, 10), facecolor='black')
    plt.imshow(rgb_image, extent=extent, origin='lower', aspect='equal')
    plt.title("Rainbow", color='white')
    plt.xlabel('X-axis', color='white')
    plt.ylabel('Y-axis', color='white')
    ax = plt.gca()
    ax.tick_params(colors='white')
    ax.set_facecolor('black')
    plt.tight_layout()
    # plt.show()
    plt.savefig('output/rainbow.png')

if __name__ == "__main__":
    directory = 'output'
    os.makedirs(directory, exist_ok=True)
    rgb_image, xedges, yedges = conv_heatmaps(directory)
    if rgb_image is not None:
        plot_rgb_image(rgb_image, xedges, yedges)
