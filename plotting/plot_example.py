import matplotlib.pyplot as plt
import numpy as np

# Data for plotting
x = np.arange(0.0, 2.0, 0.01)
y = [np.sin(2 * np.pi * x), np.sin(1 * np.pi * x)]

# Note that using plt.subplots below is equivalent to using
# fig = plt.figure and then ax = fig.add_subplot(111)
_, ax = plt.subplots()
ax.plot(x, y[0])
ax.plot(x, y[1])

ax.set(xlabel='time (s)', ylabel='voltage (mV)',
       title='About as simple as it gets, folks')
ax.grid()

plt.show()
