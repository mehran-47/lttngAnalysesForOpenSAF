#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#ifndef __USE_ISOC99
#define __USE_ISOC99 // strtof and LLONG_MAX in older gcc versions like 4.3.2
#endif
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stdarg.h>
#include <syslog.h>
#include <errno.h>
#include <stdlib.h>
#include <saImm.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <sys/time.h>
#include <fcntl.h>
#include <ctype.h>
#include <libgen.h>
#include <signal.h>
#include <poll.h>
#include <saAis.h>
#include <saImmOm.h>
#include <saImmOi.h>
#define PARAMDELIM ":"
#define maxsize 200

static SaVersionT immVersion = { 'A', 2, 11 };
int ccb_safe = 1;
const SaImmCcbFlagsT defCcbFlags = SA_IMM_CCB_REGISTERED_OI | SA_IMM_CCB_ALLOW_NULL_OI;
SaImmScopeT scope = SA_IMM_SUBLEVEL;	/* default search scope */
SaImmScopeT scope1 = SA_IMM_SUBTREE;	/* default search scope */


void findcomponenttype(SaImmHandleT immHandle,char assclass[maxsize],SaNameT objectComp,char *ptr1)
{
 char *ptr;
 SaImmAttrNameT  attributeNames[2];
 SaImmAttrValuesT_2 **attributes;
 SaAisErrorT error;
 SaImmAccessorHandleT accessorHandle;
 ptr = strstr(assclass,"safComp");
   strncpy((char *)objectComp.value,ptr , SA_MAX_NAME_LENGTH);
 objectComp.length = strlen((char *)objectComp.value);



 attributeNames[0] ="saAmfCompType";
 attributeNames[1] =NULL;
 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectComp,attributeNames, &attributes);
                                        
                                        	
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		SaNameT *objtname = (SaNameT *)attrValues;
                		             	
							strncpy(ptr1, objtname->value, SA_MAX_NAME_LENGTH);
							
						}
				error = saImmOmAccessorFinalize(accessorHandle);
 				    }
 
}

void search(SaImmHandleT immHandle,SaNameT objectnameSI,char assclass[maxsize],char pointer1[SA_MAX_NAME_LENGTH])
{
 SaNameT object,objectComp;
 SaNameT rootName = { 0, "" };
 SaNameT objectNameSU;
 int foundsafSupported=0;
 //SaImmHandleT immHandle;
  SaImmSearchParametersT_2 searchParam;
 SaImmSearchHandleT searchHandle;
 int i,start=0;
 SaUint8T *ptr;
 char *ptr1;
 char Comptype[SA_MAX_NAME_LENGTH];
 SaAisErrorT error;
 ptr = strstr(objectnameSI.value,"safApp");
 char pointer[maxsize];
 SaImmAttrNameT  attributeNames[2];
 SaImmAttrValuesT_2 **attributes;
  strncpy((char *)rootName.value,ptr , SA_MAX_NAME_LENGTH);
 rootName.length = strlen((char *)rootName.value);

int n;
  searchParam.searchOneAttr.attrName = NULL;
  searchParam.searchOneAttr.attrValue = NULL;

	error = saImmOmSearchInitialize_2(immHandle, &rootName, scope1, SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR , &searchParam, NULL , &searchHandle);

        if (SA_AIS_OK == error) {
	
	do {
		error = saImmOmSearchNext_2(searchHandle, &object, &attributes);
		
		if (error == SA_AIS_OK)
                {
                        ptr1 =strstr(object.value,"safSupportedCsType");
                      if(ptr1 == NULL)
                      {  // printf("\n%s\n",object.value);
                      }
                      else
                      {   
				
                        strncpy(assclass,object.value,maxsize);

                        foundsafSupported=1;
 
                       }
            

                 }//serchNext

  	} while (error != SA_AIS_ERR_NOT_EXIST && foundsafSupported != 1);              
     
    }


error = saImmOmSearchFinalize(searchHandle);
findcomponenttype(immHandle,assclass, objectComp,ptr);

ptr1 = strstr(assclass,"safComp");
i = ptr1 - assclass;
//printf("%d",i);
strncpy(pointer,assclass,i);
start=0;
start+=i;
pointer[start]='\0';
ptr1 = strstr(pointer,"\\");

i = ptr1 - pointer;

strncpy(pointer1,assclass,i);

strcat(pointer1,"\\");

strcat(pointer1,ptr1);
strcat(pointer1,ptr);



start=0;

start = start + strlen(pointer1)+1;
pointer1[start]='\0';


}

int main()
{
 SaImmHandleT immHandle;
 SaNameT objectnameSI;
 SaAisErrorT error;
 strncpy((char *)objectnameSI.value,"safSi=AmfDemo,safApp=AmfDemo1", SA_MAX_NAME_LENGTH);
 int no_SUs,i;
 objectnameSI.length = strlen((char *)objectnameSI.value);
 char assclass[maxsize],pointer1[SA_MAX_NAME_LENGTH];
 
 error = saImmOmInitialize( &immHandle, NULL, &immVersion);

 search(immHandle,objectnameSI,assclass,pointer1); 
printf("\nAss>>>>>>>>>>>>>>>>>>>>>>>> %s\n",pointer1);
//create_assclass_string()

return 1;
}
