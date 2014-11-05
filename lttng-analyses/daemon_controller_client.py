#!/usr/bin/env python3
import sys, time, os, re, argparse
from utilities.bash import bash
from ust_proc import ust_trace
from multiprocessing import Queue, Process as pythonProcess
from networking.connection import connection
from cputop_i import cputop_init
from systemUsage.usage_setter import *
from serverAnalyses.listedDict import listedDict

RESET_RELAYD = "sudo lttng-sessiond -d\n\
sudo lttng-relayd -d\n"
CREATE_UST = "lttng create --live 2000000 -U net://localhost\n\
lttng enable-event -u -a --loglevel=TRACE_DEBUG\n\
lttng start"
CREATE_KERNEL = "lttng create --live\n\
lttng enable-channel -k chann2 -C 20M -W 1\n\
lttng enable-event -k -a -c chann2\n\
lttng add-context -k -t pid -t procname -t tid -t ppid -t perf:major-faults -t perf:branch-load-misses\n\
lttng start"
bashc = bash(show_output=False)
bashc_v = bash()
cpu_usage_q = Queue(maxsize=0)


def start_tracing_with(command, stype, **kwargs):
	sc = bash(show_output=True)
	sc.execute(command)
	for line in sc.outputs:
		if len(line.split('Tracing started for session '))>1:
			session_name = line.split('Tracing started for session ')[1].strip()
			if stype=='ust':
				path = '/root/lttng-traces/'+os.uname()[1]+'/'+session_name+'/ust/uid/0/64-bit'
			elif stype=='kernel':
				path = '/root/lttng-traces/'+os.uname()[1]+'/'+session_name+'/kernel'
			else:
				raise Exception('Wrong trace type in "start_tracing_with"\nProvided trace type:%s\nOnly valid types are "ust" or "kernel"')
			return (session_name, path)


def destroy_session(session_name, **kwargs):
	bashc.execute('lttng stop ' + session_name)
	if not kwargs.get('suppress'):
		print('Tracing stopped for session %s' %(session_name))
	bashc.execute('lttng destroy ' + session_name)
	if not kwargs.get('suppress'):
		print('Session %s destroyed' %(session_name))


def clean_all_trace_history():
	destroy_all_sessions()
	bashc.execute('rm -rf '+'/root/lttng-traces/'+os.uname()[1])
	os.remove('__comp_csi_latest_map.json')

def stop_and_clean_all():
	clean_all_trace_history()
	bashc_v.execute('service opensafd stop')


def destroy_all_sessions():
	comm_handler = bash(show_output=False)
	comm_handler.execute('lttng list')
	list_of_sessions = comm_handler.outputs[0].split('\n')
	any_session_destroyed = False
	for session_name in list_of_sessions:
		found = re.findall(r'(auto-[0-9]{8}-[0-9]{6})', session_name)
		if len(found)>0:
			destroy_session(found[0])
			any_session_destroyed = True
	if not any_session_destroyed:
		print('No tracing session destroyed')

def check_and_send(client, to_send):
	with open('__comp_csi_latest_map.json','r') as historyFile:
		mapHistory = json.loads(historyFile.read())
	if to_send.get('component_info'):
		for component in to_send.get('component_info'):
			if to_send['component_info'][component]['CSI']=='':
				to_send['component_info'][component]['CSI'] = mapHistory[component]['CSI']
	print(to_send)
	print('\n\n\n')
	if client:
		client.send(to_send)

def save_comp_CSI_map(allcomps):
	to_save = listedDict()
	if len(allcomps.keys())>0:
		for component in allcomps:
			if allcomps[component]['CSI'] != '':
				to_save.populateNestedDict([component,'CSI'], allcomps[component]['CSI'])
	if len(to_save.keys())>0:
		with open('__comp_csi_latest_map.json','w') as historyFile:
			historyFile.write(json.dumps(to_save))
	

def get_latest_comp_CSI_map():
	pass

def start_daemon(client):
	newEventsDict = ustTrace.events_as_dict()
	allcomps = ustTrace.get_comp_csi(newEventsDict,{})
	save_comp_CSI_map(allcomps)
	oldEventsDict = {}
	to_send = {}
	try:
		while True:
			newEventsDict = ustTrace.check_new_events(oldEventsDict)
			time.sleep(2)
			if len(newEventsDict.keys())>0:
				allcomps = ustTrace.get_comp_csi(newEventsDict,allcomps)
				save_comp_CSI_map(allcomps)
				oldEventsDict.update(newEventsDict)
			to_send=fetch_and_set_func(allcomps,2)
			check_and_send(client, to_send)
			print(to_send)
			print('\n\n\n')
	except KeyboardInterrupt:
		print('\nDaemon stopped manually. Tracing stopped')


if __name__=="__main__":	
	args = argparse.Namespace()
	kt_session = argparse.Namespace()
	ust_session = argparse.Namespace()
	ipv4_pattern = re.compile('((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\.|$)){4}')
	debugging = False
	if len(sys.argv) < 2:
		print("Usage: './daemon_controller.py server.local.ip'\nOr\n'./daemon_controller.py --ld'")
		raise TypeError("Wrong usage")
		sys.exit()
	else:
		if sys.argv[1].strip()=="--ld":
			debugging = True
		elif ipv4_pattern.match(sys.argv[1].strip()):
			if ipv4_pattern.match(sys.argv[1].strip()):
				print('matched')
			args.to = sys.argv[1]
		else:
			print("Usage: './daemon_controller.py server.local.ip'\nOr\n'./daemon_controller.py --ld'")
			raise TypeError("Wrong usage")
			sys.exit()
	#Starting UST session
	try: 
		(ust_session.name, ust_session.path) = start_tracing_with(CREATE_UST, 'ust')
		time.sleep(1)
	except TypeError:
		print('Failed to create UST session. Quitting.')
		destroy_all_sessions()
		sys.exit()
	except:
		raise
	#Starting Kernel tracing session/ kt_tuple
	(kt_session.name, kt_session.path) = start_tracing_with(CREATE_KERNEL, 'kernel')
	bashc_v.execute("service opensafd start")
	while not os.path.isdir(ust_session.path):
		print("No UST event yet, waiting")
		time.sleep(10)
	# The argument 'to' below should not be necessary. To-be-fixed.
	ustTrace = ust_trace(ust_session.path)
	if not debugging:
		client = connection('172.16.159.128',5555)
		try:
			client.connect(args.to, 6666)
		except ConnectionRefusedError:
			print("No server found running at "+ args.to + ":6666'")
			sys.exit()
		except:
			print("Failed to connect to server")
			raise
		start_daemon(client)
	else:
		start_daemon(None)