#!/usr/bin/env python3
from functools import reduce #functools.reduce(dict.get,['a','b','c'],dc)
import json

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

	def getFromPath(self, path):
		if isinstance(self.get(path[0]), dict):
			listedDict.getFromPath(self.get(path.pop(0)))
		else:
			return self.get(path[0])

	def keypaths(self):
		# http://stackoverflow.com/questions/18819154/python-finding-parent-keys-for-a-specific-value-in-a-nested-dictionary
		for key, value in self.items():
			if isinstance(value, dict):
				for subkey, subvalue in listedDict.keypaths(value):
					yield [key]+subkey, subvalue
			else:
				yield [key], value

	def reverse_dict(self):
		reverse_dict = {}
		for keypath, value in self.keypaths():
		    reverse_dict[str(value)] = keypath
		return reverse_dict

	def getFromPath(self, itemPath):
		itemPath = itemPath.split('.') if isinstance(itemPath,str) else itemPath
		return reduce(dict.get, itemPath, self)

	def lookupValuePath(self, key):
		return self.reverse_dict().get(str(key))

	def deleteItem(self, itemPath):
		itemPath = itemPath.split('.') if isinstance(itemPath,str) else itemPath
		if len(itemPath)==1 and self.get(itemPath[0]):
			del self[itemPath[0]]
		else:
			listedDict.deleteItem(self[itemPath.pop(0)], itemPath)

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

if __name__ == '__main__':
	'''
	Some tests of the API
	'''
	X = listedDict()
	X.populateNestedDict('a.b1',{'ab_val':5})
	X.populateNestedDict('a.b2',{'ab_val':7})
	X.populateNestedDict('a.b.c1','abc1_val')
	X.populateNestedDict('a.b.c2','abc2_val')
	X.prettyPrint(0)
	X.saveAt(input('Save at filepath:\n> '))
	'''
	print(X.getFromPath(input('Provide path to get value: (e.g. a.b.c )\n> ')))
	#print('keypaths:\n%s'%(str(X.keypaths())))
	print('reverse-dict:\n%s'%(X.reverse_dict()))
	print(X.lookupValuePath(input('\nlookup in nested dict with value\n> ')))
	X.deleteItem(input('type path to delete item/dict from main tree\n> '))
	print(X)
	'''