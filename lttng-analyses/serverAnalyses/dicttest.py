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

	def getFromPath(dictToGetFrom, path):
		if isinstance( dictToGetFrom.get(path[0]) , dict):
			getFromPath(dictToGetFrom.get(path[0]) , dictToGetFrom.get(path.pop(0)))
		else:
			return dictToGetFrom.get(path[0])


if __name__ == '__main__':
	'''
	X= {}
	populateNestedDict(X,['a','b','c','d'],{'haha':5})
	#listToNestedDict(X,['a','b','c'],{'haha':5})
	populateNestedDict(X,['x','y'],['xy','yx'])
	print(getFromPath())
	'''
	X = listedDict()
	X.populateNestedDict(['a','b','c','d'],{'haha':5})
	X.populateNestedDict(['x','y'],['xy','yx'])
	print(X)