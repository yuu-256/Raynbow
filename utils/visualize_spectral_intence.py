import sys
import re
import numpy as np
import matplotlib.pyplot as plt

def openfile(filename):
    try:
        data = np.loadtxt(filename, delimiter=',')
        x_data = data[:, 0]
        y_data = data[:, 1]
        return x_data, y_data
    except Exception as e:
        print(f"Error reading file {filename}: {e}")
        return None

if __name__ == "__main__":
    filename = sys.argv[1] if len(sys.argv) > 1 else 'data.txt'
    data = openfile(filename)
    match = re.search(r'output_lambda_([0-9]+\.[0-9]+)\.dat', filename)
    wl = float(match.group(1)) if match else None

    if data is not None:
        x_data, y_data = data

        bins = [1000, 1000]
        heatmap, xedges, yedges = np.histogram2d(x_data, y_data, bins=bins, range=[[-2.5, 2.5], [-2.5, 2.5]])

        plt.figure(figsize=(12, 10), facecolor='black')
        extent = [xedges[0], xedges[-1], yedges[0], yedges[-1]]
        plt.imshow(heatmap, extent=extent, origin='lower', cmap='viridis', vmin=0, vmax=100, aspect='equal')

        plt.xlabel('X', color='white')
        plt.ylabel('Y', color='white')
        plt.title(f'Photon Hit Density, {wl} um, n = {len(x_data)}', color='white')
        plt.xlim(-2.5, 2.5)
        plt.ylim(-2.5, 2.5)

        ax = plt.gca()
        ax.tick_params(colors='white')
        ax.set_facecolor('black')

        cbar = plt.colorbar()
        cbar.set_label('Density', color='white')
        cbar.ax.yaxis.set_tick_params(color='white')
        plt.setp(plt.getp(cbar.ax.axes, 'yticklabels'), color='white')

        plt.tight_layout()
        plt.show()
    else:
        print("No data to plot.")

