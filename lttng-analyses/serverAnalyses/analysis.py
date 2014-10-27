#!/usr/bin/env python3
import re
import time
from queue import *

class dictParser(object):
	"""dictParser class for parsing nested dict in runtime in the monitoring server"""
	def __init__(self):
		self.Q = Queue(maxsize=0)
		self.SI = {CSIs:{}, usages:{}}
		self.SIs = {}

	def run(self, child_conn):
		"""
		------------sample dict for reference----------------- 
		{'nstime': 1413934751411769136, \
		'msg': '',\
		'time': 'Tue Oct 21 19:39:10 2014 to Tue Oct 21 19:39:11 2014', \
		'cpu_core_usages': [33.07, 21.98], \
		'from': 'node1'
		'component_info': {\
			'safComp=AmfDemo_44,safSu=SC-1,safSg=AmfDemo,safApp=AmfDemo1': {\
				'PID': 26750, \
				'cpu_usage': 2.0, \
				'component': 'safComp=AmfDemo_44,safSu=SC-1,safSg=AmfDemo,safApp=AmfDemo1', \
				'CSI': 'safCsi=AmfDemo_44,safSi=AmfDemo,safApp=AmfDemo1', \
				'HAState': 'Active', \
				'CSIFlags': 'Add One', \
				'type': 'csi_assignment'}, \
			'safComp=AmfDemo,safSu=SC-1,safSg=AmfDemo,safApp=AmfDemo1': {\
				'PID': 26770, \
				'cpu_usage': 4.0, \
				'component': 'safComp=AmfDemo,safSu=SC-1,safSg=AmfDemo,safApp=AmfDemo1', \
				'CSI': 'safCsi=AmfDemo,safSi=AmfDemo,safApp=AmfDemo1', \
				'HAState': 'Active', \
				'CSIFlags': 'Add One', \
				'type': 'csi_assignment'}\
			}, \
		}
		"""
		try:
			while True:
				oneDict = child_conn.recv()
				print(oneDict)
		except KeyboardInterrupt:
			print("\n'KeyboardInterrupt' received. Stopping dictParser.run()")
		except:
			raise

	def keypaths(self, nested):
		# http://stackoverflow.com/questions/18819154/python-finding-parent-keys-for-a-specific-value-in-a-nested-dictionary
		for key, value in nested.items():
			if isinstance(value, dict):
				for subkey, subvalue in keypaths(value):
					yield [key]+subkey, subvalue
			else:
				yield [key], value

	def lookup(self, key):
		reverse_dict = {}
		for keypath, value in keypaths(example_dict):
		    reverse_dict.setdefault(value, []).append(keypath)
		return reverse_dict.get(key)