#!/usr/bin/env python3
import shelve as sh
if __name__ == '__main__':
	nested_1 = {\
	'l11':{'l21':'level 2',\
			'l22':0.01,\
		},\
	'l12':[5,13,18,'chicken',{1:'pop',2:'push'}],\
	'l13':'trouble'\
	}
	aList = ['c1',0.129]
	db = sh.open('./shelveDB')
	db['nested_1'] = nested_1
	db['aList'] = aList
	db.close()