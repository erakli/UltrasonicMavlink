"""
=========================
Simple animation examples
=========================

Two animations where the first is a random walk plot and
the second is an image animation.
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation


def update_line(frame_num, all_data, lines):
    for data, line in zip(all_data, lines):  # zip() for correct iteration
        line.set_data(data[..., :frame_num])
    return tuple(lines)  # return an iterable of artists from animate

###############################################################################


fig1 = plt.figure()

# Fixing random state for reproducibility
np.random.seed(19680801)

all_data = [np.random.rand(2, 25), np.random.rand(2, 25) * 0.5]

styles = ['r-', 'b-']
lines = []
for style in styles:
    line, = plt.plot([], [], style)
    lines.append(line)

plt.xlim(0, 1)
plt.ylim(0, 1)
plt.xlabel('x')
plt.title('test')

# fargs - Additional arguments to pass to each call to func
line_ani = animation.FuncAnimation(fig1, update_line,
                                   frames=25, fargs=(all_data, lines),
                                   interval=50, blit=True)

# To save the animation, use the command: line_ani.save('lines.mp4')

plt.show()
