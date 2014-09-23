#!/usr/bin/env python3
from utilities.bash import bash
import sys
import time
import os
import re

CREATE_UST = "lttng create --live 2000000 -U net://localhost\n\
lttng enable-event -u -a --loglevel=TRACE_DEBUG\n\
lttng start"
CREATE_KERNEL = "lttng create --live\n\
lttng enable-channel -k chann2 -C 10M -W 1\n\
lttng enable-event -k -a -c chann2\n\
lttng add-context -k -t pid -t procname -t tid -t ppid -t perf:major-faults -t perf:branch-load-misses\n\
lttng start"
bashc = bash(show_output=False)


def start_tracing_with(command, stype, **kwargs):
	sc = bash(show_output=False)
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


def destroy_session(session_name):
	bashc.execute('lttng stop ' + session_name)
	bashc.execute('lttng destroy '+session_name)


def clean_all_trace_history():
	destroy_all_sessions()
	bashc.execute('rm -rf '+'/lttng-traces/'+os.uname()[1]+'/*')


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


if __name__=="__main__":
	sname = start_tracing_with(CREATE_UST, 'ust')
	print('UST session name: '+sname[0])
	print('UST session path: '+sname[1])
	time.sleep(1)
	sname = start_tracing_with(CREATE_KERNEL, 'kernel')
	print('kernel session name: '+ sname[0])
	print('kernel session path: '+ sname[1])
	time.sleep(10)
	destroy_all_sessions()