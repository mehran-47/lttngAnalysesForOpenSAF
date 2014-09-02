#!/usr/bin/env python3
from connection import connection
from multiprocessing import Process, Pipe
import multiprocessing
import sys
import time

addr = 'localhost'
port = 6666


def getval(parent_conn):
	try:
		while True:
			toprint = parent_conn.recv()
			if toprint==None:
				time.wait(2)
			elif toprint.get('msg')=='END_OF_Q':
				print('Reached end of queue')
				time.sleep(5)
			else:
				'''
				a_dict = {
			        'msg' : '', 
			        'from' : os.uname()[1],
			        'time' : str(ns_to_asctime(begin_ns)) + " to " + str(ns_to_asctime(end_ns)),
			        'pid_usages' : {},
			        'cpu_usages' : [],
			        'component_info' : []
		        }
				'''
				#Get end packets here
				#print(toprint)
				if(toprint.get('from') != None):
					print(toprint['from'])
					#print(toprint['pid_usages'])
					print(toprint['component_info'])
	except KeyboardInterrupt:
		print("\n'KeyboardInterrupt' received. Stopping server-reader:%r" %(multiprocessing.current_process().name))
	except:
		raise


if __name__ == '__main__':
	if len(sys.argv) == 2:
		port = sys.argv[1]
	elif len(sys.argv) == 3:
		addr = sys.argv[1]
		port = int(sys.argv[2])
	server = connection(addr,port, debug=True)
	parent_conn, child_conn = Pipe()
	print("server listening to %r:%r" %(addr,port))
	getterproc = Process(target=getval, args=((child_conn),))
	getterproc.start()
	server.listen(parent_conn)
	getterproc.join()