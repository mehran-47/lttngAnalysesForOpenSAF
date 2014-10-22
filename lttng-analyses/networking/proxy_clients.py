#!/usr/bin/env python3
import sys
import time
import json
import re
import random
from connection import connection
from multiprocessing import Process as mp

def send_text_strings(filepath):
	with open(filepath, 'r') as trace_hist:
		try:
			for line in trace_hist:
				client.send(json.loads(re.sub("'",'"',line)))
				print(re.sub("'",'"',line))
				time.sleep(random.uniform(0.5,3))
		except KeyboardInterrupt:
			print("User interrupted")

if __name__ == "__main__":
	if len(sys.argv) < 3:
		raise TypeError("Usage: './proxy_clients.py trace_dump_file_1_path.txt trace_dump_file_2_path.txt'")
	client = connection('localhost', 5432)
	try:
		client.connect('localhost', 6666)
	except ConnectionRefusedError:
		print("No server found running at "+ sys.argv[1] + ":6666'")
		sys.exit()
	except:
		print("Failed to connect to server")
		raise
	path1 = str(sys.argv[1])
	path2 = str(sys.argv[2])
	proc1 = mp(target=send_text_strings, args=(path1,))
	proc2 = mp(target=send_text_strings, args=(path2,))
	proc1.start()
	proc2.start()
		