#include <gtk/gtk.h>
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
#define Cluster "safAmfCluster=myAmfCluster"
#define maxsize 100
#define Application1 "safApp=AmfDemo1"
#define Application2 "safApp=AmfDemo2"

static SaVersionT immVersion = { 'A', 2, 11 };
int ccb_safe = 1;
const SaImmCcbFlagsT defCcbFlags = SA_IMM_CCB_REGISTERED_OI | SA_IMM_CCB_ALLOW_NULL_OI;
static const char *pidfile;
SaImmHandleT immHandle;
SaImmScopeT scope = SA_IMM_SUBLEVEL;	/* default search scope */
SaImmScopeT scope1 = SA_IMM_SUBTREE;	/* default search scope */



int num_SUofSG(SaImmHandleT,SaNameT);
static int ServiceUnit=20;
typedef enum{
singleSI =1,
multipleSI =2
}elasticitytype;

typedef enum{
increase =1,
decrease =2
}elasticityaction;

elasticitytype elasticityT=1;
elasticityaction elasticityact;
SaNameT objectnameSI;
int callbackflag=0;
int buffer_manager=0;
int buffer2_manager=0;
int argc1;
char argv1[256];
char nameSI[256];
//int elasticityact;
int flag_set=3; 
//int elasticityT=1;
int count1=0,count2=0,count3=0;

typedef struct SIAssignments{
char text[256];
char buttonName[256];
char color[256];
}SIAssignments;

SIAssignments SI1[10], SI2[10], SI3[10];



void siAssignments();
void drawSUassSI();
void drawSU(SaNameT ,SaImmHandleT ,int );
char *accessattribute(SaImmHandleT immHandle,char *SUname,char attribute[maxsize],char Nodename[maxsize]);
char *findSUname(char string[maxsize],char rootName[maxsize]);

GtkWidget               *colorbutton1;
GtkWidget *buttonSI1SU1,*buttonSI1SU2,*buttonSI1SU3,*buttonSI1SU4,*buttonSI1SU5;
GtkWidget *buttonSI2SU1,*buttonSI2SU2,*buttonSI2SU3,*buttonSI2SU4,*buttonSI2SU5;
GtkWidget *buttonSI3SU1,*buttonSI3SU2,*buttonSI3SU3,*buttonSI3SU4,*buttonSI3SU5;
GdkColor   color1,color2,color3;

void remove_all_chars(char* str, char c) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}


void siAssignments()
{

    SaAisErrorT error;
   SaImmHandleT immHandle;
   static SaVersionT immVersion = { 'A', 2, 11 };
   error = saImmOmInitialize(&immHandle, NULL,&immVersion);
   
}




void on_window1_destroy (GtkObject *object, gpointer user_data)
{
        gtk_main_quit();
       
       
}


void drawSUassSI()
{
   SaAisErrorT error;
   SaImmHandleT immHandle;
   static SaVersionT immVersion = { 'A', 2, 11 };
   error = saImmOmInitialize(&immHandle, NULL,&immVersion);
  
   char objectstr[maxsize];
   strcpy(objectstr,"safSi=");
    SaImmSearchHandleT searchHandle;
   int  SInum = 0;
    SaNameT objectName;
    SaImmSearchParametersT_2 searchParam;
    searchParam.searchOneAttr.attrName = NULL;
    searchParam.searchOneAttr.attrValue = NULL;
    SaNameT rootName = { 0, "" };
    strncpy((char *)rootName.value,Application1, SA_MAX_NAME_LENGTH);
    rootName.length = strlen((char *)rootName.value);
    SaImmAttrNameT  attributeNames[] ={objectstr,NULL};
    SaImmAttrValuesT_2 **attributes;
    SaUint8T *ch;
   
    long int pos;

    //**SIs of SG1**//
      error = saImmOmSearchInitialize_2(immHandle, &rootName, scope1, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);
    do {
                
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
                if (error == SA_AIS_OK)
                {
                    
                    ch =strstr(objectName.value,objectstr);
                    
                    if(ch == NULL)
                      {
                      }
                      else
                      {
                          pos = objectName.value - ch;
                          if(pos == 0)
                          {
                               
                                 SInum++;
                                // printf("\n%d",SInum);
                          	drawSU(objectName,immHandle,SInum);
                           	
                          }   
                      }          
                 }
        } while (error != SA_AIS_ERR_NOT_EXIST);
   error = saImmOmSearchFinalize(searchHandle);
  
  //**SIs of SG2**//
//  SInum =0;
  strncpy((char *)rootName.value,Application2, SA_MAX_NAME_LENGTH);
  rootName.length = strlen((char *)rootName.value);
  error = saImmOmSearchInitialize_2(immHandle, &rootName, scope1, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);
  do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
                if (error == SA_AIS_OK)
                {
                    
                    ch =strstr(objectName.value,objectstr);
                    
                    if(ch == NULL)
                      {
                      }
                      else
                      {
                          pos = objectName.value - ch;
                          if(pos == 0)
                          {
                                 SInum++;
                          	drawSU(objectName,immHandle, SInum);
                           	
                          }   
                      }          
                 }
        } while (error != SA_AIS_ERR_NOT_EXIST);
   error = saImmOmSearchFinalize(searchHandle);
   
   error = saImmOmFinalize(immHandle);
   
}

void drawSU(SaNameT objectName,SaImmHandleT immHandle,int SInum)
{
 // printf("\n%s",objectName.value);
  // printf("\n tHe si Nimber is %d",SInum);
   SaAisErrorT error;
   char objectstr[maxsize];
   strcpy(objectstr,"safSISU");
    SaImmSearchHandleT searchHandle1;
    int number=0,nodecheck,i;
    SaNameT objectName1;
    SaImmSearchParametersT_2 searchParam;
    searchParam.searchOneAttr.attrName = NULL;
    searchParam.searchOneAttr.attrValue = NULL;
    SaNameT rootName = { 0, "" };
   // printf("\n si : %s",objectName.value);
    strncpy((char *)rootName.value,objectName.value, SA_MAX_NAME_LENGTH);
    rootName.length = strlen((char *)rootName.value);
    SaImmAttrNameT  attributeNames[] ={objectstr,NULL};
    SaImmAttrValuesT_2 **attributes;
    SaUint8T *ch;
    long int pos;
    char *SUname,*nodename;
    char attribute[maxsize],Nodename[maxsize];
     error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle1);
 

  do {
           
		error = saImmOmSearchNext_2(searchHandle1, &objectName1, &attributes);
                if (error == SA_AIS_OK)
                {
                    
                    ch =strstr(objectName1.value,objectstr);
                    
                    if(ch == NULL)
                      {
                      }
                      else
                      {
                          
                              //  printf("\n%s",objectName1.value);
                               SUname = findSUname(objectName1.value,rootName.value);
                              // printf("\n%s",SUname);
                               strcpy(attribute,"safSUHostedByNode");
                               nodename = accessattribute(immHandle,SUname,attribute,Nodename);
                              // printf("\n%s",nodename);
                               nodecheck=strcmp(nodename,"safAmfNode=SC-1,safAmfCluster=myAmfCluster");
                             
                              
                                       if(nodecheck == 0)
                                       {
                                         
                                          if(SInum ==1)
                                          {
                                               //printf("\n1");
                                              strcpy(SI1[count1].text, "Active Assgn SI1");
                                              strcpy(SI1[count1].buttonName, "buttonSI1SU1"); 
                                              strcpy(SI1[count1].color, "&color1");
                                              gtk_widget_modify_bg (GTK_WIDGET(buttonSI1SU1), GTK_STATE_NORMAL, &color1);
          					gtk_object_set(GTK_OBJECT(buttonSI1SU1), "label", "Active Assg SI1", NULL); 
                                              count1++;
                                          }
                                          if(SInum ==2)
                                          {
                                               count2++; 
                                              strcpy(SI2[count2].text, "Active Assgn SI2");
                                              strcpy(SI2[count2].buttonName, "buttonSI2SU1"); 
                                              strcpy(SI2[count2].color, "&color2");
                                               if((count2%2)==0)
                                              {
                                                //printf("\n 2 This is active count2 is %d",count2);
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI2SU1), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI2SU1), "label", "Active Assg SI2", NULL);     
                                              }
                                              else
                                              {
                                               //printf("\n3");
                                                //printf("\n This is standby count2 is %d",count2);
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI2SU1), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI2SU1), "label", "Standby Assg SI2", NULL);     
                                              }
                                             
                                          }
                                           if(SInum ==3)
                                          {
                                                count3++;
                                              strcpy(SI3[count3].text, "Active Assgn SI3");
                                              strcpy(SI3[count3].buttonName, "buttonSI3SU1");
                                              strcpy(SI3[count3].color, "&color2");
                                               if((count3%2)==0)
                                              {
                                               //printf("\n4");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI3SU1), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI3SU1), "label", "Active Assg SI3", NULL);     
                                              }
                                              else
                                              {
                                               //printf("\n5");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI3SU1), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI3SU1), "label", "Standby Assg SI3", NULL);     
                                              } 
                                            
                                          }
                                       }
                           //  printf("\n3");
                             nodecheck=strcmp(nodename,"safAmfNode=SC-2,safAmfCluster=myAmfCluster");
                             
                                       if(nodecheck == 0)
                                       {
                                //          printf("\n4");
                                          if(SInum ==1)
                                          {   //printf("\n6");
                                              strcpy(SI1[count1].text, "Active Assgn SI1");
                                              strcpy(SI1[count1].buttonName, "buttonSI1SU2"); 
                                              strcpy(SI1[count1].color, "&color1");
                                                
          					gtk_widget_modify_bg (GTK_WIDGET(buttonSI1SU2), GTK_STATE_NORMAL, &color1);
          					gtk_object_set(GTK_OBJECT(buttonSI1SU2), "label", "Active Assg SI1", NULL); 
                                            //   printf("\n %s",SI1[count1].buttonName);
                                              count1++;
                                          }
                                          if(SInum ==2)
                                          {      count2++;
                                              strcpy(SI2[count2].text, "Active Assgn SI2");
                                              strcpy(SI2[count2].buttonName, "buttonSI2SU2"); 
                                              strcpy(SI2[count2].color, "&color2");
                                              if((count2%2)==0)
                                              {//printf("\n7");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI2SU2), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI2SU2), "label", "Active Assg SI2", NULL);     
                                              }
                                              else
                                              {//printf("\n8");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI2SU2), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI2SU2), "label", "Standby Assg SI2", NULL);     
                                              }
                                           
                                          }
                                           if(SInum ==3)
                                          {  // printf("\n9");
                                               count3++;
                                              strcpy(SI3[count3].text, "Active Assgn SI3");
                                              strcpy(SI3[count3].buttonName, "buttonSI3SU2");
                                              strcpy(SI3[count3].color, "&color2");
                                              if((count3%2)==0)
                                              {//printf("\n10");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI3SU2), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI3SU2), "label", "Active Assg SI3", NULL);     
                                              }
                                              else
                                              {//printf("\n11");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI3SU2), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI3SU2), "label", "Standby Assg SI3", NULL);     
                                              } 
                                             
                                          }
                                       }
                             nodecheck=strcmp(nodename,"safAmfNode=PL-5,safAmfCluster=myAmfCluster");
                                       if(nodecheck == 0)
                                       {
                                          if(SInum ==1)
                                          {  // printf("\n12");
                                              strcpy(SI1[count1].text, "Active Assgn SI1");
                                              strcpy(SI1[count1].buttonName, "buttonSI1SU3"); 
                                              strcpy(SI1[count1].color, "&color1");
                                                gtk_widget_modify_bg (GTK_WIDGET(buttonSI1SU3), GTK_STATE_NORMAL, &color1);
          					gtk_object_set(GTK_OBJECT(buttonSI1SU3), "label", "Active Assg SI1", NULL); 
                                              count1++;
                                          }
                                          if(SInum ==2)
                                          {    count2++;
                                              strcpy(SI2[count2].text, "Active Assgn SI2");
                                              strcpy(SI2[count2].buttonName, "buttonSI2SU3"); 
                                              strcpy(SI2[count2].color, "&color2");
                                              if((count2%2)==0)
                                              {//printf("\n13");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI2SU3), GTK_STATE_NORMAL, &color2);
          				       gtk_object_set(GTK_OBJECT(buttonSI2SU3), "label", "Active Assg SI2", NULL);     
                                              }
                                              else
                                              {//printf("\n14");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI2SU3), GTK_STATE_NORMAL, &color2);
          				       gtk_object_set(GTK_OBJECT(buttonSI2SU3), "label", "Standby Assg SI2", NULL);     
                                              }
                                             
                                          }
                                           if(SInum ==3)
                                          {   //printf("\n15");
                                               count3++;
                                              strcpy(SI3[count3].text, "Active Assgn SI3");
                                              strcpy(SI3[count3].buttonName, "buttonSI3SU3");
                                              strcpy(SI3[count3].color, "&color2");
                                               if((count3%2)==0)
                                              {//printf("\n16");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI3SU3), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI3SU3), "label", "Active Assg SI3", NULL);     
                                              }
                                              else
                                              {//printf("\n17");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI3SU3), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI3SU3), "label", "Standby Assg SI3", NULL);     
                                              } 
                                             
                                          }
                                       }
                             nodecheck=strcmp(nodename,"safAmfNode=PL-4,safAmfCluster=myAmfCluster");
                                       if(nodecheck == 0)
                                       {
                                          if(SInum ==1)
                                          {   //printf("\n18");
                                              strcpy(SI1[count1].text, "Active Assgn SI1");
                                              strcpy(SI1[count1].buttonName, "buttonSI1SU4"); 
                                              strcpy(SI1[count1].color, "&color1");
                                                gtk_widget_modify_bg (GTK_WIDGET(buttonSI1SU4), GTK_STATE_NORMAL, &color1);
          					gtk_object_set(GTK_OBJECT(buttonSI1SU4), "label", "Active Assg SI1", NULL); 
                                              count1++;
                                          }
                                          if(SInum ==2)
                                          {  count2++;
                                              strcpy(SI2[count2].text, "Active Assgn SI2");
                                              strcpy(SI2[count2].buttonName, "buttonSI2SU4"); 
                                              strcpy(SI2[count2].color, "&color2");
                                              if((count2%2)==0)
                                              { //printf("\n19");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI2SU4), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI2SU4), "label", "Active Assg SI2", NULL);     
                                              }
                                              else
                                              { // printf("\n20");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI2SU4), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI2SU4), "label", "Standby Assg SI2", NULL);     
                                              }
                                              
                                          }
                                           if(SInum ==3)
                                          {    count3++;
                                              strcpy(SI3[count3].text, "Active Assgn SI3");
                                              strcpy(SI3[count3].buttonName, "buttonSI3SU4");
                                              strcpy(SI3[count3].color, "&color2");
                                              if((count3%2)==0)
                                              { //printf("\n21");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI3SU4), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI3SU4), "label", "Active Assg SI3", NULL);     
                                              }
                                              else
                                              { //printf("\n22");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI3SU4), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI3SU4), "label", "Standby Assg SI3", NULL);     
                                              } 
                                             
                                          }
                                       }
                             nodecheck=strcmp(nodename,"safAmfNode=PL-3,safAmfCluster=myAmfCluster");
                                       if(nodecheck == 0)
                                       {
                                          if(SInum ==1)
                                          {    //printf("\n23");
                                              strcpy(SI1[count1].text, "Active Assgn SI1");
                                              strcpy(SI1[count1].buttonName, "buttonSI1SU5"); 
                                              strcpy(SI1[count1].color, "&color1");
                                                gtk_widget_modify_bg (GTK_WIDGET(buttonSI1SU5), GTK_STATE_NORMAL, &color1);
          					gtk_object_set(GTK_OBJECT(buttonSI1SU5), "label", "Active Assg SI1", NULL); 
                                              count1++;
                                          }
                                          if(SInum ==2)
                                          {   count2++;
                                              strcpy(SI2[count2].text, "Active Assgn SI2");
                                              strcpy(SI2[count2].buttonName, "buttonSI2SU5"); 
                                              strcpy(SI2[count2].color, "&color2");
                                              if((count2%2)==0)
                                              { //printf("\n24");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI2SU5), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI2SU5), "label", "Active Assg SI2", NULL);     
                                              }
                                              else
                                              { //printf("\n25");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI2SU5), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI2SU5), "label", "Standby Assg SI2", NULL);     
                                              }
                                              
                                          }
                                           if(SInum ==3)
                                          {    count3++;
                                              strcpy(SI3[count3].text, "Active Assgn SI3");
                                              strcpy(SI3[count3].buttonName, "buttonSI3SU5");
                                              strcpy(SI3[count3].color, "&color2");
                                              if((count3%2)==0)
                                              { //printf("\n26");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI3SU5), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI3SU5), "label", "Active Assg SI3", NULL);     
                                              }
                                              else
                                              { //printf("\n27");
                                               gtk_widget_modify_bg (GTK_WIDGET(buttonSI3SU5), GTK_STATE_NORMAL, &color3);
          				       gtk_object_set(GTK_OBJECT(buttonSI3SU5), "label", "Standby Assg SI3", NULL);     
                                              }  
                                             
                                          }
                                       } 
            
                      }          
                 }
        } while (error != SA_AIS_ERR_NOT_EXIST);
           
  error = saImmOmSearchFinalize(searchHandle1);
   
}



char *accessattribute(SaImmHandleT immHandle,char *SUname,char attribute[maxsize],char Nodename[maxsize])
{



// printf("\n 4");
 SaImmAccessorHandleT accessorHandle;
 int rc = EXIT_SUCCESS;

 
 SaImmAttrNameT  attributeNames[] ={"saAmfSUHostedByNode",NULL};
 //const SaNameT rootName =  objectnameSI;
 SaAisErrorT error;
 SaNameT objectName1;
 strncpy((char *)objectName1.value,SUname, SA_MAX_NAME_LENGTH);
   objectName1.length = strlen((char *)objectName1.value);

 SaNameT *objectnameNode,objectnameSG1;
 SaImmAttrValuesT_2 **attributes;
 char ch[maxsize];
//   char *ch;
 
 
 
                                        
                                        //strcpy(attributeNames[0],attribute);
					//attributeNames[1] = NULL;
					error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 					
       		   	      	 if (error == SA_AIS_OK)
   		    		{
       					error= saImmOmAccessorGet_2(accessorHandle, &objectName1, attributeNames, &attributes);
       					if(error == SA_AIS_OK)
       					{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		objectnameNode = (SaNameT *)attrValue;
                                                                                
                                                                              

                        
										strcpy(ch,objectnameNode->value);
                                                                           
                                                                                error = saImmOmAccessorFinalize(accessorHandle);
                                                                           
										strcpy((char *)Nodename, ch);
                                                                            
										//objectnameSG1.length = strlen((char *)objectName1.value);
                                                                        
               						   			

      					}
//

				}
   
      return Nodename ;
}
char *findSUname(char string[maxsize],char rootName[maxsize])
{

   int len1,len2,len3,i,j;
   
  		
                        len1=strlen(string);
      			len2=strlen("safSISU=");   
      

      			for(i=0;(i+len2+1)<=len1;i++)
            		string[i]=string[len2+i];
            		string[i]='\0';
      			//printf("\nDeleted : %s\n",string);

    			
    			//strcpy(string3,",");
    			//strcat(string3,rootName.value);
    			//printf("\nconcated :%s\n",rootName);

      			//	printf("\n%s\n",string);

      			len1=strlen(string);
  
      			len2=strlen(rootName); 
      			len3=len1-len2;
    
      

      			for(i=0;i<len3-1;i++)
            		string[i]=string[i];
            		string[i]='\0';
      			//printf("\nDeleted2 : %s\n",string);


    			remove_all_chars(string, '\\');
    			//printf("'%s'\n", string);


                      
   return string;
}

int main (int argc, char *argv[])
{
        GtkBuilder              *builder;
        GtkWidget               *window;
    
        //argc1 = argc;
       

   if (argc != 2)
   {
      //printf("Dude are you passing something at all");
      //return 1;   
   }
   //strcpy(argv1, argv[0]);

        gtk_init (&argc, &argv);
        
        builder = gtk_builder_new ();
        gtk_builder_add_from_file (builder, "Glade_GUI2.xml", NULL);

        window = GTK_WIDGET (gtk_builder_get_object (builder,"window1"));
        gtk_builder_connect_signals (builder, NULL);
              
        g_object_unref (G_OBJECT (builder));
        
        gtk_widget_show (window);   
           
        gtk_main ();
       
        return 0;
}
