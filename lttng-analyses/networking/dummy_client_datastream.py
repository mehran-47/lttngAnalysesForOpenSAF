#!/usr/bin/env python3
import sys, time, json, re, random
from connection import connection
from multiprocessing import Process as mp

def send_text_strings(filepath):
	with open(filepath, 'r') as trace_hist:
		pattern = re.compile('^\{(.+)\}')
		try:
			for line in trace_hist:
				line = pattern.match(line)
				#print(line)
				if line!=None:
					client.send(json.loads(re.sub("'",'"',line.group(0))))
					print("%s\n\n" %(re.sub("'",'"',line.group(0))))
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
		