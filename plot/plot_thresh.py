import numpy as np
import matplotlib.pyplot as plt

# Create an array of x values from 1 to 10^7
x_values = np.linspace(1, 10**7, 1000)

# Calculate the y values based on the function
y_values = (12 / 0.5**2) * np.log(8 * x_values / 0.01)

# Create the plot
plt.figure(figsize=(10, 6))

plt.plot(x_values, y_values, label="ε=0.5, δ=0.01}")
plt.title('Number of Elements vs thresh')
plt.xlabel('Number of Elements')
plt.ylabel('thresh')
plt.grid(True)
plt.legend()
plt.xscale('log')

# Show the plot
plt.show()
