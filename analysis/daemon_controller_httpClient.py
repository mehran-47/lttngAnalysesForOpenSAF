#!/usr/bin/env python3
from networking.connection import connection
from systemUsage.usage_setter import *
from copy import deepcopy
import os, sys, time, netifaces as ni, shelve as sh

if __name__ == '__main__':
	client = connection(ni.ifaddresses('eth0')[2][0]['addr'], 5555)
	client.connect(sys.argv[1], 6666)
	allcomps = {}	
	while True:
		if not os.path.isfile('/opt/SA_stats/compDB.db'):
			print('No component found in system, waiting')
			time.sleep(2)
			continue
		try:
			db = sh.open('/opt/SA_stats/compDB.db', writeback=True)
			allcomps = deepcopy(db['components'])			
			db.close()
			to_send = fetch_and_set_func(allcomps, 1)
			time.sleep(0.5)
			print(to_send)
			client.send(to_send)
		except KeyboardInterrupt:
			print('Monitoring stopped manually, quitting')
			break
