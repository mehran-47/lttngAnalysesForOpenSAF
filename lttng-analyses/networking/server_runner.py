#!/usr/bin/env python3
from connection import connection
import sys
import time

if len(sys.argv) == 1:
	port = 6666
else:
	port = argv[1]
server = connection('172.16.159.1',6666, debug=True)
print("server listening to 172.16.159.1:%r" %(port))
server.listen()

"""
time.sleep(2)
client = connection('localhost',5555)
client.send('localhost', 5432, "please!")
"""