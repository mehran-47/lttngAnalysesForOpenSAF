#!/usr/bin/env python3

import subprocess

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
				execution = subprocess.Popen(command.split(' '), stdout=subprocess.PIPE)
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