#!/usr/bin/env python

from __future__ import print_function
import immombin, sys
from immombin import AisException

_flagdict = {
    'MULTI_VALUE':0x00000001,
    'RDN':0x00000002,
    'CONFIG':0x00000100,
    'WRITABLE':0x00000200,
    'INITIALIZED':0x00000400,
    'RUNTIME':0x00010000,
    'PERSISTENT':0x00020000,
    'CACHED':0x00040000,
    }

def _flag_list(flags):
    l = []
    for k in _flagdict.keys():
        if flags & _flagdict[k]:
            l.append(k)
    return l

def _flag_parse(l):
    f = 0
    for n in l:
        f |= _flagdict[n]
    return f

def _get_rdn_attr(class_name):
    (c,a) = getclass(class_name)
    for (n,t,f,l) in a:
        if 'RDN' in f:
            return (n,t)

if __name__ == '__main__':
	if sys.argv[1:]:
		DN = sys.argv[1]
		immombin.saImmOmInitialize()
		attributes = {}
		for(n,t,v) in immombin.saImmOmAccessorGet(DN):
			attributes[n] = v
		print(attributes)
	else:
		print('DN not provided')

