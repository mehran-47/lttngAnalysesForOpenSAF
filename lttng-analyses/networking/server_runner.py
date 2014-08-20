#!/usr/bin/env python3
from connection import connection
from multiprocessing import Queue, Process
import multiprocessing
from threading import Thread
import sys
import time

addr = 'localhost'
port = 6666


def getval(queue):
	try:
		while True:
			toprint = queue.get()
			if toprint==None:
				time.wait(2)
			elif toprint.get('msg')=='END_OF_Q':
				print('Reached end of queue')
				time.sleep(5)
			else:
				print(toprint)
	except KeyboardInterrupt:
		print("\n'KeyboardInterrupt' received. Stopping server-reder:%r" %(multiprocessing.current_process().name))
	except:
		raise


if __name__ == '__main__':
	if len(sys.argv) == 2:
		port = sys.argv[1]
	elif len(sys.argv) == 3:
		addr = sys.argv[1]
		port = int(sys.argv[2])
	server = connection(addr,port, debug=True)
	queue_to_get = Queue()
	print("server listening to %r:%r" %(addr,port))
	getterproc = Process(target=getval, args=((queue_to_get),))
	getterproc.start()
	server.listen(queue_to_get)
	getterproc.join()
	"""
	time.sleep(2)
	client = connection('localhost',5555)
	client.send('localhost', 5432, "please!")
	"""