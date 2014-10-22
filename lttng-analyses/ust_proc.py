#!/usr/bin/env python3
import sys
import os
import time
import re
import json
from babeltrace import *
from LTTngAnalyzes.common import *
from cputop_i import cputop_init

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
			if event.timestamp > self.latest_timestamp:
				self.latest_timestamp = event.timestamp
			if event.timestamp not in oldEventsDict:
				newEventsDict[event.timestamp] = event.get("msg")
		return newEventsDict

if __name__ == "__main__":
	if len(sys.argv) == 2:
		path = str(sys.argv[1])
		ut = ust_trace(path)
		print('{')
		for event in sorted(ut.events_as_dict()):
			print('{\''+str(event)+'\':'+str(ut.events_as_dict()[event])+'},')
		print('}')
	elif len(sys.argv) == 3 and sys.argv[2]=='--p':
		path = str(sys.argv[1])
		ut = ust_trace(path)
		allcomps = {}
		newEvents = {}
		oldEventsDict = {}
		while True:
			newstr = re.sub("'",'"', str(input("\n\npaste new event string\n>"))) 
			newEvents = json.loads(newstr)
			if len(newEvents)!=0:
				ut.get_comp_csi(newEvents, allcomps)
				print(allcomps)
				for event in newEvents:
					oldEventsDict[event]=newEvents[event]