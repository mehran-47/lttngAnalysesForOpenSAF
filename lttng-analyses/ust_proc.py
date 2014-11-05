#!/usr/bin/env python3
import sys, os, time, re, json
from babeltrace import *
from LTTngAnalyzes.common import *

class ust_trace():
	def __init__(self, path, **kwargs):
		self.path = path
		self.traces = TraceCollection()
		self.trace_handle = self.traces.add_trace(self.path, "ctf")
		self.latest_timestamp = -1
		self.allcomps = {}

	def show_event_types(self):
		print("--- Event list ---")
		for event_declaration in self.trace_handle.events:
			print("event : {}".format(event_declaration.name))
			for field_declaration in event_declaration.fields:
				print(field_declaration)
		print("--- Done ---")

	def show_events(self, *types):
		for event in self.traces.events:
			if len(types) == 0:
				for event_type in event:
					print("event timestamp: "+ ns_to_asctime(event.timestamp) + " : " + str(event.get(event_type)))
			else:
				for event_type in types:
					if event_type in event:
						print("event timestamp: "+ ns_to_asctime(event.timestamp)  + " : " + str(event.get(event_type)))

	def events_as_dict(self):
		dict_to_ret = {}
		for event in self.traces.events:
			dict_to_ret[event.timestamp] = event.get("msg")
		return dict_to_ret

	def get_comp_csi(self, newEvents, allcomps):
		comp_csi_dict = {}
		final_dict = allcomps
		for timestamp in sorted(newEvents):
			msg = re.search('^{.+}$' ,str(newEvents[timestamp]))
			if msg:
				string_dict = re.sub("'",'"', msg.group(0))
				comp_csi_dict = json.loads(string_dict)
				if comp_csi_dict['type']=='dispatch_remove' or comp_csi_dict['type']=='dispatch_terminate':
					if comp_csi_dict['component'] in final_dict:
						del final_dict[comp_csi_dict['component']]
				elif comp_csi_dict['type']=='dispatch_set' or comp_csi_dict['type']=='csi_assignment':
					if comp_csi_dict['component'] in final_dict:
						for key in comp_csi_dict:
							final_dict[comp_csi_dict['component']][key] = comp_csi_dict[key]
					else:
						final_dict[comp_csi_dict['component']] = comp_csi_dict
		return final_dict

	def check_new_events(self, oldEventsDict):
		newEventsDict = {}
		self.traces = TraceCollection()
		self.trace_handle = self.traces.add_trace(self.path, "ctf")
		for event in self.traces.events:
			if event.timestamp not in oldEventsDict:
				newEventsDict[event.timestamp] = event.get("msg")
		return newEventsDict


if __name__ == "__main__":
	if len(sys.argv) == 2 and sys.argv[1]!='--p':
		path = str(sys.argv[1])
		ut = ust_trace(path)
		print('[')
		for event in sorted(ut.events_as_dict()):
			print('{\''+str(ns_to_asctime(event))+'\':'+str(ut.events_as_dict()[event])+'},')
		print(']')
	elif len(sys.argv) == 2 and sys.argv[1]=='--p':
		#Persistent UST tracing
		from utilities.bash import bash
		from daemon_controller_client import *
		bashc_v = bash()
		CREATE_UST = "lttng create --live 2000000 -U net://localhost\nlttng enable-event -u -a --loglevel=TRACE_DEBUG\nlttng start"
		#Starting UST session
		try: 
			(ust_name, ust_path) = start_tracing_with(CREATE_UST, 'ust')
			while not os.path.exists(ust_path):
				try:
					print('UST path does not exist yet')
					time.sleep(5)
				except KeyboardInterrupt:
					destroy_all_sessions()
					print('Trace-debugging Manually interrupted, quitting.')
					sys.exit()
			#UST Session created successfully
			time.sleep(2)
			ut = ust_trace(ust_path)
			newEventsDict = ut.events_as_dict()
			allcomps = ut.get_comp_csi(newEventsDict,{})
			oldEventsDict = {}
			while True:
				newEventsDict = ut.check_new_events(oldEventsDict)
				time.sleep(5)
				if len(newEventsDict.keys())>0:
					allcomps = ut.get_comp_csi(newEventsDict,allcomps)
					oldEventsDict.update(newEventsDict)
				print(allcomps)
				print('\n\n\n')
		except TypeError:
			print('Failed to create UST session. Quitting.')
			destroy_all_sessions()
			raise
		except:
			raise
		'''
		while True:
			newstr = re.sub("'",'"', str(input("\n\npaste new event string\n>"))) 
			newEvents = json.loads(newstr)
			if len(newEvents)!=0:
				ut.get_comp_csi(newEvents, allcomps)
				print(allcomps)
				for event in newEvents:
					oldEventsDict[event]=newEvents[event]
		'''
	else:
		raise Exception('usage: "./ust_proc.py --p" for persistent components checking\n\
			"./ust_proc.py </path/to/trace>"" for checking/viewing all events"')