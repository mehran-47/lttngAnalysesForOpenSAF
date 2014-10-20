#!/usr/bin/env python3
import sys
import time
import json
import re
from connection import connection

if __name__ == "__main__":
	client = connection('172.16.159.130', 5432)
	try:
		client.connect(sys.argv[1], 6666)
	except ConnectionRefusedError:
		print("No server found running at "+ sys.argv[1] + ":6666'")
		sys.exit()
	except:
		print("Failed to connect to server")
		raise
	with open('trace_hist.txt', 'r') as trace_hist:
		for line in trace_hist:
			client.send(json.loads(re.sub("'",'"',line)))
			#print(re.sub("'",'"',line))
			time.sleep(1)