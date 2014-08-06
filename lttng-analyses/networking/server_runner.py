#!/usr/bin/env python3
from connection import connection
import time

server = connection('localhost',6666)
server.listen()
"""
time.sleep(2)
client = connection('localhost',5555)
client.send('localhost', 5432, "please!")
"""