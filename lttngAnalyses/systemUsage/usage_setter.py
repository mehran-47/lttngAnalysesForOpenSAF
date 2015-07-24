#!/usr/bin/env python3
import sys, time, json, os, re, psutil as ps
from multiprocessing import Queue as mQueue, Process as proc
from copy import deepcopy
from subprocess import call, Popen, PIPE


def activeCompsRefresh(activeComps):
	correctedActiveComps = deepcopy(activeComps)
	for component in activeComps:
		if not os.path.exists("/proc/"+str(activeComps[component]['PID'])):
			del correctedActiveComps[component]
			#proc(target=delayed_comp_restart, args=(2,)).start()
	return correctedActiveComps

def delayed_comp_restart(delay):
	"""
	call('/opt/httpComponent/end_http'.split(' '))
	time.sleep(delay)
	call('/opt/httpComponent/start_http 8080'.split(' '))
	"""
	devnull = open(os.devnull, 'w')
	call('service opensafd restart'.split(' '), stdout=devnull)

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
				cpu_usage = ps.Process(pid).cpu_percent(interval=interval)
				activeComps[component]['cpu_usage'] = cpu_usage if cpu_usage<100.0 else 100.0
				activeComps[component]['memory_usage'] = ps.Process(pid).get_memory_percent()
			except ps.NoSuchProcess:
				activeComps[component]['cpu_usage'] = 0.0
				activeComps[component]['memory_usage'] = 0.0
				print('Error: "psutil.NoSuchProcess" process ID %d died while measuring usage.'%(pid))
			activeComps[component]['cpu_cycles_abs'] = get_cpu_abs(activeComps[component]['cpu_usage'])
			activeComps[component]['memory_usage_abs'] = activeComps[component]['memory_usage']*ps.virtmem_usage().total/10**8

	to_send['component_info']=activeComps
	to_send['time'] = time.strftime("%d-%m-%Y") + ' at ' + time.strftime("%H:%M:%S")
	return to_send

def get_cpu_abs(percent):
	cpuexec = Popen(['lscpu'], stdout=PIPE)
	cpuTotalPattern = re.compile(r'(?<=CPU MHz)(.*?)(?=\n)', re.DOTALL)
	cpuCyclesTotal = float(cpuTotalPattern.search(cpuexec.communicate()[0].decode('utf-8')).group(0).split(':')[1].strip())*ps.cpu_count()
	#return int(cpuCyclesTotal*percent/100)
	return cpuCyclesTotal*percent/100


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