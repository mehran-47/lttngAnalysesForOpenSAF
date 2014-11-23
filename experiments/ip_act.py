#!/usr/bin/env python3
from subprocess import Popen, PIPE
import shelve as sh, random as rd
from sys import argv as sysargv

'''
sh.open('./tempDB')
if sh.get('ipList')==None:
	sh['ipList']=[]
sh['ipList'].append(ip)
print(sh['ipList'])
sh.close()
'''
def ipSet():
	ip = "172.16.159."+str(rd.randint(180,254))
	with sh.open('./tempDB') as db:
		if 'ipList' not in db:
			db['ipList']=[ip]
		else:
			tl = db['ipList']
			if ip not in tl:
				tl.append(ip)
			db['ipList'] = tl
		print(db['ipList'])
	print(Popen(['echo','"ip setting to '+ip+'"'], stdout=PIPE).communicate()[0].decode('utf8'))
	print(Popen(['date','+%T+%N'], stdout=PIPE).communicate()[0].decode('utf8'))

def ipDel(ip):
	pass

def ipClearAll():
	with sh.open('./tempDB') as db:
		if 'ipList' in db:
			del db['ipList']

if __name__=='__main__':
	if len(sysargv)==2 and sysargv[1]=='-set':
		ipSet()
	elif len(sysargv)==2 and sysargv[1]=='-del':
		ipClearAll()
	else:
		exit()