#!/usr/bin/env python
from immom import *
from immombin import *
import traceback
import sys
from subprocess import call
attrListCLMNode=[('saClmNodeLockCallbackTimeout','SATIMET',[50000000000]),('saClmNodeDisableReboot','SAUINT32T',[0])]

attrListAMFNode= [('saAmfNodeSuFailoverMax','SAUINT32T',[2]),('saAmfNodeSuFailOverProb','SATIMET',[1200000000000]),('saAmfNodeFailfastOnTerminationFailure','SAUINT32T',[0]),('saAmfNodeFailfastOnInstantiationFailure','SAUINT32T',[0]),('saAmfNodeClmNode','SANAMET',['safNode=compute4,safCluster=myClmCluster']),('saAmfNodeAutoRepair','SAUINT32T',[1])]

#attrListAllNodes=[('saAmfNGNodeList+=','SANAMET',['safAmfNode=SC-1,safAmfCluster=myAmfCluster','safAmfNode=SC-2,safAmfCluster=myAmfCluster','safAmfNode=PL-3,safAmfCluster=myAmfCluster','safAmfNode=PL-4,safAmfCluster=myAmfCluster','safAmfNode=PL-5,safAmfCluster=myAmfCluster','safAmfNode=PL-4,safAmfCluster=myAmfCluster'])]

#attrListAllNodes=[('saAmfNGNodeList','SANAMET',['safAmfNode=SC-1,safAmfCluster=myAmfCluster'])]

#attrListPLsNodes=[('saAmfNGNodeList','SANAMET',['safAmfNode=PL-3,safAmfCluster=myAmfCluster (42) safAmfNode=PL-4,safAmfCluster=myAmfCluster (42) safAmfNode=PL-5,safAmfCluster=myAmfCluster (42) safAmfNode=PL-4,safAmfCluster=myAmfCluster'])]

attrListSaAmfNodeSwBundleAmfDemo=[('saAmfNodeSwBundlePathPrefix','SASTRINGT',['/opt/amf_demo'])]

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

attrListSUAMFDemo=[('saAmfSUType','SANAMET',['safVersion=1,safSuType=AmfDemo1']),('saAmfSURank','SAUINT32T',[0]),('saAmfSUHostNodeOrNodeGroup','SANAMET',['safAmfNode=PL-4,safAmfCluster=myAmfCluster']),('saAmfSUFailover','SAUINT32T',[1]),('saAmfSUAdminState','SAUINT32T',[3])]
   # SU of AMF Demo has to be added in admin state=3

attrListCompAMFDemo=[('saAmfCompType','SANAMET',['safVersion=1,safCompType=AmfDemo1'])]

attrListCompCSIAMFDemo=[]

attrListSI = [('saAmfSIPrefActiveAssignments','SAUINT32T',[3])]



if __name__=='__main__':
    if sys.argv[1:]:
        
        print getchildobjects(sys.argv[1])
        numberSUs=len(getchildobjects(sys.argv[1]))
        print "SUs = ",numberSUs           
        for i in range(0,numberSUs):
            su=getchildobjects(sys.argv[1])[i]            
            #getchildobjects
            if getattributes(su)['saAmfSUPresenceState'][0]==3 and getattributes(su)['saAmfSUNumCurrActiveSIs'][0]==0:
             print su+'   saAmfSUPresenceState=',getattributes(su)['saAmfSUPresenceState'][0],' saAmfSUNumCurrActiveSIs=',getattributes(su)['saAmfSUNumCurrActiveSIs'][0]
             
             
             immombin.saImmOmAdminOwnerInitialize('yanal')
             print('initialized')
             #immombin.saImmOmAdminOwnerClear('SA_IMM_SUBTREE','safSg=AmfDemo,safApp=AmfDemo1')
             #print('cleared')
             
             immombin.saImmOmAdminOwnerSet('SA_IMM_SUBTREE',['safSi=AmfDemo,safApp=AmfDemo1'])
             print('set')
             try:
                 immombin.saImmOmCcbInitialize(0)
                 print('Ccbinitialized')
                 immombin.saImmOmCcbObjectModify('safSi=AmfDemo,safApp=AmfDemo1', attrListSI)
                 
                 print('CcbObject added')
                 immombin.saImmOmCcbApply()
                 print('CcbApply')
                 immombin.saImmOmCcbFinalize()
                 print('CcbModify')
                 #Clearing admin-ownership of the object and finalizing
                 immombin.saImmOmAdminOwnerFinalize()
                                 
                 break;
             except:
                 print('modification within object didn\'t work')
                 traceback.print_exc();
                 raise
            
        immombin.saImmOmAdminOwnerInitialize('yanal')
        try:
             
             print('second try')
             immombin.saImmOmCcbInitialize(0)
             print('Ccbinitialized')
             immombin.saImmOmAdminOwnerSet('SA_IMM_SUBTREE',['safSi=AmfDemo,safApp=AmfDemo1'])
             immombin.saImmOmCcbObjectModify('safSi=AmfDemo,safApp=AmfDemo1', attrListSI)

             createobject('safNode=compute4,safCluster=myClmCluster', 'SaClmNode',attrListCLMNode)
             print('safNode is added successfully')
             createobject('safAmfNode=PL-4,safAmfCluster=myAmfCluster', 'SaAmfNode',attrListAMFNode)
             print('safAmfNode is added successfully')

             #immombin.saImmOmAdminOwnerSet('SA_IMM_SUBTREE',['safAmfNodeGroup=AllNodes,safAmfCluster=myAmfCluster'])
             #immombin.saImmOmCcbObjectModify('safAmfNodeGroup=AllNodes,safAmfCluster=myAmfCluster', attrListAllNodes)
             call(['immcfg','-a','saAmfNGNodeList+=safAmfNode=PL-4,safAmfCluster=myAmfCluster','safAmfNodeGroup=AllNodes,safAmfCluster=myAmfCluster'])
             print('after modify safAmfNodeGroup=AllNodes successfully')
             call(['immcfg','-a','saAmfNGNodeList+=safAmfNode=PL-4,safAmfCluster=myAmfCluster','safAmfNodeGroup=PLs,safAmfCluster=myAmfCluster'])
             print('after modify safAmfNodeGroup=PLs successfully')
                 
             createobject('safInstalledSwBundle=safSmfBundle=AmfDemo1,safAmfNode=PL-4,safAmfCluster=myAmfCluster', 'SaAmfNodeSwBundle',attrListSaAmfNodeSwBundleAmfDemo)
             print('SaAmfNodeSwBundle AMFDemo is added successfully')

             createobject('safInstalledSwBundle=safSmfBundle=OpenSAF,safAmfNode=PL-4,safAmfCluster=myAmfCluster', 'SaAmfNodeSwBundle',attrListSaAmfNodeSwBundle)
             print('SaAmfNodeSwBundle OpenSAF is added successfully')

             createobject('safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfSU',attrListSUPL)
             print('safSu=PL-4 is added successfully')

             createobject('safComp=AMFWDOG,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfComp',attrListCompWDOG)
             print('safComp=AMFWDOG is added successfully')

             createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=AMFWDOG-OpenSAF,safComp=AMFWDOG,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfCompCsType',attrListCompCSIWDOG)
             print('safCSType=AMFWDOG-OpenSAF is added successfully')

             createobject('safComp=CPND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfComp',attrListCompCPND)
             print('safComp=CPND is added successfully')

             createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=CPND-OpenSAF,safComp=CPND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfCompCsType',attrListCompCSICPND)
             print('safCSType=CPND-OpenSAF is added successfully')

             createobject('safComp=GLND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfComp',attrListCompGLND)
             print('safComp=GLND is added successfully')


             createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=GLND-OpenSAF,safComp=GLND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfCompCsType',attrListCompCSIGLND)
             print('safCSType=GLND-OpenSAF is added successfully')

             createobject('safComp=IMMND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfComp',attrListCompIMMND)
             print('safComp=IMMND is added successfully')

             createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=IMMND-OpenSAF,safComp=IMMND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfCompCsType',attrListCompCSIIMMND)
             print('safCSType=IMMND-OpenSAF is added successfully')

             createobject('safComp=MQND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfComp',attrListCompMQND)
             print('safComp=MQND is added successfully')

             createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=MQND-OpenSAF,safComp=MQND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfCompCsType',attrListCompCSIMQND)
             print('safCSType=MQND-OpenSAF is added successfully')

             createobject('safComp=SMFND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfComp',attrListCompSMFND)
             print('safComp=SMFND is added successfully')

             createobject('safSupportedCsType=safVersion=4.0.0\,safCSType=SMFND-OpenSAF,safComp=SMFND,safSu=PL-4,safSg=NoRed,safApp=OpenSAF', 'SaAmfCompCsType',attrListCompCSISMFND)
             print('safCSType=SMFND-OpenSAF')

             
             createobject('safSu=SU4,safSg=AmfDemo,safApp=AmfDemo1', 'SaAmfSU',attrListSUAMFDemo)
             print('safSu=SU4,safSg=AmfDemo,safApp=AmfDemo1 is added successfully')

             createobject('safComp=AmfDemo,safSu=SU4,safSg=AmfDemo,safApp=AmfDemo1', 'SaAmfComp',attrListCompAMFDemo)
             print('safComp=AmfDemo,safSu=SU4,safSg=AmfDemo,safApp=AmfDemo1 is added successfully')

             createobject('safSupportedCsType=safVersion=1\,safCSType=AmfDemo1,safComp=AmfDemo,safSu=SU4,safSg=AmfDemo,safApp=AmfDemo1', 'SaAmfCompCsType',attrListCompCSIAMFDemo)
             print('safCSType=AmfDemo1 is added successfully')
            #after this just run compute4 and then start opensaf on it. The VM will join the cluster


             immombin.saImmOmCcbApply()
                
             immombin.saImmOmCcbFinalize()
             print('CcbModify')
                 
             immombin.saImmOmAdminOwnerFinalize()
                                 
        except:
             print('modification within object didn\'t work')
             traceback.print_exc();
             raise


                 