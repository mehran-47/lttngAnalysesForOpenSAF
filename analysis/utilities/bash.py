#!/usr/bin/env python3
from subprocess import Popen, PIPE

def colorPrint(string, *stringAttrs, **allStringAttrs):
	colorMap = {\
		'PURPLE' : '\033[95m',\
		'CYAN' : '\033[96m',\
		'DARKCYAN' : '\033[36m',\
		'BLUE' : '\033[94m',\
		'GREEN' : '\033[92m',\
		'YELLOW' : '\033[93m',\
		'RED' : '\033[91m',\
		'BOLD' : '\033[1m',\
		'UNDERLINE' : '\033[4m',\
		'END' : '\033[0m'}
	if len(stringAttrs)>0:
		for attr in stringAttrs:
			string = colorMap.get(str(attr).upper()) + str(string) + colorMap['END']
		print(string)
	if len(allStringAttrs)>0:
		for stringAttrTuple in allStringAttrs.values():
			for attr in stringAttrTuple:
				string = colorMap.get(str(attr).upper()) + str(string) + colorMap['END']
			print(string)	

class bash():
	def __init__(self, **kwargs):
		self.show_output = True if kwargs.get('show_output')==None else kwargs.get('show_output')
		self.outputs = []
		self.keep = True if kwargs.get('keep') else False

	def execute(self, commands):
		if not self.keep:
			self.outputs = []
		count = 0
		commands = commands.split('\n')
		for command in commands:
			count += 1
			try:
				execution = Popen(command.split(' '), stdout=PIPE)
				output = execution.communicate()
				if output[0]!=None:
					self.outputs.append(output[0].decode('utf8'))
					if self.show_output:
						print(self.outputs[count-1])
			except FileNotFoundError:
					print('Error executing bash command at line %d "%s"' %(count,command))
					break
			except:
				print('Unknown exception')
				raise

if __name__ == '__main__':
	while True:
		try:
			inpt=[oneArg.strip() for oneArg in input("test 'colorPrint'. Input your string \n> ").strip().split('|')]
			colorPrint(inpt[0], allStringAttrs=inpt[1:])
		except KeyboardInterrupt:
			print('\nBye')
			break