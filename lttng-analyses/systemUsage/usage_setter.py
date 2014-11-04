#!/usr/bin/env python3
import sys
import time
import json
import os.path
from multiprocessing import Queue, Process as proc
import psutil as ps

def fetch_and_set(activeComps, usage_q):
	to_send = {
        'msg' : '', 
        'from' : os.uname()[1],
        #'time' : str(ns_to_asctime(begin_ns)) + " to " + str(ns_to_asctime(end_ns)),
        #'nstime' : end_ns,
        'component_info' : self.activeComps,
        'cpu_core_usages' : []
        }
    if len(self.activeComps) != 0:
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

def dummy(arg):
	print('dummy function called')

if __name__ == '__main__':
	dummy(7)