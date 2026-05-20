import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file produced by the C++ program
data = pd.read_csv("output/final.csv")

# Rebuild the 2D temperature grid from the x, y, temperature columns
grid = data.pivot(index="y", columns="x", values="temperature")

# Plot the heat map
plt.imshow(grid, origin="lower")
plt.colorbar(label="Temperature")
plt.title("2D Heat Equation Final State")
plt.xlabel("x")
plt.ylabel("y")
plt.show()