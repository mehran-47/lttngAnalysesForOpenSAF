#!/usr/bin/env python

from __future__ import print_function
import pyImm.immombin, pyImm.immom, pyImm.immomexamples, sys, re, pprint
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
            #Setting the admin to the latest admin initialized. In this case its self.admin 
            pyImm.immombin.saImmOmAdminOwnerSet('SA_IMM_SUBTREE', [DN])
        except:
            print('ownership didn\'t work')
            raise
        try:
            pyImm.immombin.saImmOmCcbInitialize(0)
            pyImm.immombin.saImmOmCcbObjectModify(DN, attrList)
            pyImm.immombin.saImmOmCcbApply()
            pyImm.immombin.saImmOmCcbFinalize()
            #Clearing admin-ownership of the object and finalizing
            pyImm.immombin.saImmOmAdminOwnerFinalize()
        except:
            print('modification within object didn\'t work')
            raise

    def refresh(self, DN, **kwargs):
        call(['amf-adm','lock', DN])
        call(['amf-adm','unlock', DN])
        if kwargs.get('show'): call(['service','opensafd','status'])

    def chaindedQuery(self, queryChain):
        cq = lambda DN, attribute: pyImm.immom.getattributes(DN)[attribute][0]
        return reduce(cq, queryChain)


    def getSIsOfSG(self, *SG_DN):
        SG_SI = {}
        for elem in pyImm.immom.getinstanceof('', 'SaAmfSIAssignment'):
            SG = 'safSg=' + re.findall(r'(?<=safSg=)(.+)(?=,safSi)', elem)[0].replace('\\','')
            SI = 'safSi=' + re.findall(r'(?<=safSi=)(.+)(?=)', elem)[0].replace('\\','')
            if SG in SG_SI:
                if SI not in SG_SI.get(SG):
                    SG_SI[SG].append(SI)
            else:
                SG_SI[SG] = [SI]
        return SG_SI if len(SG_DN)==0 else SG_SI.get(SG_DN[0])

    def getSUsAndNodesOfSI(self, *SI_DN):
        SI_SU_node = {}
        for elem in pyImm.immom.getinstanceof('', 'SaAmfSIAssignment'):
            SI = 'safSi='+re.findall(r'(?<=safSi=)(.+)(?=)', elem)[0].replace('\\','')
            SU = 'safSu='+re.findall(r'(?<=safSu=)(.+)(?=,safSi)', elem)[0].replace('\\','')
            node = chaindedQuery([SU, 'saAmfSUHostedByNode'])
            if SI in SI_SU_node:
                if (SU, node) not in SI_SU_node.get(SI):
                    SI_SU_node[SI].append((SU, node))
            else:
                SI_SU_node[SI] = [(SU, node)]
        return SI_SU_node if len(SI_DN)==0 else SI_SU_node.get(SI_DN[0])


    def mergeSIworkload(self, DN, **kwargs):
        reduncancyModel = self.chaindedQuery([DN, 'saAmfSIProtectedbySG', 'saAmfSGType', 'saAmfSgtRedundancyModel'])
        SG = self.chaindedQuery([DN, 'saAmfSIProtectedbySG'])
        if self.chaindedQuery([DN, 'saAmfSIPrefActiveAssignments']) >= 3:
            attrList = [
                ('saAmfSIPrefActiveAssignments', 'SAUINT32T', [self.chaindedQuery([DN,'saAmfSIPrefActiveAssignments'])-1 ] )
            ]
            self.modifyObject(DN, attrList)
            self.refresh(DN)            
        elif (self.chaindedQuery([SG,'saAmfSGNumPrefAssignedSUs'])-1)*self.chaindedQuery([SG,'saAmfSGMaxActiveSIsperSU']) >= len(self.getSIsOfSG(SG)):
            for SI in self.getSIsOfSG(SG):
                if self.chaindedQuery([SG,'saAmfSGNumPrefAssignedSUs'])-1 >=  self.chaindedQuery([SI, 'saAmfSIPrefActiveAssignments']):
                    saAmfSGNumPrefAssignedSUs
                    attrList = [
                        ('saAmfSGNumPrefAssignedSUs', 'SAUINT32T', [self.chaindedQuery([SG,'saAmfSGNumPrefAssignedSUs'])-1 ] )
                    ]
                    self.modifyObject(SG, attrList)
                    #call buffer manager

    def spreadSIWorkload(self, DN, flag, **batchAction):
        """
        DN is the DN of the SI on which the operation is to be performed. 
        'flag' set to 1 is for increasing current assignment by 1, 
        'flag' set to 2  is for decreasing current assignment by 1. 
        **batchAction is for increasing or decreasing more than 1 assignment at one go. (Haven't decided on it yet.)
        """
        reduncancyModel = self.chaindedQuery([DN, 'saAmfSIProtectedbySG', 'saAmfSGType', 'saAmfSgtRedundancyModel'])
        SG = self.chaindedQuery([DN, 'saAmfSIProtectedbySG'])
        if reduncancyModel == SaAmfRedundancyModel.index('N_WAY_ACTIVE'):
            if flag==1 and pyImm.immom.getattributes(SG)['saAmfSGNumCurrInstantiatedSpareSUs'][0] > 0:
                self.changePreferredAssignmentNums(SG, DN, 1)
            elif flag==2 and pyImm.immom.getattributes(SG)['saAmfSGNumCurrAssignedSUs'][0] > pyImm.immom.getattributes(SG)['saAmfSGNumPrefAssignedSUs'][0]:
                self.changePreferredAssignmentNums(SG, DN, -1)
            self.refresh(DN)
        else:
            print('Strategy not applicable to the SG\'s (%s) redundancy model (%s)' %(SG, SaAmfRedundancyModel[reduncancyModel]))
            


def allSIsOfSG(*SG_DN):
    SG_SI = {}
    for elem in pyImm.immom.getinstanceof('', 'SaAmfSIAssignment'):
        SG = 'safSg='+re.findall(r'(?<=safSg=)(.+)(?=,safSi)', elem)[0].replace('\\','')
        SI = 'safSi='+re.findall(r'(?<=safSi=)(.+)(?=)', elem)[0].replace('\\','')
        if SG in SG_SI:
            if SI not in SG_SI.get(SG):
                SG_SI[SG].append(SI)
        else:
            SG_SI[SG] = [SI]
    return SG_SI if len(SG_DN)==0 else SG_SI.get(SG_DN[0])

def getSUsAndNodesOfSI(*SI_DN):
    SI_SU_node = {}
    for elem in pyImm.immom.getinstanceof('', 'SaAmfSIAssignment'):
        SI = 'safSi='+re.findall(r'(?<=safSi=)(.+)(?=)', elem)[0].replace('\\','')
        SU = 'safSu='+re.findall(r'(?<=safSu=)(.+)(?=,safSi)', elem)[0].replace('\\','')
        node = chaindedQuery([SU, 'saAmfSUHostedByNode'])
        if SI in SI_SU_node:
            if (SU, node) not in SI_SU_node.get(SI):
                SI_SU_node[SI].append((SU, node))
        else:
            SI_SU_node[SI] = [(SU, node)]
    return SI_SU_node if len(SI_DN)==0 else SI_SU_node[SI_DN[0]]


            



if __name__ == '__main__':
    #print(chaindedQuery([sys.argv[1], 'saAmfSIProtectedbySG', 'saAmfSGType', 'saAmfSgtRedundancyModel']))
    #print(pyImm.immom.getchildobjects(sys.argv[1]))    
    #for elem in pyImm.immom.getinstanceof('', 'SaAmfSIAssignment'): print(elem)
    #print(chaindedQuery([sys.argv[1], 'saAmfSUHostedByNode']))
    pp = pprint.PrettyPrinter(indent=4)
    #pp.pprint(getSUsAndNodesOfSI(sys.argv[1]))

    op = osafMod()
    #op.mergeSIworkload(sys.argv[1])
    SUlistWithSIassignment = [elem[0] for elem in op.getSUsAndNodesOfSI(sys.argv[1])]
    print(SUlistWithSIassignment)
    
    '''
    ##########################
    print(\
    pyImm.immom.getattributes(\
        pyImm.immom.getattributes(\
            pyImm.immom.getattributes(\
                                        sys.argv[1]\
                                    )['saAmfSIProtectedbySG'][0]\
                                )['saAmfSGType'][0]\
                            )['saAmfSgtRedundancyModel'][0]\
        )
    ###########################
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
   
    pyImm.immombin.saImmOmInitialize()
    DN = 'opensafImm=opensafImm,safApp=safImmService'
    for (n,t,v) in pyImm.immombin.saImmOmAccessorGet(DN):
        if n == 'opensafImmClassNames':
            print(v)
    '''