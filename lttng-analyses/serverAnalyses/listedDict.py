#!/usr/bin/env python3
from functools import reduce #functools.reduce(dict.get,['a','b','c'],dc)
import json
import copy

class listedDict(dict):
	def __init__(self):
		pass

	def populateNestedDict(self, itemPath, item):
		itemPath = itemPath.split('.') if isinstance(itemPath,str) else itemPath
		if len(itemPath)==1:
			self[itemPath[0]] = item
		else:
			self[itemPath[0]] = listedDict() if not isinstance(self.get(itemPath[0]), dict) else self[itemPath[0]]
			listedDict.populateNestedDict(self[itemPath.pop(0)], itemPath, item)

	def keypaths(self):
		# http://stackoverflow.com/questions/18819154/python-finding-parent-keys-for-a-specific-value-in-a-nested-dictionary
		for key, value in self.items():
			if isinstance(value, dict):
				for subkey, subvalue in listedDict.keypaths(value):
					yield [key]+subkey, subvalue
			else:
				yield [key], value

	def reverse_dict(self):
		reverse_dict = listedDict()
		for keypath, value in self.keypaths():
		    reverse_dict[str(value)] = keypath
		return reverse_dict

	def getFromPath(self, itemPath):
		itemPath = itemPath.split('.') if isinstance(itemPath,str) else itemPath
		return reduce(dict.get, itemPath, self)

	def lookupValuePath(self, key):
		return self.reverse_dict().get(str(key))

	def deleteItem(self, itemPath):
		try:
			itemPath = itemPath.split('.') if isinstance(itemPath,str) else itemPath
			if len(itemPath)==1 and self.get(itemPath[0]):
				del self[itemPath[0]]
			else:
				listedDict.deleteItem(self[itemPath.pop(0)], itemPath)
		except KeyError:
			print('Invalid path provided to delete')
			return

	def prettyPrint(self, indent):
		for key, value in sorted(self.items()):
			if isinstance(value, dict):
				print(('  '*indent)+'"'+str(key)+'":')
				listedDict.prettyPrint(value, indent+1)
			else:
				print('  '*(indent+1)+'"'+str(key)+'":"'+str(value)+'",')

	def dictPrint(self):
		print(json.dumps(self, sort_keys=True))

	def saveAt(self, filePath):
		with open(filePath, 'w') as fs:
			fs.write(json.dumps(self, sort_keys=True))

	def loadFrom(self, filePath):
		with open(filePath, 'r') as fs:
			self = json.loads(fs.read())
		return self

	def structureDiff(self, listedDict):
		vals_1 = [str(item) for item in self.reverse_dict().values()]
		vals_2 = [str(item) for item in listedDict.reverse_dict().values()]
		return list(set(vals_1)-set(vals_2))+list(set(vals_2)-set(vals_1))




if __name__ == '__main__':
	'''
	Some tests of the API
	###
	X = listedDict()
	X.populateNestedDict('a.b1',{'ab_val':5})
	X.populateNestedDict('a.b2',{'ab_val':7})
	X.populateNestedDict('a.b.c1','abc1_val')
	X.populateNestedDict('a.b.c2','abc2_val')
	print('printing X:\n')
	X.prettyPrint(0)
	print(X.getFromPath(input('Provide path to get value: (e.g. a.b.c )\n> ')))
	Y = listedDict()
	Y = copy.deepcopy(X)
	print('printing Y:\n')
	Y.prettyPrint(0)
	pathtodel = str(input('select node to delete from Y for diff. Blank input will mean X=Y\n> '))
	if len(pathtodel)>0:
		Y.deleteItem(pathtodel)
	print('printing Y:\n')
	Y.prettyPrint(0)
	print('printing X:\n')
	X.prettyPrint(0)
	print('Difference between X and Y')
	print(X.structureDiff(Y))
	X.saveAt(input('Save at filepath:\n> '))
	Y = listedDict()
	print(Y.loadFrom(input('load from filepath:\n> ')))
	print(X.getFromPath(input('Provide path to get value: (e.g. a.b.c )\n> ')))
	print('keypaths:\n%s'%(str(X.keypaths())))
	print('reverse-dict:\n%s'%(X.reverse_dict()))
	print(X.lookupValuePath(input('\nlookup in nested dict with value\n> ')))
	X.deleteItem(input('type path to delete item/dict from main tree\n> '))
	print(X)
	'''