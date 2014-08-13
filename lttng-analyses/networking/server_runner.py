#!/usr/bin/env python3
from connection import connection
from queue import Queue
from threading import Thread
import sys
import time

addr = 'localhost'
port = 6666

if len(sys.argv) == 2:
	port = argv[1]
elif len(sys.argv) == 3:
	addr = argv[1]
	port = int(argv[2])
server = connection(addr,port, debug=True)
queue_to_get = Queue(maxsize=0)
print("server listening to %r:%r" %(addr,port))
server.listen()

"""
time.sleep(2)
client = connection('localhost',5555)
client.send('localhost', 5432, "please!")
"""