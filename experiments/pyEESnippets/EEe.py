#!/usr/bin/env python

from __future__ import print_function
import pyImm.immombin, pyImm.immom, pyImm.immomexamples, sys, re, pprint
from subprocess import call


SaAmfRedundancyModel = ['', '2N', 'NPM', 'N_WAY', 'N_WAY_ACTIVE', 'NO_REDUNDANCY_MODEL']

class operation(object):
    def __init__(self, *admin):
        pyImm.immombin.saImmOmFinalize()
        pyImm.immombin.saImmOmInitialize()
        if admin[0:]:
            #Initializing the admin of the DNs. Can't change a thing without setting the admin.
            self.admin = admin[0]
            pyImm.immombin.saImmOmAdminOwnerInitialize(self.admin)
        else:
            pyImm.immombin.saImmOmAdminOwnerInitialize('elasticity_engine')

    def chaindedQuery(self, queryChain):
        cq = lambda DN, attribute: pyImm.immom.getattributes(DN)[attribute][0]
        return reduce(cq, queryChain)

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
            node = self.chaindedQuery([SU, 'saAmfSUHostedByNode'])
            if SI in SI_SU_node:
                if (SU, node) not in SI_SU_node.get(SI):
                    SI_SU_node[SI].append((SU, node))
            else:
                SI_SU_node[SI] = [(SU, node)]
        return SI_SU_node if len(SI_DN)==0 else SI_SU_node.get(SI_DN[0])

    def getHostedSUsFromNode(self, nodeDN, *HAState):
        SUNodeTuplesWithAllAssignments = []
        for aTuplesList in op.getSUsAndNodesOfSI().values():
            for aTuple in aTuplesList:
                if aTuple[1]==nodeDN:
                    SUNodeTuplesWithAllAssignments.append(aTuple)
        if len(HAState)==0:
            return [SU_node[0] for SU_node in SUNodeTuplesWithAllAssignments]
        elif len(HAState)>0 and HAState[0]=='active':
            return [SU_node[0] for SU_node in SUNodeTuplesWithAllAssignments if self.chaindedQuery([SU_node[0], 'saAmfSUNumCurrActiveSIs'])>0 ]
        elif len(HAState)>0 and HAState[0]=='standby':
            return [SU_node[0] for SU_node in SUNodeTuplesWithAllAssignments if self.chaindedQuery([SU_node[0], 'saAmfSUNumCurrStandbySIs'])>0 ]


    def changePreferredAssignmentNums(self, SG, SI, number):
        numberOfCurrentAssignments = pyImm.immom.getattributes(SG)['saAmfSGNumCurrAssignedSUs'][0]
        attrList = [
            ('saAmfSIPrefActiveAssignments', 'SAUINT32T', [numberOfCurrentAssignments+number])
        ]
        if numberOfCurrentAssignments<4: 
            self.modifyObject(SI, attrList)
        else:
            print('Add more nodes')

    def refresh(self, DN, **kwargs):
        call(['amf-adm','lock', DN])
        call(['amf-adm','unlock', DN])
        if kwargs.get('show'): call(['service','opensafd','status'])

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
                #call buffer manager
            self.refresh(DN, show=True)
        else:
            print('Strategy not applicable to the SG\'s (%s) redundancy model (%s)' %(SG, SaAmfRedundancyModel[reduncancyModel]))

    def mergeSIworkload(self, DN, **kwargs):
        reduncancyModel = self.chaindedQuery([DN, 'saAmfSIProtectedbySG', 'saAmfSGType', 'saAmfSgtRedundancyModel'])
        SG = self.chaindedQuery([DN, 'saAmfSIProtectedbySG'])
        if self.chaindedQuery([DN, 'saAmfSIPrefActiveAssignments']) >= 3:
            attrList = [
                ('saAmfSIPrefActiveAssignments', 'SAUINT32T', [self.chaindedQuery([DN,'saAmfSIPrefActiveAssignments'])-1 ] )
            ]
            self.modifyObject(DN, attrList)
            self.refresh(DN, show=True)            
        elif (self.chaindedQuery([SG,'saAmfSGNumPrefAssignedSUs'])-1)*self.chaindedQuery([SG,'saAmfSGMaxActiveSIsperSU']) >= len(self.getSIsOfSG(SG)):
            for SI in self.getSIsOfSG(SG):
                if self.chaindedQuery([SG,'saAmfSGNumPrefAssignedSUs'])-1 >=  self.chaindedQuery([SI, 'saAmfSIPrefActiveAssignments']):
                    saAmfSGNumPrefAssignedSUs
                    attrList = [
                        ('saAmfSGNumPrefAssignedSUs', 'SAUINT32T', [self.chaindedQuery([SG,'saAmfSGNumPrefAssignedSUs'])-1 ] )
                    ]
                    self.modifyObject(SG, attrList)                    
                else:
                    print('Reached minimum configuration')
        else:
            pass
            #buffer manager

    def prioritizeLeastLoadedNode(self, DN, **kwargs):
        reduncancyModel = self.chaindedQuery([DN, 'saAmfSIProtectedbySG', 'saAmfSGType', 'saAmfSgtRedundancyModel'])
        SG = self.chaindedQuery([DN, 'saAmfSIProtectedbySG'])
        #The SU-Node tuple with the active assignment for the provided DN (SI DN)
        SUNodeTupleWithActiveSIAssignment = [SU_node_active for SU_node_active in self.getSUsAndNodesOfSI(DN) if self.chaindedQuery([SU_node_active[0],'saAmfSUNumCurrActiveSIs'])>0][0]
        #The SU-Node tuples with the active assignments for the provided DN (SI DN) and other SIs
        ActiveFinal = len(self.getHostedSUsFromNode(SUNodeTupleWithActiveSIAssignment, 'active'))
        StandbyFinal = len(self.getHostedSUsFromNode(SUNodeTupleWithActiveSIAssignment, 'standby'))



if __name__ == '__main__':
    op = operation('EE')
    pp = pprint.PrettyPrinter(indent=4)
    #op.mergeSIworkload(sys.argv[1])
    op.spreadSIWorkload(sys.argv[1], 1)
    #pp.pprint([SU_node[0] for SU_node in op.getSUsAndNodesOfSI(sys.argv[1]) if op.chaindedQuery([SU_node[0],'saAmfSUNumCurrActiveSIs'])>0])
    #SUNodeTupleWithActiveSIAssignment = [SU_node_active for SU_node_active in op.getSUsAndNodesOfSI(sys.argv[1]) if op.chaindedQuery([SU_node_active[0],'saAmfSUNumCurrActiveSIs'])>0][0]
    #for elem in pyImm.immom.getinstanceof('', 'SaAmfSIAssignment'): pp.pprint(elem)
    '''
    print('all')
    pp.pprint(op.getHostedSUsFromNode('safAmfNode=SC-2,safAmfCluster=myAmfCluster'))
    print('active')
    pp.pprint(op.getHostedSUsFromNode('safAmfNode=SC-2,safAmfCluster=myAmfCluster', 'active'))
    print('standby')
    pp.pprint(op.getHostedSUsFromNode('safAmfNode=SC-2,safAmfCluster=myAmfCluster', 'standby'))
    '''
    #SUNodeListWithActiveAssignmentFromAnySI = [SU_node_active for SU_node_active in op.getSUsAndNodesOfSI() if op.chaindedQuery([SU_node_active[0],'saAmfSUNumCurrActiveSIs'])>0 ]
    #pp.pprint(SUNodeTupleWithActiveSIAssignment)
    #pp.pprint(op.getSUsAndNodesOfSI())