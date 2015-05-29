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
#define maxsize 100

static SaVersionT immVersion = { 'A', 2, 11 };
int ccb_safe = 1;
const SaImmCcbFlagsT defCcbFlags = SA_IMM_CCB_REGISTERED_OI | SA_IMM_CCB_ALLOW_NULL_OI;
SaImmScopeT scope = SA_IMM_SUBLEVEL;	/* default search scope */
SaImmScopeT scope1 = SA_IMM_SUBTREE;	/* default search scope */


typedef struct SGSU{
SaNameT SUName;
int CurrentActiveSIs;
int CurrentStandbySIs;
int rank;
}SGSU;


int check_otherSUs_on_node(SaImmHandleT immhandle,SaNameT objectname,int i,SGSU listSGSU[])
{
// printf("\n I am gonna fo it");
 SaAisErrorT error;
 SaImmSearchHandleT searchHandle;
 SaNameT objectnodename,objectname1,objectNameSU;
 SaImmSearchParametersT_2 searchParam;
 searchParam.searchOneAttr.attrName = NULL;
 searchParam.searchOneAttr.attrValue = NULL;
 SaImmAccessorHandleT accessorHandle,accessorHandle1;
 SaImmAttrValuesT_2 **attributes,**attributes1,**attributes2;
 SaImmAttrNameT  attributeNames[2];
 int ch;
 int rc = EXIT_SUCCESS,activeSis=0,standbySis=0,foundsu=0,activeSisSum=0,standbySisSum=0;

								attributeNames[0] ="saAmfSUHostedByNode";
								attributeNames[1] =NULL;
								error = saImmOmAccessorInitialize(immhandle, &accessorHandle);
 			       					if (error != SA_AIS_OK) {
                	       						printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       						rc= EXIT_FAILURE;
								
                      						}
       			       					if (error == SA_AIS_OK)
   								{
                                 				error= saImmOmAccessorGet_2(accessorHandle, &objectname, attributeNames, &attributes);
                                        				if (error !=SA_AIS_OK)
                                        				{
                                        	       				printf("\nerror:%d",error);
										rc= EXIT_FAILURE;
                                        		        	     
                                        				}
                        	 					if (error ==SA_AIS_OK)
                                 					{
							     			SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
                        							 SaNameT *objtnamevalidsutype = (SaNameT *)attrValues;
 										//printf("The SU is hosted on node  %s ", objtnamevalidsutype->value);
 										strncpy((char *)objectnodename.value, objtnamevalidsutype->value, SA_MAX_NAME_LENGTH);
										objectnodename.length= strlen(objectnodename.value);  

                                                                                       searchParam.searchOneAttr.attrName = "saAmfSUHostedByNode";
 			    							       searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 			    							       searchParam.searchOneAttr.attrValue = &objectnodename;
				
				
			    error = saImmOmSearchInitialize_2(immhandle, NULL, scope1,SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
			    if (SA_AIS_OK != error) {
			   
                	    printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	    rc=EXIT_FAILURE;
                	
			     }
                            if (SA_AIS_OK == error)
        		    {
				do {                
                   			error = saImmOmSearchNext_2(searchHandle, &objectNameSU, &attributes);
					if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                     
					printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			  rc=EXIT_FAILURE;
					
					}
                                        


                                        if(error == SA_AIS_ERR_NOT_EXIST )
               				{
		  	  		  			
                             				rc=EXIT_FAILURE;
                             				
 		          			    
					}
                
					if (error == SA_AIS_OK)
                  			{
                                                  //  printf("\n THe su on this node is %s",objectNameSU.value);
                                                ch = strcmp(objectname.value,objectNameSU.value);
                                                if(ch ==0)
                                                {
                                                }
                                                else
                                                {
						   attributeNames[0] ="saAmfSUNumCurrActiveSIs";
						   attributeNames[1] =NULL;
						   error = saImmOmAccessorInitialize(immhandle, &accessorHandle1);
 			       			   if (error != SA_AIS_OK) {
                	       					printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       					rc= EXIT_FAILURE;
								
                      				   }
       			       			   if (error == SA_AIS_OK)
   						   {
							error= saImmOmAccessorGet_2(accessorHandle1, &objectNameSU, attributeNames, &attributes);
                                        		if (error !=SA_AIS_OK)
                                        		{
                                               			printf("\nerror:%d",error);
								rc= EXIT_FAILURE;
                                                        	
                                        		}
                        	 			if (error ==SA_AIS_OK)
                                 			{
			                     		
					     			SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    			activeSis = *((SaUint32T *)attrValues);
                                                                activeSisSum = activeSisSum + activeSis;
                                                               // printf("Active assignments :%d\n",activeSis);
                		   	               	 	error = saImmOmAccessorFinalize(accessorHandle1);
							}//if not standby
       			
                        		           }//accesor initialize
                                               	   attributeNames[0] ="saAmfSUNumCurrStandbySIs";
						   attributeNames[1] =NULL;
						   error = saImmOmAccessorInitialize(immhandle, &accessorHandle1);
 			       			   if (error != SA_AIS_OK) {
                	       					printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       					rc= EXIT_FAILURE;
								
                      				   }
       			       			   if (error == SA_AIS_OK)
   						   {
							error= saImmOmAccessorGet_2(accessorHandle1, &objectNameSU, attributeNames, &attributes);
                                        		if (error !=SA_AIS_OK)
                                        		{
                                               			printf("\nerror:%d",error);
								rc= EXIT_FAILURE;
                                                        	
                                        		}
                        	 			if (error ==SA_AIS_OK)
                                 			{
			                     		
					     			SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    			standbySis = *((SaUint32T *)attrValues);
                                                                standbySisSum = standbySisSum + standbySis;
                                                                //printf("\nthe end\n",activeSis);
                                                               // printf("Standby Assignments :%d\n",standbySis);
                		   	               	 	error = saImmOmAccessorFinalize(accessorHandle1);
							}//if accessor get
       			
                        		           }//accesor initialize
                                                }//if(ch == 0)
                                          
					         }	
					    
                                      
                                    }while (error != SA_AIS_ERR_NOT_EXIST);
                               	
                                                                                 /////////////////////////////////// 
                                }                                                        		        		     			
			}//accesor get
						
					
									error = saImmOmAccessorFinalize(accessorHandle);
                                                		}
//printf("\n the active SIs sum is %d and %d", activeSisSum ,standbySisSum);
listSGSU[i].CurrentActiveSIs = activeSisSum;
listSGSU[i].CurrentStandbySIs = standbySisSum;
listSGSU[i].rank =0;   						
 
	
return 0;
}
int create_listSGSU(SaImmHandleT immHandle,SaNameT objectnameSG1,SGSU listSGSU[])
{

  //printf("\n%s",objectnameSG1.value);
  SaImmSearchHandleT searchHandle;
  SaNameT rootName = { 0, "" };
  SaImmAttrNameT  attributeNames[1];
  strncpy((char *)rootName.value, objectnameSG1.value, SA_MAX_NAME_LENGTH);
  rootName.length = strlen((char *)rootName.value);
  int i=0;
  SaImmSearchParametersT_2 searchParam;
  
 SaImmAccessorHandleT accessorHandle;
 int rc = EXIT_SUCCESS;

char string[100],string2[100],string3[100];

 char ch1;
 char *buff;
 SaAisErrorT error;
 SaNameT objectNames,objectName;
 SaImmAttrValuesT_2 **attributes,**attributes1;
 char *s3;
 int long ch;
 

 searchParam.searchOneAttr.attrName = NULL;
 searchParam.searchOneAttr.attrValue = NULL;

error = saImmOmSearchInitialize_2(immHandle, &rootName, scope1, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);

//printf("\n3");

	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}

if (SA_AIS_OK == error) {
	
	do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                {
                                  buff = objectName.value;
                                  s3 =strstr(buff,"safSu=");
                             
              
                      if(s3 == NULL)
                      {
                         //  not an su object
                      }
                      else
                      {  
                          ch =buff - s3;
                          if(ch == 0)
                        {
			   //printf("\n **SU** : %s\n ", objectName.value);
                           strncpy((char *)listSGSU[i].SUName.value, objectName.value, SA_MAX_NAME_LENGTH);
                           listSGSU[i].SUName.length = strlen((char *)listSGSU[i].SUName.value);
                           check_otherSUs_on_node(immHandle,objectName,i,listSGSU);
 			   i++;
                          
			
                      
                        }
                        
                      }
                }

            } while (error != SA_AIS_ERR_NOT_EXIST);
error = saImmOmSearchFinalize(searchHandle);

}   
return i;                    

}

int check_array_rankedSUs(SaNameT array_rankedSUs[20],char name[SA_MAX_NAME_LENGTH],int count)
{
  int i=0,ch;
if(count == 0)
{
 return 0;
}
//printf("\n%s\n%s",name,array_rankedSUs[i][SA_MAX_NAME_LENGTH]);
 
 while(i<count)
  {

    ch = strncmp(name,array_rankedSUs[i].value,SA_MAX_NAME_LENGTH);

     if(ch == 0)
     {
        return 1; 
     }

   i++;

  }

 return 0;
  
}


void change_rankSI_increase(SaImmHandleT immHandle,SGSU listSGSU[],int no_SUs)
{
  //char* array_rankedSUs[no_SUs][SA_MAX_NAME_LENGTH];
SaNameT array_rankedSUs[20];   
int rank=0,count=0,ch,i;
  SGSU temp;
  while (rank <= no_SUs )
  {
     temp.CurrentActiveSIs=100;
     temp.CurrentStandbySIs=100;
     for(i=0;i <= no_SUs;i++)
     {
ch = check_array_rankedSUs(array_rankedSUs,listSGSU[i].SUName.value,count);

      if (ch == 0)
      {

       if(listSGSU[i].CurrentActiveSIs < temp.CurrentActiveSIs)
       {
     
          strncpy((char *)temp.SUName.value, listSGSU[i].SUName.value, SA_MAX_NAME_LENGTH);
          temp.SUName.length = strlen((char *)temp.SUName.value);
          temp.CurrentActiveSIs = listSGSU[i].CurrentActiveSIs;
          temp.CurrentStandbySIs =listSGSU[i].CurrentStandbySIs;
          temp.rank=rank;
       
       
       }
       else
       {
           if(listSGSU[i].CurrentActiveSIs == temp.CurrentActiveSIs)
           {
              if(listSGSU[i].CurrentStandbySIs <= temp.CurrentStandbySIs) 
              {
                   strncpy((char *)temp.SUName.value, listSGSU[i].SUName.value, SA_MAX_NAME_LENGTH);
          	   temp.SUName.length = strlen((char *)temp.SUName.value);
          	   temp.CurrentActiveSIs = listSGSU[i].CurrentActiveSIs;
          	   temp.CurrentStandbySIs =listSGSU[i].CurrentStandbySIs;
                   temp.rank=rank;

              }
                 
           }
        }
       }//if ch 

     }//for

/////create ccb to change rank
printf("\nAdd CCB to change %s rank to %d",temp.SUName.value,temp.rank);

//change_rank(immHandle,objectName,SUrank,argv);

i=0;
while(i<no_SUs)
{
 ch=strncmp(temp.SUName.value,listSGSU[i].SUName.value,SA_MAX_NAME_LENGTH);
if(ch==0)
{
 
                   listSGSU[i].rank=temp.rank;
                   break;
} 
i++;
}

strncpy(array_rankedSUs[count].value,temp.SUName.value,SA_MAX_NAME_LENGTH);
array_rankedSUs[count].length = strlen((char *)array_rankedSUs[count].value);
count++;
rank++;

    
  }


}

void change_rankSI_decrease(SaImmHandleT immHandle,SGSU listSGSU[],int no_SUs)
{
 
SaNameT array_rankedSUs[20];   
int rank=1,count=0,ch,i;
  SGSU temp;
  while (rank <= no_SUs )
  {
     temp.CurrentActiveSIs=0;
     temp.CurrentStandbySIs=0;
     for(i=0;i <= no_SUs;i++)
     {
ch = check_array_rankedSUs(array_rankedSUs,listSGSU[i].SUName.value,count);

      if (ch == 0)
      {

       if(listSGSU[i].CurrentActiveSIs > temp.CurrentActiveSIs)
       {
     
          strncpy((char *)temp.SUName.value, listSGSU[i].SUName.value, SA_MAX_NAME_LENGTH);
          temp.SUName.length = strlen((char *)temp.SUName.value);
          temp.CurrentActiveSIs = listSGSU[i].CurrentActiveSIs;
          temp.CurrentStandbySIs =listSGSU[i].CurrentStandbySIs;
          temp.rank=rank;
       
       
       }
       else
       {
           if(listSGSU[i].CurrentActiveSIs == temp.CurrentActiveSIs)
           {
              if(listSGSU[i].CurrentStandbySIs >= temp.CurrentStandbySIs) 
              {
                   strncpy((char *)temp.SUName.value, listSGSU[i].SUName.value, SA_MAX_NAME_LENGTH);
          	   temp.SUName.length = strlen((char *)temp.SUName.value);
          	   temp.CurrentActiveSIs = listSGSU[i].CurrentActiveSIs;
          	   temp.CurrentStandbySIs =listSGSU[i].CurrentStandbySIs;
                   temp.rank=rank;

              }
                 
           }
        }
       }//if ch 

     }//for

/////create ccb to change rank
printf("\nAdd CCB to change %s rank to %d",temp.SUName.value,temp.rank);

//change_rank(immHandle,objectName,SUrank,argv);

i=0;
while(i<no_SUs)
{
 ch=strncmp(temp.SUName.value,listSGSU[i].SUName.value,SA_MAX_NAME_LENGTH);
if(ch==0)
{
 
                   listSGSU[i].rank=temp.rank;
                   break;
} 
i++;
}

strncpy(array_rankedSUs[count].value,temp.SUName.value,SA_MAX_NAME_LENGTH);
array_rankedSUs[count].length = strlen((char *)array_rankedSUs[count].value);
count++;
rank++;

    
  }


}
int main()
{
 SaImmHandleT immHandle;
 SaNameT objectnameSG1;
 SaAisErrorT error;
 strncpy((char *)objectnameSG1.value,"safSg=AmfDemo,safApp=AmfDemo2", SA_MAX_NAME_LENGTH);
 int no_SUs,i;
 objectnameSG1.length = strlen((char *)objectnameSG1.value);
 //printf("\n%s",objectnameSG1.value);
 SGSU listSGSU[15];
 
 error = saImmOmInitialize( &immHandle, NULL, &immVersion);

 no_SUs=create_listSGSU(immHandle,objectnameSG1,listSGSU);

//printf("\n%d",no_SUs);
change_rankSI_increase(immHandle,listSGSU,no_SUs); 
/*
for (i=0;i<no_SUs;i++)
{
 change_rank(immHandle,listSGSU[i].SUName,listSGSU[I].rank,argv)
}
*/
change_rankSI_decrease(immHandle,listSGSU,no_SUs); 

 return 1;
}

