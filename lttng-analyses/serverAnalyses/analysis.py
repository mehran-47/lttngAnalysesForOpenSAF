#!/usr/bin/env python3
import re, time, matplotlib.pyplot as plt
from threading import Thread
from serverAnalyses.listedDict import listedDict

class dictParser(object):
	"""dictParser class for parsing nested dict in runtime in the monitoring server"""
	def __init__(self, **kwargs):
		self.SIs = listedDict()
		self.timeout = 5
		self.usages = listedDict()
		self.cpu_usage_list = []
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
		plotProc = Thread(target=self.plotCPUusage, args=([100,50],))
		plotProc.start()
		try:
			while True:
				oneDict = child_conn.recv()
				if oneDict.get('msg'):
					print(oneDict['msg'])
				elif len(oneDict.get('component_info')) > 0:
					#For debugging; print the received JSON : print(oneDict.get('component_info'))
					self.createSIsDict(oneDict)	
				elif len(oneDict.get('component_info')) == 0: #Potentially because service is shut down on the node
					for SI in self.SIs:
						if self.SIs.getFromPath([SI,oneDict['from']])!=None:
							self.SIs.deleteItem([SI,oneDict['from']])
				#Output hub
				if oneDict.get('time'):
					self.usages=listedDict()
					print('Usage on ' + oneDict.get('time'))
				self.SIs.prettyPrint(0)
				self.setAggregatedUsage()
				self.cpu_usage_list.append(self.usages.get('cpu_usage') if self.usages.get('cpu_usage')!=None else 0.0)
				print('\n---------SI : \'AmfDemo\' usages:---------')
				print(self.usages)
				#print(self.cpu_usage_list)
				print('\n\n\n')
		except KeyboardInterrupt:
			print("\n'KeyboardInterrupt' received. Stopping Server Daemon (dictParser.run())")
			self.cpu_usage_list=[]
			plotProc.join()
		except:
			raise
	"""	
	-------------------Used hierarchy for 'SIs' nested hashmap: -------------------------
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
			#Correcting potential double-entry of component-csi map in HA-States
			for HAS in self.SIs.getFromPath([SI,node]):
				if self.SIs.getFromPath([SI,node,HAS,CSI])!=None:
					if HAS!=HAState and component in self.SIs.getFromPath([SI,node,HAS,CSI]):
						self.SIs.deleteItem([SI,node,HAS,CSI])
	
	def refreshOneNode(self, clientDict):
		nodeFresh = listedDict()
		nodeFresh.populateNestedDict([HAState,CSI,component], usages)
		for si in self.SIs:
			self.SIs[si][node] = nodeFresh

	def setAggregatedUsage(self):
		count = 0
		for SI in self.SIs:
			for node in self.SIs[SI]:
				for HAState in self.SIs[SI][node]:
					for CSI in self.SIs[SI][node][HAState]:
						for component in self.SIs[SI][node][HAState][CSI]:
							for usage in self.SIs[SI][node][HAState][CSI][component]:
								if usage!=None and self.usages.get(usage)!=None:
									self.usages[usage]+=self.SIs[SI][node][HAState][CSI][component][usage]
								elif usage!=None:
									self.usages[usage]=self.SIs[SI][node][HAState][CSI][component][usage]
								count+=1								
		'''
		for key in self.usages:
			self.usages[key] = self.usages[key]/count if count!=0 else self.usages[key]
		'''


	def plotCPUusage(self, dimension):
		plt.axis([0, dimension[0], 0, dimension[1]])
		plt.ion()
		plt.show()
		while True:
			try:
				plt.scatter(range(0, len(self.cpu_usage_list[-dimension[0]:])), self.cpu_usage_list[-dimension[0]:])
				plt.draw()
				time.sleep(0.1)
				plt.clf()
				plt.axis([0, dimension[0], 0, dimension[1]])
			except KeyboardInterrupt:
				print('\nplotting stopped\n')
				sys.exit()