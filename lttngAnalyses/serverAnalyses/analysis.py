#!/usr/bin/env python3
import re, time, shelve as sh, json, mimetypes, os
#import matplotlib.pyplot as plt
from threading import Thread
from lttngAnalyses.serverAnalyses.listedDict import listedDict
#from lttngAnalyses.EEaction.dispatcher import dispatch as EEdispatch
from itertools import cycle
from subprocess import call
from http.server import BaseHTTPRequestHandler, HTTPServer

GUI_json = {}
root_dir = os.getcwd() + '/'

class MonitoringGUIHandler(BaseHTTPRequestHandler):
	'''Small class for streaming monitoring output via a simple HTTP server'''
	error_message_format = '<h1>404: cul-de-sac :(</h1>'
	
	def do_GET(self):
		mime = {"html":"text/html", "css":"text/css", "png":"image/png", "js":"application/javascript", "json":"application/json"}
		RequestedFileType = mimetypes.guess_type(self.path)[0] if mimetypes.guess_type(self.path)[0]!=None else 'text/html'
		try:
			if self.path == '/':
				self.send_response(200)
				self.send_header("Content-type", "application/json")
				self.end_headers()
				self.wfile.write(bytes(json.dumps(GUI_json), 'UTF-8'))
				return
			elif os.path.isfile(root_dir + self.path):
				self.send_response(200)
				self.send_header("Content-type", RequestedFileType)
				self.end_headers()
				fp = open(root_dir + self.path, 'rb')
				self.wfile.write(fp.read())
				fp.close()
				return
			else:
				self.send_response(404, 'File not found')
				self.send_header("Content-type", 'text/html')
				self.end_headers()
				self.wfile.write(bytes('File not found', 'UTF-8'))
				return
		except BrokenPipeError:
			print('Failed to complete request')
	
	def log_message(self, format, *args):
		return



class dictParser(object):
	"""dictParser class for parsing nested dict in runtime in the monitoring server"""
	def __init__(self, **kwargs):
		self.SIs = listedDict()
		self.timeout = 5		
		self.SI_usages = listedDict()
		self.cpu_usage_list = []
		self.listedUsages = listedDict()
		self.EE_triggered = False
		self.cluster_has_min_config = False

	def run(self, child_conn, GUIserver=''):
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
		if GUIserver!='':
			server, port = GUIserver.split(':')[0], int(GUIserver.split(':')[1])
			monitoringGUIserver = HTTPServer((server, port), MonitoringGUIHandler)
			Thread(target=monitoringGUIserver.serve_forever).start()
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
					GUI_json['SIs'] = self.SIs
					self.SIs.prettyPrint(0)
					self.setUsage()
				except KeyError:
					print('Warning! invalid key exists in data structure.\n(KeyError exception caught at serverAnalyses/analysis.py:run())')
					self.SIs=listedDict()
					continue
				#Dynamic long usage list data structure creation starts with last 1000 data points
				self.updateListedUsages(1000)
				self.determineEEaction('cpu_usage', 15, 45, 5)
				print('\n-----------------SI-load:-------------------')
				self.SI_usages.prettyPrint(0, keyColor=['blue','bold'], valColor=['blue'])
				GUI_json['summ'] = self.SI_usages
				#print(self.listedUsages)	
				print('\n\n\n')			
		except KeyboardInterrupt:
			print("\n'KeyboardInterrupt' received. Stopping Server Daemon (dictParser.run())")
			self.cpu_usage_list=[]
			#for saving usage dump
			"""
			import json
			with open('./listedUsages.json', 'w') as jsonDump: jsonDump.write(json.dumps(self.listedUsages))
			"""
		except:
			raise
	"""	
	-------------------Used hierarchy for 'SIs' nested hashmap: -------------------------
	SIs > SI > Node > HAState > CSI > Component > Usages
	"""
	
	def createSIsDict(self, clientDict):
		for component in clientDict['component_info']:
			#line below was for RDN, I'm keeping it if in case that's what is needed, for now, using DN in the next line
			#SI = re.findall(r'(?<=safSi=)(.+)(?=,)', clientDict['component_info'][component]['CSI'])[0]
			SI = 'safSi=' + re.findall(r'(?<=safSi=)(.+)(?=)', clientDict['component_info'][component]['CSI'])[0]
			CSI = clientDict['component_info'][component]['CSI'] #For DN
			#CSI = re.findall(r'(?<=safCsi=)(.+)(?=,)', clientDict['component_info'][component]['CSI'])[0] #For RDN
			node = clientDict.get('from')
			HAState = clientDict['component_info'][component]['HAState']
			usages = {'cpu_usage': clientDict['component_info'][component]['cpu_usage'],\
			 'memory_usage' : clientDict['component_info'][component]['memory_usage'],\
			 'cpu_cycles_abs' : clientDict['component_info'][component]['cpu_cycles_abs'],\
			 'memory_usage_abs' : clientDict['component_info'][component]['memory_usage_abs']}
			self.SIs.populateNestedDict([SI,node,HAState,CSI,component], usages)
			#Correcting potential double-entry of component-csi map in HA-States
			for HAS in self.SIs.getFromPath([SI,node]):
				if self.SIs.getFromPath([SI,node,HAS,CSI])!=None:
					if HAS!=HAState and component in self.SIs.getFromPath([SI,node,HAS,CSI]):
						self.SIs.deleteItem([SI,node,HAS,CSI])

	def setUsage(self):
		nodeCount=0
		for SI in self.SIs:
			nodeCount = len(self.SIs.get(SI).keys()) if self.SIs.get(SI)!=None else 0
			for node in self.SIs[SI]:
				if SI not in self.SI_usages:
					self.SI_usages[SI]=listedDict()
				self.SIs[SI][node].updateUsage(self.SI_usages[SI])
			for key in self.SI_usages[SI]:
				if key.split('_')[-1]!='abs':
					self.SI_usages[SI][key] = self.SI_usages[SI][key]/nodeCount if nodeCount!=0 else 0.0
				else:
					self.SI_usages[SI][key] = self.SI_usages[SI][key]


	def updateListedUsages(self, dataPointsLimit):
		#---S---Dynamic long usage list data structure creation starts
		for SI in self.SI_usages:
			for usageKey in self.SI_usages[SI]:
				if usageKey not in self.listedUsages.keys():
					self.listedUsages[usageKey]=listedDict()
				if SI not in self.listedUsages[usageKey].keys():
					self.listedUsages[usageKey][SI]=[]							
				self.listedUsages[usageKey][SI].append(self.SI_usages[SI][usageKey])
				#keeping number of entries within 1000 data points
				self.listedUsages[usageKey][SI] = self.listedUsages[usageKey][SI][-dataPointsLimit:]
		#---E---Dynamic long usage list data structure creation ends

	
	def determineEEaction(self, usageKey, numOfConsideredDataPoints, upperLim, lowerLim):
		if self.listedUsages.get(usageKey)!=None:
			for SI in self.listedUsages[usageKey]:
				nodeCount = len(self.SIs.get(SI).keys()) if self.SIs.get(SI)!=None else 0
				slidingWindowAverage = sum(self.listedUsages[usageKey][SI][-numOfConsideredDataPoints:])/numOfConsideredDataPoints
				"""
				print('#########\nSI: %s\nSliding window: %s\nAverage: %d\nUpperlim: %d and LowerLim: %d\n nodeCount: %d\n#########\n'\
				 %( str(SI), 
					str(self.listedUsages[usageKey][SI][-numOfConsideredDataPoints:]),\
					slidingWindowAverage,\
					upperLim,\
					lowerLim,\
					nodeCount\
				 ), end='\r')
				"""
				if slidingWindowAverage > upperLim and not self.EE_triggered:
					print('###############################Triggered %s, increase ####################################' %(SI))
					cmd = 'python -m EE.main '+ str(SI) + ' 1 1'
					call(cmd.split(' '))
					Thread(target=self.__countDownForEEFlag, args=(numOfConsideredDataPoints+50, )).start()
					self.EE_triggered = True
				elif slidingWindowAverage < lowerLim and not self.EE_triggered and nodeCount > 2:
					#nodeCount here is essentially the 'minimum configutaion' (temporary solution)
					print('###############################Triggered %s, decrease ####################################' %(SI))
					cmd = 'python -m EE.main '+ str(SI) + ' 1 2'
					call(cmd.split(' '))
					Thread(target=self.__countDownForEEFlag, args=(numOfConsideredDataPoints+50, )).start()
					self.EE_triggered = True



	def __countDownForEEFlag(self, t):
		time.sleep(t)
		self.EE_triggered = False