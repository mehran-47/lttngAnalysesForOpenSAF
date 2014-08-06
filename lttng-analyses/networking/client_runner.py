#!/usr/bin/env python3
from connection import connection

#server = connection('localhost',6666)
#server.listen()
client = connection('localhost',5555)
client.send('localhost', 6666, "please!")