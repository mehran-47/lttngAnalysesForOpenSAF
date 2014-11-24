#!/usr/bin/python3
from subprocess import Popen, PIPE
import shelve as sh, random as rd, time
from sys import argv as sysargv

def __shellIpSet(ip):
	print(Popen(['echo','"IP component selected for IP '+ip+'"'], stdout=PIPE).communicate()[0].decode('utf8'))
	startLogString  = "IP component is starting:" + Popen(['date','+%T+%N'], stdout=PIPE).communicate()[0].decode('utf8')
	Popen(['logger', startLogString])
	return Popen('ip addr add'.split(' ')+[ip]+'dev eth0'.split(' '), stdout=PIPE).communicate()[0].decode('utf8')

def __shellIpDel(ip):
	print(Popen(['echo','"IP component terminating for IP '+ip+'"'], stdout=PIPE).communicate()[0].decode('utf8'))
	startLogString  = "component is terminating:" + Popen(['date','+%T+%N'], stdout=PIPE).communicate()[0].decode('utf8')
	Popen(['logger', startLogString])
	return Popen('ip addr del'.split(' ')+[ip]+'dev eth0'.split(' '), stdout=PIPE).communicate()[0].decode('utf8')

def ipSet(**kwargs):
	ip = "172.16.159."+str(rd.randint(180,254))+"/24"
	execRet = __shellIpSet(ip)
	if execRet=="":
		with sh.open('/tmp/tempDb') as db:
			if 'ipList' not in db:
				db['ipList']=[ip]
			else:
				tl = db['ipList']
				if ip not in tl:
					tl.append(ip)
				db['ipList'] = tl
			print(db['ipList'])
	else:
		print(execRet)
	if kwargs.get('verbose')==True:
		print(execRet)

def ipDel(ip):
	pass

def ipClearAll(**kwargs):
	with sh.open('/tmp/tempDb') as db:
		if 'ipList' in db:
			ipl = db['ipList']
			cloneCopy = ipl[:]
			for ip in ipl:
				execRet = __shellIpDel(ip)
				if execRet=="":
					cloneCopy.remove(ip)
				else:
					print(execRet)				
			db['ipList'] = cloneCopy
	ipAddr = Popen(['ip','addr'], stdout=PIPE).communicate()[0].decode('utf8')
	if kwargs.get('verbose')==True:
		print(ipAddr)

if __name__=='__main__':
	if len(sysargv)==2 and sysargv[1]=='--help':
		print('-set\n\
			-delall\n\
			-showdb\n\
			-flushdb\n\
			--help')
	elif len(sysargv)==2 and sysargv[1]=='-set':
		ipSet()
	elif len(sysargv)==2 and sysargv[1]=='-delall':
		ipClearAll()
	elif len(sysargv)==2 and sysargv[1]=='-showdb':
		with sh.open('/tmp/tempDb') as db:
			print(db['ipList'])
	elif len(sysargv)==2 and sysargv[1]=='-flushdb':
		with sh.open('/tmp/tempDb') as db:
			db['ipList']=[]
	else:
		exit()