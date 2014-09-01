#!/usr/bin/env python3
import sys
import os
import time
import re
import json
from babeltrace import *
from LTTngAnalyzes.common import *

class ust_trace():
	def __init__(self, path, **kwargs):
		self.path = path
		self.traces = TraceCollection()
		self.trace_handle = self.traces.add_trace(self.path, "ctf")
		self.latest_timestamp = -1
		self.check_break = False
		if self.trace_handle is None:
			raise IOError("Error adding trace")

	def show_event_types(self):
		print("--- Event list ---")
		for event_declaration in self.trace_handle.events:
			print("event : {}".format(event_declaration.name))
			#if event_declaration.name == "lttng_ust_tracef":
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

	def __events_as_dict(self):
		dict_to_ret = {}
		for event in self.traces.events:
			dict_to_ret[event.timestamp] = event.get("msg")
		return dict_to_ret

	def get_comp_csi(self, newEvents):
	comp_csi_dict = {}
	final_dict = {}
	for timestamp in sorted(newEvents):
		msg = re.search('^{.+}$' ,newEvents[timestamp])
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
			#print("\n\n in comparison \n\n")
			if event.timestamp > self.latest_timestamp:
				self.latest_timestamp = event.timestamp
			if event.timestamp not in oldEventsDict:
				newEventsDict[event.timestamp] = event.get("msg")
		return newEventsDict

	def start_daemon(self):
		oldEventsDict = {}
		while not self.check_break:
			newEvents = self.check_new_events(oldEventsDict)
			if len(newEvents) == 0:
				print("nothing new; waiting...")
				#oldEventsDict = self.__events_as_dict()
				time.sleep(5)
			else:
				'''
				print('saving new events')
				with open('/home/node2/Documents/UST_events.txt','w') as ustf:
					for timestamp in sorted(newEvents):
						ustf.write(str(timestamp) + ":" + newEvents[timestamp]+"\n")
				'''
				print('\nCurrently active components:' + self.get_comp_csi(newEvents))
			oldEventsDict = self.__events_as_dict()


if __name__ == "__main__":
	# Check for path arg:
	if len(sys.argv) < 2:
		raise TypeError("Usage: python ust-proc.py path/to/file")

	# Create TraceCollection and add trace:
	path = sys.argv[1] + "/ust/uid/0/64-bit"
	while not os.path.isdir(path):
		print("No UST event yet, waiting")
		time.sleep(10)
	
	ustTrace = ust_trace(path)
	#ustTrace.show_events("msg")
	try:
		ustTrace.start_daemon()
	except KeyboardInterrupt:
		print("\nDaemon stopped by 'KeyboardInterrupt'")
	except:
		print("UST Processing Daemon interrupted")
		raise
	#print(ustTrace.check_new_events())
