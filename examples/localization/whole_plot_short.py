import numpy as np
from collections import deque
import matplotlib.pyplot as plt
import csv
import sys

fname = "diff.txt"
if len(sys.argv) > 1:  # name of the file to read
	fname = sys.argv[1]


# kalman params
K = 0.2

def kalman_whole(fn, x_1, P_1):
	qs = [deque(), deque(), deque()]
	filtereds = [deque(), deque(), deque()]
	
	with open(fn, 'rb') as file:
		reader = csv.reader(file)
		for row in reader:
			if len(row) == 5:  # contain a marker # at the end meaning the line is complete
				for i in range(3):
					rssi = float(row[i + 1])
					K = 0.2
					x_1[i] = x_1[i] + K * (rssi - x_1[i])
					P_1[i] = (1 - K) * P_1[i]
					qs[i].append(rssi)
					filtereds[i].append(x_1[i])
	return qs, filtereds

if __name__ == "__main__":
	x_1 = [-40.0, -40.0, -40.0]  # init for state
	P_1 = [1000.0, 1000.0, 1000.0]  # init for variance
	colors = ['black', 'red', 'blue']
	

	qs, filtereds = kalman_whole(fname, x_1, P_1)

	plt.axis([0, 450, -80, -10])
	for i in range(3):
		plt.plot(filtereds[i], c=colors[i], linestyle='dashed', label='filtered')	
		plt.plot(qs[i], c=colors[i], label='raw')

	plt.annotate('30cm', xy=(50, -40), xytext=(50, -30),arrowprops=dict(facecolor='black', shrink=0.05),)
	plt.annotate('50cm', xy=(90, -40), xytext=(90, -30),arrowprops=dict(facecolor='black', shrink=0.05),)
	plt.annotate('70cm', xy=(140, -40), xytext=(140, -30),arrowprops=dict(facecolor='black', shrink=0.05),)
	plt.annotate('90cm', xy=(200, -40), xytext=(200, -30),arrowprops=dict(facecolor='black', shrink=0.05),)
	plt.annotate('110cm', xy=(265, -40), xytext=(265, -30),arrowprops=dict(facecolor='black', shrink=0.05),)
	plt.annotate('130cm', xy=(345, -40), xytext=(345, -30),arrowprops=dict(facecolor='black', shrink=0.05),)
	plt.annotate('150cm', xy=(415, -40), xytext=(415, -30),arrowprops=dict(facecolor='black', shrink=0.05),)
	plt.legend()
	plt.axes().set_xlabel('time (s)')
	plt.axes().set_ylabel('rssi (dB)')
	plt.axes().set_title('rssi received from node at same distance to 3 anchors')

	plt.show()
