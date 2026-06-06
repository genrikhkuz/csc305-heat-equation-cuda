import pandas as pd
import matplotlib.pyplot as plt

filename = "output/final_cuda.csv" # CHOOSE FILE

data = pd.read_csv(filename)

grid = data.pivot(index="y", columns="x", values="temperature")

plt.imshow(grid, origin="lower")
plt.colorbar(label="Temperature")
plt.title(filename)
plt.xlabel("x")
plt.ylabel("y")
plt.show()