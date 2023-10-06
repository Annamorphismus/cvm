import numpy as np
import matplotlib.pyplot as plt

# Erstelle ein Array von x-Werten von 1 bis 10^7
x_values = np.linspace(1, 10**7, 1000)

# Berechne die y-Werte basierend auf der Funktion
y_values = (12 / 0.5**2) * np.log(8 * x_values / 0.01)

# Erstelle das Diagramm
plt.figure(figsize=(10, 6))

plt.plot(x_values, y_values, label="ε=0.5, δ=0.01}")
plt.title('Number of Elements vs thresh')
plt.xlabel('Number of Elements')
plt.ylabel('thresh')
plt.grid(True)
plt.legend()
plt.xscale('log')

# Zeige das Diagramm
plt.show()
