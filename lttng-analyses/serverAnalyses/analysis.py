#!/usr/bin/env python3
import re
import time
from multiprocessing import Queue
from serverAnalyses.listedDict import listedDict

class dictParser(object):
	"""dictParser class for parsing nested dict in runtime in the monitoring server"""
	def __init__(self, **kwargs):
		self.Q = Queue(maxsize=0)
		self.SIs = listedDict()
		self.timeout = 5
	def run(self, child_conn):
		"""
		-------------------Sample clientDict for reference----------------------------
		
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
				#self.Q.put(child_conn.recv())
				oneDict = child_conn.recv()
				if oneDict.get('msg'):
					print(oneDict['msg'])
				elif len(oneDict.get('component_info')) != 0:
					#print(oneDict.get('component_info'))
					self.createSIsDict(oneDict)
					self.SIs.prettyPrint(0)
					print('\n\n\n')

		except KeyboardInterrupt:
			print("\n'KeyboardInterrupt' received. Stopping dictParser.run()")
		except:
			raise
	"""	
	-------------------Used hierarchy for 'SIs' hashmap: -------------------------
	SIs > SI > Node > HAState > CSI > Component > Usages
	"""
	def createSIsDict(self, clientDict):
		for component in clientDict['component_info']:
			SI = re.findall(r'(?<=safSi=)(.+)(?=,)', clientDict['component_info'][component]['CSI'])[0]
			CSI = clientDict['component_info'][component]['CSI'] #For DN
			#CSI = re.findall(r'(?<=safCsi=)(.+)(?=,)', clientDict['component_info'][component]['CSI'])[0] #For RDN
			node = clientDict.get('from')
			HAState = clientDict['component_info'][component]['HAState']
			usages = {'cpu_usage': clientDict['component_info'][component]['cpu_usage']}
			self.SIs.populateNestedDict([SI,node,HAState,CSI,component], usages)
			self.SIs.populateNestedDict([SI,node,'time'], clientDict.get('time'))