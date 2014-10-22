#!/usr/bin/env python3
from connection import connection
from multiprocessing import Process, Pipe
import multiprocessing
import sys
import time
import re

addr = '172.16.159.1'
port = 6666

def getval(parent_conn, SIs, nodes):
	try:
		while True:
			toprint = parent_conn.recv()
			if toprint==None:
				time.wait(2)
			elif toprint.get('msg')=='END_OF_Q':
				print('Reached end of queue')
				time.sleep(5)
			else:
				#SIs = {'components':[], 'active':{}, 'standby':{}, 'cpu_usage':0}
				if toprint.get('component_info') != None:
					if toprint.get('from') in nodes:
						nodes.remove(toprint.get('from'))
					component_info = toprint.get('component_info')
					for component in component_info:
						SI = re.findall(r'(?<=safSi=)(.+)(?=,)', component_info[component]['CSI'])[0]
						if SI not in SIs:
							SIs[SI] = {'active': [], 'standby': [], 'zombie':[], 'cpu_usage':0.0}
						else:
							if component_info[component]['HAState']=='Active' and component not in SIs[SI]['active']:
								SIs[SI]['active'].append(component)
							if component_info[component]['HAState']=='Standby' and component not in SIs[SI]['standby']:
								SIs[SI]['standby'].append(component)
							elif component not in SIs[SI]['active'] and component not in SIs[SI]['standby']:
								SIs[SI]['zombie'].append(component)
							SIs[SI]['cpu_usage'] += float(component_info[component]['cpu_usage'])
					if len(nodes)==0:
						for SI in SIs:
							print('SI name : ' + SI)
							print('Total CPU usage : ' + str(SIs[SI]['cpu_usage']))
							print('Active components : ')
							print('\t'+str(SIs[SI]['active']))
							print('Standby components : ') 
							print('\t'+str(SIs[SI]['standby']))
							print('Zombie components : ')
							print('\t'+str(SIs[SI]['zombie']))
						print('\n\n\n\n\n\n\n\n\n\n\n\n')
						SIs = {}
						nodes = ['node1','node2']
	
	except KeyboardInterrupt:
		print("\n'KeyboardInterrupt' received. Stopping server-reader:%r" %(multiprocessing.current_process().name))
	except:
		raise


if __name__ == '__main__':
	if len(sys.argv) == 2:
		port = sys.argv[1]
	elif len(sys.argv) == 3:
		addr = sys.argv[1]
		port = int(sys.argv[2])
	server = connection(addr,port, debug=True)
	parent_conn, child_conn = Pipe()
	print("server listening to %r:%r" %(addr,port))
	SIs = {}
	getterproc = Process(target=getval, args=((child_conn),SIs,['node1','node2']))
	getterproc.start()
	server.listen(parent_conn)
	getterproc.join()