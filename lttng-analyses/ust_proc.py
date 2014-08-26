#!/usr/bin/env python3
import sys
from babeltrace import *

# Check for path arg:
if len(sys.argv) < 2:
	raise TypeError("Usage: python example-api-test.py path/to/file")

# Create TraceCollection and add trace:
traces = TraceCollection()
trace_handle = traces.add_trace(sys.argv[1], "ctf")
if trace_handle is None:
	raise IOError("Error adding trace")

# Listing events
print("--- Event list ---")
for event_declaration in trace_handle.events:
	print("event : {}".format(event_declaration.name))
	#if event_declaration.name == "lttng_ust_tracef":
	for field_declaration in event_declaration.fields:
		print(field_declaration)
print("--- Done ---")


for event in traces.events:
	#print("TS: {}, {} : {}".format(event.timestamp,	event.cycles, event.name))
	print("msg:" + event["msg"])
	print("timestamp begin: " + str(event.timestamp))
	#print("timestamp begin: " + str(event.host))
	#print("timestamp end: " + str(event["timestamp_end"]))

'''
	if event.name == "sched_switch":
		prev_comm = event["prev_comm"]
		if prev_comm is None:
			print("ERROR: Missing prev_comm context info")
		else:
			print("sched_switch prev_comm: {}".format(prev_comm))

	if event.name == "exit_syscall":
		ret_code = event["ret"]
		if ret_code is None:
			print("ERROR: Unable to extract ret")
		else:
			print("exit_syscall ret: {}".format(ret_code))