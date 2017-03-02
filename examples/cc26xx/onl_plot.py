import numpy as np
from collections import deque
import matplotlib.pyplot as plt
import csv

fname = "test_onl3.txt"

max_length = 20  # plot only 20 latest data points

# kalman params
sigma = 0.8
gamma = 0.000001


# q1 = deque(maxlen = max_length)
# filtered1 = deque(maxlen = max_length)

qs = [deque(maxlen=max_length), deque(maxlen=max_length), deque(maxlen=max_length)]
filtereds = [deque(maxlen=max_length), deque(maxlen=max_length), deque(maxlen=max_length)]

def kalman_online(fn, x_1, P_1, time_stamp):
	# search in filename the line having current time_stamp
	cur_time = str(time_stamp)
	with open(fn, 'rb') as file:
		reader = csv.reader(file)
		for row in reader:
			if len(row) == 5:  # contain a marker # at the end meaning the line is complete
				if row[0] == cur_time:
					print(row)
					for i in range(3):
						rssi = float(row[i + 1])
						K = P_1[i] / (P_1[i] + sigma)
						x_1[i] = x_1[i] + K * (rssi - x_1[i])
						P_1[i] = (1 - K) * P_1[i]
						qs[i].append(rssi)
						filtereds[i].append(x_1[i])
					time_stamp += 1
	return x_1, P_1, time_stamp



if __name__ == "__main__":
	x_1 = [0.0, 0.0, 0.0]  # init for state
	P_1 = [1000.0, 1000.0, 1000.0]  # init for variance 
	time_stamp = 1
	colors = ['black', 'red', 'blue']
	plt.axis([0, 10, -100, 10])
	plt.ion()
	while True:
		x_1, P_1, new_time_stamp = kalman_online(fname, x_1, P_1, time_stamp)
		if new_time_stamp > time_stamp:
			plt.clf()
			plt.axis([0, 20, -100, 0])
			for i in range(3):
				plt.plot(filtereds[i], c=colors[i], linestyle='dashed')	
				plt.plot(qs[i], c=colors[i])
			time_stamp = new_time_stamp	
		plt.pause(1)