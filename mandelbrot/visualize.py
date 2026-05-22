import numpy as np
import matplotlib.pyplot as plt
import copy

# Image settings
width = 2048
height = 2048
max_iter = 1000

print("Loading data...")
data = np.fromfile('mandelbrot.bin', dtype=np.int32)
image = data.reshape((height, width))

# Mask the points that reached max_iter (these are inside the Mandelbrot set)
masked_image = np.ma.masked_where(image == max_iter, image)

print("Drawing image...")
plt.figure(figsize=(16, 9))

# Get the 'jet' colormap and tell it to color our masked values (the set) black
cmap = copy.copy(plt.cm.jet)
cmap.set_bad(color='black')

# Plot the masked image using a standard linear scale
plt.imshow(masked_image, cmap=cmap, extent=[-2.0, 2.0, -2.0, 2.0])

plt.title("Mandelbrot Set")
plt.axis('off')

plt.savefig('mandelbrot.png', dpi=300, bbox_inches='tight')
plt.show()