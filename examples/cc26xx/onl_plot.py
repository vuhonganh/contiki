import numpy as np
from collections import deque
import matplotlib.pyplot as plt
import csv

fname = "test_onl4.txt"

max_length = 20  # plot only 20 latest data points

# kalman params
sigma = 0.8
gamma = 0.000001

# default posi of anchors
edge_length = 150.0
x_default = np.array([0.0, edge_length, edge_length])
y_default = np.array([0.0, 0.0, edge_length])

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
					#print(row)
					for i in range(3):
						rssi = float(row[i + 1])
						K = P_1[i] / (P_1[i] + sigma)
						x_1[i] = x_1[i] + K * (rssi - x_1[i])
						P_1[i] = (1 - K) * P_1[i]
						qs[i].append(rssi)
						filtereds[i].append(x_1[i])
					time_stamp += 1
	return x_1, P_1, time_stamp


def trilateration(dists, x_anchors=x_default, y_anchors=y_default):
	"""
	input:
		x_anchors: np array of x-coordinate of anchors
		y_anchors: np array of y-coordinate of anchors
	   	dists: np array of dist from corresponding anchor to the node
	output:	
		(x, y): the coordinate of the node and the residual
   	"""
   	nb_anchors = len(dists)
   	x_sq_p_y_sq = x_anchors * x_anchors + y_anchors * y_anchors   	
   	d_sq = dists * dists

   	nb_pair = (nb_anchors * (nb_anchors - 1)) / 2
   	A = np.zeros((nb_pair, 2))
   	b = np.zeros(nb_pair)
   	id_row = 0
	for i in range(nb_anchors - 1):
		for j in range(i + 1, nb_anchors):
			A[id_row, 0] = x_anchors[j] - x_anchors[i]
			A[id_row, 1] = y_anchors[j] - y_anchors[i]
			b[id_row] = d_sq[i] - x_sq_p_y_sq[i] + x_sq_p_y_sq[j] - d_sq[j]
			id_row += 1

	point, residual, _, _ = np.linalg.lstsq(2 * A, b)
	print np.linalg.lstsq(2 * A, b)
	if residual:
		return point, residual[0]
	else:
		return point, 0


def test_trilateration():
	x = np.array([0, 1, 1])
	y = np.array([0, 0, 1])
	# d = np.array([1, np.sqrt(2), 1])
	d = np.array([np.sqrt(5)/2.0, np.sqrt(5)/2.0, 1/2.0])

	point, resi = trilateration(d, x, y)	
	print(point)


def rssi_to_dist(rssi_vals, slope=-22.988, intercept=70.762):
	"""
		suppose a relation rssi = slope * log(d) + intercept  (d in cm, rssi in dB)
		given np array rssi_vals, compute the corresponding distances d (in cm)
	"""
	dists = np.exp((rssi_vals - intercept)/slope)
	print('dists = ', dists)
	return dists


if __name__ == "__main__":
	test_trilateration()
	x_1 = [0.0, 0.0, 0.0]  # init for state
	P_1 = [1000.0, 1000.0, 1000.0]  # init for variance
	time_stamp = 1
	colors = ['black', 'red', 'blue']
	
	point_in_time = []  # list to store the coor evolve during time (or noise)

	plt.axis([0, 10, -100, 10])
	plt.ion()


	while True:
		x_1, P_1, new_time_stamp = kalman_online(fname, x_1, P_1, time_stamp)
		if new_time_stamp > time_stamp:
			plt.clf()
			plt.axis([0, 20, -100, 0])
			rssi_vals = []
			for i in range(3):
				plt.plot(filtereds[i], c=colors[i], linestyle='dashed')	
				plt.plot(qs[i], c=colors[i])
				rssi_vals.append(filtereds[i][-1])
			print('rssi vals = ', rssi_vals)
			dists = rssi_to_dist(np.array(rssi_vals))
			point, resi = trilateration(dists)
			print(point)
			point_in_time.append(point)
			time_stamp = new_time_stamp	
		plt.pause(0.5)