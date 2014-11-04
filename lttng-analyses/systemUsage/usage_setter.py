#!/usr/bin/env python3
import sys
import time
import json
import os
from multiprocessing import Queue as mQueue, Process as proc
import psutil as ps
import re

def fetch_and_set(activeComps, usage_q):
	to_send = {
        'msg' : '', 
        'from' : os.uname()[1],
        #'time' : str(ns_to_asctime(begin_ns)) + " to " + str(ns_to_asctime(end_ns)),
        #'nstime' : end_ns,
        'component_info' : activeComps,
        'cpu_core_usages' : []
    }
	if len(activeComps)!=0:
		for component in activeComps:
			pid = int(activeComps[component]['PID'])
			if os.path.exists("/proc/"+str(pid)):
				activeComps[component]['cpu_usage'] = ps.Process(pid).cpu_percent(interval=0.5)
			else:
				activeComps[component]['cpu_usage'] = None
	usage_q.put(to_send)
""" 
if len(self.activeComps) != 0:
    for component in self.activeComps:
        pid = int(self.activeComps[component]['PID'])
        if os.path.exists("/proc/"+str(pid)):
            if pid in usage_dict:
                pc = float("%0.02f" % ((usage_dict[pid].cpu_ns * 100) / total_ns))
                self.activeComps[component]['cpu_usage'] = pc
            else:
                self.activeComps[component]['cpu_usage'] = ps.Process(pid).cpu_percent(interval=0.5)
        else:
            self.activeComps[component]['cpu_usage'] = None

to_send['component_info'] = self.activeComps

nb_cpu = len(self.cpus.keys())
for cpu in sorted(self.cpus.values(),
        key=operator.attrgetter('cpu_ns'), reverse=True):
    cpu_total_ns = cpu.cpu_ns
    cpu_pc = float("%0.02f" % cpu.cpu_pc)
    to_send["cpu_core_usages"].append(cpu_pc)
self.usage_q.put(to_send)
"""

def dummy(arg1, arg2):
	while True:
		print('dummy function called '+arg1+' '+arg2)
		time.sleep(1)

if __name__ == '__main__':
	Q = mQueue(maxsize=0)
	allcomps = "{'safComp=AmfDemo_44,safSu=SC-1,safSg=AmfDemo,safApp=AmfDemo1': \
	{'PID': 26750, 'cpu_usage': 2.0, 'component': 'safComp=AmfDemo_44,safSu=SC-1,safSg=AmfDemo,safApp=AmfDemo1', 'CSI': 'safCsi=AmfDemo_44,safSi=AmfDemo,safApp=AmfDemo1', 'HAState': 'Active', 'CSIFlags': 'Add One', 'type': 'csi_assignment'}, \
	'safComp=AmfDemo,safSu=SC-1,safSg=AmfDemo,safApp=AmfDemo1': \
	{'PID': 26770, 'cpu_usage': 4.0, 'component': 'safComp=AmfDemo,safSu=SC-1,safSg=AmfDemo,safApp=AmfDemo1', 'CSI': 'safCsi=AmfDemo,safSi=AmfDemo,safApp=AmfDemo1', 'HAState': 'Active', 'CSIFlags': 'Add One', 'type': 'csi_assignment'}}"
	#multiprocess experiment
	"""
	pr = proc(target=fetch_and_set, args=(json.loads(re.sub("'",'"',allcomps)),Q))
	pr.start()
	while True:
		if not Q.empty():
			print(Q.get())
		else:
			if pr.is_alive():
				print('was alive')
				time.sleep(0.5)
			else:
				pr.join()
				print('wating...')
				time.sleep(0.5)
				pr.start()
	"""

