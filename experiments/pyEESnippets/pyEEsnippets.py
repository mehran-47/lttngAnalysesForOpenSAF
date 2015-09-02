#!/usr/bin/env python

from __future__ import print_function
import pyImm.immombin, pyImm.immom, pyImm.immomexamples, sys
from pyImm.immombin import AisException
from subprocess import call


class osafMod( object):
    def __init__(self, *admin):
        pyImm.immombin.saImmOmFinalize()
        pyImm.immombin.saImmOmInitialize()        
        if admin[0:]:
            #Initializing the admin of the DNs. Can't change a thing without setting the admin.
            self.admin = admin[0]
            pyImm.immombin.saImmOmAdminOwnerInitialize(self.admin)
        else:
            pyImm.immombin.saImmOmAdminOwnerInitialize('monitoring_admin')

    def modifyObject(self, DN, attrList):
        try:
            pyImm.immombin.saImmOmAdminOwnerClear('SA_IMM_SUBTREE', [DN])
            #Setting the admin to self.admin
            pyImm.immombin.saImmOmAdminOwnerSet('SA_IMM_SUBTREE', [DN])            
        except:
            print('ownership didn\'t work')
            raise
        #'''
        try:
            pyImm.immombin.saImmOmCcbInitialize(0)
            pyImm.immombin.saImmOmCcbObjectModify(DN, attrList)
            pyImm.immombin.saImmOmCcbApply()
            pyImm.immombin.saImmOmCcbFinalize()
            pyImm.immombin.saImmOmAdminOwnerFinalize()
        except:
            print('modification within object didn\'t work')
            raise
        call(['amf-adm','lock', DN])
        call(['amf-adm','unlock', DN])
        call(['service','opensafd','status'])
        #'''

if __name__ == '__main__':    
    if sys.argv[3:]:
        modObj = osafMod('mk')
        DN = sys.argv[1]
        attribute = sys.argv[2]
        value = sys.argv[3]
        print('chaning the attribute %r to %r in object DN %r' %(attribute, value, DN))
        print('prev-state:')
        #print(pyImm.immom.getobject(DN))
        print(pyImm.immom.getattributes(DN))
        print('owner of the DN')
        print(pyImm.immomexamples.getadminowner(DN))
        attrList = [
            (attribute, 'SAUINT32T', [int(value)])
        ]        
        modObj.modifyObject(DN, attrList)
        print('owner of the DN')
        print(pyImm.immomexamples.getadminowner(DN))
    else:
        print('DN not provided')
    
    """
    pyImm.immombin.saImmOmInitialize()
    DN = 'opensafImm=opensafImm,safApp=safImmService'
    for (n,t,v) in pyImm.immombin.saImmOmAccessorGet(DN):
        if n == 'opensafImmClassNames':
            print(v)
    """

