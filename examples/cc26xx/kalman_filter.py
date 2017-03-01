import matplotlib.pyplot as plt
import numpy as np
import sys
import csv

arg = sys.argv[1]  # name of the file to read

# creates empty lists for Kalman
xpost = []
xprior = []
Pprior = []
Ppost = []
K = []


# reads the file and filters the values
def FilterFile(arg):
	rssi = []
	i = 0

	# reads the file with the rssi values and appends it to the rssi list
	with open(arg, 'rb') as f:
		reader = csv.reader(f)
		for row in reader:
			if len(row)>0:
				rssi.append(int(row[0]))

	# initialization of kalman values
	sigma = 0.8
	gamma = 0.000001
	xprior.append(0)
	Pprior.append(1000)  # high variance at the beginning because we are not sure 
	K.append(Pprior[0]/(Pprior[0] + sigma))
	xpost.append(xprior[0] + K[0]*(float(rssi[0])-xprior[0]))  # contains the filtered values
	Ppost.append((1-K[0])*Pprior[0])

	# Filtering 
	for t in range(len(rssi)-1):
	    xprior.append(xpost[t])
	    Pprior.append(Ppost[t] + gamma)
	    K.append(Pprior[t+1]/(Pprior[t+1] + sigma))
	    xpost.append(xprior[t+1] + K[t+1]*(float(rssi[t+1])-xprior[t+1]))
	    Ppost.append((1-K[t+1])*Pprior[t+1])

	plt.plot(xpost)
	plt.plot(rssi)
	plt.show()

if __name__ == "__main__":
	FilterFile(arg)
