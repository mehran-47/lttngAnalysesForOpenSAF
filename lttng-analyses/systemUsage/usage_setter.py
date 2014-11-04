#!/usr/bin/env python3
import sys, time, json, os, re, psutil as ps
from multiprocessing import Queue as mQueue, Process as proc

def fetch_and_set(activeComps, usage_q, interval):
	to_send = {
		'msg' : '', 
		'from' : os.uname()[1],
		#'time' : str(ns_to_asctime(begin_ns)) + " to " + str(ns_to_asctime(end_ns)),
		'time' : '',
		#'nstime' : end_ns,
		'component_info' : activeComps,
		'cpu_core_usages' : ps.cpu_percent(interval=interval, percpu=True)
	}
	while True:
		if len(activeComps)!=0:
			for component in activeComps:
				pid = int(activeComps[component]['PID'])
				if os.path.exists("/proc/"+str(pid)):
					activeComps[component]['cpu_usage'] = ps.Process(pid).cpu_percent(interval=interval)
				else:
					activeComps[component]['cpu_usage'] = None
					time.sleep(1)
		to_send['time'] = time.strftime("%d-%m-%Y") + ' at ' + time.strftime("%H:%M:%S")
		usage_q.put(to_send)

def dummy(arg1, arg2):
	while True:
		print('dummy function called '+arg1+' '+arg2)
		time.sleep(1)

if __name__ == '__main__':
	Q = mQueue(maxsize=0)
	allcomps = "{'safComp=AmfDemo_44,safSu=SC-1,safSg=AmfDemo,safApp=AmfDemo1': \
	{'PID': 1288, 'cpu_usage': 2.0, 'component': 'safComp=AmfDemo_44,safSu=SC-1,safSg=AmfDemo,safApp=AmfDemo1', 'CSI': 'safCsi=AmfDemo_44,safSi=AmfDemo,safApp=AmfDemo1', 'HAState': 'Active', 'CSIFlags': 'Add One', 'type': 'csi_assignment'}, \
	'safComp=AmfDemo,safSu=SC-1,safSg=AmfDemo,safApp=AmfDemo1': \
	{'PID': 1288, 'cpu_usage': 4.0, 'component': 'safComp=AmfDemo,safSu=SC-1,safSg=AmfDemo,safApp=AmfDemo1', 'CSI': 'safCsi=AmfDemo,safSi=AmfDemo,safApp=AmfDemo1', 'HAState': 'Active', 'CSIFlags': 'Add One', 'type': 'csi_assignment'}}"
	#multiprocess experiment
	pr = proc(target=fetch_and_set, args=(json.loads(re.sub("'",'"',allcomps)),Q,1))
	pr.start()
	while True:
		try:
			if not Q.empty():
				print(Q.get())
			else:
				if pr.is_alive():
					print('was alive')
					time.sleep(0.5)
				else:
					pr.join()
					print('Process died')
					time.sleep(0.5)
					#pr.start()
		except KeyboardInterrupt:
			pr.join()
			break
			raise

