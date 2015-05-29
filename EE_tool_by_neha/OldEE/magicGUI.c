#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#ifndef __USE_ISOC99
#define __USE_ISOC99 // strtof and LLONG_MAX in older gcc versions like 4.3.2
#endif

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



#include <saAis.h>
#include <saImmOm.h>
#include <saImmOi.h>
#define PARAMDELIM ":"
#define red "\033[0;31m"
#define cyan "\033[1;36m"
#define green "\033[4;32m"
#define blue "\033[9;34m"
#define black "\033[0;30m"
#define brown "\033[0;33m"
#define magenta "\033[0;35m"
#define gray "\033[0;37m"
#define none "\033[0m"

static SaVersionT immVersion = { 'A', 2, 11 };
int ccb_safe = 1;
const SaImmCcbFlagsT defCcbFlags = SA_IMM_CCB_REGISTERED_OI | SA_IMM_CCB_ALLOW_NULL_OI;

int argc1;
char argv1[256];
char nameSI[256];
int elasticityact;
int flag_set=0; 
int elasticityT=1;
int elasticity_engine_main(int , char []);


 GtkWidget               *entry;
 GtkWidget               *radiobutton;

void on_button1_clicked(GtkWidget *button, gpointer user_data)
{
     int result1;
     flag_set =0;
     gchar *text;
     //GtkWidget * entry = lookup_widget(GTK_WIDGET(button), "entry1");
      //d_string=gtk_entry_get_text(GTK_ENTRY(textValue));
     text = (gchar *)gtk_entry_get_text(GTK_ENTRY(entry));
     strcpy(nameSI,text);
     //printf("\n%s",nameSI);
    
     if( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( radiobutton )))
     {
        flag_set =3;
        elasticityact =1;
     }
     else
     {
       // printf( "\n1 is chosen\n" );
         flag_set =3;
         elasticityact =2;
     }
   
   result1= elasticity_engine_main(argc1,(char *)argv1);    
  
  printf("\nThe service %s and the elasticity action is %d\n",nameSI,elasticityact);
}

void on_window_destroy (GtkObject *object, gpointer user_data)
{
        gtk_main_quit();
       
}

int elasticity_engine_main(int argc, char argv[])
{


  SaInt8T nameSG[256];
  SaInt8T nameSG3[256];
  SaNameT objectnameSI;
  SaNameT objectnameSG1,objectnameSG3;
  int redmodT;
  int i = 0, j;
  SaAisErrorT error,error1,error3;
  SaImmHandleT immHandle;
  SaImmOiHandleT immOiHandle;
  const SaImmOiCallbacksT_2 immOiCallbacks;
  SaImmAccessorHandleT accessorHandle,accessorHandle1,accessorHandle2;
  SaImmAttrValuesT_2 **attributes;
  SaNameT objectNameSU,objectname;
  SaImmAttrValuesT_2 *attr,*attr1,*attr2;
  const SaImmAttrNameT  attributeNames[] ={"saAmfSIProtectedbySG",NULL};
  const SaImmAttrNameT  attributeNamesSGT[] ={"saAmfSgtRedundancyModel",NULL};
  SaImmAttrNameT  attributeNames1[2];
  //the wed
  int x=0,y=0,z=0;
  SaImmAttrValuesT_2 *attrSG;
  //SaAisErrorT error;

  const SaImmAttrNameT  attributeNamesSG[] ={"saAmfSGType",NULL};
  SaImmAttrValuesT_2 **attributesSG;
  SaImmAttrValuesT_2 **attributesSGT;
  //SaNameT **objectNameSG1;
  //the wed
  int counter=0;
  char strs[10][35];
  SaImmSearchParametersT_2 searchParam;
  SaImmScopeT scope = SA_IMM_SUBTREE;	/* default search scope */
  SaImmSearchHandleT searchHandle;
  int rc = EXIT_SUCCESS;
  const SaImmOiImplementerNameT implementerName = "safAmfServices";
  const SaImmClassNameT className = "SaAmfSI";
   // printf("\n I got a call");
  if(flag_set ==3)
  {
	objectnameSI.length= strlen(nameSI);
	strncpy((char *)objectnameSI.value, nameSI, SA_MAX_NAME_LENGTH);
        
  } 
   
  
  printf("\nTHe SI is %s and elasticity type is %d and elasticity action is %d",objectnameSI.value,elasticityT,elasticityact);
  return 0;
}


int main (int argc, char *argv[])
{
        GtkBuilder              *builder;
        GtkWidget               *window;
       
        argc1 = argc;
       

   if (argc != 2)
   {
      //printf("Dude are you passing something at all");
      //return 1;   
   }
   strcpy(argv1, argv[0]);

        gtk_init (&argc, &argv);
        
        builder = gtk_builder_new ();
        gtk_builder_add_from_file (builder, "Glad_GUI.xml", NULL);

        window = GTK_WIDGET (gtk_builder_get_object (builder,"window"));
        gtk_builder_connect_signals (builder, NULL);
        entry = GTK_WIDGET (gtk_builder_get_object (builder,"entry1")); 
        radiobutton = GTK_WIDGET (gtk_builder_get_object (builder,"radiobutton1")); 
        //g_signal_connect(G_OBJECT(Button1),"clicked",G_CALLBACK(on_button1_clicked),entry1);        
        g_object_unref (G_OBJECT (builder));
        
        gtk_widget_show (window);   
           
        gtk_main ();
       
        return 0;
}
