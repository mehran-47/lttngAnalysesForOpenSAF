#!/usr/bin/env python3
import matplotlib.pyplot as plt
import time
import threading
import random
import sys

data = []
def data_listener():
    while True:
        time.sleep(1)
        data.append(random.random())

if __name__ == '__main__':
	thread = threading.Thread(target=data_listener)
	thread.daemon = True
	thread.start()

	plt.axis([0, 20, 0, 50])
	plt.ion()
	plt.show()
	while True:
		try:
			plt.scatter(range(0, len(data[-20:])), data[-20:])
			plt.draw()
			time.sleep(0.05)
			plt.clf()
			plt.axis([0, 20, 0, 50])
		except KeyboardInterrupt:
			print('\nplotting stopped\n')
			sys.exit()