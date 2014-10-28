#!/usr/bin/env python3
from networking.connection import connection
from serverAnalyses.analysis import dictParser
from multiprocessing import Process, Pipe
import sys


if __name__ == '__main__':
	if len(sys.argv) == 2:
		port = sys.argv[1]
	elif len(sys.argv) == 3:
		addr = sys.argv[1]
		port = int(sys.argv[2])
	server = connection(addr,port, debug=True)
	parent_conn, child_conn = Pipe()
	dp = dictParser()
	print("server listening to %r:%r" %(addr,port))
	getterproc = Process(target=dp.run , args=((child_conn),))
	getterproc.start()
	server.listen(parent_conn)
	if getterproc.is_alive():
		getterproc.join()