#!/usr/bin/env python3 
'''
def populateNestedDict(dictToPopulate, itemPath, item):
	if len(itemPath)==1:
		dictToPopulate[itemPath[0]] = item
	else:
		dictToPopulate[itemPath[0]] = {}
		populateNestedDict(dictToPopulate[itemPath.pop(0)], itemPath, item)

def getFromPath(dictToGetFrom, path):
	if isinstance( dictToGetFrom.get(path[0]) , dict):
		getFromPath(dictToGetFrom.get(path[0]) , dictToGetFrom.get(path.pop(0)))
	else:
		return dictToGetFrom.get(path[0])
'''


class listedDict(dict):
	def __init__(self):
		pass

	def populateNestedDict(self, itemPath, item):
		if len(itemPath)==1:
			self[itemPath[0]] = item
		else:
			self[itemPath[0]] = listedDict()
			listedDict.populateNestedDict(self[itemPath.pop(0)], itemPath, item)

	def getFromPath(self, path):
		if isinstance(self.get(path[0]), dict):
			listedDict.getFromPath(self.get(path.pop(0)))
		else:
			return self.get(path[0])