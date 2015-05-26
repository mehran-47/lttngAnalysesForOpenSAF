#!/usr/bin/env python3
import re, time
#import matplotlib.pyplot as plt
from threading import Thread
from lttngAnalyses.serverAnalyses.listedDict import listedDict
from itertools import cycle

class dictParser(object):
	"""dictParser class for parsing nested dict in runtime in the monitoring server"""
	def __init__(self, **kwargs):
		self.SIs = listedDict()
		self.timeout = 5		
		self.SI_usages = listedDict()
		self.cpu_usage_list = []
		self.listedUsages = listedDict()

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
		#Thread spawner for plotting
		#Thread(target=self.plotThreadPerSI).start()
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
				try:
					if oneDict.get('time'):					
						self.SI_usages = listedDict()
						print('Usage on ' + oneDict.get('time'))
					self.SIs.prettyPrint(0)		
					self.setUsage()
				except KeyError:
					print('Warning! invalid key exists in data structure.\n(KeyError exception caught at serverAnalyses/analysis.py:run())')
					self.SIs=listedDict()
					continue
				#---S---Dynamic long usage list data structure creation starts
				for SI in self.SI_usages:
					for usageKey in self.SI_usages[SI]:
						if usageKey not in self.listedUsages.keys():
							self.listedUsages[usageKey]=listedDict()
						if SI not in self.listedUsages[usageKey].keys():
							self.listedUsages[usageKey][SI]=[]							
						self.listedUsages[usageKey][SI].append(self.SI_usages[SI][usageKey])
				#---E---Dynamic long usage list data structure creation ends				
				print('\n-----------------SI-load:-------------------')
				self.SI_usages.prettyPrint(0, keyColor=['cyan','bold'], valColor=['DARKCYAN'])
				#print(self.listedUsages)		
				print('\n\n\n')			
		except KeyboardInterrupt:
			print("\n'KeyboardInterrupt' received. Stopping Server Daemon (dictParser.run())")
			self.cpu_usage_list=[]
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
			usages = {'cpu_usage': clientDict['component_info'][component]['cpu_usage'], 'memory_usage':clientDict['component_info'][component]['memory_usage']}
			self.SIs.populateNestedDict([SI,node,HAState,CSI,component], usages)
			#Correcting potential double-entry of component-csi map in HA-States
			for HAS in self.SIs.getFromPath([SI,node]):
				if self.SIs.getFromPath([SI,node,HAS,CSI])!=None:
					if HAS!=HAState and component in self.SIs.getFromPath([SI,node,HAS,CSI]):
						self.SIs.deleteItem([SI,node,HAS,CSI])
	
	#Deprecated?! Will be deleted soon					
	def setAggregatedUsage(self):
		nodeCount = 0
		for SI in self.SIs:
			for node in self.SIs[SI]:
				nodeCount+=1
				for HAState in self.SIs[SI][node]:
					for CSI in self.SIs[SI][node][HAState]:
						for component in self.SIs[SI][node][HAState][CSI]:
							for usage in self.SIs[SI][node][HAState][CSI][component]:
								if usage!=None and self.usages.get(usage)!=None:
									self.usages[usage]+=self.SIs[SI][node][HAState][CSI][component][usage]
								elif usage!=None:
									self.usages[usage]=self.SIs[SI][node][HAState][CSI][component][usage]																		
		for key in self.usages:
			self.usages[key] = self.usages[key]/nodeCount if nodeCount!=0 else self.usages[key]

	def setUsage(self):
		nodeCount=0
		for SI in self.SIs:
			for node in self.SIs[SI]:
				nodeCount+=1
				if SI not in self.SI_usages:
					self.SI_usages[SI]=listedDict()
				self.SIs[SI][node].updateUsage(self.SI_usages[SI])
			for key in self.SI_usages[SI]:
				self.SI_usages[SI][key] = self.SI_usages[SI][key]/nodeCount if nodeCount!=0 else self.SI_usages[SI][key]/nodeCount


	def plotThreadPerSI(self):
		thr = Thread(target=self.plotList, args=([100,50],))
		while True:
			prevListedUsg = len(self.listedUsages)
			time.sleep(4)
			if len(self.listedUsages)!=prevListedUsg:
				if thr.isAlive(): 
					thr.join()					
				thr = Thread(target=self.plotList, args=([100,50],))
				thr.start()

	def plotList(self, dimension):
		print('##############starting thread###############')
		colors = cycle(["b", "g"])
		plt.figure(1)
		seqNum=1
		for usage in self.listedUsages:						
			plt.subplot(len(self.listedUsages[usage].keys())*100+10+seqNum)
			plt.axis([0, dimension[0], 0, dimension[1]])
			plt.ion()
			seqNum+=1		
		plt.show()
		while True:
			try:
				seqNum=1
				for usage in self.listedUsages:
					for SI in self.listedUsages[usage]:
						plt.subplot(len(self.listedUsages[usage].keys())*100+10+seqNum)
						##For progressing plots. Could omit for short demos
						#plt.cla()
						plt.axis([0, dimension[0], 0, dimension[1]])					
						plt.scatter(range(0, len(self.listedUsages[usage][SI])),\
						 self.listedUsages[usage][SI][-dimension[0]:],\
						 color=next(colors))
						plt.draw()
						time.sleep(0.01)
					seqNum+=1
					#break
			except KeyboardInterrupt:
				print('\nplotting stopped\n')
				sys.exit()		

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
		