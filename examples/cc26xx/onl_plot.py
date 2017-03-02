import numpy as np
from collections import deque
import matplotlib.pyplot as plt
import csv

fname = "test_onl2.txt"

max_length = 20  # plot only 20 latest data points

# kalman params
sigma = 0.8
gamma = 0.000001


q1 = deque(maxlen = max_length)
filtered1 = deque(maxlen = max_length)

def kalman_online(fn, x_1, P_1, time_stamp):
	# search in filename the line having current time_stamp
	cur_time = str(time_stamp)
	with open(fn, 'rb') as file:
		reader = csv.reader(file)
		for row in reader:
			if len(row) == 5:  # contain a marker # at the end meaning the line is complete
				if row[0] == cur_time:
					print(row)
					rssi_1 = float(row[1])
					K = P_1 / (P_1 + sigma)
					x_1 = x_1 + K * (rssi_1 - x_1)
					P_1 = (1 - K) * P_1
					q1.append(rssi_1)
					filtered1.append(x_1)
					time_stamp += 1
	return x_1, P_1, time_stamp



if __name__ == "__main__":
	x_1 = 0.0  # init for state
	P_1 = 1000.0  # init for variance 
	time_stamp = 1
	
	plt.axis([0, 10, -100, 10])
	plt.ion()
	while True:
		x_1, P_1, new_time_stamp = kalman_online(fname, x_1, P_1, time_stamp)
		if new_time_stamp > time_stamp:
			plt.clf()
			plt.axis([0, 20, -100, 0])
			plt.plot(filtered1, 'black')	
			plt.plot(q1)
			time_stamp = new_time_stamp	
		plt.pause(1)