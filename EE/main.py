#/usr/bin/env python
from __future__ import print_function
import pyImm.immombin, pyImm.immom, pyImm.immomexamples, sys, re
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
            node = chaindedQuery([SU, 'saAmfSUHostedByNode'])
            if SI in SI_SU_node:
                if (SU, node) not in SI_SU_node.get(SI):
                    SI_SU_node[SI].append((SU, node))
            else:
                SI_SU_node[SI] = [(SU, node)]
        return SI_SU_node if len(SI_DN)==0 else SI_SU_node.get(SI_DN[0])

    def changePreferredAssignmentNums(self, SG, SI, number):
        numberOfCurrentAssignments = pyImm.immom.getattributes(SG)['saAmfSGNumCurrAssignedSUs'][0]
        attrList = [
            ('saAmfSIPrefActiveAssignments', 'SAUINT32T', [numberOfCurrentAssignments+number])
        ]
        self.modifyObject(SI, attrList)

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
            self.refresh(DN)
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

    def prioritizeLeastLoadedNode(self, DN, **kwargs):
        pass

    def mergeSIworkload(self, DN, **kwargs):
        reduncancyModel = self.chaindedQuery([DN, 'saAmfSIProtectedbySG', 'saAmfSGType', 'saAmfSgtRedundancyModel'])
        SG = self.chaindedQuery([DN, 'saAmfSIProtectedbySG'])
        if self.chaindedQuery([DN, 'saAmfSIPrefActiveAssignments']) >= 3:
            attrList = [
                ('saAmfSIPrefActiveAssignments', 'SAUINT32T', self.chaindedQuery([DN,'saAmfSIPrefActiveAssignments'])-1 )
            ]
            self.modifyObject(SI, attrList)
        else:
            pass



def act(SI, SInumber, workloadChangeType):
    op = operation('EE')
    op.spreadSIWorkload(SI, workloadChangeType)


if __name__=='__main__':
    command_output_map = { 'action': {  
                                        1: 'increase', 
                                        2: 'decrease'
                                      }, 
                           'SI_number': {
                                        1: 'single SI',
                                        2: 'multiple SIs'
                                        }
                         }
    if sys.argv[3:]:
        print('Received command to increase assignment of SI: %s, for %s %s\n\n' %(sys.argv[1],\
                                                                               command_output_map['SI_number'].get(int(sys.argv[2])),\
                                                                               command_output_map['action'].get(int(sys.argv[3])) ))
        SI = sys.argv[1]
        SG = pyImm.immom.getattributes(SI)['saAmfSIProtectedbySG'][0]
        #print('attributes of the SI: %s\n%s\n\n' %(SI, str(pyImm.immom.getattributes(SI)) ) )
        #print('attributes of the SG:%s protecting the SI: %s\n%s\n\n' %(SG, SI ,str(pyImm.immom.getattributes(SG)) ) )
        print('saAmfSGNumCurrInstantiatedSpareSUs :' + str(pyImm.immom.getattributes(SG)['saAmfSGNumCurrInstantiatedSpareSUs']))
        print('saAmfSGNumPrefAssignedSUs :' + str(pyImm.immom.getattributes(SG)['saAmfSGNumPrefAssignedSUs']))
        print('saAmfSGNumCurrAssignedSUs :' + str(pyImm.immom.getattributes(SG)['saAmfSGNumCurrAssignedSUs']))
        op = operation('EE')
        op.spreadSIWorkload(SI, int(sys.argv[3]))

    else:
        print("Invalid input")
        print('Command example:\n\
            <SI DN> <1 for single SI or 2 for multiple SIs> <1 for increase 2 for decrease>\n\
            e.g.: "python -m EE.main safSi=SI_1_NWayActiveHTTP,safApp=AppNWayActiveHTTP 1 2" signifies command: safSi=SI_1_NWayActiveHTTP,safApp=AppNWayActiveHTTP, single SI decrease')
