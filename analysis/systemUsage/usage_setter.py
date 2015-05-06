#!/usr/bin/env python3
import sys, time, json, os, re, psutil as ps, shelve as sh
from multiprocessing import Queue as mQueue, Process as proc
from copy import deepcopy

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
					activeComps[component]['memory_usage'] = ps.Process(pid).memory_percent()
				else:
					activeComps[component]['cpu_usage'] = 0
					activeComps[component]['memory_usage'] = 0
					time.sleep(1)
		to_send['time'] = time.strftime("%d-%m-%Y") + ' at ' + time.strftime("%H:%M:%S")
		usage_q.put(to_send)

def activeCompsRefresh(activeComps):
	db = sh.open('/opt/SA_stats/compDB.db', writeback=True)
	correctedActiveComps = deepcopy(db['components'])
	deadPIDs = []
	for component in activeComps:
		if not os.path.exists("/proc/"+str(activeComps[component]['PID'])):
			deadPIDs.append(activeComps[component]['PID'])
			if component in correctedActiveComps: del correctedActiveComps[component]
	db['components'] = correctedActiveComps
	for pid in deadPIDs: del db['PIDs'][db['PIDs'].index(pid)]
	db.close()
	return correctedActiveComps

def fetch_and_set_func(activeComps, interval):
	to_send = {
		'msg' : '', 
		'from' : os.uname()[1],
		#'time' : str(ns_to_asctime(begin_ns)) + " to " + str(ns_to_asctime(end_ns)),
		'time' : '',
		#'nstime' : end_ns,
		'component_info' : activeComps,
		'cpu_core_usages' : ps.cpu_percent(interval=interval, percpu=True)
	}
	activeComps = activeCompsRefresh(activeComps)

	if len(activeComps)>0:
		for component in activeComps:
			pid = int(activeComps[component]['PID'])
			try:
				activeComps[component]['cpu_usage'] = ps.Process(pid).get_cpu_percent(interval=interval)
				activeComps[component]['memory_usage'] = ps.Process(pid).get_memory_percent()
			except ps.NoSuchProcess:
				activeComps[component]['cpu_usage'] = 0.0
				activeComps[component]['memory_usage'] = 0.0
				print('Error: "psutil.NoSuchProcess" process ID %d died while measuring usage.'%(pid))				

	to_send['component_info']=activeComps
	to_send['time'] = time.strftime("%d-%m-%Y") + ' at ' + time.strftime("%H:%M:%S")
	return to_send


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
	if len(sys.argv)==2 and sys.argv[1]=='--p':
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
	#Single function experiment
	elif len(sys.argv)==2 and sys.argv[1]=='--f':
		while True:
			try:
				print(fetch_and_set_func(json.loads(re.sub("'",'"',allcomps)),0.5))
				print('\n\n')				
			except KeyboardInterrupt:
				print("\nStopped debugging; python-KeyboardInterrupt\n")
				break
				sys.exit()
			except:
				raise
	else:
		raise Exception('Usage: ./usage_setter.py --p\nor\n./usage_setter.py --p')
		sys.exit()