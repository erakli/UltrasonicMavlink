import matplotlib.pyplot as plt
import matplotlib.animation as animation
import matplotlib as mpl
from collections import deque
import serial

# оптмиизации
mpl.rcParams['path.simplify'] = True
mpl.rcParams['path.simplify_threshold'] = 1.0
mpl.rcParams['agg.path.chunksize'] = 10000

series_num = 5
series_size = 250

fig, ax = plt.subplots()
ax.grid()

styles = ['r-', 'b-', 'g-', 'y-', 'c-']
labels = ['front', 'right', 'back', 'left', 'bottom']
lines = []
for i in range(series_num):
    line_obj, = ax.plot([], [], styles[i], label=labels[i], lw=1)
    lines.append(line_obj)

plt.legend(loc='best')
plt.xlim(0, series_size - 1)
plt.ylim(0, 10)
# plt.xlabel('x')
# plt.title('test')

time_series = range(series_size)
lines_data = []
for i in range(series_num):
    lines_data.append(deque([0] * series_size))

port_name = 'COM16'
port = serial.Serial(port_name, 9600, timeout=1)

print('reading from serial port {0}...'.format(port_name))

###############################################################################


def update_limits(new_x, new_y):
    # plt.xlim([time_series[0], new_x])
    # plt.ylim([0, new_y])

    # _, xmax = ax.get_xlim()
    # if new_x >= xmax:
    #     ax.set_xlim(time_series[0], xmax + series_size)

    ymin, ymax = ax.get_ylim()
    if new_y >= ymax:
        ax.set_ylim(ymin, ymax * 1.5)


def update_func(data):
    t, new_data = data

    # time_series.pop(0)
    # time_series.append(t)

    for line_obj, y_vals, new_val in zip(lines, lines_data, new_data):  # zip() for correct iteration
        y_vals.popleft()
        y_vals.append(new_val)
        line_obj.set_data(time_series, y_vals)

    update_limits(t, max(new_data))
    plt.pause(0.00000001)

    return tuple(lines)  # return an iterable of artists from animate


# функция генерации данных. считывает данные с порта
def data_gen():
    t = 0
    while True:
        t += 1
        try:
            raw_data = port.readline().rstrip()
            val_list = raw_data.replace(b'\t', b'').split(b',')
            new_data = [float(val) for val in val_list if val != b'']
        except:
            print('except')
            new_data = tuple([0] * series_num)
        yield t, tuple(new_data)  # вот эта строчка будет генерировать данные наружу


###############################################################################

print('plotting data...')

ani = animation.FuncAnimation(fig, update_func, frames=data_gen, interval=0, blit=True)
plt.show()

port.flush()
port.close()
