#!/usr/bin/env python3
from connection import connection
import pickle
import os

dp = {
	'from':os.uname()[1],
	'author':'mk',
	'dict_ception' : {'fall':2, 'is_true': True, 'season': 'fall'},
	}
#server = connection('localhost',6666)
#server.listen()
client = connection('172.16.159.1',5555)
client.connect('172.16.159.1', 6666)
client.send(dp)