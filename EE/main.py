#/usr/bin/env python
from __future__ import print_function
import pyImm.immombin, pyImm.immom, pyImm.immomexamples, sys, re, time
from subprocess import call


SaAmfRedundancyModel = ['', '2N', 'NPM', 'N_WAY', 'N_WAY_ACTIVE', 'NO_REDUNDANCY_MODEL']

class operation(object):
    def __init__(self, *admin):
        pyImm.immombin.saImmOmFinalize()
        pyImm.immombin.saImmOmInitialize()
        if admin[0:]:
            #Initializing the admin of the DNs. Can't change a thing without setting the admin.
            self.admin = admin[0]
        else:
            self.admin='elasticity_engine'
        pyImm.immombin.saImmOmAdminOwnerInitialize(self.admin)

    def getadminowner(self, dn):
        """Returns the adminowner of an object or '' if there are no adminowner.
        """
        for (n,t,v) in pyImm.immom.getobject(dn):
            if n == 'SaImmAttrAdminOwnerName':
                if v:
                   return v[0]
                return ''

    def chaindedQuery(self, queryChain):
        cq = lambda DN, attribute: pyImm.immom.getattributes(DN)[attribute][0]
        return reduce(cq, queryChain)

    def modifyObject(self, DN, attrList):
        try:
            if self.getadminowner(DN)!=self.admin:
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
            ##pyImm.immombin.saImmOmAdminOwnerFinalize()
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
            currentInserviceBuff = int(self.chaindedQuery(['safBuff='+SG, 'saInserviceBuff']))
            if flag==1 and pyImm.immom.getattributes(SG)['saAmfSGNumCurrInstantiatedSpareSUs'][0] > 0:
                self.changePreferredAssignmentNums(SG, DN, 1)
                attrList = [
                        ('saInserviceBuff', 'SAUINT32T', [currentInserviceBuff-1])
                    ]
                self.modifyObject('safBuff='+SG, attrList)
            elif flag==2 and pyImm.immom.getattributes(SG)['saAmfSGNumCurrAssignedSUs'][0] > pyImm.immom.getattributes(SG)['saAmfSGNumPrefAssignedSUs'][0]:
                self.changePreferredAssignmentNums(SG, DN, -1)
                attrList = [
                        ('saInserviceBuff', 'SAUINT32T', [currentInserviceBuff+1])
                    ]
            self.refresh(DN)
            self.modifyObject('safBuff='+SG, attrList)
            self.checkAndAdjustNodesFor(SG)
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

    def checkAndAdjustNodesFor(self, SG_DN):
        if int(op.chaindedQuery(['safBuff='+SG_DN, 'saInserviceBuff']))==0:
            print('bringing up new node')
            attrListSaAmfNodeSwBundleHTTP=[('saAmfNodeSwBundlePathPrefix','SASTRINGT',['/opt/httpComponent'])]
            attrListSaAmfNodeSwBundle=[('saAmfNodeSwBundlePathPrefix','SASTRINGT',['/usr/local/lib/opensaf/clc-cli'])]
            #for adding payload
            attrListSUPL=[('saAmfSUType','SANAMET',['safVersion=4.0.0,safSuType=OpenSafSuTypeND']),('saAmfSURank','SAUINT32T',[0]),('saAmfSUHostNodeOrNodeGroup','SANAMET',['safAmfNode=PL-4,safAmfCluster=myAmfCluster']),('saAmfSUFailover','SAUINT32T',[1]),('saAmfSUAdminState','SAUINT32T',[1])]
            attrListCompWDOG=[('saAmfCompType','SANAMET',['safVersion=4.0.0,safCompType=OpenSafCompTypeAMFWDOG'])]
            attrListCompCSIWDOG=[]
            attrListCompCPND=[('saAmfCompType','SANAMET',['safVersion=4.0.0,safCompType=OpenSafCompTypeCPND'])]
            attrListCompCSICPND=[]
            attrListCompGLND=[('saAmfCompType','SANAMET',['safVersion=4.0.0,safCompType=OpenSafCompTypeGLND'])]
            attrListCompCSIGLND=[]
            attrListCompIMMND=[('saAmfCompType','SANAMET',['safVersion=4.0.0,safCompType=OpenSafCompTypeIMMND'])]
            attrListCompCSIIMMND=[]
            attrListCompMQND=[('saAmfCompType','SANAMET',['safVersion=4.0.0,safCompType=OpenSafCompTypeMQND'])]
            attrListCompCSIMQND=[]
            attrListCompSMFND=[('saAmfCompType','SANAMET',['safVersion=4.0.0,safCompType=OpenSafCompTypeSMFND'])]
            attrListCompCSISMFND=[]
            attrListCompCSISMFND=[]
            attrListSUHTTP=[('saAmfSUType','SANAMET',['safVersion=4.0.0,safSuType=SUBaseTypeForNWayActiveHTTP']),('saAmfSURank','SAUINT32T',[0]),('saAmfSUHostNodeOrNodeGroup','SANAMET',['safAmfNode=PL-4,safAmfCluster=myAmfCluster']),('saAmfSUFailover','SAUINT32T',[1]),('saAmfSUAdminState','SAUINT32T',[3])]
            attrListCompHTTP=[('saAmfCompType','SANAMET',['safVersion=4.0.0,safCompType=CompBaseTypeForNWayActiveHTTP']),('saAmfCompInstantiateCmdArgv','SASTRINGT',['8080'])]
            attrListCompCSIHTTP=[]
            ##creating objects in the configuration
            #creating CLM node object
            try:
		pyImm.immombin.saImmOmCcbInitialize(0)
                attrListCLMNode=[('saClmNodeLockCallbackTimeout','SATIMET',[50000000000]),('saClmNodeDisableReboot','SAUINT32T',[0])]
                pyImm.immom.createobject('safNode=node4,safCluster=myClmCluster', 'SaClmNode', attrListCLMNode)
                #creating AMF node object
                call(['immcfg','-a','saAmfNGNodeList+=safAmfNode=PL-4,safAmfCluster=myAmfCluster','safAmfNodeGroup=AllNodes,safAmfCluster=myAmfCluster'])
		call(['immcfg','-a','saAmfNGNodeList+=safAmfNode=PL-4,safAmfCluster=myAmfCluster','safAmfNodeGroup=PLs,safAmfCluster=myAmfCluster'])
		attrListAMFNode= [('saAmfNodeSuFailoverMax','SAUINT32T',[2]),('saAmfNodeSuFailOverProb','SATIMET',[1200000000000]),('saAmfNodeFailfastOnTerminationFailure','SAUINT32T',[0]),('saAmfNodeFailfastOnInstantiationFailure','SAUINT32T',[0]),('saAmfNodeClmNode','SANAMET',['safNode=node4,safCluster=myClmCluster']),('saAmfNodeAutoRepair','SAUINT32T',[1])]
                pyImm.immom.createobject('safAmfNode=PL-4,safAmfCluster=myAmfCluster', 'SaAmfNode',attrListAMFNode)
                pyImm.immom.createobject('safInstalledSwBundle=safSmfBundle=SmfBundleNWayActiveHTTP,safAmfNode=PL-4,safAmfCluster=myAmfCluster', 'SaAmfNodeSwBundle',attrListSaAmfNodeSwBundleHTTP)
                pyImm.immom.createobject('safInstalledSwBundle=safSmfBundle=OpenSAF,safAmfNode=PL-4,safAmfCluster=myAmfCluster', 'SaAmfNodeSwBundle',attrListSaAmfNodeSwBundle)
                pyImm.immom.createobject('safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfSU',attrListSUPL)
                pyImm.immom.createobject('safComp=AMFWDOG,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfComp',attrListCompWDOG)
                print('safComp=AMFWDOG is added successfully')
                pyImm.immom.createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=AMFWDOG-OpenSAF,safComp=AMFWDOG,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfCompCsType',attrListCompCSIWDOG)
                print('safCSType=AMFWDOG-OpenSAF is added successfully')
                pyImm.immom.createobject('safComp=CPND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfComp',attrListCompCPND)
                print('safComp=CPND is added successfully')
                pyImm.immom.createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=CPND-OpenSAF,safComp=CPND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfCompCsType',attrListCompCSICPND)
                print('safCSType=CPND-OpenSAF is added successfully')
                pyImm.immom.createobject('safComp=GLND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfComp',attrListCompGLND)
                print('safComp=GLND is added successfully')           
                pyImm.immom.createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=GLND-OpenSAF,safComp=GLND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfCompCsType',attrListCompCSIGLND)
                print('safCSType=GLND-OpenSAF is added successfully')
                pyImm.immom.createobject('safComp=IMMND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfComp',attrListCompIMMND)
                pyImm.immom.createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=IMMND-OpenSAF,safComp=IMMND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfCompCsType',attrListCompCSIIMMND)
                pyImm.immom.createobject('safComp=MQND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfComp',attrListCompMQND)
                pyImm.immom.createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=MQND-OpenSAF,safComp=MQND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfCompCsType',attrListCompCSIMQND)
                pyImm.immom.createobject('safComp=SMFND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfComp',attrListCompSMFND)
                pyImm.immom.createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=SMFND-OpenSAF,safComp=SMFND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfCompCsType',attrListCompCSISMFND)
                pyImm.immom.createobject('safSu=SU_4_NWayActiveHTTP,safSg=SGNWayActiveHTTP,safApp=AppNWayActiveHTTP', 'SaAmfSU', attrListSUHTTP)
                pyImm.immom.createobject('safComp=comp_1_NWayActiveHTTP,safSu=SU_4_NWayActiveHTTP,safSg=SGNWayActiveHTTP,safApp=AppNWayActiveHTTP', 'SaAmfComp', attrListCompHTTP)
                pyImm.immom.createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=CSBaseTypeNWayActiveHTTP,safComp=comp_1_NWayActiveHTTP,safSu=SU_4_NWayActiveHTTP,safSg=SGNWayActiveHTTP,safApp=AppNWayActiveHTTP', 'SaAmfCompCsType',attrListCompCSIHTTP)
		pyImm.immombin.saImmOmCcbApply()
                pyImm.immombin.saImmOmCcbFinalize()
             	#pyImm.immombin.saImmOmAdminOwnerFinalize()
            except:
                print('modification within object didn\'t work')
                raise
            re_check = True
            while re_check:
                try:
                    nodeState = self.chaindedQuery(['safAmfNode=PL-4,safAmfCluster=myAmfCluster', 'saAmfNodeOperState'])
                    re_check = False
                except IndexError:
                    print('Waiting for node to join', end='\r')
                    re_check = True
                    time.sleep(5)
            if nodeState==1:
                print('Node joined cluster successfully')
                currentInserviceBuff = int(self.chaindedQuery(['safBuff='+SG, 'saInserviceBuff']))
                currentSpareBuff = int(self.chaindedQuery(['safBuff='+SG, 'saSpareSUBuff']))
                attrList = [('saInserviceBuff','SAUINT32T',[currentInserviceBuff+1])]
                self.modifyObject('safBuff='+SG, attrList)
                attrList = [('saSpareSUBuff','SAUINT32T',[currentSpareBuff-1])]
                self.modifyObject('safBuff='+SG, attrList)
            else:
                print('Node join failed')
            pyImm.immombin.saImmOmAdminOwnerFinalize()



def act(SI, SInumber, workloadChangeType):
    op = operation('EE')
    op.spreadSIWorkload(SI, workloadChangeType)



if __name__=='__main__':
    #op = operation('EE')
    #print(op.chaindedQuery(['safBuff=safSg=SGNWayActiveHTTP,safApp=AppNWayActiveHTTP', 'saInserviceBuff']))
    #attrList = [
    #                    ('saInserviceBuff', 'SAUINT32T', [3])
    #                ]
    #print("%r, %r" %('safBuff=safSg=SGNWayActiveHTTP,safApp=AppNWayActiveHTTP', attrList))
    #op.modifyObject('safBuff=safSg=SGNWayActiveHTTP,safApp=AppNWayActiveHTTP', attrList) 
    #'''
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
    #'''
