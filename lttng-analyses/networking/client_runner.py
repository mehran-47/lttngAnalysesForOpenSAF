#!/usr/bin/env python3
from connection import connection

#server = connection('localhost',6666)
#server.listen()
client = connection('localhost',5555)
client.send('172.16.159.1', 6666, "please!")