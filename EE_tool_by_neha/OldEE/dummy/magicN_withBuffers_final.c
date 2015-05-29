#include <stdbool.h>
#include <stdio.h>
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
//#include <curses.h>
//#include <ncurses.h>
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
static const char *pidfile;
SaImmHandleT immHandle;

SaAisErrorT imm_objcreate_set_callback(SaImmOiHandleT ,SaImmOiCcbIdT ,const SaImmClassNameT ,const SaNameT *,const SaImmAttrValuesT_2 **);
static SaAisErrorT imm_objdel_set_callback(SaImmOiHandleT ,SaImmOiCcbIdT ,const SaNameT *);
static SaAisErrorT imm_objmod_set_callback(SaImmOiHandleT ,SaImmOiCcbIdT ,const SaNameT *,const SaImmAttrModificationT_2 **);
static SaAisErrorT imm_ccbcomplete_set_callback(SaImmOiHandleT ,SaImmOiCcbIdT );
static void imm_ccbApply_callback(SaImmOiHandleT ,SaImmOiCcbIdT );
static void imm_ccbAbort_callback(SaImmOiHandleT,SaImmOiCcbIdT );
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




int argc1;
char argv1[256];
char nameSI[256];
//int elasticityact;
int flag_set=0; 
//int elasticityT=1;
int elasticity_engine_main(int , char []);


 GtkWidget  *entry;
 GtkWidget  *radiobutton;

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
         elasticityT = 1;
     }
     else
     {
       // printf( "\n1 is chosen\n" );
         flag_set =3;
         elasticityact =2;
          elasticityT = 1;
     }
  
   result1= elasticity_engine_main(argc1,(char *)argv1);    
  printf("\n the result is %d",result1);
  printf("\nThe service %s and the elasticity action is %d\n",nameSI,elasticityact);
}

void on_window_destroy (GtkObject *object, gpointer user_data)
{
        gtk_main_quit();
       
}



static char *create_adminOwnerName(char *base){
	char hostname[HOST_NAME_MAX];
	char *unique_adminOwner = malloc(HOST_NAME_MAX+10+strlen(base)+5);

	if (gethostname(hostname, sizeof(hostname)) != 0){
		fprintf(stderr, "error while retrieving hostname\n");
		exit(EXIT_FAILURE);
	}
	sprintf(unique_adminOwner, "%s_%s_%d", base, hostname, getpid());
	return unique_adminOwner;
}


SaAisErrorT get_attrValueType(const SaImmClassNameT className,
	SaImmAttrNameT attrName, SaImmValueTypeT *attrValueType)
{
	SaAisErrorT rc;
	SaImmHandleT omHandle;
	SaImmClassCategoryT classCategory;
	SaImmAttrDefinitionT_2 *attrDef;
	SaImmAttrDefinitionT_2 **attrDefinitions;
	int i = 0;

        (void)saImmOmInitialize(&omHandle, NULL, &immVersion);

	if ((rc = saImmOmClassDescriptionGet_2(omHandle, className, &classCategory, &attrDefinitions)) != SA_AIS_OK)
		return rc;

	rc = SA_AIS_ERR_INVALID_PARAM;
	while ((attrDef = attrDefinitions[i++]) != NULL) {
		if (!strcmp(attrName, attrDef->attrName)) {
			*attrValueType = attrDef->attrValueType;
			rc = SA_AIS_OK;
			break;
		}
	}

	(void)saImmOmClassDescriptionMemoryFree_2(omHandle, attrDefinitions);
	(void)saImmOmFinalize(omHandle);
	return rc;
}



SaImmClassNameT immutil_get_className(const SaNameT *objectName)
{
	SaImmHandleT omHandle;
        SaAisErrorT error;
	SaImmClassNameT className;
	SaImmAccessorHandleT accessorHandle;
	SaImmAttrValuesT_2 **attributes;
	SaImmAttrNameT attributeNames[] = { "SaImmAttrClassName", NULL };

	(void)saImmOmInitialize(&omHandle, NULL, &immVersion);
	(void)saImmOmAccessorInitialize(omHandle, &accessorHandle);
        error = saImmOmAccessorGet_2(accessorHandle, objectName, attributeNames, &attributes);
       // printf("\n%d\n",error);
        if (saImmOmAccessorGet_2(accessorHandle, objectName, attributeNames, &attributes) != SA_AIS_OK)
                return NULL;
	className = strdup(*((char **)attributes[0]->attrValues[0]));
	(void)saImmOmAccessorFinalize(accessorHandle);
	(void)saImmOmFinalize(omHandle);

	return className;
}

void *immutil_new_attrValue(SaImmValueTypeT attrValueType, const char *str)
{
	void *attrValue = NULL;
	size_t len;
	char *endptr;

	/*
	** sizeof(long) varies between 32 and 64 bit machines. Therefore on a 
	** 64 bit machine, a check is needed to ensure that the value returned
	** from strtol() or strtoul() is not greater than what fits into 32 bits.
	*/
	switch (attrValueType) {
	case SA_IMM_ATTR_SAINT32T: {
		errno = 0;
		long value = strtol(str, &endptr, 0);
		SaInt32T attr_value = value;
		if ((errno != 0) || (endptr == str) || (*endptr != '\0')) {
			fprintf(stderr, "int32 conversion failed\n");
			return NULL;
		}
		if (value != attr_value) {
			printf("int32 conversion failed, value too large\n");
			return NULL;
		}
		attrValue = malloc(sizeof(SaInt32T));
		*((SaInt32T *)attrValue) = value;
		break;
	}
	case SA_IMM_ATTR_SAUINT32T: {
		errno = 0;
		unsigned long value = strtoul(str, &endptr, 0);
		SaUint32T attr_value = value;
		if ((errno != 0) || (endptr == str) || (*endptr != '\0')) {
			fprintf(stderr, "uint32 conversion failed\n");
			return NULL;
		}
		if (value != attr_value) {
			printf("uint32 conversion failed, value too large\n");
			return NULL;
		}
		attrValue = malloc(sizeof(SaUint32T));
		*((SaUint32T *)attrValue) = value;
		break;
	}
	case SA_IMM_ATTR_SAINT64T:
		// fall-through, same basic data type
	case SA_IMM_ATTR_SATIMET: {
		errno = 0;
		long long value = strtoll(str, &endptr, 0);
		if ((errno != 0) || (endptr == str) || (*endptr != '\0')) {
			fprintf(stderr, "int64 conversion failed\n");
			return NULL;
		}
		attrValue = malloc(sizeof(SaInt64T));
		*((SaInt64T *)attrValue) = value;
		break;
	}
	case SA_IMM_ATTR_SAUINT64T: {
		errno = 0;
		unsigned long long value = strtoull(str, &endptr, 0);
		if ((errno != 0) || (endptr == str) || (*endptr != '\0')) {
			fprintf(stderr, "uint64 conversion failed\n");
			return NULL;
		}
		attrValue = malloc(sizeof(SaUint64T));
		*((SaUint64T *)attrValue) = value;
		break;
	}
	case SA_IMM_ATTR_SAFLOATT: {
		errno = 0;
		float myfloat = strtof(str, &endptr);
		if (((myfloat == 0) && (endptr == str)) ||
		    (errno == ERANGE) || (*endptr != '\0')) {
			fprintf(stderr, "float conversion failed\n");
			return NULL;
		}
		attrValue = malloc(sizeof(SaFloatT));
		*((SaFloatT *)attrValue) = myfloat;
		break;
	}
	case SA_IMM_ATTR_SADOUBLET: {
		errno = 0;
		double mydouble = strtod(str, &endptr);
		if (((mydouble == 0) && (endptr == str)) ||
		    (errno == ERANGE) || (*endptr != '\0')) {
			fprintf(stderr, "double conversion failed\n");
			return NULL;
		}
		attrValue = malloc(sizeof(SaDoubleT));
		*((SaDoubleT *)attrValue) = mydouble;
		break;
	}
	case SA_IMM_ATTR_SANAMET: {
		SaNameT *mynamet;
		len = strlen(str);
		if (len > SA_MAX_NAME_LENGTH) {
			fprintf(stderr, "too long SaNameT\n");
			return NULL;
		}
		attrValue = mynamet = malloc(sizeof(SaNameT));
		mynamet->length = len;
		strncpy((char *)mynamet->value, str, SA_MAX_NAME_LENGTH);
		break;
	}
	case SA_IMM_ATTR_SASTRINGT: {
		attrValue = malloc(sizeof(SaStringT));
		*((SaStringT *)attrValue) = strdup(str);
		break;
	}
	case SA_IMM_ATTR_SAANYT: {
		char* endMark;
		SaBoolT even = SA_TRUE;
		char byte[5];
		unsigned int i;

		len = strlen(str);
		if(len % 2) {
			len = len/2 + 1;
			even = SA_FALSE;
		} else {
			len = len/2;
		}
		attrValue = malloc(sizeof(SaAnyT));
		((SaAnyT*)attrValue)->bufferAddr = 
			(SaUint8T*)malloc(sizeof(SaUint8T) * len);
		((SaAnyT*)attrValue)->bufferSize = len;

		byte[0] = '0';
		byte[1] = 'x';
		byte[4] = '\0';

		endMark = byte + 4;

		for (i = 0; i < len; i++) {
			byte[2] = str[2*i];
			if(even || (i + 1 < len)) {
				byte[3] = str[2*i + 1];
			} else {
				byte[3] = '0';
			}
			((SaAnyT*)attrValue)->bufferAddr[i] = 
				(SaUint8T)strtod(byte, &endMark);
		}
	}
	default:
		break;
	}

	return attrValue;
}



static SaImmAttrModificationT_2 *new_attr_mod(const SaNameT *objectName, char *nameval)
{
	int res = 0;
	char *tmp = strdup(nameval);
	char *name, *value;
	SaImmAttrModificationT_2 *attrMod = NULL;
        //printf("\n%s!!!\n",objectName->value);
	SaImmClassNameT className = immutil_get_className(objectName);
	SaAisErrorT error;
	SaImmAttrModificationTypeT modType = SA_IMM_ATTR_VALUES_REPLACE;

	if (className == NULL) {
		fprintf(stderr, "Object with DN '%s' does not exist\n", objectName->value);
		res = -1;
		goto done;
	}

	attrMod = malloc(sizeof(SaImmAttrModificationT_2));

	if ((value = strstr(tmp, "=")) == NULL) {
		res = -1;
		goto done;
	}

	if (value[-1] == '+') {
		modType = SA_IMM_ATTR_VALUES_ADD;
		value[-1] = 0;
	}
	else if (value[-1] == '-') {
		modType = SA_IMM_ATTR_VALUES_DELETE;
		value[-1] = 0;
	}

	name = tmp;
	*value = '\0';
	value++;

	error = get_attrValueType(className, name, &attrMod->modAttr.attrValueType);
	if (error == SA_AIS_ERR_NOT_EXIST) {
		fprintf(stderr, "Class '%s' does not exist\n", className);
		res = -1;
		goto done;
	}

	if (error != SA_AIS_OK) {
		printf("Attribute '%s' does not exist in class '%s'\n", name, className);
		res = -1;
		goto done;
	}

	attrMod->modType = modType;
	attrMod->modAttr.attrName = name;
	if (strlen(value)) {
		attrMod->modAttr.attrValuesNumber = 1;
		attrMod->modAttr.attrValues = malloc(sizeof(SaImmAttrValueT *));
		attrMod->modAttr.attrValues[0] = immutil_new_attrValue(attrMod->modAttr.attrValueType, value);
		if (attrMod->modAttr.attrValues[0] == NULL)
			res = -1;
	} else {
		attrMod->modAttr.attrValuesNumber = 0;
		attrMod->modAttr.attrValues = NULL;
	}
	
 done:
	free(className);
	if (res != 0) {
		free(attrMod);
		attrMod = NULL;
	}
	return attrMod;
}



int object_modify(const SaNameT **objectNames, SaImmAdminOwnerHandleT ownerHandle,char rankt[256])
{
// printf("\ni got call\n");
 SaAisErrorT error,error1,error2=0;
 int i;
 int attr_len = 1;
 int rc = EXIT_FAILURE;
 //int rc = EXIT_SUCCESS;

 SaImmCcbHandleT ccbHandle;
 const SaStringT* errStrings=NULL;

 char *optargs = NULL;

 optargs = rankt;

 SaImmAttrModificationT_2 **attrMods;

 SaImmAttrModificationT_2 *attrMod;
	//SaImmClassNameT className = immutil_get_className(objectName);
	
	SaImmAttrModificationTypeT modType = SA_IMM_ATTR_VALUES_ADD;
	//attrMods = malloc(2 * sizeof(SaImmAttrModificationT_2 *));
       
attrMods = realloc(attrMods, 2 * sizeof(SaImmAttrModificationT_2 *));

        if ((attrMod = new_attr_mod(objectNames[0], optargs)) == NULL)   

        	exit(EXIT_FAILURE);

		attrMods[attr_len - 1] = attrMod;
		attrMods[attr_len] = NULL;
		//attr_len++;

error2 = saImmOmAdminOwnerSet(ownerHandle, (const SaNameT **)objectNames, SA_IMM_ONE);

if(error2 ==SA_AIS_OK)
{

	//printf("\nsaImmOmAdminOwnerSet %d\n",error2);

if ((error = saImmOmCcbInitialize(ownerHandle, ccb_safe?defCcbFlags:0x0, &ccbHandle))
		== SA_AIS_OK) {
		//printf("error - saImmOmCcbInitialize FAILED: %d\n", error);
		//goto done_release;

	i = 0;

	while (objectNames[i] != NULL) {

		if ((error = saImmOmCcbObjectModify_2(ccbHandle, objectNames[i],(const SaImmAttrModificationT_2 **)attrMods)) == SA_AIS_OK) {
			//printf("error - saImmOmCcbObjectModify_2 FAILED: %d\n", error);
                        printf("CCB successfully Applied\n");

			if((error == SA_AIS_ERR_NOT_EXIST) && ccb_safe) {
				fprintf(stderr, "Missing: implementer, or object, or attribute "
					"(see: immcfg -h under '--unsafe')\n");
			} else {
				SaAisErrorT rc2 = saImmOmCcbGetErrorStrings(ccbHandle, &errStrings);
				if(errStrings) {
					int ix = 0;
					while(errStrings[ix]) {
						printf("OI reports: %s\n", errStrings[ix]);
						++ix;
					}
				} else if(rc2 != SA_AIS_OK) {
					printf("saImmOmCcbGetErrorStrings failed: %u\n", rc2);
				}
			}
			//goto done_release;
		}
		i++;
	}

if ((error = saImmOmCcbApply(ccbHandle)) != SA_AIS_OK) {
 
		if(error == SA_AIS_ERR_TIMEOUT) {
			printf("saImmOmCcbApply returned SA_AIS_ERR_TIMEOUT, result for CCB is unknown\n");
			//goto done_release;
		}
        
                
		
/**
		SaAisErrorT rc2 = saImmOmCcbGetErrorStrings(ccbHandle, &errStrings);
		if(errStrings) {
			int ix = 0;
			while(errStrings[ix]) {
				fprintf(stderr, "OI reports: %s\n", errStrings[ix]);
				++ix;
			}
		} else if(rc2 != SA_AIS_OK) {
			fprintf(stderr, "saImmOmCcbGetErrorStrings failed: %u\n", rc2);
		}
		//goto done_release;
**/
//	}
if(error != SA_AIS_OK)
                {
		// printf("error - saImmOmCcbApply FAILED: %d\n", error);
                }
} 
else if(error == SA_AIS_OK)
{
  rc = EXIT_SUCCESS;
}
if ((error = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK) {
	//	printf("error - saImmOmCcbFinalize FAILED: %d\n", error);
		//goto done_release;
                 
	}
}
}

// printf("\nThe return value%d\n",rc);
 return rc;
}


//preety printer functions
static void print_attr_value_raw(SaImmValueTypeT attrValueType, SaImmAttrValueT *attrValue)
{
    switch (attrValueType) {
	case SA_IMM_ATTR_SAINT32T:
		printf("%d", *((SaInt32T *)attrValue));
		break;
	case SA_IMM_ATTR_SAUINT32T:
		printf("%u", *((SaUint32T *)attrValue));
		break;
	case SA_IMM_ATTR_SAINT64T:
		printf("%lld", *((SaInt64T *)attrValue));
		break;
	case SA_IMM_ATTR_SAUINT64T:
		printf("%llu", *((SaUint64T *)attrValue));
		break;
	case SA_IMM_ATTR_SATIMET:
		printf("%llu",  *((SaTimeT *)attrValue));
		break;
	case SA_IMM_ATTR_SAFLOATT:
		printf("%f", *((SaFloatT *)attrValue));
		break;
	case SA_IMM_ATTR_SADOUBLET:
		printf("%17.15f", *((SaDoubleT *)attrValue));
		break;
	case SA_IMM_ATTR_SANAMET: {
		SaNameT *myNameT = (SaNameT *)attrValue;
		printf("%s", myNameT->value);
		break;
	}
	case SA_IMM_ATTR_SASTRINGT:
		printf("%s", *((char **)attrValue));
		break;
	case SA_IMM_ATTR_SAANYT: {
		SaAnyT *anyp = (SaAnyT *)attrValue;
		unsigned int i = 0;
		if(anyp->bufferSize == 0) {
			printf("-empty-");
		} else {
			printf("0x");
			for (; i < anyp->bufferSize; i++)
			{
				if(((int) anyp->bufferAddr[i]) < 0x10) {
					printf("0");
				}
				printf("%x", (int)anyp->bufferAddr[i]);
			}
		}
		break;
	}
	default:
		printf("Unknown");
		break;
	}
}


static void print_attr_value(SaImmValueTypeT attrValueType, SaImmAttrValueT *attrValue)
{
	switch (attrValueType) {
	case SA_IMM_ATTR_SAINT32T:
		printf("%d (0x%x)", *((SaInt32T *)attrValue), *((SaInt32T *)attrValue));
		break;
	case SA_IMM_ATTR_SAUINT32T:
		printf("%u (0x%x)", *((SaUint32T *)attrValue), *((SaUint32T *)attrValue));
		break;
	case SA_IMM_ATTR_SAINT64T:
		printf("%lld (0x%llx)", *((SaInt64T *)attrValue), *((SaInt64T *)attrValue));
		break;
	case SA_IMM_ATTR_SAUINT64T:
		printf("%llu (0x%llx)", *((SaUint64T *)attrValue), *((SaUint64T *)attrValue));
		break;
	case SA_IMM_ATTR_SATIMET:
		{
			char buf[32];
			const time_t time = *((SaTimeT *)attrValue) / SA_TIME_ONE_SECOND;

			ctime_r(&time, buf);
			buf[strlen(buf) - 1] = '\0';	/* Remove new line */
			printf("%llu (0x%llx, %s)", *((SaTimeT *)attrValue), *((SaTimeT *)attrValue), buf);
			break;
		}
	case SA_IMM_ATTR_SAFLOATT:
		printf("%f ", *((SaFloatT *)attrValue));
		break;
	case SA_IMM_ATTR_SADOUBLET:
		printf("%17.15f", *((SaDoubleT *)attrValue));
		break;
	case SA_IMM_ATTR_SANAMET:
		{
			SaNameT *myNameT = (SaNameT *)attrValue;
			printf("%s (%u) ", myNameT->value, myNameT->length);
			break;
		}
	case SA_IMM_ATTR_SASTRINGT:
		printf("%s ", *((char **)attrValue));
		break;
	case SA_IMM_ATTR_SAANYT: {
		SaAnyT *anyp = (SaAnyT *)attrValue;
		unsigned int i = 0;
		if(anyp->bufferSize) {
			printf("0x");
			for (; i < anyp->bufferSize; i++)
			{
				if(((int) anyp->bufferAddr[i]) < 0x10) {
					printf("0");
				}
				printf("%x", (int)anyp->bufferAddr[i]);
			}
		}
		printf(" size(%u)", (unsigned int) anyp->bufferSize);

		break;
	}
	default:
		printf("Unknown");
		break;
	}
}

static SaImmValueTypeT str2_saImmValueTypeT(const char *str)
{
	if (!str)
		return -1;

	if (strcmp(str, "SA_INT32_T") == 0)
		return SA_IMM_ATTR_SAINT32T;
	if (strcmp(str, "SA_UINT32_T") == 0)
		return SA_IMM_ATTR_SAUINT32T;
	if (strcmp(str, "SA_INT64_T") == 0)
		return SA_IMM_ATTR_SAINT64T;
	if (strcmp(str, "SA_UINT64_T") == 0)
		return SA_IMM_ATTR_SAUINT64T;
	if (strcmp(str, "SA_TIME_T") == 0)
		return SA_IMM_ATTR_SATIMET;
	if (strcmp(str, "SA_NAME_T") == 0)
		return SA_IMM_ATTR_SANAMET;
	if (strcmp(str, "SA_FLOAT_T") == 0)
		return SA_IMM_ATTR_SAFLOATT;
	if (strcmp(str, "SA_DOUBLE_T") == 0)
		return SA_IMM_ATTR_SADOUBLET;
	if (strcmp(str, "SA_STRING_T") == 0)
		return SA_IMM_ATTR_SASTRINGT;
	if (strcmp(str, "SA_ANY_T") == 0)
		return SA_IMM_ATTR_SAANYT;

	return -1;
}

 static char *get_attr_type_name(SaImmValueTypeT attrValueType)
{
	switch (attrValueType) {
	case SA_IMM_ATTR_SAINT32T:
		return "SA_INT32_T";
		break;
	case SA_IMM_ATTR_SAUINT32T:
		return "SA_UINT32_T";
		break;
	case SA_IMM_ATTR_SAINT64T:
		return "SA_INT64_T";
		break;
	case SA_IMM_ATTR_SAUINT64T:
		return "SA_UINT64_T";
		break;
	case SA_IMM_ATTR_SATIMET:
		return "SA_TIME_T";
		break;
	case SA_IMM_ATTR_SANAMET:
		return "SA_NAME_T";
		break;
	case SA_IMM_ATTR_SAFLOATT:
		return "SA_FLOAT_T";
		break;
	case SA_IMM_ATTR_SADOUBLET:
		return "SA_DOUBLE_T";
		break;
	case SA_IMM_ATTR_SASTRINGT:
		return "SA_STRING_T";
		break;
	case SA_IMM_ATTR_SAANYT:
		return "SA_ANY_T";
		break;
	default:
		return "Unknown";
		break;
	}
}
//preety printer functions



static int init_param(SaImmAdminOperationParamsT_2 *param, char *arg)
{
	int res = 0;
	char *attrValue;
	char *tmp = strdup(arg);
	char *paramType = NULL;
	char *paramName = NULL;
	int offset = 0;
        printf("\n1%d",res);
	if ((paramName = strtok(tmp, PARAMDELIM)) == NULL) {
		printf("\n2%d",res);	
		res = -1;
		goto done;
	}

	offset += strlen(paramName);

	if ((param->paramName = strdup(paramName)) == NULL) {
		printf("\n3%d",res);		
		res = -1;
		goto done;
	}

	if ((paramType = strtok(NULL, PARAMDELIM)) == NULL) {
		printf("\n4%d",res);		
		res = -1;
                printf("\n4%d",res);
		goto done;
	}

	offset += strlen(paramType);

	if ((param->paramType = str2_saImmValueTypeT(paramType)) == -1) {
		printf("\n5%d",res);		
		res = -1;
		goto done;
	}

	/* make sure there is a param value */
	if ((attrValue = strtok(NULL, PARAMDELIM)) == NULL) {
                printf("\n6%d",res);		
		res = -1;
		goto done;
	}

	/* get the attrValue. Also account for the 2 colons used to separate the parameters */
	attrValue = arg + offset + 2;

	param->paramBuffer = immutil_new_attrValue(param->paramType, attrValue);

	if (param->paramBuffer == NULL)
		return -1;

 done:
	return res;
}




void swapsi(SaImmHandleT immHandle,char *argv[],SaImmAdminOperationIdT operationId,SaNameT objectName)
{
 int rc = EXIT_SUCCESS;
 SaAisErrorT error;
 SaImmAdminOwnerHandleT ownerHandle;

 SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
// SaNameT objectName;
 const SaNameT *objectNames[] = { &objectName, NULL };
 //char s[] = "safSi=AmfDemo,safApp=AmfDemo1";
 //SaImmAdminOperationIdT operationId = 7;
  //char* optarg = "7";
 //char* optarg = "immadm -o 7 safSi=AmfDemo,safApp=AmfDemo1";
 
 SaImmAdminOperationParamsT_2 *param;
 const SaImmAdminOperationParamsT_2 **params;
 SaImmAdminOperationParamsT_2 **out_params=NULL;
 SaAisErrorT operationReturnValue = -1;
 int disable_tryagain = 0;
 unsigned long timeoutVal = 60;  /* Default timeout value */
 //timeoutVal = strtoll(optarg, (char **)NULL, 10);
 (void) alarm(timeoutVal);
 int params_len = 0;
 //SaImmAdminOperationParamsT_2 **out_params=NULL;
 //operationId = strtoll(optarg, (char **)NULL, 10);
 //if (operationId == 0) {
//				printf("Illegal operation ID\n");
//				exit(EXIT_FAILURE);
//			}
 params = realloc(NULL, sizeof(SaImmAdminOperationParamsT_2 *));
 params[0] = NULL;

 //immutilWrapperProfile.errorsAreFatal = 0;
 //immutilWrapperProfile.nTries = disable_tryagain ? 0 : timeoutVal;
 //immutilWrapperProfile.retryInterval = 1000;
/**
 params_len++;
 params = realloc(params, (params_len + 1) * sizeof(SaImmAdminOperationParamsT_2 *));
 param = malloc(sizeof(SaImmAdminOperationParamsT_2));
 params[params_len - 1] = param;
 params[params_len] = NULL;
 //if (init_param(param, optarg) == -1) {
 //printf("Illegal parameter: %s\n", optarg);
 //exit(EXIT_FAILURE);
//}

**/

/**
 error = saImmOmInitialize( &immHandle, NULL, &immVersion);
	if (error != SA_AIS_OK) {
		printf("error - saImmOmInitialize FAILED:%d",error);
		error= EXIT_FAILURE;
                      }
       if (error == SA_AIS_OK)
          printf("\nIMM handle gained\n");
**/
  error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       error = EXIT_FAILURE;

		      }
 


  // strncpy((char *)objectName.value, s, SA_MAX_NAME_LENGTH);
//		objectName.length = strlen((char *)objectName.value);

  
   error = saImmOmAdminOwnerSet(ownerHandle, objectNames, SA_IMM_ONE);
		if (error != SA_AIS_OK) {
			if (error == SA_AIS_ERR_NOT_EXIST)
				printf("error - saImmOmAdminOwnerSet - object '%s' does not exist\n",
					objectName.value);
			else
				printf("error - saImmOmAdminOwnerSet FAILED: %d\n", error);
			exit(EXIT_FAILURE);
		}

 
   error = saImmOmAdminOperationInvoke_o2(ownerHandle, &objectName, 0, operationId,
			params, &operationReturnValue, SA_TIME_ONE_SECOND * 60,&out_params);
   
  if ( operationReturnValue != SA_AIS_OK) {
	//		printf("error - operationReturnValue: %d\n",operationReturnValue);
			//exit(EXIT_FAILURE);
		}
   if (error != SA_AIS_OK) {
	//		printf("error - saImmOmAdminOperationInvoke_2 FAILED: %d\n",error);
			//exit(EXIT_FAILURE);
		}
   if(error == SA_AIS_OK)
   {
      //printf("\nSuccesfull\n");
   }
error = saImmOmAdminOperationMemoryFree(ownerHandle, out_params);
		if (error != SA_AIS_OK) {
			printf("error - saImmOmAdminOperationMemoryFree FAILED: %d\n", error);
			exit(EXIT_FAILURE);
		}
   error = saImmOmAdminOwnerRelease(ownerHandle, objectNames, SA_IMM_ONE);
   if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerRelease FAILED: %d\n", error);
		exit(EXIT_FAILURE);
	} 
  
         error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n", error);
		exit(EXIT_FAILURE);
	}

	

}



/**
 * Alloc SaImmAttrValuesT_2 object and initialize its attributes from nameval (x=y)
 * @param className
 * @param nameval
 *
 * @return SaImmAttrValuesT_2*
 */
static SaImmAttrValuesT_2 *new_attr_value(SaImmClassNameT className, char *nameval, int isRdn)
{
          //printf("\n9");
	int res = 0;
	char *name = strdup(nameval), *p;
	char *value;
	SaImmAttrValuesT_2 *attrValue;
	SaAisErrorT error;

	attrValue = malloc(sizeof(SaImmAttrValuesT_2));

	p = strchr(name, '=');
	if (p == NULL){
		//sprintf("The Attribute '%s' does not contain a equal sign ('=')\n", nameval);
		res = -1;
		goto done;
	}
	*p = '\0';
	value = p + 1;

	attrValue->attrName = strdup(name);
	//VERBOSE_INFO("new_attr_value attrValue->attrName: '%s' value:'%s'\n", attrValue->attrName, isRdn ? nameval : value);

	error = get_attrValueType(className, attrValue->attrName, &attrValue->attrValueType);

	if (error == SA_AIS_ERR_NOT_EXIST) {
		fprintf(stderr, "Class '%s' does not exist\n", className);
		res = -1;
		goto done;
	}

	if (error != SA_AIS_OK) {
		fprintf(stderr, "Attribute '%s' does not exist in class '%s'\n", name, className);
		res = -1;
		goto done;
	}

	attrValue->attrValuesNumber = 1;
	attrValue->attrValues = malloc(sizeof(SaImmAttrValueT *));
	attrValue->attrValues[0] = immutil_new_attrValue(attrValue->attrValueType, isRdn ? nameval : value);
	if(attrValue->attrValues[0] == NULL)
		res = -1;

 done:
	free(name);
	if (res != 0) {
		free(attrValue);
		attrValue = NULL;
	}

	return attrValue;
}


int object_create(const SaNameT **objectNames,SaImmAdminOwnerHandleT ownerHandle, char *optargs[],int optargs_len,char *classname)
{
    //    printf("\nI got a call");

	SaAisErrorT error;
        

      
	int i;
       // char *optargs[2];
       // int optargs_len=1;
	SaImmAttrValuesT_2 *attrValue;
	SaImmAttrValuesT_2 **attrValues = NULL;
	int attr_len = 1;
	int rc = EXIT_FAILURE;
	char *parent = NULL;
	SaNameT dn;
	SaNameT *parentName = NULL;
	char *str, *delim;
	const SaNameT *parentNames[] = {parentName, NULL};
	SaImmCcbHandleT ccbHandle;
	const SaStringT* errStrings=NULL;
      //  optargs[0]="saAmfSUType=safVersion=1,safSuType=AmfDemo2";
       // optargs[1]="saAmfSUAdminState=3";
        //optargs[0]="saAmfSUType=safVersion=1,safSuType=AmfDemo2";

	for (i = 0; i < optargs_len; i++) {
               // printf("\n7");
		attrValues = realloc(attrValues, (attr_len + 1) * sizeof(SaImmAttrValuesT_2 *));
		//VERBOSE_INFO("object_create optargs[%d]: '%s'\n", i, optargs[i]);
		if ((attrValue = new_attr_value(classname, optargs[i], 0)) == NULL){
			printf("error - creating attribute from '%s'\n", optargs[i]);
			//goto done;
		}

		attrValues[attr_len - 1] = attrValue;
		attrValues[attr_len] = NULL;
		attr_len++;
	}

	if ((error = saImmOmCcbInitialize(ownerHandle, ccb_safe?defCcbFlags:0x0, &ccbHandle)) 
		!= SA_AIS_OK) {
		printf("error - saImmOmCcbInitialize FAILED: %d\n", error);
		//goto done;
	}

	i = 0;
	while (objectNames[i] != NULL) {
		str = strdup((char*)objectNames[i]->value);
		if ((delim = strchr(str, ',')) != NULL) {
			
			while (*(delim - 1) == 0x5c) {
				
				delim += 2;
				delim = strchr(delim, ',');
			}

			*delim = '\0';
			parent = delim + 1;
			if (!parent) {
				printf("error - malformed object DN\n");
				//goto done;
			}

			dn.length = sprintf((char*)dn.value, "%s", parent);
			parentName = &dn;
			parentNames[0] = parentName;

			//VERBOSE_INFO("call saImmOmAdminOwnerSet for parent: %s\n", parent);
			if ((error = saImmOmAdminOwnerSet(ownerHandle, parentNames, SA_IMM_SUBTREE)) != SA_AIS_OK) {
				if (error == SA_AIS_ERR_NOT_EXIST)
					printf("error - parent '%s' does not exist\n", dn.value);
				else {
					printf("error - saImmOmAdminOwnerSet FAILED: %d\n", error);
					
				}
			}
		}

		attrValues = realloc(attrValues, (attr_len + 1) * sizeof(SaImmAttrValuesT_2 *));
		//VERBOSE_INFO("object_create rdn attribute attrValues[%d]: '%s' \n", attr_len - 1, str);
		if ((attrValue = new_attr_value(classname, str, 1)) == NULL){
			printf("error - creating rdn attribute from '%s'\n", str);
			
		}
		attrValues[attr_len - 1] = attrValue;
		attrValues[attr_len] = NULL;

		if ((error = saImmOmCcbObjectCreate_2(ccbHandle,classname, parentName,(const SaImmAttrValuesT_2**)attrValues)) != SA_AIS_OK) {

			printf("\nerror - saImmOmCcbObjectCreate_2 FAILED with %d\n",
				error);

			if((error == SA_AIS_ERR_NOT_EXIST) && ccb_safe) {
				printf("Missing: implementer, or object, or attribute "
					"(see: immcfg -h under '--unsafe')\n");
			} else {
				SaAisErrorT rc2 = saImmOmCcbGetErrorStrings(ccbHandle, &errStrings);
				if(errStrings) {
					int ix = 0;
					while(errStrings[ix]) {
						printf("OI reports: %s\n", errStrings[ix]);
						++ix;
					}
				} else if(rc2 != SA_AIS_OK) {
					printf("saImmOmCcbGetErrorStrings failed: %u\n", rc2);
				}
			}
			//goto done;
		}
		i++;
	}

	if ((error = saImmOmCcbApply(ccbHandle)) != SA_AIS_OK) {

		if(error == SA_AIS_ERR_TIMEOUT) {
			printf("saImmOmCcbApply returned SA_AIS_ERR_TIMEOUT, result for CCB is unknown\n");
			//goto done_release;
		}
		
		printf("error - saImmOmCcbApply FAILED: %d\n", error);
		SaAisErrorT rc2 = saImmOmCcbGetErrorStrings(ccbHandle, &errStrings);
		if(errStrings) {
			int ix = 0;
			while(errStrings[ix]) {
				printf("OI reports: %s\n", errStrings[ix]);
				++ix;
			}
		} else if(rc2 != SA_AIS_OK) {
			printf("saImmOmCcbGetErrorStrings failed: %u\n", rc2);
		}
		
		//goto done_release;
	}
        if(error = SA_AIS_OK)
        {
                printf("\nThe object is successfully created");
        }

	if ((error = saImmOmCcbFinalize(ccbHandle)) != SA_AIS_OK) {
		printf("error - saImmOmCcbFinalize FAILED: %d\n", error);
		//goto done_release;
	}

	rc = 0;



	return rc;
}


////////////////////////////////////////////////////////////Buffer2_manager_increase_start//////////////////////////////////////////////
void buffer2_checkker(SaImmHandleT immHandle,SaNameT objectnameSG1,SaNameT objectnameSI,char *argv[])
{
// printf("\n I will check the buffer2");
 
   printf("%c[1;31m\n***********************************************************************************",27);

   printf("\n************************************** Buffer2 Manager **********************************");
  printf("%c[0m",27);

    SaImmAdminOwnerHandleT ownerHandle;
 SaNameT **objectNames = NULL;
 SaNameT *objectNamec;
 char *optargs[2];
SaNameT **objectNames1 = NULL;
 SaNameT *objectNamec1;
 char *optargs1[1];

   SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
   //printf("\nI am gonna check the buffer");
   int buffer1,buffer2,assigned,actSUs,stdSUs,insSUs;
   SaImmAccessorHandleT accessorHandle,accessorHandle1,accessorHandleX;
   SaImmSearchHandleT searchHandle;
   SaImmAttrValuesT_2 **attributes,**attributesSU2,**attributes1,**attributes2;
   int rc = EXIT_SUCCESS,uninstantiated = 0,noassgSUs;
   char noassg[256];
  char nameSI[256];
  SaNameT objtnamesgtype,namevalidsutype;
  SaNameT objectnameSG11;
  SaNameT objectName;
  SaImmScopeT scope = SA_IMM_SUBTREE;	/* default search scope */
  SaImmSearchParametersT_2 searchParam;
  SaNameT rootName = { 0, "" };
int i=0,found=0;
   SaAisErrorT error;



 int c;
 SaAisErrorT errorc;

 

 //SaNameT **objectNames = NULL;
 //SaNameT *objectNamec;
 //int objectNames_len = 1;
 //char *s = "safSg=AmfDemo,safApp=AmfDemo1";
 char *s;
 




  
 
   char buff[256];
   SaNameT nodename;
   int NodeHosting;
   int objectNames_len = 1,flag=0;
   char string[256];
  strcpy(string,"safSu=SU");
  char buff1[256];
   SaNameT objectname;
   char nameccbattribute[256];
   SaImmAttrNameT  attributeNames[2] ={"saAmfSGNumPrefActiveSUs",NULL};


   loop31 :
//printf("\nIN the loop again");
   attributeNames[0] = "saAmfSGNumCurrNonInstantiatedSpareSUs";
   attributeNames[1] = NULL;
   SaNameT *objectnameSG2;

   error = saImmOmAccessorInitialize( immHandle, &accessorHandleX);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandleX, &objectnameSG1, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		 printf("\n7");
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		uninstantiated = *((SaUint32T *)attrValues);
                		             						
						
						
					        	
                                                 }
                                       

 printf("\n The number of uninstantiated spare SUs of SG are %d",uninstantiated);                                       
}
error = saImmOmAccessorFinalize(accessorHandleX);
                                      strcpy(nameSI,"safBuff=");
                                      strcat(nameSI,objectnameSG1.value);
                                      //nameSI = "safBuff=safSg=AmfDemo,safApp=AmfDemo2";
                                      objectnameSG11.length = strlen(nameSI);

                                      strncpy((char *)objectnameSG11.value, nameSI, SA_MAX_NAME_LENGTH);
                                      printf("\nThe Buffer object is :%s",objectnameSG11.value);
                
                                        attributeNames[0] ="saSpareSUBuff";
                                        attributeNames[2] =NULL;
                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandleX);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandleX, &objectnameSG11, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		buffer2 = *((SaUint32T *)attrValues);
                		             						
						
						
					        	
                                                 }

                                        }
                                     error = saImmOmAccessorFinalize(accessorHandleX);
          
 if(uninstantiated > buffer2)
 {

 }
 else
 {
   printf("\n Need to configure SU");
   					attributeNames[0] ="saAmfSGSuHostNodeGroup";
                                        attributeNames[2] =NULL;
                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandleX);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandleX, &objectnameSG1, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValue1 = attributes[0]->attrValues[0];
	  						objectnameSG2 = (SaNameT *)attrValue1;
	//printf("\nThe SG valid host node group is : %s (%u) \n", objectnameSG2->value,objectnameSG2->length);
          
	  
	  						strncpy((char *)objectname.value, objectnameSG2->value, SA_MAX_NAME_LENGTH);
         						objectname.length=strlen(objectname.value);
                		             						
						
						
					        	
                                                        printf("\nThe node group is %s",objectname.value);
                                                   
                                                 }
                                       }
error = saImmOmAccessorFinalize(accessorHandleX);
                                        attributeNames[0] ="saAmfNGNodeList";
                                        attributeNames[2] =NULL;
                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectname, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{


                                
					

                                                if (attributes[0]->attrValuesNumber > 0) {
                                                found=0;
                                           //  printf("\nthe number of %d",attributes[0]->attrValuesNumber);
						printf("\nThe list of node is:");
			for (i = 0; i < attributes[0]->attrValuesNumber; i++)
			                     	{

                                                        //  printf("\n%d",i);
                                                          SaImmAttrValueT *attrValue1 = attributes[0]->attrValues[i];
	  						  objectnameSG2 = (SaNameT *)attrValue1;
	                                                  strncpy((char *)objectname.value, objectnameSG2->value, SA_MAX_NAME_LENGTH);
         						  objectname.length=strlen(objectname.value);
                                                        printf("\n%s",objectname.value);
                                                }
				                for (i = 0; (i < attributes[0]->attrValuesNumber) && (found==0); i++)
			                     	{
					     		
                                                                  
                                                                   
                                              


                                                        //  printf("\n%d",i);
                                                          SaImmAttrValueT *attrValue1 = attributes[0]->attrValues[i];
	  						  objectnameSG2 = (SaNameT *)attrValue1;
	                                                  strncpy((char *)objectname.value, objectnameSG2->value, SA_MAX_NAME_LENGTH);
         						  objectname.length=strlen(objectname.value);
                                                         // printf("\nThe list of nodes is %s",objectname.value);
                                                         
                                                                        attributeNames[0]="saAmfSGType";
  								        attributeNames[1]=NULL;
                                                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle1);
                                                                      
 									if (error != SA_AIS_OK) {
                                                                     
                							printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                							rc= EXIT_FAILURE;
                      							}
       									if (error == SA_AIS_OK)
   									{
                                                                               
  										//     printf("\nWe got the accessor handle to access SG\n");
         
      											error= saImmOmAccessorGet_2(accessorHandle1, &objectnameSG1, attributeNames, &attributes1);
                                                                                  
											if (error !=SA_AIS_OK)
											{
												 printf("\n%d",error);
         											 rc= EXIT_FAILURE;
                                                                                        
											}


											if (error ==SA_AIS_OK)
											{
												//printf("\nsaImmOmAccessorGet_2 function returned the SG object and attribute\n"); 
                                                                                           

												SaImmAttrValueT *attrValue = attributes1[0]->attrValues[0];
												SaNameT *objtnamevalidsutype = (SaNameT *)attrValue;


 												strncpy(objtnamesgtype.value,objtnamevalidsutype->value, SA_MAX_NAME_LENGTH);
 												objtnamesgtype.length=strlen(objtnamesgtype.value);


											}
											error = saImmOmAccessorFinalize(accessorHandle1);
                                                                        //  printf("\nThe saAmfSGType of %s SG  is %s\n",objectnameSG1.value,objtnamesgtype.value);
									}
                                                            
                                                                        attributeNames[0]="saAmfSgtValidSuTypes";
  								        attributeNames[1]=NULL;
 									error = saImmOmAccessorInitialize( immHandle, &accessorHandle1);
 									if (error != SA_AIS_OK) {
                									printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                									rc= EXIT_FAILURE;
                      								}
       									if (error == SA_AIS_OK)
   									{
       										//printf("\nWe got the accessor handle to access SG\n");
  												// } 

										error= saImmOmAccessorGet_2(accessorHandle1, &objtnamesgtype, attributeNames, &attributes1);

										if(error == SA_AIS_OK)
										{
 											//printf("\nok ok");
 											SaImmAttrValueT *attrValue = attributes1[0]->attrValues[0];
 											SaNameT *objtnamevalidsutype = (SaNameT *)attrValue;
 											
//printf("\n\n%u\n\n",x);

											strncpy((char *)namevalidsutype.value, objtnamevalidsutype->value, SA_MAX_NAME_LENGTH);
                                                                                        namevalidsutype.length = strlen(namevalidsutype.value);

										}
									error = saImmOmAccessorFinalize(accessorHandle1);
        							//	printf("\nThe saAmfSgtValidSuTypes of %s SG  is %s\n",objectnameSG1.value,namevalidsutype.value);
                                                                         }




//printf("\n1");





                                                                         searchParam.searchOneAttr.attrName = "saAmfSUType";
 								    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam.searchOneAttr.attrValue = &namevalidsutype;

								    error = saImmOmSearchInitialize_2(immHandle,&objectnameSG1 , scope,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							//flag=1;

			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");

                                			                        NodeHosting=0;
            								do {      
                                                                                        
                                                                                        // printf("\n5");         
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes1);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
            
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  							
                             										rc=EXIT_FAILURE;
                             										flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK && flag==0)
                  				  					{
 
							   					attributeNames[0] ="saAmfSUHostedByNode";
							  				        attributeNames[1] =NULL;
							   					error = saImmOmAccessorInitialize( immHandle, &accessorHandle1);
 			       			 	   					if (error != SA_AIS_OK) {
                	       										printf("error12 - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       										rc= EXIT_FAILURE;
													//flag = 1;
                      				   	     					}
       			       			 	  					if (error == SA_AIS_OK)
   						  						{

													error= saImmOmAccessorGet_2(accessorHandle1, &objectName, attributeNames, &attributes2);
                                        								if (error !=SA_AIS_OK)
                                        								{
                                               									printf("\nerror18:%d",error);
														rc= EXIT_FAILURE;
                                                        							 
                                        		        					 }
                        	 									 if (error ==SA_AIS_OK)
                                 									 {

			                     		                                                        //no_of_SIs++;
					     									SaImmAttrValueT *attrValues = attributes2[0]->attrValues[0];
					    									nodename = *((SaNameT *)attrValues);
                                                              	  
                		   	               	 							error = saImmOmAccessorFinalize(accessorHandle1);
							       						 }
                                                                                                   
       													//printf("\n The SU %s is hosted on %s NODE",objectName.value,nodename.value);
                                                                                                        if(strcmp(nodename.value,objectname.value)==0)
                                                                                                        {

                                                                                                             NodeHosting =1;
                                                                                                             break;
                                                                                                        }
                        		                 					}//accesor initialize
                                                                                           }//search next 
                                               	

                                         						
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST && flag !=1);
                                                                                  error = saImmOmSearchFinalize(searchHandle);    
                   								
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
                                                                      //   printf("\n The nodehosting is %d",NodeHosting);
                                                                           if(NodeHosting!=1)
                                                                           {
                                                                                    found = 1;
                                                                           }




                                                                   

                                                  }//for
                   
                                                     error = saImmOmAccessorFinalize(accessorHandle);
                                                     if(found == 0)
                                                     {
                                                                  printf("\n Need to increase Nodes"); 
                                                     }
                                                     else
                                                     {
                                                                 
                                      //////////////////////////////////////// create CCB ro instantiate SU
                                                                    printf("%c[1;33m\nCreate a CCB to instantiate SU\n",27);
            printf("Creating...\n");
             ////???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
//printf("%d",ServiceUnit);
sprintf(buff,"%d,",ServiceUnit);

ServiceUnit++;
//printf("After:%d",ServiceUnit);
strcat(string,buff);

//strcat(string,",");
strcat(string,objectnameSG1.value);
printf("\nthe SU name is %s",string);
  objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 objectNamec = objectNames[objectNames_len - 1] = malloc(sizeof(SaNameT));
 objectNames[objectNames_len++] = NULL;
 objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", string);


////attributes
 //optargs[0]="saAmfSUType=safVersion=1,safSuType=AmfDemo2";
 strcpy(buff,"saAmfSUType=");
 strcat(buff,namevalidsutype.value);
 optargs[0]=buff;
 optargs[1]="saAmfSUAdminState=3";



 errorc = saImmOmInitialize(&immHandle, NULL, &immVersion);
        if (errorc != SA_AIS_OK) {
                printf("errorc - saImmOmInitialize FAILED: %d\n", errorc);
                rc = EXIT_FAILURE;  
               // goto done_om_finalize;             
        }//immomIniialize
 errorc = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
	if (errorc != SA_AIS_OK) {
		printf("errorc - saImmOmAdminOwnerInitialize FAILED: %d\n", errorc);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}

// rc = object_modify((const SaNameT **)objectNames, ownerHandle); 
rc = object_create( (const SaNameT **)objectNames,ownerHandle,optargs,2,"SaAmfSU"); 

  printf("%c[0m",27);
errorc = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != errorc) {
		printf("errorc - saImmOmAdminOwnerFinalize FAILED: %d\n",errorc);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
 //done_om_finalize:
 errorc = saImmOmFinalize(immHandle);
	if (SA_AIS_OK != errorc) {
                printf("errorc - saImmOmFinalize FAILED: %d\n", errorc);
                rc = EXIT_FAILURE;
        }

///////////////////////////component creation of the SU////////////////////////////////////////////////////////////////////
objectNames_len = 1;
strcpy(buff1,"safComp=AmfDemo,");
strcat(buff1,objectNamec->value);
printf("\nthe Comp name is %s",buff1);
printf("\nthe digit:%d\n",objectNames_len);
  objectNames1 = realloc(objectNames1, (objectNames_len + 1) * sizeof(SaNameT*));
 objectNamec1 = objectNames1[objectNames_len - 1] = malloc(sizeof(SaNameT));
 objectNames1[objectNames_len++] = NULL;
 objectNamec1->length = snprintf((char*)objectNamec1->value, SA_MAX_NAME_LENGTH, "%s", buff1);

 optargs1[0]="saAmfCompType=safVersion=1,safCompType=AmfDemo2";
// optargs1[1]="saAmfSUAdminState=3";

errorc = saImmOmInitialize(&immHandle, NULL, &immVersion);
        if (errorc != SA_AIS_OK) {
                printf("errorc - saImmOmInitialize FAILED: %d\n", errorc);
                rc = EXIT_FAILURE;  
               // goto done_om_finalize;             
        }//immomIniialize
 errorc = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
	if (errorc != SA_AIS_OK) {
		printf("errorc - saImmOmAdminOwnerInitialize FAILED: %d\n", errorc);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}

// rc = object_modify((const SaNameT **)objectNames, ownerHandle); 
rc = object_create( (const SaNameT **)objectNames1,ownerHandle,optargs1,1,"SaAmfComp"); 

  printf("%c[0m",27);

errorc = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != errorc) {
		printf("errorc - saImmOmAdminOwnerFinalize FAILED: %d\n",errorc);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
 //done_om_finalize:
 errorc = saImmOmFinalize(immHandle);
	if (SA_AIS_OK != errorc) {
                printf("errorc - saImmOmFinalize FAILED: %d\n", errorc);
                rc = EXIT_FAILURE;
        }
// exit(rc);

             ////???????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
            
                                                     // error = saImmOmAccessorFinalize(accessorHandleX);               
                                                                ///////////////////////This is left ///////////////////////////////////////
                                                               // goto loop31;

                                                     }
       

                                                }//if
                                             }
                                       }
     
 }
 

}
////////////////////////////////////////////////////////////Buffer2_manager_increase_end//////////////////////////////////////////////


////////////////////////////////////////////////////////////Buffer1_manager_increase_start//////////////////////////////////////////////
void buffer_checkker(SaImmHandleT immHandle,SaNameT objectnameSG1,SaNameT objectnameSI,int isnpm,char *argv[])
{
   printf("%c[1;31m\n***********************************************************************************",27);
   printf("\n************************************** Buffer Manager **********************************");
   printf("%c[0m",27);
    SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
   //printf("\nI am gonna check the buffer");
   int buffer1,buffer2,assigned,actSUs,stdSUs,insSUs;
   SaImmAccessorHandleT accessorHandle;
   SaImmSearchHandleT searchHandle;
   SaImmAttrValuesT_2 **attributes,**attributesSU2;
   int rc = EXIT_SUCCESS,uninstantiated = 0,noassgSUs;
    char noassg[256];
   SaAisErrorT error;
   SaImmAdminOwnerHandleT ownerHandle;
    SaNameT **objectNames = NULL;
    SaNameT *objectNamec;
    int objectNames_len = 1;
    char *s;
   char nameSI[256];
  SaNameT objectnameSG11;
 char nameccbattribute[256];
   SaImmAttrNameT  attributeNames[2] ={"saAmfSGNumPrefActiveSUs",NULL};
   loop30 :
   attributeNames[0] = "saAmfSGNumPrefActiveSUs";
   attributeNames[1] = NULL;
   if (isnpm == 1)
   {
      //the SG is N+M,hence assigned =activeSUs+StandbySUs
      
					error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		actSUs = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
                                                 }

                                        }
            //printf("\nActiveSUs : %d",actSUs);
       
                                        attributeNames[0] ="saAmfSGNumPrefStandbySUs";
                                        attributeNames[2] =NULL;
                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		stdSUs = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
                                                 }

                                        }
          assigned = stdSUs + actSUs;
          printf("\n The number of assigned SUs of SG are %d",assigned);
   }
   else
   {
     //the SG is not N+M,hence assigned =assignedSUs
     attributeNames[0] ="saAmfSGNumPrefAssignedSUs";
                                        attributeNames[2] =NULL;
                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		stdSUs = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
                                                 }

                                        }
          assigned = stdSUs;
          printf("\n The number of assigned SUs of SG are %d",assigned);
   }
                                        attributeNames[0] ="saAmfSGNumPrefInserviceSUs";
                                        attributeNames[2] =NULL;
                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		insSUs = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
                                                 }
                  printf("\n The number of inservice SUs of SG are %d",insSUs);

                                        }
                                      strcpy(nameSI,"safBuff=");
                                      strcat(nameSI,objectnameSG1.value);
                                      //nameSI = "safBuff=safSg=AmfDemo,safApp=AmfDemo2";
                                      objectnameSG11.length = strlen(nameSI);

                                      strncpy((char *)objectnameSG11.value, nameSI, SA_MAX_NAME_LENGTH);
                                      printf("\nTHe BUffer :%s",objectnameSG11.value);
                
                                        attributeNames[0] ="saInserviceBuff";
                                        attributeNames[2] =NULL;
                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG11, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		buffer1 = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
                                                 }

                                        }
          
          printf("\n The number of inservice SUs buffer of SG is %d",buffer1);
          printf("\n Checking if the buffer condition is satisfied ....");
          if((insSUs - assigned) > buffer1)
          {
              printf("\n The buffer condition is satisfied ....");
              printf("\n Check buffer2\n");
              buffer2_checkker(immHandle,objectnameSG1,objectnameSI,argv);
          }
          else
          {
              printf("\n The buffer condition is not satisfied ....");
              printf("\n Check to increase Inservice SUs");
/*
              attributeNames[0] ="saAmfSGNumCurrNonInstantiatedSpareSUs";
                                        attributeNames[2] =NULL;
                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		uninstantiated = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
                                                 }

                                        }
              printf("\n The number of inservice SUs of SG are %d",uninstantiated);
              uninstantiated = 2;
*/
/////FUNC CALL//////
uninstantiated = num_SUofSG(immHandle,objectnameSG1);
/////FUNC CALL//////
              if(uninstantiated > insSUs)
              {
                  printf("%c[1;31m\nThere are Spare SUs to increase inservice SUs\n",27);
                     printf("%c[0m",27);
                      printf("%c[1;33m\nCreate a CCB to increase saAmfSGNumPrefInserviceSUs\n",27);
                      printf("Creating...\n");
                    swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
                  //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		      noassgSUs = insSUs + 1; 
		          strcpy(nameccbattribute,"saAmfSGNumPrefInserviceSUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	    //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
	printf("%c[0m",27);
        goto loop30;
              }
               
               else
               {
                        printf("%c[1;31m\n Need to increase Nodes\n",27);
                        printf("%c[0m",27);
               }
                     
              
          }
         

}
////////////////////////////////////////////////////////////Buffer1_manager_increase_end//////////////////////////////////////////////


/////////////////////////////////Buffer_checker_for_decrease_START////////////////////////////////
void buffer_checkker_decrease(SaImmHandleT immHandle,SaNameT objectnameSG1,SaNameT objectnameSI,int i,char *argv[],int preinstantiable)
{
printf("%c[1;31m\n***********************************************************************************",27);
   printf("\n************************************** Buffer Manager **********************************");
printf("%c[0m",27);
   
    SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
   //printf("\nI am gonna check the buffer");
   int buffer,assigned,actSUs,stdSUs,insSUs;
   SaImmAccessorHandleT accessorHandle;
   SaImmSearchHandleT searchHandle;
   SaImmAttrValuesT_2 **attributes,**attributesSU2;
   int rc = EXIT_SUCCESS,uninstantiated = 0,noassgSUs;
    char noassg[256];
   SaAisErrorT error;
   SaImmAdminOwnerHandleT ownerHandle;
    SaNameT **objectNames = NULL;
    SaNameT *objectNamec;
    int objectNames_len = 1;
    char *s;
     char nameSI[256];
    SaNameT objectnameSG11;
 char nameccbattribute[256];
   SaImmAttrNameT  attributeNames[2] ={"saAmfSGNumPrefActiveSUs",NULL};
   loop32 :
   attributeNames[0] ="saAmfSGNumPrefActiveSUs";
   attributeNames[1] = NULL;
   if (i == 1)
   {
      //the SG is N+M,hence assigned =activeSUs+StandbySUs
      
					error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		actSUs = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
                                                 }

                                        }
            //printf("\nActiveSUs : %d",actSUs);
       
                                        attributeNames[0] ="saAmfSGNumPrefStandbySUs";
                                        attributeNames[2] =NULL;
                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18: %d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                    		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		stdSUs = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
                                                 }

                                        }
          assigned = stdSUs + actSUs;
          printf("\n The number of assigned SUs of SG are %d",assigned);
   }
   else
   {
 
     //the SG is not N+M,hence assigned =assignedSUs
     attributeNames[0] ="saAmfSGNumPrefAssignedSUs";
                                        attributeNames[2] =NULL;
                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		stdSUs = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
                                                 }

                                        }
  
          assigned = stdSUs;
          printf("\n The number of assigned SUs of SG are %d",assigned);
   }
if(preinstantiable == 1)
  {
                                        attributeNames[0] ="saAmfSGNumPrefInserviceSUs";
                                        attributeNames[2] =NULL;
                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		insSUs = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
                                                 }

                                        }
         
          printf("\n The number of inservice SUs of SG are %d",insSUs);
}
else
{
       insSUs =  num_SUofSG(immHandle,objectnameSG1);  
}
strcpy(nameSI,"safBuff=");
                                      strcat(nameSI,objectnameSG1.value);
                                      //nameSI = "safBuff=safSg=AmfDemo,safApp=AmfDemo2";
                                      objectnameSG11.length = strlen(nameSI);

                                      strncpy((char *)objectnameSG11.value, nameSI, SA_MAX_NAME_LENGTH);
                                      printf("\nTHe BUffer :%s",objectnameSG11.value);
                
                                        attributeNames[0] ="saInserviceBuff";
                                        attributeNames[2] =NULL;
                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						//flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG11, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        //flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		buffer = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
                                                 }

                                        }
          
          printf("\n The number of inservice SUs buffer of SG is %d",buffer);
          printf("\n Checking if the buffer condition is satisfied ....");
printf("\n Checking to decrease the Buffer");
      if ((insSUs - assigned) <= buffer)
      {
            
      }
      else
      {
          printf("%c[1;31m\nWe decrease number of inservice SUs\n",27);
                     printf("%c[0m",27);
                      printf("%c[1;33m\nCreate a CCB to decrease saAmfSGNumPrefInserviceSUs\n",27);
                      printf("Creating...\n");
                    swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
                  //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		      noassgSUs = insSUs - 1; 
		          strcpy(nameccbattribute,"saAmfSGNumPrefInserviceSUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	    //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
	printf("%c[0m",27);
          goto loop32;      
printf("%c[1;31m\n***********************************************************************************",27);
 
printf("%c[0m",27);
      }
}
/////////////////////////////////Buffer_checker_for_decrease_END////////////////////////////////

//int single_SIdecrease2n(SaImmHandleT immHandle,SaInt8T nameSI[256],SaNameT objectnameSG1,char *argv[])
int single_SIdecrease2n(SaImmHandleT immHandle,SaNameT objectnameSG1,char *argv[],SaNameT objectnameSI)
{
// printf("\n I got a call");
   SaAisErrorT error,errorc;
   SaImmAccessorHandleT accessorHandle;
   SaImmSearchHandleT searchHandle;
   SaImmAttrValuesT_2 **attributes,**attributesSU2;
   SaImmAttrNameT  attributeNames[2] ={"saAmfSGType",NULL};
   SaNameT objectnamesgtype1,objectname,objectNameSU,objectnameact,objectnamesutype,objectnameSUTemp,objectname1,objectNameSU1;
   SaImmSearchParametersT_2 searchParam;
   int flag=0,flag1=0,sus=0,foundsu =0;
   int activeSis,compareLimit=100,standbySis=0,activeSis1=0,standbySis1=0;
   SaImmScopeT scope = SA_IMM_SUBTREE;	/* default search scope */
   int rc = EXIT_SUCCESS;
   SaUint32T rankact,surankTemp;
 //ccb
    int rc1 = EXIT_FAILURE;
    int rt;
    SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
    SaImmAdminOwnerHandleT ownerHandle;
    SaNameT **objectNames = NULL;
    SaNameT *objectNamec;
    int objectNames_len = 1;
 //char *s = "safSg=AmfDemo,safApp=AmfDemo1";
 char *s;
 char nameccbattribute[256],rankt[256];
   //ccb
   
   //
      error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
                                                flag =1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1,attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror16:%d",error);
							 flag =1;
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		SaNameT *objtnamesgtype = (SaNameT *)attrValues;
                		             		//error = saImmOmAccessorFinalize(accessorHandle15);

							//printf("\nThe SG type is set to %s ", objtnamesgtype->value);
							strncpy((char *)objectname.value, objtnamesgtype->value, SA_MAX_NAME_LENGTH);
							objectname.length= strlen(objectname.value);
						}
				error = saImmOmAccessorFinalize(accessorHandle);
 				    }
if (flag!= 1)
{
	attributeNames[0] ="saAmfSgtValidSuTypes";
	attributeNames[1] =NULL;
	error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectname, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror17:%d",error);
							rc= EXIT_FAILURE;
                                                        flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		SaNameT *objtnamesgtype12 = (SaNameT *)attrValues;
                		             		//error = saImmOmAccessorFinalize(accessorHandle15);

							//printf("\nThe Su type is set to %s ", objtnamesgtype12->value);
							strncpy((char *)objectnamesutype.value, objtnamesgtype12->value, SA_MAX_NAME_LENGTH);
							objectnamesutype.length= strlen(objectname.value);
						}
				error = saImmOmAccessorFinalize(accessorHandle);
 				  }
}//flag

if (flag!= 1)
{
 // printf("\n flag is not set");
 searchParam.searchOneAttr.attrName = "saAmfSUType";
 searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 searchParam.searchOneAttr.attrValue = &objectnamesutype;

error = saImmOmSearchInitialize_2(immHandle, &objectnameSG1, scope,
					  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,
					  &searchHandle);
	if (SA_AIS_OK != error) {
		//printf("error - saImmOmSearchInitialize_2 FAILED");
                printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                rc=EXIT_FAILURE;
                flag=1;
	}
 
        

        if (SA_AIS_OK == error)
        {
		         printf("\nSearch the active SU in the SG\n");
		           

            do {
          	error = saImmOmSearchNext_2(searchHandle, &objectNameSU, &attributesSU2);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       // printf("\n\n How r u\n\n");
			printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                        rc=EXIT_FAILURE;
			flag=1;
		}
                if(error == SA_AIS_ERR_NOT_EXIST )
                {
		     if(flag1==1)
                     { 
                     	//arraySU[counter]==NULL;
                     	//printf("\nobject does not exist"); 
        	     }
		     else
		     { 
                       rc=EXIT_FAILURE;
                       flag=1;
 		     }        
		}
                
		if (error == SA_AIS_OK)
                  {
                        flag1=1;
			//printf("%s", objectNameSU.value);
                   	strncpy((char *)objectname.value, objectNameSU.value, SA_MAX_NAME_LENGTH);
			objectname.length= strlen(objectNameSU.value);

               		attributeNames[0] ="saAmfSUNumCurrActiveSIs";
			attributeNames[1] =NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectname, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		activeSis = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
				     		if(activeSis > 0)
						{
							
								//printf("\nIt is active\n");
								strncpy((char *)objectnameact.value, objectname.value, SA_MAX_NAME_LENGTH);
								objectnameact.length= strlen(objectNameSU.value);
								printf("%s is the active SU\n", objectnameact.value);
								attributeNames[0] ="saAmfSUHostedByNode";
								attributeNames[1] =NULL;
								error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       					if (error != SA_AIS_OK) {
                	       						printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       						rc= EXIT_FAILURE;
									flag = 1;
                      						}
       			       					if (error == SA_AIS_OK)
   								{
       					                      //    printf("\n1\n");
                                 				error= saImmOmAccessorGet_2(accessorHandle, &objectname, attributeNames, &attributes);
                                        				if (error !=SA_AIS_OK)
                                        				{
                                        	       				printf("\nerror18:%d",error);
										rc= EXIT_FAILURE;
                                        		        	        flag = 1; 
                                        				}
                        	 					if (error ==SA_AIS_OK)
                                 					{
			                     					//printf("\n2\n");
							     			SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
							    			
                        							 SaNameT *objtnamevalidsutype = (SaNameT *)attrValues;
 										printf
										("The active SU is hosted on node  %s ", objtnamevalidsutype->value);

 										strncpy((char *)objectname1.value, objtnamevalidsutype->value, SA_MAX_NAME_LENGTH);
										objectname1.length= strlen(objectname1.value);              		        		     			
									}//accesor get
						
					
									error = saImmOmAccessorFinalize(accessorHandle);
                                                		}	
                                                }//if active
					}//accesor get	
 				      }//accesor	                              
               }//search next
	} while (error != SA_AIS_ERR_NOT_EXIST);
 
  error = saImmOmSearchFinalize(searchHandle);
    }//search handle

}//flag


if(flag != 1)
{	
        printf("\nElasticity Engine searches if the node hosting active SU is hosting any other SU that is handling assignments \n");
	searchParam.searchOneAttr.attrName = "saAmfSUHostedByNode";
	searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 	searchParam.searchOneAttr.attrValue = &objectname1;
       // printf("\nnode name %s",objectname1.value);
	error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,
					  &searchHandle);
	if (SA_AIS_OK != error) {
		//printf("error - saImmOmSearchInitialize_2 FAILED");
                printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                rc=EXIT_FAILURE;
                flag=1;



	}

        if (SA_AIS_OK == error)
        {
		         printf("\n\nWe got the search handle \n");
		           

            do {
                       // printf("\n\n1\n");
          		error = saImmOmSearchNext_2(searchHandle, &objectNameSU1, &attributesSU2);
                        printf("searchnext error %d\n",error);
			if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       // printf("\n\n How r u\n\n");
			printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                        rc=EXIT_FAILURE;
			flag=1;
			}
               		if(error == SA_AIS_ERR_NOT_EXIST )
               		{
		  	   if(flag1==1)
                  	   { 
                     	//arraySU[counter]==NULL;
                     	//printf("\nobject does not exist"); 
        	    	   }
		     	   else
		     	  { 
                             rc=EXIT_FAILURE;
                             flag=1;
 		          }        
			}
                
			if (error == SA_AIS_OK)
                  	{       
                          //      printf("\n2\n");
                        	flag1=1;
				//printf("%s", objectNameSU1.value);
                   		strncpy((char *)objectname.value, objectNameSU1.value, SA_MAX_NAME_LENGTH);
				objectname.length= strlen(objectNameSU1.value);
                   		
                            
                       		if(strncmp(objectnameact.value,objectname.value,compareLimit)==0)
                       		{        
					//printf("\nthis is the same su");
                                        attributeNames[0] ="saAmfSURank";
					attributeNames[1] =NULL;
					error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectnameact, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		rankact = *((SaUint32T *)attrValues);
                		             						
						
						
					        	error = saImmOmAccessorFinalize(accessorHandle);
                                                 }

                                        }
		       		}
			        else
                                {
                                     //  printf("\n4\n");
                                    // printf("\nYou will need to acces this SUs curr active and curr standbys\n");
                                    
                                  //***********************
                                   printf("\n%s", objectname.value);
                                   standbySis1=0;
                                   activeSis1=0;
		
			
			attributeNames[0] ="saAmfSUNumCurrActiveSIs";
			attributeNames[1] =NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectname, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			               //      	        printf("\n5\n");	
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		activeSis1 = *((SaUint32T *)attrValues);
                                                        //printf("\nActive Sis %d",activeSis1);
                		   	                error = saImmOmAccessorFinalize(accessorHandle);
						}
                                        }
                        
			attributeNames[0] ="saAmfSUNumCurrStandbySIs";
			attributeNames[1] =NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectname, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                 //    		printf("\n6\n");
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		standbySis1 = *((SaUint32T *)attrValues);
							//printf("\nstandby Sis %d",standbySis1);
                                                        error = saImmOmAccessorFinalize(accessorHandle);
                                                         
						}
                                        }

                                  //***********************
                                }
	                              
              	      }//search next
              printf("is handling %d active assignments and %d standby assignments\n",activeSis1,standbySis1);
              if(standbySis1 >0 || activeSis1 >0)
              {
                 sus=1;
              }
	} while (error != SA_AIS_ERR_NOT_EXIST && sus !=1);
 
  error = saImmOmSearchFinalize(searchHandle);
    }//search handle

//}//flag

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

  
}//flag

if (flag !=1 && sus !=1)
{
 //printf("\nsus !=1\n");
 
 //??search su s of the SG

 //printf("\n flag is not set");
 printf("We need to search a SU of the SG which is hosted on a more laoded node than the node hosting active SU of the SG\n");
 searchParam.searchOneAttr.attrName = "saAmfSUType";
 searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 searchParam.searchOneAttr.attrValue = &objectnamesutype;

 error = saImmOmSearchInitialize_2(immHandle, &objectnameSG1, scope,
					  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,
					  &searchHandle);
	if (SA_AIS_OK != error) {
		//printf("error - saImmOmSearchInitialize_2 FAILED");
                printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                rc=EXIT_FAILURE;
                flag=1;
	}
 
        

        if (SA_AIS_OK == error)
        {
		         //printf("\n\nWe got the search handle \n");
			
		           

            do {
                 activeSis=0;
                 standbySis=0;
                //printf("\n1)%d %d",activeSis,standbySis);
          	error = saImmOmSearchNext_2(searchHandle, &objectNameSU, &attributesSU2);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       // printf("\n\n How r u\n\n");
			printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                        rc=EXIT_FAILURE;
			flag=1;
		}
                if(error == SA_AIS_ERR_NOT_EXIST )
                {
		     if(flag1==1)
                     { 
                     	//arraySU[counter]==NULL;
                     	//printf("\nobject does not exist"); 
        	     }
		     else
		     { 
                       rc=EXIT_FAILURE;
                       flag=1;
 		     }        
		}
                
		if (error == SA_AIS_OK)
                {
                        flag1=1;
			//printf("%s", objectNameSU.value);
		
			////???*
			attributeNames[0] ="saAmfSUNumCurrActiveSIs";
			attributeNames[1] =NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		activeSis = *((SaUint32T *)attrValues);
                		   	                error = saImmOmAccessorFinalize(accessorHandle);
						}
                                        }
                        ///???*  
 
                        ///???**
                        	
			attributeNames[0] ="saAmfSUNumCurrStandbySIs";
			attributeNames[1] =NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		standbySis = *((SaUint32T *)attrValues);
                                                       // printf("\n<<)%d %d",activeSis,standbySis);
                                                   	error = saImmOmAccessorFinalize(accessorHandle);
                                                         
						}
                                        }
                        ///???**
                        //printf("\n2)%d %d",activeSis,standbySis);  
			if(flag != 1)
                        {
                         	 //printf("\n3)%d %d",activeSis,standbySis);
                          	 if(activeSis >0)
                          	{
                                	 //printf("It is ACTIVE");
                                	 //DO NOTHING
                          	}    
                                //{
                                   //?????????????????
                          	if(standbySis >0 && foundsu!=1)
                          	{
                                                            //  printf("It is Standby");
                                 //DO NOTHING
                                  strncpy((char *)objectnameSUTemp.value, objectNameSU.value, SA_MAX_NAME_LENGTH);
				  objectnameSUTemp.length= strlen(objectNameSU.value);
                                 // printf("\n%s<3<3",objectnameSUTemp.value);
                                  attributeNames[0] ="saAmfSURank";
				  attributeNames[1] =NULL;
			          error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		surankTemp = *((SaUint32T *)attrValues);
                                                       // printf("\n<<)%d",surankTemp);
                                                   	error = saImmOmAccessorFinalize(accessorHandle);
                                                         
						}
                                        }
       /////Access node
      				  attributeNames[0] ="saAmfSUHostedByNode";
				  attributeNames[1] =NULL;
			          error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		SaNameT *objtnamevalidsutype = (SaNameT *)attrValues;
                                                   	strncpy((char *)objectname.value, objtnamevalidsutype->value, SA_MAX_NAME_LENGTH);
							objectname.length= strlen(objectname.value);
                                                         
						}
                                        error = saImmOmAccessorFinalize(accessorHandle);
                                        }
            
     				/////Access node                  
                                ////access sus on the node
                        if(flag != 1)
                        {
                            searchParam.searchOneAttr.attrName = "saAmfSUHostedByNode";
 			    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 			    searchParam.searchOneAttr.attrValue = &objectname;

			    error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,
					  &searchHandle);
			    if (SA_AIS_OK != error) {
			    //printf("error - saImmOmSearchInitialize_2 FAILED");
                	    printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	    rc=EXIT_FAILURE;
                	    flag=1;
			     }
                            if (SA_AIS_OK == error)
        		    {
		         	//printf("\n\nWe got the search handle \n");
			
		           

            			do {                
                   			error = saImmOmSearchNext_2(searchHandle, &objectNameSU, &attributesSU2);
					if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
					printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			  rc=EXIT_FAILURE;
					  flag=1;
					}
                                        


                                        if(error == SA_AIS_ERR_NOT_EXIST )
               				{
		  	  			 if(flag1==1)
                  	   			{ 
                     					//arraySU[counter]==NULL;
                     					//printf("\nobject does not exist"); 
        	    	   			}
		     	   			else
		     	  			{ 
                             				rc=EXIT_FAILURE;
                             				flag=1;
 		          			}        
					}
                
					if (error == SA_AIS_OK)
                  			{
 
                                              //     printf("\n%s\n", objectNameSU.value);
		                                   if(strncmp(objectNameSU.value,objectnameSUTemp.value,compareLimit)==0)
                                                   {
                                  //                      printf("\nthe standby su of the node");
                                                    }
					  else
   						{
						   attributeNames[0] ="saAmfSUNumCurrActiveSIs";
						   attributeNames[1] =NULL;
						   error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			   if (error != SA_AIS_OK) {
                	       					printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       					rc= EXIT_FAILURE;
								flag = 1;
                      				   }
       			       			   if (error == SA_AIS_OK)
   						   {
							error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
                                        		if (error !=SA_AIS_OK)
                                        		{
                                               			printf("\nerror18:%d",error);
								rc= EXIT_FAILURE;
                                                        	flag = 1; 
                                        		}
                        	 			if (error ==SA_AIS_OK)
                                 			{
			                     		
					     			SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    			activeSis = *((SaUint32T *)attrValues);
                                                                //printf("\nthe end\n",activeSis);
                                                                printf("Active assignments :\n%d",activeSis);
                		   	               	 	error = saImmOmAccessorFinalize(accessorHandle);
							}//if not standby
       			
                        		           }//accesor initialize
                                               	   attributeNames[0] ="saAmfSUNumCurrStandbySIs";
						   attributeNames[1] =NULL;
						   error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			   if (error != SA_AIS_OK) {
                	       					printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       					rc= EXIT_FAILURE;
								flag = 1;
                      				   }
       			       			   if (error == SA_AIS_OK)
   						   {
							error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
                                        		if (error !=SA_AIS_OK)
                                        		{
                                               			printf("\nerror18:%d",error);
								rc= EXIT_FAILURE;
                                                        	flag = 1; 
                                        		}
                        	 			if (error ==SA_AIS_OK)
                                 			{
			                     		
					     			SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    			standbySis = *((SaUint32T *)attrValues);
                                                                //printf("\nthe end\n",activeSis);
                                                                printf("Standby Assignments :%d\n",standbySis);
                		   	               	 	error = saImmOmAccessorFinalize(accessorHandle);
							}//if accessor get
       			
                        		           }//accesor initialize
                                                  if(activeSis >0 || standbySis > 0)
                                                  {
                                                        printf("\nWe got the Sus to change the rank\n");
                                                        foundsu= 1;
                                                  }
                                          
					         }	

                                         }//search next
                                       //printf("\n%d",foundsu);
                                    }while (error != SA_AIS_ERR_NOT_EXIST && foundsu != 1);
                                }
                               }	
                                   //?????????????????
                             } //standby 
                         ///<><><><>
                                if(standbySis ==0 && activeSis ==0 && foundsu!=1)
                          	{
                                                            //  printf("It is Spare");
                                 //DO NOTHING
                                  strncpy((char *)objectnameSUTemp.value, objectNameSU.value, SA_MAX_NAME_LENGTH);
				  objectnameSUTemp.length= strlen(objectNameSU.value);
                                 // printf("\n%s<3<3",objectnameSUTemp.value);
                                  attributeNames[0] ="saAmfSURank";
				  attributeNames[1] =NULL;
			          error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		surankTemp = *((SaUint32T *)attrValues);
                                                       // printf("\n<<)%d",surankTemp);
                                                   	error = saImmOmAccessorFinalize(accessorHandle);
                                                         
						}
                                        }
       /////Access node
      				  attributeNames[0] ="saAmfSUHostedByNode";
				  attributeNames[1] =NULL;
			          error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       		if (error != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       			rc= EXIT_FAILURE;
						flag = 1;
                      			}
       			       		if (error == SA_AIS_OK)
   					{
       					     
                                 		error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
                                        	if (error !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror18:%d",error);
							rc= EXIT_FAILURE;
                                                        flag = 1; 
                                        	}
                        	 		if (error ==SA_AIS_OK)
                                 		{
			                     		
					     		SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    		SaNameT *objtnamevalidsutype = (SaNameT *)attrValues;
                                                   	strncpy((char *)objectname.value, objtnamevalidsutype->value, SA_MAX_NAME_LENGTH);
							objectname.length= strlen(objectname.value);
                                                         
						}
                                        error = saImmOmAccessorFinalize(accessorHandle);
                                        }
            
     				/////Access node                  
                                ////access sus on the node
                        if(flag != 1)
                        {
                            searchParam.searchOneAttr.attrName = "saAmfSUHostedByNode";
 			    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 			    searchParam.searchOneAttr.attrValue = &objectname;

			    error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,
					  &searchHandle);
			    if (SA_AIS_OK != error) {
			    //printf("error - saImmOmSearchInitialize_2 FAILED");
                	    printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	    rc=EXIT_FAILURE;
                	    flag=1;
			     }
                            if (SA_AIS_OK == error)
        		    {
		         	//printf("\n\nWe got the search handle \n");
			
		           

            			do {                
                   			error = saImmOmSearchNext_2(searchHandle, &objectNameSU, &attributesSU2);
					if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
					printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			  rc=EXIT_FAILURE;
					  flag=1;
					}
                                        


                                        if(error == SA_AIS_ERR_NOT_EXIST )
               				{
		  	  			 if(flag1==1)
                  	   			{ 
                     					//arraySU[counter]==NULL;
                     					//printf("\nobject does not exist"); 
        	    	   			}
		     	   			else
		     	  			{ 
                             				rc=EXIT_FAILURE;
                             				flag=1;
 		          			}        
					}
                
					if (error == SA_AIS_OK)
                  			{
 
                                              //     printf("\n%s\n", objectNameSU.value);
		                                   if(strncmp(objectNameSU.value,objectnameSUTemp.value,compareLimit)==0)
                                                   {
                                  //                      printf("\nthe standby su of the node");
                                                    }
					  else
   						{
						   attributeNames[0] ="saAmfSUNumCurrActiveSIs";
						   attributeNames[1] =NULL;
						   error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			   if (error != SA_AIS_OK) {
                	       					printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       					rc= EXIT_FAILURE;
								flag = 1;
                      				   }
       			       			   if (error == SA_AIS_OK)
   						   {
							error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
                                        		if (error !=SA_AIS_OK)
                                        		{
                                               			printf("\nerror18:%d",error);
								rc= EXIT_FAILURE;
                                                        	flag = 1; 
                                        		}
                        	 			if (error ==SA_AIS_OK)
                                 			{
			                     		
					     			SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    			activeSis = *((SaUint32T *)attrValues);
                                                                //printf("\nthe end\n",activeSis);
                                                                printf("Active assignments :\n%d",activeSis);
                		   	               	 	error = saImmOmAccessorFinalize(accessorHandle);
							}//if not standby
       			
                        		           }//accesor initialize
                                               	   attributeNames[0] ="saAmfSUNumCurrStandbySIs";
						   attributeNames[1] =NULL;
						   error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			   if (error != SA_AIS_OK) {
                	       					printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       					rc= EXIT_FAILURE;
								flag = 1;
                      				   }
       			       			   if (error == SA_AIS_OK)
   						   {
							error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
                                        		if (error !=SA_AIS_OK)
                                        		{
                                               			printf("\nerror18:%d",error);
								rc= EXIT_FAILURE;
                                                        	flag = 1; 
                                        		}
                        	 			if (error ==SA_AIS_OK)
                                 			{
			                     		
					     			SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    			standbySis = *((SaUint32T *)attrValues);
                                                                //printf("\nthe end\n",activeSis);
                                                                printf("Standby Assignments :%d\n",standbySis);
                		   	               	 	error = saImmOmAccessorFinalize(accessorHandle);
							}//if accessor get
       			
                        		           }//accesor initialize
                                                  if(activeSis >0 || standbySis > 0)
                                                  {
                                                        printf("\nWe got the Sus to change the rank\n");
                                                        foundsu= 1;
                                                  }
                                          
					         }	

                                         }//search next
                                       //printf("\n%d",foundsu);
                                    }while (error != SA_AIS_ERR_NOT_EXIST && foundsu != 1);
                                }
                               }	
                                   //?????????????????
                             } //spare
                          
                         ///<><><><>


                  
                         }//flag		
					
				////access sus on the node

 		}//search next
                
             }while(error != SA_AIS_ERR_NOT_EXIST && foundsu != 1);
       error= saImmOmSearchFinalize(searchHandle);
    }//search
///////create a ccb if foundsu is set
if(foundsu == 1)
{
	//printf("\nccb to set\n");
        printf("\nCreate a CCB to swap the rank of the SU(%s) on loaded node ranked %d with the rank %d of the current active SU(%s)		\n",objectnameSUTemp.value,surankTemp,rankact,objectnameact.value);
                       //printf("\n Ranking a SU on the least loaded node by creating a CCB\n");
                       //??
			strcpy(nameccbattribute,"saAmfSURank=");
                         sprintf(rankt, "%d",rankact);
               		 strcat(nameccbattribute,rankt);
             		//printf("\n%s oooooooo",nameccbattribute);
                        s= objectnameSUTemp.value;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[objectNames_len - 1] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       errorc = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (errorc != SA_AIS_OK) {
		       printf("errorc - saImmOmAdminOwnerInitialize FAILED: %d\n", errorc);
		       rc = EXIT_FAILURE;
		      }
		     if (errorc == SA_AIS_OK)
             	     {   

	 	       rc1 = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute); 
		        //printf("\n%d\n",rc1);
                        printf("The IMM Ccb API does not allow to change the Rank Attribute of the SU");
		        if(rc1==0)
		        {
		           rt=1;
		        }
        
	             }
	errorc = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != errorc) {
		printf("errorc - saImmOmAdminOwnerFinalize FAILED: %d\n",errorc);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
//swap rank of SU 2
 printf("\nCreate a CCB to swap the rank of the current active SU(%s) ranked %d with the rank  %d of the SU(%s) on loaded node\n",objectnameact.value,rankact,surankTemp,objectnameSUTemp.value);

       //       printf("\n Ranking a SU on the least loaded node by creating a CCB\n");
                       //??
			strcpy(nameccbattribute,"saAmfSURank=");
                         sprintf(rankt, "%d",surankTemp);
               		 strcat(nameccbattribute,rankt);
             		 //printf("\noooooo %s oooooooo",nameccbattribute);
                        s= objectnameact.value;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[objectNames_len - 1] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       errorc = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (errorc != SA_AIS_OK) {
		       printf("errorc - saImmOmAdminOwnerInitialize FAILED: %d\n", errorc);
		       rc = EXIT_FAILURE;
		      }
		     if (errorc == SA_AIS_OK)
             	     {   

	 	       rc1 = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute); 
		        //printf("\n%d\n",rc1);
                        printf("The IMM Ccb API does not allow to change the Rank Attribute of the SU");
                        rc1=0;
		        if(rc1==0)
		        {
		           rt=1;
		        }
        
	             }
	errorc = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != errorc) {
		printf("errorc - saImmOmAdminOwnerFinalize FAILED: %d\n",errorc);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
 swapsi(immHandle,argv,7,objectnameSI);
  //swap rank of SU 2
}//flag foundsu == 1

///////create a ccb if foundsu is set
//??              
}//if 
else
{
  printf("\n The SU is on the loaded node...need elasticity at cluster level\n");
  rt = 2;
}

return rt;
/////searcxxxxxxxxxxxxxxx  
}//decrease function





int single_SIincrease2n(SaImmHandleT immHandle,SaNameT objectnameSG1,char *argv[],SaNameT objectnameSI)
{


  SaAisErrorT error60,error61,error62,error63,error64,errorx,error15,error16,error17,error18,error19,error20,error21,error23,error43;
  SaImmAccessorHandleT accessorHandle60,accessorHandle62,accessorHandle15,accessorHandle23;
  const SaImmAttrNameT  attributeNamesSgtype[] ={"saAmfSGType",NULL};
  const SaImmAttrNameT  attributeNamesSgtypezz[] ={"saAmfImmAttrAdminOwnerName",NULL};
  const SaImmAttrNameT  attributeNamesSgtype1[] ={"saAmfSgtValidSuTypes",NULL};
  const SaImmAttrNameT  attributeNamesSUrank[] ={"saAmfSURank",NULL};
  const SaImmAttrNameT  attributeNamesSU1[] ={"saImmAttrAdminOwnerName",NULL};
  SaImmAttrValuesT_2 **attributesSGtype,**attributesSGtype1,**attributesSGSUs,**attributesassignmentact,**attributesassignmentstd,**attributesSUnode;
  SaInt8T namesgtype[256],namevalidsutype[256],namenode[256];
  SaNameT objectnamesgtype1,objectnamevalidsutype1,objectnameSGSUs,objectnamesuc; 
  int l=0; 
  char *arraySU1[100] ;
  int counter1=0,assignmentact,assignmentstd;
  SaImmSearchHandleT searchHandle63,searchHandle21; 
   
 ///////////////////////////

 /////////////////// ccb id ////////////////////
 int k=0,rt;
 SaInt8T namesgg[256],SuNameFinal[256];
 //SaBoolT releaseOwnershipOnFinalize = SA_TRUE;
 //SaImmCcbHandleT ccbHandle,ccbHandle42;
 //SaImmAdminOwnerHandleT ownerHandle,ownerHandle40;
 //SaImmCcbFlagsT ccbFlags;
 SaNameT objectName1,objectnameSGG2,objectnameSGG1;
 //SaImmScopeT scope1 = SA_IMM_ONE;	/* default search scope */
 

 int rc = EXIT_SUCCESS;
 int rc1 = EXIT_FAILURE;
 int c;
 SaAisErrorT errorc;
 //SaImmHandleT immHandle;
 SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
 SaImmAdminOwnerHandleT ownerHandle;
 SaNameT **objectNames = NULL;
 SaNameT *objectNamec;
 int objectNames_len = 1;
 //char *s = "safSg=AmfDemo,safApp=AmfDemo1";
 char *s;
/**
 objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 objectNamec = objectNames[objectNames_len - 1] = malloc(sizeof(SaNameT));
 objectNames[objectNames_len++] = NULL;
 objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
**/
 /////////////////// ccb id ////////////////////

//counte assignmenst
SaNameT objectnameSUtemp;
SaAisErrorT error50,error51,error24,error25,error26;
SaImmAttrValuesT_2 **attributesSUtemp;
SaUint32T actassignment;
//counte assignmenst


  SaUint32T autoAdjust;
  int counter=0,d=0;
 //printf("\n I got a call\n");
  //int rc = EXIT_SUCCESS;
 int i=0,j=0,x=0;
 SaUint32T actassg=0,stdassg=0,actass1=0,stdass1=0,assignmentactF=0,assignmentstdF=0,assignmentactFinal=0,assignmentstdFinal=0,SuRankFinal=0,activet=1000,standbyt=1000,Rankt;
 SaInt8T nameSU[256]="safSu=SU2,safSg=AmfDemo,safApp=AmfDemo1";
 SaInt8T suname[256],namet[256]="";
 SaNameT objectnameSU,objectnameNODE1,objectNameSUc,objectsuname,objectnameNODE2,objectsu2,*objSU,objectname;
// objectnameSI.length= strlen(nameSI);
 //strncpy((char *)objectnameSI.value, nameSI, SA_MAX_NAME_LENGTH);
 
 SaImmAccessorHandleT accessorHandle,accessorHandle1; 
 SaImmAttrValuesT_2 **attributes,**attributesSU,**attributesSG,**attributesSU1,**attributesSU2,**attributesSU3,**attributesSUc,**attributesSUc1,**attributessu2,**attributesSUrank;
 SaAisErrorT error,error1,error2,error3,error4,error5,error6,error7,error8,error9,error10,error11,error100,error101,error22;
 
 SaImmAttrModificationT_2 attrMod;
 SaImmSearchHandleT searchHandle,searchHandle1,searchHandle2,searchHandle22;
 SaImmSearchParametersT_2 searchParam,searchParam1,searchParam2,searchParam3;
 SaNameT objectName , objectNameSU2;	
 SaImmAttrValuesT_2 *attr,attr1;
 SaNameT objectNameSU,objectNameSU3;
 SaNameT rootName = { 0, "" };
 //SaNameT rootName2 = { 0, "" };
 SaImmScopeT scope = SA_IMM_SUBTREE;	/* default search scope */
 char classNameBuf[SA_MAX_NAME_LENGTH] = {0};
 const SaImmAttrNameT  attributeNamesSU[] ={"saAmfSUHostedByNode",NULL};
 const SaImmAttrNameT  attributeNamesSg[] ={"saAmfSGAutoAdjust",NULL};
 const SaImmAttrNameT  attributeNamesSUc[] ={"saAmfSUNumCurrActiveSIs",NULL};
 const SaImmAttrNameT  attributeNamesSUc1[] ={"saAmfSUNumCurrStandbySIs",NULL};
 const SaImmAttrNameT  attributeNamesSg1[] ={"saAmfSGType",NULL};
 const char *arraySU[100];
 const char *arraysu[100];
 char nameccbattribute[256],rankt[256];
 //const SaImmAttrNameT  attributeNamesSU3[] ={"saAmfSUNumCurrActiveSI's","saAmfSUNumCurrActiveSI's",NULL};
   
 SaUint32T activeassign=0,standbyassign=0;
 int counteract=0,counterstd=0;
  error5 = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 if (error5 != SA_AIS_OK) {
                printf("error - saImmOmAccessorInitialize FAILED:%d",error5);
                rc= EXIT_FAILURE;
                      }
       if (error5 == SA_AIS_OK)
      {
	printf("\n*****The autoadjust attribute of SG*****\n");
	error6= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNamesSg, &attributesSG);
	if (error6 ==SA_AIS_OK)
	{

		SaImmAttrValueT *attrValue = attributesSG[0]->attrValues[0];
		//SaNameT *objectnameSG = (SaNameT *)attrValue;
		printf("\nThe SG auto-adjust is set to %u (0x%x)", *((SaUint32T *)attrValue), *((SaUint32T *)attrValue));
		//printf("The SG auto-adjust is set to : %u \n", objectnameSG);

		autoAdjust= *((SaUint32T *)attrValue);
		//printf("\n%u\n",autoAdjust);


		error = saImmOmAccessorFinalize(accessorHandle);
	}
	//if auto-adjust set to SA_FALSE ccb to set to SA_TRUE

	if(autoAdjust==0)
	{      // const SaNameT objnameSG = objectnameSG1;
    		printf("\nIt is set to false.We create a ccb to set it to true");                 
        }//if autoadjust is set to false   
}
//printf("\n========================================================================================================================================================\n");
//if auto-adjust set to SA_FALSE ccb to set to SA_TRUE


//CHECK THE AUTO ADJUST ATTRIBUTE OF THE SG


//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//printf("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<Initializing the activetemp and standbytemp>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  error60 = saImmOmAccessorInitialize( immHandle, &accessorHandle60);
 if (error60 != SA_AIS_OK) {
                printf("error - saImmOmAccessorInitialize FAILED:%d",error60);
                rc= EXIT_FAILURE;
                      }
       if (error60 == SA_AIS_OK)
   {
  //     printf("\nWe got the accessor handle to access SG <3<3<3\n");
         
      	error61= saImmOmAccessorGet_2(accessorHandle60, &objectnameSG1, attributeNamesSgtype, &attributesSGtype);
	if (error61 !=SA_AIS_OK)
	{
	 printf("\n%d",error61);
         rc= EXIT_FAILURE;
	}


	if (error61 ==SA_AIS_OK)
	{
	//printf("\nsaImmOmAccessorGet_2 function returned the SG object and attribute<3<3<3\n");

	SaImmAttrValueT *attrValuesgtype = attributesSGtype[0]->attrValues[0];
	SaNameT *objtnamesgtype = (SaNameT *)attrValuesgtype;
	//printf("\nThe SG type is set to %s !!!!", objtnamesgtype->value);

	strcpy(namesgtype,objtnamesgtype->value);
	x=objtnamesgtype->length;
	objectnamesgtype1.length=x;
	//printf("\n\n%u\n\n",x);

	strncpy((char *)objectnamesgtype1.value, namesgtype, SA_MAX_NAME_LENGTH);

	}
error = saImmOmAccessorFinalize(accessorHandle60);
}
//>>>
 error62 = saImmOmAccessorInitialize( immHandle, &accessorHandle62);
 if (error62 != SA_AIS_OK) {
                printf("error - saImmOmAccessorInitialize FAILED:%d",error62);
                rc= EXIT_FAILURE;
                      }
       if (error62 == SA_AIS_OK)
   {
       //printf("\nWe got the accessor handle to access SG\n");
  // } 

error63= saImmOmAccessorGet_2(accessorHandle62, &objectnamesgtype1, attributeNamesSgtype1, &attributesSGtype1);

if(error62 == SA_AIS_OK)
{
 //printf("\nok ok");
 SaImmAttrValueT *attrValuesgtype1 = attributesSGtype1[0]->attrValues[0];
 SaNameT *objtnamevalidsutype = (SaNameT *)attrValuesgtype1;
// printf("\nThe valid SU type for this SG type is set to %s @@@@@@@@@@@@", objtnamevalidsutype->value);

 strcpy(namevalidsutype,objtnamevalidsutype->value);
 l=objtnamevalidsutype->length;
 objectnamevalidsutype1.length=l;
//printf("\n\n%u\n\n",x);

strncpy((char *)objectnamevalidsutype1.value, namevalidsutype, SA_MAX_NAME_LENGTH);

}
error = saImmOmAccessorFinalize(accessorHandle62);

//??
 searchParam2.searchOneAttr.attrName = "saAmfSUType";
 searchParam2.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 searchParam2.searchOneAttr.attrValue = &objectnamevalidsutype1;

error63 = saImmOmSearchInitialize_2(immHandle, &objectnameSG1, scope,
					  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam2, NULL,
					  &searchHandle63);
	if (SA_AIS_OK != error63) {
		//printf("error - saImmOmSearchInitialize_2 FAILED");
                printf("error - saImmOmAccessorInitialize FAILED:%d",error63);
                exit(EXIT_FAILURE);
	}

        if (SA_AIS_OK == error63)
        {
		//printf("error - saImmOmSearchInitialize_2 FAILED");
               //   printf("\n\nWe got the search handle to search the SUs in SG protecting the SI\n");
		   //printf("error - saImmOmSearchInitialize :%d\n",error);


                   //-------------------------------------------//
                  counter1=0;
                      do {
                
				error64 = saImmOmSearchNext_2(searchHandle63, &objectnameSGSUs, &attributesSGSUs);
                		if (error64 != SA_AIS_OK && error64 != SA_AIS_ERR_NOT_EXIST)
				{
               				printf("error - saImmOmSearchNext_2 FAILED: %d\n", error64);
                          			//exit(EXIT_FAILURE);
				}
               		 	if(error64 == SA_AIS_ERR_NOT_EXIST)
                		{
                     		 // printf("\n it does not exist");
                     		   arraySU1[counter1]==NULL; 
                		}
                
				if (error64 == SA_AIS_OK)
                 		 {     
                        
                        		strcpy(suname, objectnameSGSUs.value);
                       		        //printf("\n<3># %s",objectnameSGSUs.value);
                                //?????
                                    //access SUs of the SG
                               		//printf("\nNow lets access this SU object:\n");
 			       		error15 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
 			       		if (error15 != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d",error15);
                	       			rc= EXIT_FAILURE;
                      			}
       			       		if (error15 == SA_AIS_OK)
   					{
       					//	printf("\nWe got the accessor handle to access SU!!\n");     
      				        	strncpy((char *)objectsuname.value, suname, SA_MAX_NAME_LENGTH);
      						objectsuname.length=strlen(suname);
					//	printf("%s\n", objectsuname.value);
                                	
                                       		//su su sus su
                                       	
                                 		error16= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUc, &attributesassignmentact);
                                        	if (error16 !=SA_AIS_OK)
                                        	{
                                               		printf("\nerror16:%d",error16);
                                        	}
                        	 		if (error16 ==SA_AIS_OK)
                                 		{
			                     		assignmentactF=0;
					     		SaImmAttrValueT *attrValueassg1 = attributesassignmentact[0]->attrValues[0];
					    		assignmentactF = *((SaUint32T *)attrValueassg1);
                		             		error = saImmOmAccessorFinalize(accessorHandle15);
                                            		//printf("\n%d;;;;;\n",assignmentactF);   
                                              		if(assignmentactF > 0)
                                              		{                                                     		
                                                     //???????
					//		printf("\nit is active\n");
		                                         assignmentactFinal=assignmentactF;
  						         strcpy(SuNameFinal,objectsuname.value);
                                                         //{{{{{{
 							 	 error16 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
                       						if (error16 != SA_AIS_OK)
		   						{	
                							printf("error16 - saImmOmAccessorInitialize FAILED:%d",error16);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error16 == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,  

                                                                        &attributesassignmentact);  
                                                                        if (error17 !=SA_AIS_OK)
 									{
										printf("\n%d",error17);					
									} 
                                       					if (error17 ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValuecrk = attributesassignmentact[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										SuRankFinal = *((SaUint32T *)attrValuecrk);
               						   			error = saImmOmAccessorFinalize(accessorHandle15);
                                                                                
                                   						//"%u", *((SaUint32T *)attrValue)
                                                                           
					               	                }
                                                                        //printf("\n 1");
   		                                        	 }
      								//printf("\n2");
                                                         //{{{{{{
    							 error16 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
                       						if (error16 != SA_AIS_OK)
		   						{	
                							printf("error16 - saImmOmAccessorInitialize FAILED:%d",error16);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error16 == SA_AIS_OK)
   	           						{       
                                                        	        
                      				//			printf("\nWe got the accessor handle to access SU s stanfby !!\n");
                                                        	        error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUc1,   		
									&attributesSUc);        
                                       					if (error17 ==SA_AIS_OK)
                                                                        {
	                                                              
										SaImmAttrValueT *attrValuec1 = attributesSUc[0]->attrValues[0];
                                                                    		//printf("%d\n",*((SaUint32T *)attrValuec1));
										assignmentstdF = *((SaUint32T *)attrValuec1);
               						   			error = saImmOmAccessorFinalize(accessorHandle15);
                                                                                assignmentstdFinal=assignmentstdF;
                                                                               // printf("\n%d;;;",assignmentstdF);
					               	                }
   		                                        	 }
                                                     //???????
                                              }          
			        	 }
                                 }
                                 
                                //?????
                  		 }
                   }while(error64 != SA_AIS_ERR_NOT_EXIST);
 		error = saImmOmSearchFinalize(searchHandle63);
         }//search ok if 
//??
//printf("\n%s : is the present active SU",SuNameFinal);
//printf("\n%d : is the  active ass of SU",assignmentactFinal);
//printf("\n%d : is the  standby ass of SU",assignmentstdFinal);
//printf("\n%d : is the rank of SU",SuRankFinal);
}
//>>>
//printf("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<End Initializing the activetemp and standbytemp>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


  error60 = saImmOmAccessorInitialize( immHandle, &accessorHandle60);
 if (error60 != SA_AIS_OK) {
                printf("error - saImmOmAccessorInitialize FAILED:%d",error60);
                rc= EXIT_FAILURE;
                      }
       if (error60 == SA_AIS_OK)
   {
  //     printf("\nWe got the accessor handle to access SG\n");
   //}      
	error61= saImmOmAccessorGet_2(accessorHandle60, &objectnameSG1, attributeNamesSgtype, &attributesSGtype);
	if (error61 !=SA_AIS_OK)
	{
 		printf("\n%d",error61);
	}


	if (error61 ==SA_AIS_OK)
	{
		//	printf("\nsaImmOmAccessorGet_2 function returned the SG object and attribute\n");

		SaImmAttrValueT *attrValuesgtype = attributesSGtype[0]->attrValues[0];
		SaNameT *objtnamesgtype = (SaNameT *)attrValuesgtype;
	//	printf("\nThe SG type is set to %s ", objtnamesgtype->value);

		strcpy(namesgtype,objtnamesgtype->value);
		x=objtnamesgtype->length;
		objectnamesgtype1.length=x;
		
		strncpy((char *)objectnamesgtype1.value, namesgtype, SA_MAX_NAME_LENGTH);

	}
	error = saImmOmAccessorFinalize(accessorHandle60);
   }


	error62 = saImmOmAccessorInitialize( immHandle, &accessorHandle62);
 	if (error62 != SA_AIS_OK) {
                printf("error - saImmOmAccessorInitialize FAILED:%d",error62);
                rc= EXIT_FAILURE;
                      }
       if (error62 == SA_AIS_OK)
   	{
       		//printf("\nWe got the accessor handle to access SG\n");
   //}      

		error63= saImmOmAccessorGet_2(accessorHandle62, &objectnamesgtype1, attributeNamesSgtype1, &attributesSGtype1);

		if(error62 == SA_AIS_OK)
		{
 			 SaImmAttrValueT *attrValuesgtype1 = attributesSGtype1[0]->attrValues[0];
                         SaNameT *objtnamevalidsutype = (SaNameT *)attrValuesgtype1;
 	//		 printf("\nThe valid SU type for this SG type is set to %s ", objtnamevalidsutype->value);

 			strcpy(namevalidsutype,objtnamevalidsutype->value);
 			l=objtnamevalidsutype->length;
 			objectnamevalidsutype1.length=l;


			strncpy((char *)objectnamevalidsutype1.value, namevalidsutype, SA_MAX_NAME_LENGTH);

		}
	error = saImmOmAccessorFinalize(accessorHandle62);
        }
//;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 searchParam2.searchOneAttr.attrName = "saAmfSUType";
 searchParam2.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 searchParam2.searchOneAttr.attrValue = &objectnamevalidsutype1;
printf("\n\n\n*****Search the SUs of the SG*****\n");
error63 = saImmOmSearchInitialize_2(immHandle, &objectnameSG1, scope,
					  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam2, NULL,
					  &searchHandle63);
	if (SA_AIS_OK != error63) {
		//printf("error - saImmOmSearchInitialize_2 FAILED");
                printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                exit(EXIT_FAILURE);
	}

        if (SA_AIS_OK == error63)
        {
		//printf("error - saImmOmSearchInitialize_2 FAILED");
                  printf("\nSearch the SUs in the SG protecting the SI requesting capacity\n");
		   //printf("error - saImmOmSearchInitialize :%d\n",error);


                   //-------------------------------------------//
                  //printf("\nactivet%d,standbyt%d",activet,standbyt);
                  counter1=0;
                      do {
                
				error64 = saImmOmSearchNext_2(searchHandle63, &objectnameSGSUs, &attributesSGSUs);
                
				if (error64 != SA_AIS_OK && error64 != SA_AIS_ERR_NOT_EXIST) {
                      
				printf("error - saImmOmSearchNext_2 FAILED: %d\n", error64);
                          			//exit(EXIT_FAILURE);
				}
                		if(error64 == SA_AIS_ERR_NOT_EXIST)
                		{
                      			// printf("\n it does not exist");
                     			arraySU1[counter1]==NULL; 
                		}
                
				if (error64 == SA_AIS_OK)
                  		{     
                        
                        		strcpy(suname, objectnameSGSUs.value);
                              		arraySU1[counter1]=suname;
                                           
                        		//access SUs of the SG
                          		//     printf("\nNow lets access this SU object:\n");
 			       		error15 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
 			       		if (error15 != SA_AIS_OK) {
                	       			printf("error - saImmOmAccessorInitialize FAILED:%d",error15);
                	       			rc= EXIT_FAILURE;
                      			}
       		                        if (error15 == SA_AIS_OK)
   					{
       					//	printf("\nWe got the accessor handle to access SU!!\n");     
      				        	strncpy((char *)objectsuname.value, suname, SA_MAX_NAME_LENGTH);
      						objectsuname.length=strlen(suname);
						printf("\n(%d)%s\n",counter1+1, objectsuname.value);
                                
                                       		//su su sus su
                                 		error16= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUc, &attributesassignmentact);
                                        if (error16 !=SA_AIS_OK)
                                        {
                                               printf("\nerror16:%d",error16);
                                        }
                        	 	if (error16 ==SA_AIS_OK)
                                 	{
			                     	assignmentact=0;
					     	SaImmAttrValueT *attrValueassg1 = attributesassignmentact[0]->attrValues[0];
					     	assignmentact = *((SaUint32T *)attrValueassg1);
                		             	error = saImmOmAccessorFinalize(accessorHandle15);
                                                // printf("\n%d;;;;;\n",assignmentact);             
			        	 }
                                 }
                                //su sus sus us
                                //she she she
                               error17 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
 			       if (error17 != SA_AIS_OK) {
                	       		printf("error !!!- saImmOmAccessorInitialize FAILED:%d",error17);
                	       		rc= EXIT_FAILURE;
                      		}
       			       if (error17 == SA_AIS_OK)
   				{
       					//printf("\nWe got the accessor handle to access SU!!\n");     
      				        //strncpy((char *)objectsuname.value, suname, SA_MAX_NAME_LENGTH);
      					//objectsuname.length=strlen(suname);
					//printf("%s\n", objectsuname.value);
                                
                                      
                                 	error18= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUc1, &attributesassignmentstd);
                                        if (error18 !=SA_AIS_OK)
                                        {
                                               printf("\nerror16:%d",error18);
                                        }
                        	 	if (error18 ==SA_AIS_OK)
                                 	{
			                     assignmentstd=0;
					     SaImmAttrValueT *attrValueassg2 = attributesassignmentstd[0]->attrValues[0];
					     assignmentstd = *((SaUint32T *)attrValueassg2);
                		             error = saImmOmAccessorFinalize(accessorHandle15);
                                            // printf("\n%d;;;;;;;;\n",assignmentstd);
                                             
			        	 }
                                 }
 
                  
                                //she she she
			       //checking standby su/active su/spare su and calculating the assignments handled by the hostnodes of these SUs 
                               if (assignmentact >0)
                               {
                                   printf("It is the active SU\n");
//"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
                                                                         //acces the host node
                                   
	                                     error19 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
					     if (error19 != SA_AIS_OK) 
                                             {
	                		     	printf("error - saImmOmAccessorInitialize FAILED:%d",error19);
	                		     	rc= EXIT_FAILURE;
	                      		     }
	       				     if (error19 == SA_AIS_OK)
	   				     {
	       					//printf("\n<3<3<3\n"); 
	              			       error20= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSU, &attributesSUnode);
	      				       if (error20 ==SA_AIS_OK)
	                                       {
		                  //                 printf("\nsaImmOmAccessorGet_2 function returned the SU object and attribute\n");
	                                      //  }
	                                

	
	                                	//we initialize to attribute to acces our second object SG
	
	                                   		 SaImmAttrValueT *attrValuenode = attributesSUnode[0]->attrValues[0];
	 			            		 SaNameT *objectnameSUnode = (SaNameT *)attrValuenode;
				            		 printf("The SU is on Node : %s (%u)", objectnameSUnode->value,objectnameSUnode->length);
	                                    		 strcpy(namenode,objectnameSUnode->value);
	                                    		 x=objectnameSUnode->length;
	                                    		 objectnameNODE2.length=x;
	                                    		 strncpy((char *)objectnameNODE2.value, namenode, SA_MAX_NAME_LENGTH);
                                                   }//iferror20_sa_ok
	                                    		 error = saImmOmAccessorFinalize(accessorHandle);
	                                     }
	                               
	                                //printf(" is on %s\n",namenode);
	                                //search the other SU's on the same node
	                                searchParam3.searchOneAttr.attrName = "saAmfSUHostedByNode";
	                                searchParam3.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
	                                searchParam3.searchOneAttr.attrValue = &objectnameNODE2;
	
	                                error21 = saImmOmSearchInitialize_2(immHandle, &rootName, scope,
						  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam3, NULL,
						  &searchHandle21);
		                        if (SA_AIS_OK != error21) {
			
	                		printf("error - saImmOmAccessorInitialize FAILED:%d",error21);
	                		exit(EXIT_FAILURE);
					}
	
	       				 if (SA_AIS_OK == error21)
	        			{        c=0;
						 actass1=0,stdass1=0;
                                                 
                               			printf("\nSearch all the SUs on the node hosting this SU of the SG");
			   		   
                                        //
                                               
                                             do {
                
							error22 = saImmOmSearchNext_2(searchHandle21, &objectsu2, &attributessu2);
							if (error22 != SA_AIS_OK && error22 != SA_AIS_ERR_NOT_EXIST) {
                					printf("error22 - saImmOmSearchNext_2 FAILED: %d\n", error22);
                            			        exit(EXIT_FAILURE);
						        }
                				        if(error22 == SA_AIS_ERR_NOT_EXIST)
                                                        {
                                                             arraysu[c]==NULL; 
                                                        }
                
		                                         if (error22 == SA_AIS_OK)
                                                         {
                                                           arraysu[c]=objectsu2.value;
			                                   printf("\n%s", objectsu2.value);
                                                 		//acces the su attribute
                                                        
             							error23 = saImmOmAccessorInitialize( immHandle, &accessorHandle23);
                       						if (error23 != SA_AIS_OK)
		   						{	
                							printf("error23 - saImmOmAccessorInitialize FAILED:%d",error23);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error23 == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU on the node !!\n");
                                                        	        error24= saImmOmAccessorGet_2(accessorHandle23, &objectsu2, attributeNamesSUc,   		
									&attributesSUc);        
                                       					if (error24 ==SA_AIS_OK)
                                                                        {
	                                                              
										SaImmAttrValueT *attrValuec1 = attributesSUc[0]->attrValues[0];
                                                                    		printf("\nActive assignment :%d\n",*((SaUint32T *)attrValuec1));
										actass1 = actass1 + *((SaUint32T *)attrValuec1);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
					               	                }
   		                                        	 }
                                                                 //???
                                                                  error25 = saImmOmAccessorInitialize( immHandle, &accessorHandle23);
                       						if (error25 != SA_AIS_OK)
		   						{	
                							printf("error25 - saImmOmAccessorInitialize FAILED:%d",error25);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error25 == SA_AIS_OK)
   	           						{       
                                                        	        
                      						//	printf("\nWe got the accessor handle to access SU on the node !!\n");
                                                        	        error26= saImmOmAccessorGet_2(accessorHandle23, &objectsu2, attributeNamesSUc1,   		
									&attributesSUc);        
                                       					if (error26 ==SA_AIS_OK)
                                                                        {
	                                                              
										SaImmAttrValueT *attrValuec1 = attributesSUc[0]->attrValues[0];
                                                                    		printf("Standby assignment :%d\n",*((SaUint32T *)attrValuec1));
										stdass1 = stdass1 + *((SaUint32T *)attrValuec1);
               						   			error = saImmOmAccessorFinalize(accessorHandle23);
					               	                }
   		                                        	 }
                                                                 //???
                                                  
                                                     		//access the su attribute
                                                           
                                                           c++; 
							              
                                                          }
	                                         } while (error22 != SA_AIS_ERR_NOT_EXIST);
//
                                        	   printf("The total number of active assignment handled=%d\n",actass1);
                                                   printf("The total number of standby assignment handled=%d\n",stdass1);
                                  
					error = saImmOmSearchFinalize(searchHandle21);                                           
                                      }//        
				//}
                                   //acces the host node
                                   //compare the assignments
                                   
                                     printf("Comparing the assignments handled by the SU hosted on this Node..\n ");
                                     //printf("\n%d",activet);
                                     if(activet > actass1)
                                     {
                                         printf("The SUs hosted on this Node handle least number of assignments hence this node has maximum capacity\n");
 				         activet = actass1;
					 standbyt = stdass1;
					 strcpy(namet,objectsuname.value);
                                         printf("\nthe namet of su is %s",namet);
					 //??
						 error16 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
                       						if (error16 != SA_AIS_OK)
		   						{	
                							printf("error16 - saImmOmAccessorInitialize FAILED:%d",error16);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error16 == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error17= saImmOmAccessorGet_2(accessorHandle15, &objectsu2, attributeNamesSUrank,  

                                                                        &attributesassignmentact);  
                                                                        if (error17 !=SA_AIS_OK)
 									{
										printf("\n%d",error17);					
									} 
                                       					if (error17 ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValuecrk = attributesassignmentact[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										Rankt = *((SaUint32T *)attrValuecrk);
               						   			error = saImmOmAccessorFinalize(accessorHandle15);
                                                                                
                                   						//"%u", *((SaUint32T *)attrValue)
                                            //                                    printf("\n%drank",SuRankFinal);
					               	                }
   		                                        	 }
      								
 					 //??
					 Rankt = Rankt;
					 //printf("\n%d%d%s%d!!!",activet,standbyt,namet,Rankt);
                                      }
				      else if(activet == actass1)
				      {
						if(standbyt > stdass1)
						{
							//<>
							activet = actass1;
					 		standbyt = stdass1;
					 		strcpy(namet,objectsuname.value);
					 //??
							error16 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
                       					if (error16 != SA_AIS_OK)
		   					{	
             							printf("error16 - saImmOmAccessorInitialize FAILED:%d",error16);
               							rc= EXIT_FAILURE;
                   					}
     		   					if (error16 == SA_AIS_OK)
   	           					{            
                      						//printf("\nWe got the accessor handle to access SU for rank !!\n");   
								error17= saImmOmAccessorGet_2(accessorHandle15, &objectsu2,
								 attributeNamesSUrank,   &attributesassignmentact);                                         
                                                                 if (error17 !=SA_AIS_OK)
 								{
									printf("\n%d",error17);					
								} 
                                       				if (error17 ==SA_AIS_OK)
                                                                {
	                                  //                    		printf("\nRank!!!");
									SaImmAttrValueT *attrValuecrk = attributesassignmentact[0]->attrValues[0];
                                                                   		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
									Rankt = *((SaUint32T *)attrValuecrk);
               						   		error = saImmOmAccessorFinalize(accessorHandle15);
                                                                                
                                   						//"%u", *((SaUint32T *)attrValue)
                                            //                                    printf("\n%drank",SuRankFinal);
					               	          }
   		                                         }
      								
 					 //??
					 		Rankt = Rankt;
					 		//printf("\n%d%d%s%d!!!",activet,standbyt,namet,Rankt);
								
							//<>
						}
				      }
				      else
                                      { //do nothing 
				      }
                                   //compare the assignments
                                    


//"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
                               }
                               if(assignmentstd >0)
                               {
                                    printf("It is the standby SU\n");
                                    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 	                                                                        //acces the host node
                                   
	                                     error19 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
					     if (error19 != SA_AIS_OK) 
                                             {
	                		     	printf("error - saImmOmAccessorInitialize FAILED:%d",error19);
	                		     	rc= EXIT_FAILURE;
	                      		     }
	       				     if (error19 == SA_AIS_OK)
	   				     {
	       					//printf("\n<3<3<3\n"); 
	              			       error20= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSU, &attributesSUnode);
	      				       if (error20 ==SA_AIS_OK)
	                                       {
		                  //                 printf("\nsaImmOmAccessorGet_2 function returned the SU object and attribute\n");
	                                      //  }
	                                

	
	                                	//we initialize to attribute to acces our second object SG
	
	                                   		 SaImmAttrValueT *attrValuenode = attributesSUnode[0]->attrValues[0];
	 			            		 SaNameT *objectnameSUnode = (SaNameT *)attrValuenode;
				            		 printf("The SU is on Node : %s (%u)", objectnameSUnode->value,objectnameSUnode->length);
	                                    		 strcpy(namenode,objectnameSUnode->value);
	                                    		 x=objectnameSUnode->length;
	                                    		 objectnameNODE2.length=x;
	                                    		 strncpy((char *)objectnameNODE2.value, namenode, SA_MAX_NAME_LENGTH);
                                                   }//iferror20_sa_ok
	                                    		 error = saImmOmAccessorFinalize(accessorHandle);
	                                     }
	                               
	                                //printf(" is on %s\n",namenode);
	                                //search the other SU's on the same node
	                                searchParam3.searchOneAttr.attrName = "saAmfSUHostedByNode";
	                                searchParam3.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
	                                searchParam3.searchOneAttr.attrValue = &objectnameNODE2;
	
	                                error21 = saImmOmSearchInitialize_2(immHandle, &rootName, scope,
						  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam3, NULL,
						  &searchHandle21);
		                        if (SA_AIS_OK != error21) {
			
	                		printf("error - saImmOmAccessorInitialize FAILED:%d",error21);
	                		exit(EXIT_FAILURE);
					}
	
	       				 if (SA_AIS_OK == error21)
	        			{        c=0;
						 actass1=0,stdass1=0;
                               			printf("\nSearch all the SUs on the Node hosting the SU of the SG \n");
			   		   
                                        //
                                               
                                             do {
                
							error22 = saImmOmSearchNext_2(searchHandle21, &objectsu2, &attributessu2);
							if (error22 != SA_AIS_OK && error22 != SA_AIS_ERR_NOT_EXIST) {
                					printf("error22 - saImmOmSearchNext_2 FAILED: %d\n", error22);
                            			        exit(EXIT_FAILURE);
						        }
                				        if(error22 == SA_AIS_ERR_NOT_EXIST)
                                                        {
                                                             arraysu[c]==NULL; 
                                                        }
                
		                                         if (error22 == SA_AIS_OK)
                                                         {
                                                           arraysu[c]=objectsu2.value;
			                                   printf("%s\n", objectsu2.value);
                                                 		//acces the su attribute
                                                        
             							error23 = saImmOmAccessorInitialize( immHandle, &accessorHandle23);
                       						if (error23 != SA_AIS_OK)
		   						{	
                							printf("error23 - saImmOmAccessorInitialize FAILED:%d",error23);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error23 == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU on the node !!\n");
                                                        	        error24= saImmOmAccessorGet_2(accessorHandle23, &objectsu2, attributeNamesSUc,   		
									&attributesSUc);        
                                       					if (error24 ==SA_AIS_OK)
                                                                        {
	                                                              
										SaImmAttrValueT *attrValuec1 = attributesSUc[0]->attrValues[0];
                                                                    		printf("Active assignment :%d\n",*((SaUint32T *)attrValuec1));
										actass1 = actass1 + *((SaUint32T *)attrValuec1);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
					               	                }
   		                                        	 }
                                                                 //???
                                                                  error25 = saImmOmAccessorInitialize( immHandle, &accessorHandle23);
                       						if (error25 != SA_AIS_OK)
		   						{	
                							printf("error25 - saImmOmAccessorInitialize FAILED:%d",error25);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error25 == SA_AIS_OK)
   	           						{       
                                                        	        
                      						//	printf("\nWe got the accessor handle to access SU on the node !!\n");
                                                        	        error26= saImmOmAccessorGet_2(accessorHandle23, &objectsu2, attributeNamesSUc1,   		
									&attributesSUc);        
                                       					if (error26 ==SA_AIS_OK)
                                                                        {
	                                                              
										SaImmAttrValueT *attrValuec1 = attributesSUc[0]->attrValues[0];
                                                                    		printf("Standby Assignment :%d\n",*((SaUint32T *)attrValuec1));
										stdass1 = stdass1 + *((SaUint32T *)attrValuec1);
               						   			error = saImmOmAccessorFinalize(accessorHandle23);
					               	                }
   		                                        	 }
                                                                 //???
                                                  
                                                     		//access the su attribute
                                                           
                                                           c++; 
							              
                                                          }
	                                         } while (error22 != SA_AIS_ERR_NOT_EXIST);
//
                                        	   printf("The total number of active assignment handled=%d\n",actass1);
                                                   printf("The total number of standby assignment handled=%d\n",stdass1);
                                  
					error = saImmOmSearchFinalize(searchHandle21);                                           
                                      }//        
				//}
                                   //acces the host node
                                   //compare the assignments
                                      // printf("\nactivetemp::::%d",activet);
                                       printf("Comparing the assignments handled ny the SU hosted on this Node..\n ");
                                     //printf("\n **%d**",activet);
                                     if(activet > actass1+assignmentactFinal)
                                     {
                                         printf("This SUs hosted on this Node handle least number of assignments hence this node has maximum capacity\n");
 				         activet = actass1+assignmentactFinal;
					 standbyt = stdass1-assignmentactFinal;
					 strcpy(namet,objectsuname.value);
                                         printf("\nthe namet of su is %s",namet);
					 //??
						 error16 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
                       						if (error16 != SA_AIS_OK)
		   						{	
                							printf("error16 - saImmOmAccessorInitialize FAILED:%d",error16);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error16 == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error17= saImmOmAccessorGet_2(accessorHandle15, &objectsu2, attributeNamesSUrank,  

                                                                        &attributesassignmentact);  
                                                                        if (error17 !=SA_AIS_OK)
 									{
										printf("\n%d",error17);					
									} 
                                       					if (error17 ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValuecrk = attributesassignmentact[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										Rankt = *((SaUint32T *)attrValuecrk);
               						   			error = saImmOmAccessorFinalize(accessorHandle15);
                                                                                
                                   						//"%u", *((SaUint32T *)attrValue)
                                            //                                    printf("\n%drank",SuRankFinal);
					               	                }
   		                                        	 }
      								
 					 //??
					 Rankt = Rankt;
					// printf("\n%d%d%s%d!!!",activet,standbyt,namet,Rankt);
                                      }
				      else if(activet == actass1+assignmentactFinal)
				      {
						if(standbyt > stdass1-assignmentactFinal)
						{
							//<>
							activet = actass1+assignmentactFinal;
					 		standbyt = stdass1-assignmentactFinal;
					 		strcpy(namet,objectsuname.value);
					 //??
							error16 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
                       					if (error16 != SA_AIS_OK)
		   					{	
             							printf("error16 - saImmOmAccessorInitialize FAILED:%d",error16);
               							rc= EXIT_FAILURE;
                   					}
     		   					if (error16 == SA_AIS_OK)
   	           					{            
                      						//printf("\nWe got the accessor handle to access SU for rank !!\n");   
								error17= saImmOmAccessorGet_2(accessorHandle15, &objectsu2,
								 attributeNamesSUrank,   &attributesassignmentact);                                         
                                                                 if (error17 !=SA_AIS_OK)
 								{
									printf("error17-\n%d",error17);	
													
								} 
                                       				if (error17 ==SA_AIS_OK)
                                                                {
	                                  //                    		printf("\nRank!!!");
									SaImmAttrValueT *attrValuecrk = attributesassignmentact[0]->attrValues[0];
                                                                   		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
									Rankt = *((SaUint32T *)attrValuecrk);
               						   		error = saImmOmAccessorFinalize(accessorHandle15);
                                                                                
                                   						//"%u", *((SaUint32T *)attrValue)
                                            //                                    printf("\n%drank",SuRankFinal);
					               	          }
   		                                         }
      								
 					 //??
					 		Rankt = Rankt;
					 	//	printf("\n%d%d%s%d!!!",activet,standbyt,namet,Rankt);
								
							//<>
						}
				      }
				      else
                                      { //do nothing 
				      }
                                   //compare the assignments
                                    
			     

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////                                 
                               }                                    
                               if(assignmentstd ==0 && assignmentact ==0)
                               {
                                   printf("It is spare SU\n");
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
                                                                                  //acces the host node
                                   
	                                     error19 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
					     if (error19 != SA_AIS_OK) 
                                             {
	                		     	printf("error - saImmOmAccessorInitialize FAILED:%d",error19);
	                		     	rc= EXIT_FAILURE;
	                      		     }
	       				     if (error19 == SA_AIS_OK)
	   				     {
	       					//printf("\n<3<3<3\n"); 
	              			       error20= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSU, &attributesSUnode);
	      				       if (error20 ==SA_AIS_OK)
	                                       {
		                  //                 printf("\nsaImmOmAccessorGet_2 function returned the SU object and attribute\n");
	                                      //  }
	                                

	
	                                	//we initialize to attribute to acces our second object SG
	
	                                   		 SaImmAttrValueT *attrValuenode = attributesSUnode[0]->attrValues[0];
	 			            		 SaNameT *objectnameSUnode = (SaNameT *)attrValuenode;
				            		 printf("The SU is on Node : %s (%u)", objectnameSUnode->value,objectnameSUnode->length);
	                                    		 strcpy(namenode,objectnameSUnode->value);
	                                    		 x=objectnameSUnode->length;
	                                    		 objectnameNODE2.length=x;
	                                    		 strncpy((char *)objectnameNODE2.value, namenode, SA_MAX_NAME_LENGTH);
                                                   }//iferror20_sa_ok
	                                    		 error = saImmOmAccessorFinalize(accessorHandle);
	                                     }
	                               
	                                //printf(" is on %s\n",namenode);
	                                //search the other SU's on the same node
	                                searchParam3.searchOneAttr.attrName = "saAmfSUHostedByNode";
	                                searchParam3.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
	                                searchParam3.searchOneAttr.attrValue = &objectnameNODE2;
	
	                                error21 = saImmOmSearchInitialize_2(immHandle, &rootName, scope,
						  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam3, NULL,
						  &searchHandle21);
		                        if (SA_AIS_OK != error21) {
			
	                		printf("error - saImmOmAccessorInitialize FAILED:%d",error21);
	                		exit(EXIT_FAILURE);
					}
	
	       				 if (SA_AIS_OK == error21)
	        			{        c=0;
						 actass1=0,stdass1=0;
                                                 printf("\n activetemp:%d,standbytemp:%d,actass:%d,stdass:%d",activet,standbyt,actass1,stdass1);
                               			printf("\nSearch all the SU on the node hosting the SU of the SG \n");
			   		   
                                        //
                                               
                                             do {
                
							error22 = saImmOmSearchNext_2(searchHandle21, &objectsu2, &attributessu2);
							if (error22 != SA_AIS_OK && error22 != SA_AIS_ERR_NOT_EXIST) {
                					printf("error22 - saImmOmSearchNext_2 FAILED: %d\n", error22);
                            			        exit(EXIT_FAILURE);
						        }
                				        if(error22 == SA_AIS_ERR_NOT_EXIST)
                                                        {
                                                             arraysu[c]==NULL; 
                                                        }
                
		                                         if (error22 == SA_AIS_OK)
                                                         {
                                                           arraysu[c]=objectsu2.value;
			                                   printf("%s\n", objectsu2.value);
                                                 		//acces the su attribute
                                                        
             							error23 = saImmOmAccessorInitialize( immHandle, &accessorHandle23);
                       						if (error23 != SA_AIS_OK)
		   						{	
                							printf("error23 - saImmOmAccessorInitialize FAILED:%d",error23);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error23 == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU on the node !!\n");
                                                        	        error24= saImmOmAccessorGet_2(accessorHandle23, &objectsu2, attributeNamesSUc,   		
									&attributesSUc);        
                                       					if (error24 ==SA_AIS_OK)
                                                                        {
	                                                              
										SaImmAttrValueT *attrValuec1 = attributesSUc[0]->attrValues[0];
                                                                    		printf("Active Assignment :%d\n",*((SaUint32T *)attrValuec1));
										actass1 = actass1 + *((SaUint32T *)attrValuec1);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
					               	                }
   		                                        	 }
                                                                 //???
                                                                  error25 = saImmOmAccessorInitialize( immHandle, &accessorHandle23);
                       						if (error25 != SA_AIS_OK)
		   						{	
                							printf("error25 - saImmOmAccessorInitialize FAILED:%d",error25);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error25 == SA_AIS_OK)
   	           						{       
                                                        	        
                      						//	printf("\nWe got the accessor handle to access SU on the node !!\n");
                                                        	        error26= saImmOmAccessorGet_2(accessorHandle23, &objectsu2, attributeNamesSUc1,   		
									&attributesSUc);        
                                       					if (error26 ==SA_AIS_OK)
                                                                        {
	                                                              
										SaImmAttrValueT *attrValuec1 = attributesSUc[0]->attrValues[0];
                                                                    		printf("Standby Assignment :%d\n",*((SaUint32T *)attrValuec1));
										stdass1 = stdass1 + *((SaUint32T *)attrValuec1);
               						   			error = saImmOmAccessorFinalize(accessorHandle23);
					               	                }
   		                                        	 }
                                                                 //???
                                                  
                                                     		//access the su attribute
                                                           
                                                           c++; 
							              
                                                          }
	                                         } while (error22 != SA_AIS_ERR_NOT_EXIST);
//
                                        	   printf("The total number of active assignment handled=%d\n",actass1);
                                                   printf("The total number of standby assignment handled=%d\n",stdass1);
                                  
					error = saImmOmSearchFinalize(searchHandle21);                                           
                                      }//        
				//}
                                   //acces the host node
                                   //compare the assignments
                                     printf("Comparing the assignments handled ny the SU hosted on this Node..\n ");
                                     if(activet > actass1+assignmentactFinal)
                                     {
                                          printf("This SUs hosted on this Node handle least number of assignments hence this node has maximum capacity\n");
 				         activet = actass1+assignmentactFinal;
					 standbyt = stdass1;
					 strcpy(namet,objectsuname.value);
					 //??
						 error16 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
                       						if (error16 != SA_AIS_OK)
		   						{	
                							printf("error16 - saImmOmAccessorInitialize FAILED:%d",error16);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error16 == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error17= saImmOmAccessorGet_2(accessorHandle15, &objectsu2, attributeNamesSUrank,  

                                                                        &attributesassignmentact);  
                                                                        if (error17 !=SA_AIS_OK)
 									{
										printf("\n%d",error17);					
									} 
                                       					if (error17 ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValuecrk = attributesassignmentact[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										Rankt = *((SaUint32T *)attrValuecrk);
               						   			error = saImmOmAccessorFinalize(accessorHandle15);
                                                                                
                                   						//"%u", *((SaUint32T *)attrValue)
                                            //                                    printf("\n%drank",SuRankFinal);
					               	                }
   		                                        	 }
      								
 					 //??
					 Rankt = Rankt;
					// printf("\n%d%d'%s'%d!!!",activet,standbyt,namet,Rankt);
                                      }
				      else if(activet == actass1+assignmentactFinal)
				      {
						if(standbyt > stdass1)
						{
							//<>
							activet = actass1+assignmentactFinal;
					 		standbyt = stdass1;
					 		strcpy(namet,objectsuname.value);
					 //??
							error16 = saImmOmAccessorInitialize( immHandle, &accessorHandle15);
                       					if (error16 != SA_AIS_OK)
		   					{	
             							printf("error16 - saImmOmAccessorInitialize FAILED:%d",error16);
               							rc= EXIT_FAILURE;
                   					}
     		   					if (error16 == SA_AIS_OK)
   	           					{            
                      						//printf("\nWe got the accessor handle to access SU for rank !!\n");   
								error17= saImmOmAccessorGet_2(accessorHandle15, &objectsu2,
								 attributeNamesSUrank,   &attributesassignmentact);                                         
                                                                 if (error17 !=SA_AIS_OK)
 								{
									printf("\n%d",error17);					
								} 
                                       				if (error17 ==SA_AIS_OK)
                                                                {
	                                  //                    		printf("\nRank!!!");
									SaImmAttrValueT *attrValuecrk = attributesassignmentact[0]->attrValues[0];
                                                                   		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
									Rankt = *((SaUint32T *)attrValuecrk);
               						   		error = saImmOmAccessorFinalize(accessorHandle15);
                                                                                
                                   						//"%u", *((SaUint32T *)attrValue)
                                            //                                    printf("\n%drank",SuRankFinal);
					               	          }
   		                                         }
      								
 					 //??
					 		Rankt = Rankt;
					 		printf("\n%d%d'%s'%d!!!",activet,standbyt,namet,Rankt);
								
							//<>
						}
				      }
				      else
                                      { //do nothing 
				      }
                                   //compare the assignments
                                    
                        
                                  }//////////////////  
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
                               }
                               //checking standby su/active su/spare su and calculating the assignments handled by the hostnodes of these SUs

                        //access SUs of the SG
			
                        counter1++;              
                        printf("\n");  
                  }while (error64 != SA_AIS_ERR_NOT_EXIST);
	
       errorx = saImmOmSearchFinalize(searchHandle63);
	
printf("\n******************************************************************************************\n");
      
        //CRAETE CHANGE BUNDLE HERE JUST TAKE THE NAME OF THE RANKT AND SUNAMET??????????????????????
 
                   //-------------------------------------------//             
                   if(strcmp(namet,SuNameFinal)== 0)
                   {
                      printf("\n SI is already assigned to the SU which is on the least loaded Node");
                      rt=2;
		
		   }
		   else
		   {
                    printf("Create a CCB to swap the rank of the SU(%s) on less loaded node ranked %d with the rank %d of the current active SU(%s)		\n",namet,Rankt,SuRankFinal,SuNameFinal);
                       //printf("\n Ranking a SU on the least loaded node by creating a CCB\n");
                       //??
			strcpy(nameccbattribute,"saAmfSURank=");
                         sprintf(rankt, "%d",SuRankFinal);
               		 strcat(nameccbattribute,rankt);
             		//printf("\n%s",nameccbattribute);
                        s= namet;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[objectNames_len - 1] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       errorc = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (errorc != SA_AIS_OK) {
		       printf("errorc - saImmOmAdminOwnerInitialize FAILED: %d\n", errorc);
		       rc = EXIT_FAILURE;
		      }
		     if (errorc == SA_AIS_OK)
             	     {   

	 	       rc1 = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute); 
		        //printf("\n%d\n",rc1);
                        printf("The IMM Ccb API does not allow to modify the Rank atrribute of SU\n");
			rc1=0;		        
			if(rc1==0)
		        {
		           rt=1;
		        }
        
	             }
	errorc = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != errorc) {
		printf("errorc - saImmOmAdminOwnerFinalize FAILED: %d\n",errorc);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
//swap rank of SU 2
 printf("Create a CCB to swap the rank of the current active SU(%s) ranked %d with the rank  %d of the SU(%s) on less loaded node\n",SuNameFinal,SuRankFinal,Rankt,namet);

       //       printf("\n Ranking a SU on the least loaded node by creating a CCB\n");
                       //??
			strcpy(nameccbattribute,"saAmfSURank=");
                         sprintf(rankt, "%d",Rankt);
               		 strcat(nameccbattribute,rankt);
             		// printf("\n%s",nameccbattribute);
                        s= SuNameFinal;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[objectNames_len - 1] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       errorc = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (errorc != SA_AIS_OK) {
		       printf("errorc - saImmOmAdminOwnerInitialize FAILED: %d\n", errorc);
		       rc = EXIT_FAILURE;
		      }
		     if (errorc == SA_AIS_OK)
             	     {   

	 	       rc1 = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute); 
		        //printf("\n%d\n",rc1);
                        printf("The IMM Ccb API does not allow to modify the Rank atrribute of SU\n");
                        
                        rc1=0;
		        if(rc1==0)
		        {
		           rt=1;
		        }
        
	             }
	errorc = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != errorc) {
		printf("errorc - saImmOmAdminOwnerFinalize FAILED: %d\n",errorc);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
      swapsi(immHandle,argv,7,objectnameSI);
  //swap rank of SU 2

                    
                   }//else to create a ccb
           return rt;
         }          

//the su of the sgs

}

insert(char *str,size_t len,char c,size_t pos)
{
  memmove(&str[pos+1],&str[pos],len - pos +1);
  str[pos] = c;
}


/////N-way Active Single SI Increase_start
int single_SIincreaseNwayAct(SaImmHandleT immHandle,SaNameT objectnameSG1,SaNameT objectnameSI,char *argv[],int preinstantiable)
{
printf("%c[1;32m\n***********************************************************************************",27);
printf("\n*******************************RM Adjustor*****************************************");
printf("%c[0m",27);
//printf("I got a function call\n"); 
//printf("the SI : %s\n",objectnameSI.value);
SaImmAttrNameT  attributeNames[2] ={"saAmfSIPrefActiveAssignments",NULL};
SaAisErrorT error;

SaImmScopeT scope = SA_IMM_SUBTREE;	/* default search scope */
int flag=1,flag1=1;
SaUint32T noassgSI;
 int objectNames_len = 1;
SaImmAccessorHandleT accessorHandle;
SaImmSearchHandleT searchHandle;
SaNameT objectName,objectsity;
SaImmAttrValuesT_2 **attributes;
 SaImmAdminOwnerHandleT ownerHandle;
 SaNameT **objectNames = NULL;
SaNameT *objectNamec;
SaNameT *objectName1c;
char *s;

char nameccbattribute[256],noassg[256];
SaImmSearchParametersT_2 searchParam;
SaUint32T noSIassg,noassgSUs,noinserviceSUs,maxactSIsperSU,activeSIs,activeSIssum,maxactSIsperSUt,DefActiveNumCSIs;
int rc = EXIT_SUCCESS;
SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
loop6:
printf("%c[1;32m\nCheck if the SI is assigned to all the SUs of the SG\n",27);
printf("%c[0m",27);
//printf("\n**\n");
error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{        
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSI, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassg = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of SI assignment of %s SI are %d\n",objectnameSI.value,noSIassg);

					               	                }
   		                                        	 }
attributeNames[0]="saAmfSGNumPrefAssignedSUs";
attributeNames[1]=NULL;
error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of SUs assignment of %s of SG are %d\n",objectnameSG1.value,noassgSUs);
					               	                }
   		                                        	 }

if(noassgSUs > noSIassg)
{
  printf("%c[1;32m\nThe SI is not assigned to all the SUs\n",27);
  printf("We check if enough capacity\n");
  printf("%c[0m",27);
  loop4:
  //printf("\n&&\n");
  attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxActiveSIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefAssignedSUs of %s SG is %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }
	//"Calculating the total required capacity i.e summing up the SI assignments");							
 
			    printf("Calculating the total required capacity i.e summing up the SI assignments");
        		    searchParam.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 			    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 			    searchParam.searchOneAttr.attrValue = &objectnameSG1;

			    error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 if (SA_AIS_OK != error) {
			  		 	         //printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			         rc=EXIT_FAILURE;
                	   				 flag=1;
			     }
                            if (SA_AIS_OK == error)
        		    {
		         	//printf("\n\nWe got the search handle \n");
                                 activeSIs=0,activeSIssum=0;
            					do {                
                   					error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
							if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
							printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 rc=EXIT_FAILURE;
					  		flag=1;
						    }
                                        


                                       		    if(error == SA_AIS_ERR_NOT_EXIST )
               					    {
		  	  				 if(flag1==1)
                  	   			         { 
                     					//arraySU[counter]==NULL;
                     					//printf("\nobject does not exist"); 
        	    	   			         }
		     	   				else
		     	  				{ 
                             					rc=EXIT_FAILURE;
                             					flag=1;
 		          				}        
						   }
                
						  if (error == SA_AIS_OK)
                  				  {
 
                              
							   attributeNames[0] ="saAmfSIPrefActiveAssignments";
							   attributeNames[1] =NULL;
							   error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   if (error != SA_AIS_OK) {
                	       					printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       					rc= EXIT_FAILURE;
								flag = 1;
                      				   	     }
       			       			 	  if (error == SA_AIS_OK)
   						  	 {
								error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        			if (error !=SA_AIS_OK)
                                        			{
                                               				printf("\nerror18:%d",error);
									rc= EXIT_FAILURE;
                                                        		flag = 1; 
                                        		         }
                        	 				if (error ==SA_AIS_OK)
                                 				{
			                     		
					     				SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    				activeSIs = *((SaUint32T *)attrValues);
                                                              	  //printf("\nthe end\n",activeSis);
                                                               		// printf("Active assignments :\n%d",activeSIs);
                		   	               	 		error = saImmOmAccessorFinalize(accessorHandle);
							        }
       								activeSIssum = activeSIssum + activeSIs;
                        		                 }//accesor initialize
                                               	

                                         }//search next
                                       
                                    }while (error != SA_AIS_ERR_NOT_EXIST);
                    printf(" total number of SIs is %d\n",activeSIssum);
	//"Calculating the total required capacity i.e summing up the SI assignments");
  }//search initialize
 	
	 if(noassgSUs * maxactSIsperSU >= activeSIssum+1) 
	 {
	        printf("%c[1;32mThere is enough capacity to increase the SI assignment\n",27);
                printf("%c[0m",27);
	 	printf("%c[1;33m\nCreate a CCB to increase saAmfSIPrefActiveAssignments\n",27);
                printf("Creating...\n");
    	 	noassgSI = noSIassg+1; 
		strcpy(nameccbattribute,"saAmfSIPrefActiveAssignments=");
                         sprintf(noassg, "%d",noassgSI);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSI.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectName1c = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectName1c->length = snprintf((char*)objectName1c->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
		       //printf("\n%s in increase the num of  Si assgn",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
		        printf("%c[0m",27);
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
		error = saImmOmAdminOwnerFinalize(ownerHandle);
		if (SA_AIS_OK != error) {
			printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
			rc = EXIT_FAILURE;
		//	goto done_om_finalize;
		}
 	}
 	else
////////////////////////////////////////////////////////////////////////////////////////////////////////increase the max /////////////////////////////////////////////////////////////////////
 	{
 /** need this code for creating the string that is harcoded
 		    printf("need to check conditions to increase max active SIper SU\n");
        	    attributeNames[0]="saAmfSvcType";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										//syctype = *((SaUint32T *)attrValue);

//SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
										SaNameT *objectsy = (SaNameT *)attrValue;
			printf("The si syc type is : %s (%u) \n", objectsy->value,objectsy->length);
//strcpy(sisy,objectsy->value);

objectsity.length= objectsy->length;
strncpy((char *)objectsity.value, objectsy->value, SA_MAX_NAME_LENGTH);


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe SI syc type is %s \n",objectsity.value);
					               	                }
need this code for creating the string that is harcoded**/	
//'''''just create the string which is hard coded below a....put the code here.......''''''//



printf("%c[1;31m\nNeed to increase saAmfSGMaxActiveSIsperSU\n",27);
printf("%c[0m",27);
strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
printf("The SI .. type is %s \n",objectsity.value);
  	    attributeNames[0]="saAmfCtDefNumMaxActiveCSIs";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefActiveNumCSIs = *((SaUint32T *)attrValue);

										
										printf("The saAmfCtDefNumMaxActiveCSIs : %d \n", DefActiveNumCSIs );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe cSI is %d \n",DefActiveNumCSIs);
					               	                }
								 }

        ////get the maxactiveSIperSU attribute
         attributeNames[0]="saAmfSGMaxActiveSIsperSU";
 	 attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxActiveSIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
					
   								 }
        if(maxactSIsperSU < DefActiveNumCSIs)  
        {
            printf("%c[1;33m\nCreate a CCB to increase saAmfSGMaxActiveSIsperSU\n",27);
            printf("Creating...\n");
             
            //////ccb to increase maxactiveSiperSU
            swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		noassgSUs = maxactSIsperSU+1; 
		strcpy(nameccbattribute,"saAmfSGMaxActiveSIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
	printf("%c[0m",27);
            //////ccb to increase maxactiveSiperSU
            goto loop4;
        }  
        else
        {
           goto loop2;
        }
	///////get maxActiveSiperSU attribute
 	}// noassgSUs * maxactSIsperSU >= activeSIssum+1


/**????????????????????

strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
printf("\nThe SI .. type is %s \n",objectsity.value);
  	    attributeNames[0]="safSupportedCsType";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										//syctype = *((SaUint32T *)attrValue);

//SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
										SaNameT *objectsy = (SaNameT *)attrValue;
			printf("The saSuppertedCstype : %s (%u) \n", objectsy->value,objectsy->length);
//strcpy(sisy,objectsy->value);

objectsity.length= objectsy->length;
strncpy((char *)objectsity.value, objectsy->value, SA_MAX_NAME_LENGTH);


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe SI syc type is %s \n",objectsity.value);
					               	                }
								 }

 	//}
**/
//????????????????




////////////////////////////////////////////////////////////////////////////////////////////////////////increase the max /////////////////////////////////////////////////////////////////////

}// IF(noassgSUs > noSIassg)
else
{
  loop2:
  //printf("\nloop2");
  printf("%c[1;31m\nWe check to increase saAmfSGNumPrefAssignedSUs\n",27);
  printf("%c[0m",27);
 if(preinstantiable == 1)
  {
    attributeNames[0]="saAmfSGNumPrefInserviceSUs";
    attributeNames[1]=NULL;
    error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noinserviceSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of inservice SUs of %s SG are %d\n",objectnameSG1.value,noinserviceSUs);
					               	                }
 //printf("To check to increase saAmfSGNumPrefAssignedSUs we check for enough inservice SUs\n");
  		                                        	 }
}
else
{
       noinserviceSUs =  num_SUofSG(immHandle,objectnameSG1);  
}
 	if (noinserviceSUs > noassgSUs)
 	{
 	    printf("%c[1;31mThere is enough capacity ... \n",27);
            printf("%c[0m",27);
            printf("%c[1;33m\nCreate a CCB to increase saAmfSGNumPrefAssignedSUs\n",27);
            printf("Creating...\n");
            swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		noassgSUs = noassgSUs+1; 
		strcpy(nameccbattribute,"saAmfSGNumPrefAssignedSUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	    //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
	printf("%c[0m",27);
	//give a asynchronous call to  buffer manager functions//
if(preinstantiable == 1)
  {
     buffer_checkker(immHandle,objectnameSG1,objectnameSI,0,argv);
  }
  else
  {
    buffer2_checkker(immHandle,objectnameSG1,objectnameSI,argv);
  }
	//give a asynchronous call to  buffer manager functions//

///////////////////////////////////////////Decrease the max acrive SI per SU ///////////////////////////////////////////////////////
loop3:
//printf("\n!!!!\n");
printf("%c[1;32m\nChecking to spread the SI assignment i.e decrease the saAmfSGMaxActiveSIsperSU",27);
printf("%c[0m",27);
 attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe number of saAmfSGMaxActiveSIsperSU of %s SG are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }
	//"Calculating the total required capacity i.e summing up the SI assignments");							
 
			    printf("Calculating the total required capacity i.e summing up the SI assignments");
        		    searchParam.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 			    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 			    searchParam.searchOneAttr.attrValue = &objectnameSG1;

			    error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 if (SA_AIS_OK != error) {
			  		 	         //printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			         rc=EXIT_FAILURE;
                	   				 flag=1;
			     }
                            if (SA_AIS_OK == error)
        		    {
		         	//printf("\n\nWe got the search handle \n");
                                 activeSIs=0,activeSIssum=0;
            					do {                
                   					error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
							if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
							printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 rc=EXIT_FAILURE;
					  		flag=1;
						    }
                                        


                                       		    if(error == SA_AIS_ERR_NOT_EXIST )
               					    {
		  	  				 if(flag1==1)
                  	   			         { 
                     					//arraySU[counter]==NULL;
                     					//printf("\nobject does not exist"); 
        	    	   			         }
		     	   				else
		     	  				{ 
                             					rc=EXIT_FAILURE;
                             					flag=1;
 		          				}        
						   }
                
						  if (error == SA_AIS_OK)
                  				  {
 
                              
							   attributeNames[0] ="saAmfSIPrefActiveAssignments";
							   attributeNames[1] =NULL;
							   error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   if (error != SA_AIS_OK) {
                	       					printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       					rc= EXIT_FAILURE;
								flag = 1;
                      				   	     }
       			       			 	  if (error == SA_AIS_OK)
   						  	 {
								error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        			if (error !=SA_AIS_OK)
                                        			{
                                               				printf("\nerror18:%d",error);
									rc= EXIT_FAILURE;
                                                        		flag = 1; 
                                        		         }
                        	 				if (error ==SA_AIS_OK)
                                 				{
			                     		
					     				SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    				activeSIs = *((SaUint32T *)attrValues);
                                                              	  //printf("\nthe end\n",activeSis);
                                                               		// printf("Active assignments :\n%d",activeSIs);
                		   	               	 		error = saImmOmAccessorFinalize(accessorHandle);
							        }
       								activeSIssum = activeSIssum + activeSIs;
                        		                 }//accesor initialize
                                               	

                                         }//search next
                                       
                                    }while (error != SA_AIS_ERR_NOT_EXIST);
                    printf(" total number of SIs is %d\n",activeSIssum);
	//"Calculating the total required capacity i.e summing up the SI assignments");
	  }//search initialize
	
	 maxactSIsperSUt= maxactSIsperSU-1;
	 //printf("\n%d\n", maxactSIsperSUt);
	printf("Comparing the available capacity and required capacity after subtrating\n");
	if((noassgSUs*maxactSIsperSUt) >= activeSIssum) 
	 {
   	  swapsi(immHandle,argv,2,objectnameSG1); //lock the SG
    	 printf("%c[1;32mThere is enough capacity ...\n",27);
         printf("%c[0m",27);
         printf("%c[1;33mCreate CCB to decrease number of  saAmfSGMaxActiveSIperSU\n",27);
         printf("Creating...\n");
    	 	noassgSI = maxactSIsperSUt; 
		strcpy(nameccbattribute,"saAmfSGMaxActiveSIsperSU=");
                         sprintf(noassg, "%d",noassgSI);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
		       
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
		
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
		error = saImmOmAdminOwnerFinalize(ownerHandle);
		if (SA_AIS_OK != error) {
			printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
			rc = EXIT_FAILURE;
		//	goto done_om_finalize;
		}
     	  swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
          printf("%c[0m",27);
    printf("%c[1;32m\nCheck if need to spread it further..\n",27);
    printf("%c[0m",27);
/////////////////////////////////////////////////////////////////////////////////////////////////////Check if have to spread further///////////////////////////////////////////////////////////////
printf("%c[1;32mChecking to spread the SI assignment i.e decrease the saAmfSGMaxActiveSIsperSU\n",27);
printf("%c[0m",27);
 attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of saAmfSGMaxActiveSIsperSU of %s SG are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }
	//"Calculating the total required capacity i.e summing up the SI assignments");							
 
			    printf("Calculating the total required capacity i.e summing up the SI assignments");
        		    searchParam.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 			    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 			    searchParam.searchOneAttr.attrValue = &objectnameSG1;

			    error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 if (SA_AIS_OK != error) {
			  		 	         //printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			         rc=EXIT_FAILURE;
                	   				 flag=1;
			     }
                            if (SA_AIS_OK == error)
        		    {
		         	//printf("\n\nWe got the search handle \n");
                                 activeSIs=0,activeSIssum=0;
            					do {                
                   					error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
							if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
							printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 rc=EXIT_FAILURE;
					  		flag=1;
						    }
                                        


                                       		    if(error == SA_AIS_ERR_NOT_EXIST )
               					    {
		  	  				 if(flag1==1)
                  	   			         { 
                     					//arraySU[counter]==NULL;
                     					//printf("\nobject does not exist"); 
        	    	   			         }
		     	   				else
		     	  				{ 
                             					rc=EXIT_FAILURE;
                             					flag=1;
 		          				}        
						   }
                
						  if (error == SA_AIS_OK)
                  				  {
 
                              
							   attributeNames[0] ="saAmfSIPrefActiveAssignments";
							   attributeNames[1] =NULL;
							   error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   if (error != SA_AIS_OK) {
                	       					printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       					rc= EXIT_FAILURE;
								flag = 1;
                      				   	     }
       			       			 	  if (error == SA_AIS_OK)
   						  	 {
								error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        			if (error !=SA_AIS_OK)
                                        			{
                                               				printf("\nerror18:%d",error);
									rc= EXIT_FAILURE;
                                                        		flag = 1; 
                                        		         }
                        	 				if (error ==SA_AIS_OK)
                                 				{
			                     		
					     				SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    				activeSIs = *((SaUint32T *)attrValues);
                                                              	  //printf("\nthe end\n",activeSis);
                                                               		// printf("Active assignments :\n%d",activeSIs);
                		   	               	 		error = saImmOmAccessorFinalize(accessorHandle);
							        }
       								activeSIssum = activeSIssum + activeSIs;
                        		                 }//accesor initialize
                                               	

                                         }//search next
                                       
                                    }while (error != SA_AIS_ERR_NOT_EXIST);
                    printf(" total number of SIs is %d\n",activeSIssum);
	//"Calculating the total required capacity i.e summing up the SI assignments");
	  }//search initialize
	
	 //maxactSIsperSUt= maxactSIsperSU-1;
	 //printf("\n%d\n", maxactSIsperSUt);
	printf("%c[1;32mComparing the available capacity and required capacity after subtrating\n",27);
        printf("%c[0m",27);
	if((noassgSUs*maxactSIsperSU) == activeSIssum)
	{
           rc=1;
           printf("%c[1;32m\nCapacity is enought",27);
           printf("%c[0m",27);
	} 
        else
        {
                     goto loop3;      
        }
////////////////////////////////////////////////////////////////////////////////////////////////////Check  if have to spread further///////////////////////////////////////////////////////////////



	}//if maxactive*numassinedSU >= sum of SI assinment condition
	else
	{
            printf("%c[1;31mNot enough capacity to decrease saAmfSGMaxActiveSIperSU",27);
            printf("%c[0m",27);
            goto loop6;
	}

///////////////////////////////////////////////////////////////////Decrease the max active SI per SU////////////////////////////////////////////////////





 	
 	}
 	else
	{
            printf("%c[1;31mCluster level elasticity\n",27);
            printf("%c[0m",27);
            rc = 2;
            
	}//if
  
}//if
 //printf("rc2:%d\n",rc);
return rc;
}
/////N-way Active Single SI Increase_end




////N-way Active Multiple SI increase_start
int multiple_SIincreaseNwayAct(SaImmHandleT immHandle,SaNameT objectnameSG1,SaNameT objectnameSI,char *argv[],int preinstantiable)
{
 //*********attribute declariation*************************//
printf("%c[1;32m\n***********************************************************************************",27);
printf("\n***********************************RM Adjustor*************************************");
printf("%c[0m",27);
  SaImmAttrNameT  attributeNames[2] ={"saAmfSIPrefActiveAssignments",NULL};
  SaAisErrorT error;
   int objectNames_len = 1;
  SaNameT objectName,objectsity;;
  SaImmScopeT scope = SA_IMM_SUBTREE;	/* default search scope */
  int flag=1,flag1=1;
  SaUint32T noassgSI;
  SaImmAccessorHandleT accessorHandle;
  SaImmSearchHandleT searchHandle;
  SaImmAttrValuesT_2 **attributes;
  SaImmAdminOwnerHandleT ownerHandle;
  SaNameT **objectNames = NULL;
  SaNameT *objectNamec;
  SaNameT *objectName1c;
  char *s;
  char nameccbattribute[256],noassg[256];
  SaImmSearchParametersT_2 searchParam;
  SaUint32T noSIassg,noassgSUs,noinserviceSUs,maxactSIsperSU,activeSIs,activeSIssum,maxactSIsperSUt,DefActiveNumCSIs;
  int rc = EXIT_SUCCESS;
  SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
 //*********attribute declariation*************************//
  loop7:
  attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{      
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                          			maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGMaxActiveSIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{         
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGNumPrefAssignedSUs of %s SG is %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }
	//"Calculating the total required capacity i.e summing up the SI assignments");							
 
			    printf("Calculating the total required capacity i.e summing up the SI assignments");
        		    searchParam.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 			    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 			    searchParam.searchOneAttr.attrValue = &objectnameSG1;

			    error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 if (SA_AIS_OK != error) {
			  		 	         //printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			         rc=EXIT_FAILURE;
                	   				 flag=1;
			     }
                            if (SA_AIS_OK == error)
        		    {
		         	//printf("\n\nWe got the search handle \n");
                                 activeSIs=0,activeSIssum=0;
            					do {                
                   					error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
							if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
							printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 rc=EXIT_FAILURE;
					  		flag=1;
						    }
                                        


                                       		    if(error == SA_AIS_ERR_NOT_EXIST )
               					    {
		  	  				 if(flag1==1)
                  	   			         { 
                     					//arraySU[counter]==NULL;
                     					//printf("\nobject does not exist"); 
        	    	   			         }
		     	   				else
		     	  				{ 
                             					rc=EXIT_FAILURE;
                             					flag=1;
 		          				}        
						   }
                
						  if (error == SA_AIS_OK)
                  				  {
 
                              
							   attributeNames[0] ="saAmfSIPrefActiveAssignments";
							   attributeNames[1] =NULL;
							   error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   if (error != SA_AIS_OK) {
                	       					printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       					rc= EXIT_FAILURE;
								flag = 1;
                      				   	     }
       			       			 	  if (error == SA_AIS_OK)
   						  	 {
								error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        			if (error !=SA_AIS_OK)
                                        			{
                                               				printf("\nerror18:%d",error);
									rc= EXIT_FAILURE;
                                                        		flag = 1; 
                                        		         }
                        	 				if (error ==SA_AIS_OK)
                                 				{
			                     		
					     				SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    				activeSIs = *((SaUint32T *)attrValues);
                                                              	  //printf("\nthe end\n",activeSis);
                                                               		// printf("Active assignments :\n%d",activeSIs);
                		   	               	 		error = saImmOmAccessorFinalize(accessorHandle);
							        }
       								activeSIssum = activeSIssum + activeSIs;
                        		                 }//accesor initialize
                                               	

                                         }//search next
                                       
                                    }while (error != SA_AIS_ERR_NOT_EXIST);
                    printf(" total number of SIs is %d\n",activeSIssum);
	//"Calculating the total required capacity i.e summing up the SI assignments");
  }//search initialize
 	
	 if(noassgSUs * maxactSIsperSU < activeSIssum) 
	 {
                printf("\nthe new SI is not been assigned\n");

///////////////////increase maxActiveSIperSU/////////////////////////////////////////////
                strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
		objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
		printf("\nThe SI .. type is %s \n",objectsity.value);
                printf("Lets check the components capabiltiy model for the service type...\n");
  	    	attributeNames[0]="saAmfCtDefNumMaxActiveCSIs";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefActiveNumCSIs = *((SaUint32T *)attrValue);

										
										//printf("The saAmfCtDefNumMaxActiveCSIs : %d \n", DefActiveNumCSIs );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                printf("The component capabiltiy model allows : %d \n", DefActiveNumCSIs );
                                   						//printf("\nThe cSI is %d \n",DefActiveNumCSIs);
					               	                }
								 }
								attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  								attributeNames[1]=NULL;
  								error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{      
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                          			maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGMaxActiveSIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
								if(maxactSIsperSU<DefActiveNumCSIs)
								{
  									printf("Create a CCB to increase saAmfSGMaxActiveSIsperSU of %s SG\n",objectnameSG1.value);
 									 ////////////////////////create ccb to increase saAmfSGMaxActiveSIsperSU
  								         swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
 								        //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 								        noassgSUs = maxactSIsperSU+1; 
		    					                strcpy(nameccbattribute,"saAmfSGMaxActiveSIsperSU=");
                         						sprintf(noassg, "%d",noassgSUs);
               								strcat(nameccbattribute,noassg);
             								// printf("\n%s",nameccbattribute);
                        						s= objectnameSG1.value ;
                        						//printf("\n%s\n",s);
                        						objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 									objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
									objectNames[objectNames_len++] = NULL;
 									objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       							//??
      		       
		      							 error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      						        if (error != SA_AIS_OK) {
		       								printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		      								 rc = EXIT_FAILURE;
		     							 }
		     				         		if (error == SA_AIS_OK)
             	    					                {   
			  
	 	      	  							 rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                      								 free(objectNamec);
                      
		       								 if(rc==0)
		        							{
		          	 							rc=1;
		      							        }
        
	          						         }
	
									error = saImmOmAdminOwnerFinalize(ownerHandle);
									if (SA_AIS_OK != error) {
									printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
									rc = EXIT_FAILURE;
								//	goto done_om_finalize;
									}
								    //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
									swapsi(immHandle,argv,1,objectnameSG1);
 							/////////////////////////////////////////////create a ccb to increase saAmfSGMaxActiveSIsperSU
 									 goto loop7;
  	
								 }
								 else
								{
                                       				        ///////////////////////////////////check to increase the saAmfSGNumPrefAssignedSUs
                                        			         printf("We check to increase saAmfSGNumPrefAssignedSUs\n");
                                                                        if(preinstantiable == 1)
                                                                       {
									attributeNames[0]="saAmfSGNumPrefInserviceSUs";
									attributeNames[1]=NULL;
									error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       							if (error != SA_AIS_OK)
		   							{	
                								printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                								rc= EXIT_FAILURE;
                    							}
       		   							if (error == SA_AIS_OK)
   	           							{       
                                                        	        
                      							     
										error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                 	        if (error !=SA_AIS_OK)
 										{
											printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
										} 
                                       						if (error ==SA_AIS_OK)
                                                                      	        {
	                                  //                            		
											SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    			//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
											noinserviceSUs = *((SaUint32T *)attrValue);
               						   				error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   							printf("\nThe number of inservice SUs of %s SG are %d\n",objectnameSG1.value,noinserviceSUs);
					               	               		 }
 									//printf("To check to increase saAmfSGNumPrefAssignedSUs we check for enough inservice SUs\n");
  		                                        	       }
                                                                      }
                                                                      else
                                                                   
                                                                         {
                                                                              noinserviceSUs=  num_SUofSG(immHandle,objectnameSG1);  
                                                                          }
                                                                   
								
     									//printf("We check to increase saAmfSGNumPrefAssignedSUs\n");
  
									attributeNames[0]="saAmfSGNumPrefAssignedSUs";
									attributeNames[1]=NULL;
									error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       							if (error != SA_AIS_OK)
		   							{	
                								printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                								rc= EXIT_FAILURE;
                    							}
       		   							if (error == SA_AIS_OK)
   	           							{       
                                                        	        
                      							     
										error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                 	        if (error !=SA_AIS_OK)
 										{
											printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
										} 
                                       						if (error ==SA_AIS_OK)
                                                                      	        {
	                                  //                            		
											SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    			//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
											noassgSUs = *((SaUint32T *)attrValue);
               						   				error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   							printf("\nThe number of assigned SUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
					               	               		 }
 									//printf("To check to increase saAmfSGNumPrefAssignedSUs we check for enough inservice SUs\n");
  		                                        	       }


 									if (noinserviceSUs > noassgSUs)
 									{
											


										printf("There is enough capacity ... we increase number of saAmfSGNumPrefAssignedSUs\n");
            				  					swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             				 					 //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 					 					 noassgSUs = noassgSUs+1; 
					 					 strcpy(nameccbattribute,"saAmfSGNumPrefAssignedSUs=");
                         		 					 sprintf(noassg, "%d",noassgSUs);
               		 	         					 strcat(nameccbattribute,noassg);
             									// printf("\n%s",nameccbattribute);
                        		 					 s= objectnameSG1.value ;
                        		 					 //printf("\n%s\n",s);
                                         					 objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 					 					 objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
				       	 					 objectNames[objectNames_len++] = NULL;
 					 					 objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       									//??
      		       
		       			 					 error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      			 					 if (error != SA_AIS_OK) {
		       			  					 printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       								 rc = EXIT_FAILURE;
		      								}
		     								if (error == SA_AIS_OK)
             	     								{   
											//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       									rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       									free(objectNamec);
                      
		        								if(rc==0)
		       									{
		           									rc=1;
		        								}
        
	             								}
	
										error = saImmOmAdminOwnerFinalize(ownerHandle);
										if (SA_AIS_OK != error) {
										printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
										rc = EXIT_FAILURE;
			 							//	goto done_om_finalize;
										}
	   									 //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
										swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
	
                                          					     //////////////////////////////////check to increase the saAmfSGNUmPrefAssignedSUs    
                                         
											///////////////////////////////////////////asynchronous call to buffer
                                                                                         if(preinstantiable == 1)
  										        {
   												  buffer_checkker(immHandle,objectnameSG1,objectnameSI,0,argv);
  											}
 											 else
 											 {
 												   buffer2_checkker(immHandle,objectnameSG1,objectnameSI,argv);
 											 }
                               								 //////////////////////////////////////////asynchrounous call to buffer
                               							goto loop7;     


                                                                        }
                                                                        else
                                                                        {
                                                                            
                                					    printf("Need to increase the number of nodes in the cluster");
                                 						   rc = 3;
                              	
                                                                        }   
 	  				   
				       				 }//maxactSIsperSU<DefActiveNumCSIs
							///////////////////increase maxActiveSIperSU/////////////////////////////////////////////
                              
                               
         }
         else
         {
               printf("\nthe new SI is already assigned\n");
               rc=1;
         }//noassgSUs * maxactSIsperSU < activeSIssum
 return rc;

}
///N-way Active Multiple SI increase_end

///N-way Active Single SI decrease_start
int single_SIdecreaseNwayAct(SaImmHandleT immHandle,SaNameT objectnameSG1,SaNameT objectnameSI,char *argv[],int elasticityT,int preinstantiable)
{
printf("%c[1;32m\n***********************************************************************************",27);
printf("\n**************************************RM Adjustor**********************************");
printf("%c[0m",27);
   SaImmAttrNameT  attributeNames[2] ={"saAmfSIPrefActiveAssignments",NULL};
SaAisErrorT error;

SaImmScopeT scope = SA_IMM_SUBTREE;	/* default search scope */
int flag=1,flag1=1,no_SUs=1,i;
SaUint32T noassgSI,assignmentactF;
 int objectNames_len = 1;
SaImmAccessorHandleT accessorHandle;
SaImmSearchHandleT searchHandle;
SaNameT objectName,objectsity,objectname,objectname1;
SaImmAttrValuesT_2 **attributes,**attributes1;
 SaImmAdminOwnerHandleT ownerHandle;
 SaNameT **objectNames = NULL;
SaNameT *objectNamec;
SaNameT *objectName1c;
SaNameT objtnamesgtype;
 SaNameT objectnamevalidsutype1;
char *s;
int l=0;
SaInt8T namevalidsutype[256];
 SaImmSearchParametersT_2 searchParam;
int no_of_SIs=0,curr_assg_SUs=0;

char nameccbattribute[256],noassg[256];

SaUint32T noSIassg,noassgSUs,noinserviceSUs,maxactSIsperSU,activeSIs,activeSIssum,maxactSIsperSUt,DefActiveNumCSIs;
int rc = EXIT_SUCCESS;
SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
if(elasticityT ==1)
{
loop8:
//printf("\n**\n");
error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{        
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSI, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassg = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("%c[1;32m\nThe number of SI assignment of %s SI are %d\n",27,objectnameSI.value,noSIassg);
printf("%c[0m",27);					               	                }
   		                                        	 }
	if(noSIassg >= 3)
	{
 		 printf("%c[1;33mCreate a CCB to decrease number of SI assignment\n",27);
 		 noassgSI = noSIassg-1; 
		 strcpy(nameccbattribute,"saAmfSIPrefActiveAssignments=");
                         sprintf(noassg, "%d",noassgSI);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSI.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectName1c = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectName1c->length = snprintf((char*)objectName1c->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
		       //printf("\n%s in increase the num of  Si assgn",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
		       printf("%c[0m",27);
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
		error = saImmOmAdminOwnerFinalize(ownerHandle);
		if (SA_AIS_OK != error) {
			printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
			rc = EXIT_FAILURE;
		//	goto done_om_finalize;
		}
                goto loop8;
	}
        else
        {
            goto loop9;
        }
}
	else
	{       
		printf("%c[1;32mChecking to decrease saAmfSGNumPrefAssignedSUs\n",27);
                printf("%c[0m",27);
                loop9:
            //    printf("**\n");
                
                attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  	        attributeNames[1]=NULL;
  		error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGMaxActiveSIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  								attributeNames[0]="saAmfSGNumPrefAssignedSUs";
 								attributeNames[1]=NULL;
  								error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGNumPrefAssignedSUs of %s SG is %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }
								//"Calculating the total required capacity i.e summing up the SI assignments");							
 
								    printf("Calculating the total required capacity i.e summing up the SI assignments");
        							    searchParam.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                activeSIs=0,activeSIssum=0,no_of_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									 if(flag1==1)
                  	   			         					{ 
                     											//arraySU[counter]==NULL;
                     											//printf("\nobject does not exist"); 
        	    	   			       						 }
		     	   									else
		     	  									{ 
                             										rc=EXIT_FAILURE;
                             										flag=1;
 		          									}        
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					attributeNames[0] ="saAmfSIPrefActiveAssignments";
							  				        attributeNames[1] =NULL;
							   					error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   					if (error != SA_AIS_OK) {
                	       										printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       										rc= EXIT_FAILURE;
													flag = 1;
                      				   	     					}
       			       			 	  					if (error == SA_AIS_OK)
   						  						{
													error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        								if (error !=SA_AIS_OK)
                                        								{
                                               									printf("\nerror18:%d",error);
														rc= EXIT_FAILURE;
                                                        							flag = 1; 
                                        		        					 }
                        	 									 if (error ==SA_AIS_OK)
                                 									 {
			                     		                                                        no_of_SIs++;
					     									SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    									activeSIs = *((SaUint32T *)attrValues);
                                                              	  
                		   	               	 							error = saImmOmAccessorFinalize(accessorHandle);
							       						 }
       													activeSIssum = activeSIssum + activeSIs;
                        		                 					}//accesor initialize
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",activeSIssum);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
                                               
     									     noassgSUs = noassgSUs-1;																	
									     if(noassgSUs * maxactSIsperSU >= activeSIssum) 
									     {
                                                                                        printf("\nAlso checking if after decreasing the number of SUs the capacity remaining is enough for each SI assignment\n");
										 ////////////
									    		//printf("Calculating the total required capacity i.e summing up the SI assignments");
        							    			searchParam.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    			searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								   			searchParam.searchOneAttr.attrValue = &objectnameSG1;

								    			error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					       	                    			SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 			   			if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					  		 }
                           				           		if (SA_AIS_OK == error)
        		    					  		 {
		         								//printf("\n\nWe got the search handle \n");
                                			               			 activeSIs=0;
            										do {                
                   										error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
												if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
												printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 					rc=EXIT_FAILURE;
					  							flag=1;
						    					   }
                                        


                                       		    						if(error == SA_AIS_ERR_NOT_EXIST )
               					    						{
		  	  									 	if(flag1==1)
                  	   			         						{ 
                     												//arraySU[counter]==NULL;
                     												//printf("\nobject does not exist"); 
        	    	   			       						 	}
		     	   										else
		     	  										{ 
                             											rc=EXIT_FAILURE;
                             											flag=1;
 		          										}        
						  					 	}
                
						 						if (error == SA_AIS_OK)
                  				  						{
 
                              
							   						attributeNames[0] ="saAmfSIPrefActiveAssignments";
							  				        	attributeNames[1] =NULL;
							   						error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   						if (error != SA_AIS_OK) {
                	       										printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       										rc= EXIT_FAILURE;
													flag = 1;
                      				   	     						}
       			       			 	  						if (error == SA_AIS_OK)
   						  							{
														error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        									if (error !=SA_AIS_OK)
                                        									{
                                               										printf("\nerror18:%d",error);
															rc= EXIT_FAILURE;
                                                        								flag = 1; 
                                        		        					 	}
                        	 									 	if (error ==SA_AIS_OK)
                                 									 	{
			                     		
					     										SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    										activeSIs = *((SaUint32T *)attrValues);
                                                              	  
                		   	               	 								error = saImmOmAccessorFinalize(accessorHandle);
							       							}
       													        if(noassgSUs >= activeSIs)
                                         									{
                                                                                                                    
                                                                                                                }
													        else
                                                                                                                {
                                                                                                                       no_SUs=0;
                                                                                                                }
 														
                        		                 					        }//accesor initialize
                                               	

                                         						     }//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST||no_SUs==0);
                   								 //printf(" total number of SIs is %d\n",activeSIssum);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
       										 ////////////										
                                                                               if(no_SUs==1)
                                                                               { 
										 for(i=0;i<2;i++)
										{
											//printf("\nsleeping\n");
											usleep(60000);
											//printf("\nGot up!\n");
										}   
	       									 printf("%c[1;32mThere is enough capacity to decrease the number of SUs",27);
                                                                                 printf("%c[0m",27);
	 									 printf("\n%c[1;33mCreate CCB to decrease number of  SUs\n",27);
                                                                                 //printf("locking SG\n");
                                                                                 printf("%c[0m",27);
                                                                                 //swapsi(immHandle,argv,2,objectnameSG1); //LOCK SG
                                                                                  swapsi(immHandle,argv,2,objectnameSG1); //LOCK SG
    	 									 noassgSI = noassgSUs; 
										 strcpy(nameccbattribute,"saAmfSGNumPrefAssignedSUs=");
                         							 sprintf(noassg, "%d",noassgSI);
               									 strcat(nameccbattribute,noassg);
             									 // printf("\n%s",nameccbattribute);
                        							 s= objectnameSG1.value ;
                        							 //printf("\n%s\n",s);
                        							 objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 										 objectName1c = objectNames[0] = malloc(sizeof(SaNameT));
										 objectNames[objectNames_len++] = NULL;
 										 objectName1c->length = snprintf((char*)objectName1c->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       								 error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      								 if (error != SA_AIS_OK) {
		      									 printf("error - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		      									 rc = EXIT_FAILURE;
		     								 }
		     								 if (error == SA_AIS_OK)
             	    								 {   
		     									  //printf("\n%s in increase the num of  Si assgn",objectNames[0]->value);
	 	     									  rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
		
		       									  if(rc==0)
		       									 {
		          									 rc=1;
		       									 }
        
	            								 }
										error = saImmOmAdminOwnerFinalize(ownerHandle);
										if (SA_AIS_OK != error) {
										printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
										rc = EXIT_FAILURE;
										//	goto done_om_finalize;
									        }
                                                                           swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
                                                                            //printf("%c[0m",27);
                                                                      
                                                                               if(rc==1)
                                                                               {
                                                                                 /////
                                                                                   //Asynchronous call to Buffer Managers
                                                                                   buffer_checkker_decrease(immHandle,objectnameSG1,objectnameSI,0,argv,preinstantiable);
                                                                                 ////
                                                                               }
                                                                         
                                                                          
                                                                                                                                                  
                                                                       }//if(no_SUs==1)
                                                                       else
                                                                       {    
                                                                            printf("\nWe cannot decrease saAmfSGNumPrefAssignedSUs..\n");
                                                                            goto loop10;
                                                                       }            
                                                                                 
							     }//noassgSUs * maxactSIsperSU >= activeSIssum
				                             else
                                                             {
                                                                
                                                                loop10:
                                                                printf("\nNeed to check conditions to increase the max activeSIsperSU\n");
                                                                     ////////////////////////////////////////////////////////increase saAmfSGMaxActiveSIsperSU////////////////////////////////////////////////////////
                                                                            strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
									    objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
									    printf("\nThe SI .. type is %s \n",objectsity.value);
                							    printf("Lets check the components capabiltiy model for the service type...\n");
  	    	  							    attributeNames[0]="saAmfCtDefNumMaxActiveCSIs";
 		    						            attributeNames[1]=NULL;
  	            							    error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefActiveNumCSIs = *((SaUint32T *)attrValue);

										
										//printf("The saAmfCtDefNumMaxActiveCSIs : %d \n", DefActiveNumCSIs );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                printf("The component capabiltiy model allows : %d \n", DefActiveNumCSIs );
                                   						//printf("\nThe cSI is %d \n",DefActiveNumCSIs);
					               	                }
								 }
								attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  								attributeNames[1]=NULL;
  								error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{      
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                          			maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGMaxActiveSIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
								if(maxactSIsperSU<DefActiveNumCSIs)
								{
                                                                        
                                                                        printf("Checking if for any SU in the SG saAmfSGMaxActiveSIsperSU is equal to current number of SI Assignment\n");
                                                                        attributeNames[0]="saAmfSGType";
  								        attributeNames[1]=NULL;
                                                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 									if (error != SA_AIS_OK) {
                							printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                							rc= EXIT_FAILURE;
                      							}
       									if (error == SA_AIS_OK)
   									{
  										//     printf("\nWe got the accessor handle to access SG\n");
         
      											error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);
											if (error !=SA_AIS_OK)
											{
												 printf("\n%d",error);
         											 rc= EXIT_FAILURE;
											}


											if (error ==SA_AIS_OK)
											{
												//printf("\nsaImmOmAccessorGet_2 function returned the SG object and attribute\n");

												SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
												SaNameT *objtnamevalidsutype = (SaNameT *)attrValue;


 												strncpy(objtnamesgtype.value,objtnamevalidsutype->value, SA_MAX_NAME_LENGTH);
 												objtnamesgtype.length=strlen(objtnamesgtype.value);


											}
											error = saImmOmAccessorFinalize(accessorHandle);
                                                                          printf("\nThe saAmfSGType of %s SG  is %s\n",objectnameSG1.value,objtnamesgtype.value);
									}
									//>>>
                							attributeNames[0]="saAmfSgtValidSuTypes";
  								        attributeNames[1]=NULL;
 									error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 									if (error != SA_AIS_OK) {
                									printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                									rc= EXIT_FAILURE;
                      								}
       									if (error == SA_AIS_OK)
   									{
       										//printf("\nWe got the accessor handle to access SG\n");
  												// } 

										error= saImmOmAccessorGet_2(accessorHandle, &objtnamesgtype, attributeNames, &attributes);

										if(error == SA_AIS_OK)
										{
 											//printf("\nok ok");
 											SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
 											SaNameT *objtnamevalidsutype = (SaNameT *)attrValue;
 											strcpy(namevalidsutype,objtnamevalidsutype->value);
 											l=objtnamevalidsutype->length;
 											objectnamevalidsutype1.length=l;
//printf("\n\n%u\n\n",x);

											strncpy((char *)objectnamevalidsutype1.value, namevalidsutype, SA_MAX_NAME_LENGTH);

										}
									error = saImmOmAccessorFinalize(accessorHandle);
        								printf("\nThe saAmfSgtValidSuTypes of %s SG  is %s\n",objectnameSG1.value,objectnamevalidsutype1.value);
                                                                         }
                                                                          searchParam.searchOneAttr.attrName = "saAmfSUType";
 								    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam.searchOneAttr.attrValue = &objectnamevalidsutype1;

								    error = saImmOmSearchInitialize_2(immHandle,&objectnameSG1 , scope,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							//flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			               curr_assg_SUs=1,activeSIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									 if(flag1==1)
                  	   			         					{ 
                     											//arraySU[counter]==NULL;
                     											//printf("\nobject does not exist"); 
        	    	   			       						 }
		     	   									else
		     	  									{ 
                             										rc=EXIT_FAILURE;
                             										//flag=1;
 		          									}        
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					attributeNames[0] ="saAmfSUNumCurrActiveSIs";
							  				        attributeNames[1] =NULL;
							   					error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   					if (error != SA_AIS_OK) {
                	       										printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       										rc= EXIT_FAILURE;
													//flag = 1;
                      				   	     					}
       			       			 	  					if (error == SA_AIS_OK)
   						  						{
													error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        								if (error !=SA_AIS_OK)
                                        								{
                                               									printf("\nerror18:%d",error);
														rc= EXIT_FAILURE;
                                                        							flag = 1; 
                                        		        					 }
                        	 									 if (error ==SA_AIS_OK)
                                 									 {
			                     		                                                        //no_of_SIs++;
					     									SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    									activeSIs = *((SaUint32T *)attrValues);
                                                              	  
                		   	               	 							error = saImmOmAccessorFinalize(accessorHandle);
							       						 }
       													printf(" total number of SIs handled by SU %s is %d\n",objectName.value,activeSIs);
                                                                                                        if(activeSIs == maxactSIsperSU)
                                                                                                        {
                                                                                                             curr_assg_SUs ==1;
                                                                                                        }
                        		                 					}//accesor initialize
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST && curr_assg_SUs != 1);
                   								 
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
									








//curr_assg_SUs=1;
                                                                             printf("\ncurr_assg_SUs:%d\n",curr_assg_SUs); 
                                                                             if(curr_assg_SUs==0)
                                                                             {
                                                                                 goto loop11;
                                                                             }
                                                                             else
                                                                             {
                                                                               printf("Checking if maxactSIsperSU < total number of SIs:");
                                                                               printf("\n The total number of SIs is %d\n",no_of_SIs);
                                                                                  if(maxactSIsperSU<no_of_SIs )
                                                                                  {
                                                                                     printf("Increase the saAmfSGMaxActiveSIsperSU \n");
                                                                                     printf("Create a CCB to increase saAmfSGMaxActiveSIsperSU of %s SG\n",objectnameSG1.value);
 									 	     ////////////////////////create ccb to increase saAmfSGMaxActiveSIsperSU
  								         	     swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
 								        	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 								        	     noassgSUs = maxactSIsperSU+1; 
		    					                	     strcpy(nameccbattribute,"saAmfSGMaxActiveSIsperSU=");
                         							     sprintf(noassg, "%d",noassgSUs);
               									     strcat(nameccbattribute,noassg);
             									     // printf("\n%s",nameccbattribute);
                        							     s= objectnameSG1.value ;
                        							     //printf("\n%s\n",s);
                        							     objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 										     objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
										     objectNames[objectNames_len++] = NULL;
 										     objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       							//??
      		       
		      							 	      error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      						        	      if (error != SA_AIS_OK) {
		       										printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		      								 		rc = EXIT_FAILURE;
		     							 	      }
		     				         			      if (error == SA_AIS_OK)
             	    					                	      {   
			  
	 	      	  							 		rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                      								 		free(objectNamec);
                      
		       								 		if(rc==0)
		        							                {
		          	 									rc=1;
		      							        		}
        
	          						         	       }
	
										   error = saImmOmAdminOwnerFinalize(ownerHandle);
										   if (SA_AIS_OK != error) {
											printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
											rc = EXIT_FAILURE;
										//	goto done_om_finalize;
										}
								   		 //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
										swapsi(immHandle,argv,1,objectnameSG1);  //unlock SG
                                                                                ////////////CHECK IF FUTHER REDUCTION OF SUs is required
                                                                                attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  	        attributeNames[1]=NULL;
  		error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGMaxActiveSIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  								attributeNames[0]="saAmfSGNumPrefAssignedSUs";
 								attributeNames[1]=NULL;
  								error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGNumPrefAssignedSUs of %s SG is %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }
								//"Calculating the total required capacity i.e summing up the SI assignments");							
 
								    printf("Calculating the total required capacity i.e summing up the SI assignments");
        							    searchParam.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                activeSIs=0,activeSIssum=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									 if(flag1==1)
                  	   			         					{ 
                     											//arraySU[counter]==NULL;
                     											//printf("\nobject does not exist"); 
        	    	   			       						 }
		     	   									else
		     	  									{ 
                             										rc=EXIT_FAILURE;
                             										flag=1;
 		          									}        
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					attributeNames[0] ="saAmfSIPrefActiveAssignments";
							  				        attributeNames[1] =NULL;
							   					error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   					if (error != SA_AIS_OK) {
                	       										printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       										rc= EXIT_FAILURE;
													flag = 1;
                      				   	     					}
       			       			 	  					if (error == SA_AIS_OK)
   						  						{
													error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        								if (error !=SA_AIS_OK)
                                        								{
                                               									printf("\nerror18:%d",error);
														rc= EXIT_FAILURE;
                                                        							flag = 1; 
                                        		        					 }
                        	 									 if (error ==SA_AIS_OK)
                                 									 {
			                     		                                                        
					     									SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    									activeSIs = *((SaUint32T *)attrValues);
                                                              	  
                		   	               	 							error = saImmOmAccessorFinalize(accessorHandle);
							       						 }
       													activeSIssum = activeSIssum + activeSIs;
                        		                 					}//accesor initialize
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",activeSIssum);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
                                               
     									     																
									     if(noassgSUs * maxactSIsperSU >= activeSIssum) 
									     {
                                                                                goto loop9;
 							                     }
                                                                             else
                                                                             {
                                                                                     rc=1;
                                                                             }
                                                                              ////////////////////////////////////////CHECK IF FURTHER REDUCTION OF SUs IS REQUIRED
                                                                          }
                                                                          else
                                                                          {
                                                                                      goto loop11;
                                                                          }
                                                                         }//if(cur_ass_SUs==0)
                                                                     }//if(maxactSIsperSU<DefActiveNumCSIs)
                                                                     else
                                                                     {
                                                                              loop11:
                                                                              rc =2;
                                                                     }
                                                            
  									
                                                                //   }
                                                             ////////////////////////////////////////////////////////increase saAmfSGMaxActiveSIsperSU///////////////////////////////////////////
                                                                
                                                             }

							 }//noSIassg >= 3
  								return rc;
}
///N-way Active Single SI increase_end


void remove_all_chars(char* str, char c) {
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

//magicSicurractive(objectnameSI_start
int magicSicurractive(SaNameT objectnameSI,SaImmHandleT immHandle)
{
   SaNameT rootName = { 0, "" };
    
 SaNameT objectNameSU;
 SaNameT objectNameSg;
 strncpy((char *)rootName.value, "safSi=AmfDemo,safApp=AmfDemo2", SA_MAX_NAME_LENGTH);
 rootName.length = strlen((char *)rootName.value);
 strncpy((char *)objectNameSg.value, "safSg=AmfDemo,safApp=AmfDemo2", SA_MAX_NAME_LENGTH);
 objectNameSg.length = strlen((char *)objectNameSg.value);
 //SaImmHandleT immHandle;
 SaImmSearchParametersT_2 searchParam;
 SaImmScopeT scope = SA_IMM_SUBLEVEL;	/* default search scope */
 SaImmScopeT scope1 = SA_IMM_SUBTREE;
 SaImmSearchHandleT searchHandle,searchHandle1;
 SaImmAccessorHandleT accessorHandle;
 int rc = EXIT_SUCCESS;
 char string[100],string2[100],string3[100];
 char ch1;
 
 SaImmAttrNameT  attributeNames[] ={"safSISU",NULL};
 //const SaNameT rootName =  objectnameSI;
 SaAisErrorT error;
 SaNameT objectName,objectName1;
 SaImmAttrValuesT_2 **attributes,**attributes1;
 char * ch;
 int len1,len2,len3,i,cntr1,cntr2,flag=0,noSIassg;


 // printf("\nOOOOOOOOOOOOO\n");
  searchParam.searchOneAttr.attrName = NULL;
searchParam.searchOneAttr.attrValue = NULL;

printf("\n%s\n",rootName.value);
error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	
                printf("\nSearch Hnadle Obtained\n");
                rc=EXIT_FAILURE;
                
	}

do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                {
                        ch =strstr(objectName.value,"safSISU");
                      if(ch == NULL)
                      {
                      }
                      else
                      {   
			printf("%s\n", objectName.value);
                        strcpy(string,objectName.value);
                        
                      }
                }
	} while (error != SA_AIS_ERR_NOT_EXIST);
error = saImmOmSearchFinalize(searchHandle);
}
//magicSicurractive(objectnameSI_end

//N+M Single SI increase_start
int single_SIIncreaseNPM(SaImmHandleT immHandle,SaNameT objectnameSG1,char *argv[],SaNameT objectnameSI,int preinstantiable)
{
printf("%c[1;32m\n***********************************************************************************",27);
printf("\n**************************************RM Adjustor**********************************");
printf("%c[0m",27);
 //printf("\nI got a call\n");
 /////SU handling the SI
  SaNameT rootName = { 0, "" };
 SaNameT objectNameSU;
 strncpy((char *)rootName.value,objectnameSI.value , SA_MAX_NAME_LENGTH);
 rootName.length = strlen((char *)rootName.value);
 //SaImmHandleT immHandle;
 SaImmSearchParametersT_2 searchParam,searchParam1;
 SaImmScopeT scope = SA_IMM_SUBLEVEL;	/* default search scope */
 SaImmScopeT scope1 = SA_IMM_SUBTREE;	/* default search scope */
 SaImmSearchHandleT searchHandle,searchHandle1;
 SaImmAccessorHandleT accessorHandle,accessorHandle3;
 int rc = EXIT_SUCCESS;
 char string[100],string2[100],string3[100];
 SaImmAdminOwnerHandleT ownerHandle;
 char ch1;
 int objectNames_len = 1;
 SaNameT objectsity;
 unsigned l;
 
 SaImmAttrNameT  attributeNames[] ={"safSISU",NULL};
 //const SaNameT rootName =  objectnameSI;
 SaAisErrorT error;
 SaNameT objectName;
 SaImmAttrValuesT_2 **attributes;
 char * ch;
 SaNameT **objectNames = NULL;
  SaNameT *objectNamec;
  SaNameT *objectName1c;
  char *s;
 char nameccbattribute[256],noassg[256];
 SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
 int len1,len2,len3,i,cntr1,cntr2,flag=0,noSIassg,noSIassgactSU,noSIassgstdSU,noassgSUs,maxactSIsperSU,no_SIs,activeSIssum,activeSIs,flag1,noSIassgactSUF,noSIassgstdSUF,noassginsSUs,noactassgSUs,noassgstdSUs,maxstdSIsperSU,DefStandbyNumCSIs;


searchParam.searchOneAttr.attrName = NULL;
searchParam.searchOneAttr.attrValue = NULL;


//printf("\n%s\n",rootName.value);
error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	
               // printf("\nSearch hanadle Obtained\n");
                rc=EXIT_FAILURE;
                
	}

noSIassgactSUF =0 ,noSIassgstdSUF=0;
do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                {
                                ch =strstr(objectName.value,"safSISU");
                      if(ch == NULL)
                      {
                      }
                      else
                      {   
			//printf("%s\n", objectName.value);
                        strcpy(string,objectName.value);
                        
                      }
                                          

      
 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);

			//printf("\nstring:%s",objectNameSU.value);
			attributeNames[0] = "saAmfSISUHAState";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                	printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	rc= EXIT_FAILURE;
                     	}
       			if (error == SA_AIS_OK)
   			{
       				
       				error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       				if(error == SA_AIS_OK)
       				{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassg = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of SU %s is %d\n",objectNameSU.value,noSIassg);
					               	              
      				}
//

				error = saImmOmAccessorFinalize(accessorHandle);
                   
                        strcpy(string2,"safSISU=");
                        len1=strlen(string);
      			len2=strlen(string2);   
      

      			for(i=0;(i+len2+1)<=len1;i++)
            		string[i]=string[len2+i];
            		string[i]='\0';
      			//printf("\nDeleted : %s\n",string);

    			strcpy(string3,",");
    			strcat(string3,rootName.value);
    			//printf("\nconcated :%s\n",string3);

      				//printf("\n%s\n",string);

      			len1=strlen(string);
  
      			len2=strlen(string3); 
      			len3=len1-len2;
    
      

      			for(i=0;i<len3;i++)
            		string[i]=string[i];
            		string[i]='\0';
      			//printf("\nDeleted2 : %s\n",string);


    			remove_all_chars(string, '\\');
    			//printf("'%s'\n", string);


 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);
                    if (noSIassg == 1)
                    {
                        attributeNames[0] = "saAmfSUNumCurrActiveSIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgactSUF = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current active SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgactSUF);
					               	                       // printf("\nThe SU : %s is handling active assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgactSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 }
                 if (noSIassg == 2)
                    {
                        attributeNames[0] = "saAmfSUNumCurrStandbySIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgstdSUF = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current standby SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgstdSUF);
					               	            // printf("The SU : %s is handling standby assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgstdSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 }
                 if(noSIassg !=1 && noSIassg !=2)
                 {
                     //printf("\n error : no association class\n");
                 }
                 

                      
             }
          }
 ////SU handling the SI

} while (error != SA_AIS_ERR_NOT_EXIST);
error = saImmOmSearchFinalize(searchHandle);

 /////////////Decrease MaxActive SI per SU
loop12:
//printf("\n!!!!\n");
printf("%c[1;32m\nChecking to spread the SI assignment i.e decrease the saAmfSGMaxActiveSIsperSU\n",27);
printf("%c[0m",27);
 attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of saAmfSGMaxActiveSIsperSU of %s SG are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  attributeNames[0]="saAmfSGNumPrefActiveSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefActiveSUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }

///search number of SIs_start
  printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			printf("error - saImmOmSearchInitialize_2 FAILED:%d",error);
                	   		 		 		        //printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
///search number of SIs_end
//////////// Decrease MaxActive SI per SU 
printf("Checking if we can decrease the saAmfSGMaxActiveSIsperSU\n");
   if((maxactSIsperSU-1)*noassgSUs >= no_SIs)
   {
      printf("%c[1;32m\nThere is enough capacity hence we can decrease saAmfSGMaxActiveSIsperSU\n",27);
      printf("%c[0m",27);
      printf("%c[1;33mCreating a CCB ....\n ",27);
      //////ccb to increase maxactiveSiperSU

            swapsi(immHandle,argv,2,objectnameSG1);  //lock SG

             //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		noassgSUs = maxactSIsperSU-1; 
		strcpy(nameccbattribute,"saAmfSGMaxActiveSIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        //noSIassgactSU=magicSicurractive(objectnameSI,immHandle);

for(i=0;i<2;i++)
{
//printf("\nsleeping\n");
usleep(60000);
//printf("\nGot up!\n");
}      

//swapsi(immHandle,argv,1,objectnameSG1);
//swapsi(immHandle,argv,1,objectnameSG1);
/////////////////////////////////////////////////
searchParam.searchOneAttr.attrName = NULL;
searchParam.searchOneAttr.attrValue = NULL;


//printf("\n%s\n",rootName.value);
error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle1);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	        
               // printf("\nSearch handle Obtained\n");
                rc=EXIT_FAILURE;
                noSIassgactSU=0,noSIassgstdSU=0;
                i=0;
		do {
                        
                        //strcpy(string,"");
                        
                        //printf("%d\n",i);
			error = saImmOmSearchNext_2(searchHandle1, &objectName, &attributes);
			if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			//exit(EXIT_FAILURE);
			}
                        if (error != SA_AIS_OK) {
			//printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			//exit(EXIT_FAILURE);
			}
			if (error == SA_AIS_OK)
                	{      
                               //printf("%d\n",i);
                  	       //printf("\nobject found: %s\n",objectName.value);
                               ch = strstr(objectName.value,"safSISU");
                               if(ch == NULL)
                               {
                               }
                               else
                               {        //printf("%d\n",i);
                                        strcpy(string,objectName.value);
                                        strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 					objectNameSU.length = strlen((char *)objectNameSU.value);
                                     	//printf("%s\n",objectNameSU.value);
                                     	attributeNames[0] = "saAmfSISUHAState";
				     	attributeNames[1] = NULL;
                                        //swapsi(immHandle,argv,1,objectnameSG1);
					error = saImmOmAccessorInitialize( immHandle, &accessorHandle3);
 					if (error != SA_AIS_OK) {
                			printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                			rc= EXIT_FAILURE;
                     			}
       					if (error == SA_AIS_OK)
   					{
       				
       						error= saImmOmAccessorGet_2(accessorHandle3, &objectNameSU, attributeNames, &attributes);
                                                if(error != SA_AIS_OK)
                                                {
                                                   printf("error - saImmOmAccessorGet %d\n",error);
                                                 }
       						if(error == SA_AIS_OK)
       						{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassg = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle3);
                                                                                
                                   						//printf("\nThe number of SU %s is %d\n",objectName.value,noSIassg);
					               	                        strcpy(string,objectName.value);
                                                                                //printf("copied %s\n",string);
      						}
//
                                        }
				error = saImmOmAccessorFinalize(accessorHandle);
                                //printf("copied %s\n",string);
                                strcpy(string2,"safSISU=");
                        	len1=strlen(string);
      				len2=strlen(string2);   
      

      				for(i=0;(i+len2+1)<=len1;i++)
            			string[i]=string[len2+i];
            			string[i]='\0';
      				//printf("\nDeleted : %s\n",string);
                                strcpy(string3,",");
    				strcat(string3,rootName.value);
    				//printf("\nconcated :%s\n",string3);

      				//printf("\n%s\n",string);

      				len1=strlen(string);
  
      				len2=strlen(string3); 
      				len3=len1-len2;
    
      

      				for(i=0;i<len3;i++)
            			string[i]=string[i];
            			string[i]='\0';
      				//printf("\nDeleted2 : %s\n",string);


    				remove_all_chars(string, '\\');
    				//printf("'%s'\n", string);
      
                                //??
                                      	strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 					objectNameSU.length = strlen((char *)objectNameSU.value);
                    			if (noSIassg == 1)
                    			{
                        			attributeNames[0] = "saAmfSUNumCurrActiveSIs";
						attributeNames[1] = NULL;
						error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 						if (error != SA_AIS_OK) {
                					printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                					rc= EXIT_FAILURE;
                      				}
       		    				if (error == SA_AIS_OK)
   		   				 {
       						if (error !=SA_AIS_OK)
       						{
	 						printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       						} 
       						error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       						if(error == SA_AIS_OK)
       						{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgactSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current active SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgactSU);
					               	           //   printf("\nThe SU : %s is handling active assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgactSUF);
      						}
//

						error = saImmOmAccessorFinalize(accessorHandle);
      
   		  			 }
                 		}
                 		if (noSIassg == 2)
                    		{
                        		attributeNames[0] = "saAmfSUNumCurrStandbySIs";
					attributeNames[1] = NULL;
					error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 					if (error != SA_AIS_OK) {
                				printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                				rc= EXIT_FAILURE;
                     		 }
       		   		 if (error == SA_AIS_OK)
   		    		{
       					if (error !=SA_AIS_OK)
       					{
	 					//printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       					} 
       					error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       					if(error == SA_AIS_OK)
       					{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgstdSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current standby SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgstdSU);
					               	           //   printf("\nThe SU : %s is handling standby assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgstdSUF);
      					}
//

					error = saImmOmAccessorFinalize(accessorHandle);
      
   		   		}
                 	}
                 		if(noSIassg !=1 && noSIassg !=2)
                 		{
                    		 //printf("\n error : no association class\n");
                 		}
                 

                      
             		 }
          		}
                                //??
                      i++;    
    		   } while (error != SA_AIS_ERR_NOT_EXIST);              
     
	}//serchInitialize


error = saImmOmSearchFinalize(searchHandle1);
/////////////////////////////////////////////////
      // printf("\n assg su *******%d",noSIassgactSU);
      
 	printf("%c[1;32mNeed to check if the load is reduced\n",27);
        printf("%c[0m",27);
        printf("The SI was assigned to the SU which was handling SI active assinments : %d\n",noSIassgactSUF);
        printf("After applying the CCB the SI is assigned  to SU which is handling SI active assinment : %d\n",noSIassgactSU);
        printf("Comparing to check if the load is reduced...");
	if(noSIassgactSU < noSIassgactSUF)
	{	
   		printf("%c[1;32mThe load is reduced\n",27);
   		rc = 1;
                printf("\nSuccessfully increased Active capacity now trying to increase Standby capacity\n");
                printf("%c[0m",27);
                /////////////////////////////////////////////////////////////////Now need to increase standby capacity////////////////////////////////////////////////
                goto loop15;
	}
        else
        {
              goto loop12;
	}
   }   
   else
   {
     printf("%c[1;31m\nThere is not enough capacity hence we cannot decrease saAmfSGMaxActiveSIsperSU\n",27);
     printf("%c[0m",27);
     attributeNames[0]="saAmfSGNumPrefActiveSUs";
     attributeNames[1]=NULL;
     error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe saAmfSGNumPrefActiveSUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
                                                                                
					               	                }
								  }
     if(noassgSUs < no_SIs ) 
     {
        loop13:
        printf("%c[1;32m\nNeed to check conditions to increase number of SUs\n",27);
   
        printf("%c[0m",27);
        attributeNames[0]="saAmfSGNumPrefActiveSUs";
     	attributeNames[1]=NULL;
     	error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noactassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefActiveSUs of %s SG are %d\n",objectnameSG1.value,noactassgSUs);
					               	                }
								  }
     	attributeNames[0]="saAmfSGNumPrefStandbySUs";
     	attributeNames[1]=NULL;
     	error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgstdSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefStandbySUs of %s SG are %d\n",objectnameSG1.value,noassgstdSUs);
					               	                }
						  }
	 if(preinstantiable == 1)
  {	
     	attributeNames[0]="saAmfSGNumPrefInserviceSUs";
     	attributeNames[1]=NULL;
     	error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassginsSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefInserviceSUs of %s SG are %d\n",objectnameSG1.value,noassginsSUs);
					               	                }
								  }
   }
   else
   {
       noassginsSUs =  num_SUofSG(immHandle,objectnameSG1);  
   }
     	if(noassginsSUs >noactassgSUs + noassgstdSUs)
     	{
         	printf("%c[1;33m\nCreate a CCB to increase number of active SUs\n",27);
                printf("Creating a CCB ....\n ");
               
 //////ccb to increase number  of preffered assigned SU
            swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		noassgSUs = noactassgSUs+1; 
		strcpy(nameccbattribute,"saAmfSGNumPrefActiveSUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
         printf("%c[0m",27);
//////////////////////////////////////////call to buffer checkker///////////////////////////////////////////
	 if(preinstantiable == 1)
  {	
        buffer_checkker(immHandle,objectnameSG1,objectnameSI,1,argv);
  }
  else
  {
        buffer2_checkker(immHandle,objectnameSG1,objectnameSI,argv);
        
  }
//////////////////////////////////////////call to buffer checkker///////////////////////////////////////////
        goto loop12;
 //////ccb to increase number  of preffered assigned SU               
     	}
        else
        {
              printf("%c[1;32m\nNeed to adjust standby SUs\n",27);
        printf("%c[0m",27);
              loop14:
              /////////////////////////////checking if we can decrease number of standbySUs
              attributeNames[0]="saAmfSGMaxStandbySIsperSU";
  	      attributeNames[1]=NULL;
  	      error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe number of saAmfSGMaxStandbySIsperSU of %s SG are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
								 }
  		attributeNames[0]="saAmfSGNumPrefStandbySUs";
  		attributeNames[1]=NULL;
  		error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgstdSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGNumPrefStandbySUs of %s SG are %d\n",objectnameSG1.value,noassgstdSUs);
					               	                }
								  }

		///search number of SIs_start
  		printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
              ////////////////////////////checking if we can decrease number of standbySUs
              if((noassgstdSUs-1) * maxstdSIsperSU >= no_SIs )
              {
                     printf("%c[1;33m\nCreate a CCB to decrease standby SUs\n",27);
                     printf("Creating a CCB ....\n ");
 //////ccb to increase number  of preffered assigned SU
            swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
     
 		noassgSUs = noassgstdSUs-1; 
		strcpy(nameccbattribute,"saAmfSGNumPrefStandbySUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
         printf("%c[0m",27);
        goto loop13;
                     
                     
              }
              else
              {
                     printf("%c[1;32m\nCheck to increase saAmfSGMaxStandbySIsperSU\n",27);
                      printf("%c[0m",27);



                     



//////////////////////////////////////////////find maxstandbySIperSU///////////////////////////
strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
printf("The SI .. type is %s \n",objectsity.value);
  	    attributeNames[0]="saAmfCtDefNumMaxStandbyCSIs";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefStandbyNumCSIs = *((SaUint32T *)attrValue);

										
										printf("The saAmfCtDefNumMaxStandbyCSIs : %d \n", DefStandbyNumCSIs );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe cSI is %d \n",DefStandbyNumCSIs);
					               	                }
								 }

        ////get the maxactiveSIperSU attribute
         attributeNames[0]="saAmfSGMaxStandbySIsperSU";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
					
   								 }


/////////////////////////////////////////////find maxstandbySIperSU////////////////////////////
if(DefStandbyNumCSIs > maxstdSIsperSU)
{
  if(maxstdSIsperSU+1 <= no_SIs)
  {
     printf("%c[1;33mCreate a CCB to increase saAmfSGMaxStandbySIsperSU\n",27);
     printf("Creating...\n");
      

     swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = maxstdSIsperSU+1; 
		    // strcpy(nameccbattribute,"saAmfSGMaxStandbySIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of max standby SIs per SU",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        //swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        goto loop14;
    }
    else
    {
     printf("\nElasticity at Cluster level\n");
     rc = 2;
}
    
}
else
{
     printf("\nElasticity at Cluster level\n");
     rc = 2;
}
                     
              }
              
        }
     }
     else
     {
         rc = 2;
     }

   }


loop15:
for(i=0;i<2;i++)
{
//printf("\nsleeping\n");
usleep(60000);
//printf("\nGot up!\n");
}
swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
searchParam.searchOneAttr.attrName = NULL;
searchParam.searchOneAttr.attrValue = NULL;


//printf("\n%s\n",rootName.value);
error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	
                //printf("\nSearch hanadle Obtained\n");
                rc=EXIT_FAILURE;
                
	}

noSIassgactSU =0 ,noSIassgstdSU=0;
do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			//exit(EXIT_FAILURE);
		}
                if (error != SA_AIS_OK) {
			//printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			//exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                {
                                ch =strstr(objectName.value,"safSISU");
                      if(ch == NULL)
                      {
                      }
                      else
                      {   
			//printf("%s\n", objectName.value);
                        strcpy(string,objectName.value);
                        
                      }
                                          

      
 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);

			//printf("\nstring:%s",objectNameSU.value);
			attributeNames[0] = "saAmfSISUHAState";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                	printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	rc= EXIT_FAILURE;
                     	}
       			if (error == SA_AIS_OK)
   			{
       				
       				error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       				if(error == SA_AIS_OK)
       				{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassg = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of SU %s is %d\n",objectNameSU.value,noSIassg);
					               	              
      				}
//

				error = saImmOmAccessorFinalize(accessorHandle);
                   
                        strcpy(string2,"safSISU=");
                        len1=strlen(string);
      			len2=strlen(string2);   
      

      			for(i=0;(i+len2+1)<=len1;i++)
            		string[i]=string[len2+i];
            		string[i]='\0';
      			//printf("\nDeleted : %s\n",string);

    			strcpy(string3,",");
    			strcat(string3,rootName.value);
    			//printf("\nconcated :%s\n",string3);

      				//printf("\n%s\n",string);

      			len1=strlen(string);
  
      			len2=strlen(string3); 
      			len3=len1-len2;
    
      

      			for(i=0;i<len3;i++)
            		string[i]=string[i];
            		string[i]='\0';
      			//printf("\nDeleted2 : %s\n",string);


    			remove_all_chars(string, '\\');
    			//printf("'%s'\n", string);


 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);
                 if (noSIassg == 1)
                    {
                        attributeNames[0] = "saAmfSUNumCurrActiveSIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgactSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current active SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgactSU);
					               	           //   printf("The SU : %s is handling active assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgstdSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 }
                    
                 if (noSIassg == 2)
                    {
                        attributeNames[0] = "saAmfSUNumCurrStandbySIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgstdSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current standby SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgstdSU);
					               	          //    printf("The SU : %s is handling standby assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgstdSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 }
                 if(noSIassg !=1 && noSIassg !=2)
                 {
                     //printf("\n error : no association class\n");
                 }
                 

                      
             }
          }
 ////SU handling the SI

} while (error != SA_AIS_ERR_NOT_EXIST);
error = saImmOmSearchFinalize(searchHandle);
printf("%c[1;32m\nThe current number of active SI assignments handled by SU handling SI assignments of the loaded SI before CCB implementation %d\n",27,noSIassgactSUF);
printf("The current number of active SI assignments handled by SU handling SI assignments of the loaded SI after CCB implementation %d\n",noSIassgactSU);
printf("The current number of standby SI assignments handled by SU handling SI assignments of the loaded SI before CCB implementation %d\n",noSIassgstdSUF);
printf("The current number of standby SI assignments handled by SU handling SI assignments of the loaded SI after CCB implementation %d\n",noSIassgstdSU);
printf("Comparing to check if the load is reduced..");
printf("%c[0m",27);
if(noSIassgstdSUF > noSIassgstdSU)
{
   printf("%c[1;32mThe load is reduced\n",27);
   printf("%c[0m",27);
   rc= 1;
}
else
{
   printf("%c[1;31mNeed to increase standby capacity\n",27);
   printf("%c[0m",27);
   attributeNames[0]="saAmfSGMaxStandbySIsperSU";
   attributeNames[1]=NULL;
  	      error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe number of saAmfSGMaxStandbySIsperSU of %s SG are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
								 }
  		attributeNames[0]="saAmfSGNumPrefStandbySUs";
  		attributeNames[1]=NULL;
  		error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgstdSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefStandbySUs of %s SG are %d\n",objectnameSG1.value,noassgstdSUs);
					               	                }
								  }

		///search number of SIs_start
  		printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
              ////////////////////////////checking if we can decrease number of standbySUs
              if((maxstdSIsperSU-1) * noassgstdSUs > no_SIs )
              {
                  printf("%c[1;33m\nCreating a CCB to decrease saAmfSGMaxStandbySIsperSU\n",27);
                  printf("Creating CCB..");
                  swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = maxstdSIsperSU-1; 
		     strcpy(nameccbattribute,"saAmfSGMaxStandbySIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                       // printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in decrease the num of max standby SIs per SU\n",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        goto loop15;
                   
              }
else
{
  printf("%c[1;31m\nNeed to increase number of Standby SUs\n",27);
  printf("%c[0m",27);
    attributeNames[0]="saAmfSGNumPrefStandbySUs";
     attributeNames[1]=NULL;
     error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("The saAmfSGNumPrefStandbySUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
                                                                                
					               	                }
								  }
     if(noassgSUs < no_SIs ) 
     {
         //printf("Need to check conditions to increase number of SUs\n");
        attributeNames[0]="saAmfSGNumPrefActiveSUs";
     	attributeNames[1]=NULL;
     	error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noactassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefActiveSUs of %s SG are %d\n",objectnameSG1.value,noactassgSUs);
					               	                }
								  }
     	attributeNames[0]="saAmfSGNumPrefStandbySUs";
     	attributeNames[1]=NULL;
     	error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgstdSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefStandbySUs of %s SG are %d\n",objectnameSG1.value,noassgstdSUs);
					               	                }
								  }
	 if(preinstantiable == 1)
  {
     	attributeNames[0]="saAmfSGNumPrefInserviceSUs";
     	attributeNames[1]=NULL;
     	error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassginsSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefInserviceSUs of %s SG are %d\n",objectnameSG1.value,noassginsSUs);
					               	                }
  								  }
        }
        else
        {
            noassginsSUs =  num_SUofSG(immHandle,objectnameSG1);  
        }
       
     	if(noassginsSUs >noactassgSUs + noassgstdSUs)
     	{
              printf("%c[1;33m\nCreating a CCB to increase the number of standby SUs\n",27);
              printf("Creating..\n");
              swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = noassgstdSUs + 1; 
		    // strcpy(nameccbattribute,"saAmfSGNumPrefStandbySUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                      //  s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in decrease the num of max standby SIs per SU",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
////////////////////////////////////////////////////////buffer_cal/////////////////////////////////////////
 if(preinstantiable == 1)
  {
     buffer_checkker(immHandle,objectnameSG1,objectnameSI,1,argv);
  }
  else
  {
    buffer2_checkker(immHandle,objectnameSG1,objectnameSI,argv);
  }
///////////////////////////////////////////////////////buffer_cal//////////////////////////////////////////
        goto loop15;
         }
         else
         {
                rc = 2;
	}
     }
else
{
  rc= 2;
}
}

}


//////////////////////////////////////////////////////////////////////Increase Standby Capacity//////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////Increase STandby Capacity//////////////////////////////////////////////////////////////////////////////////////////////

 return rc;
}
//N+M Single SI increase_end

//N+M Multiple SI increase_start
int multiple_SIIncreaseNPM(SaImmHandleT immHandle,SaNameT objectnameSG1,char *argv[],SaNameT objectnameSI,int preinstantiable)
{
//printf("\n I got a call");
printf("%c[1;32m\n***********************************************************************************",27);
printf("\n**************************************RM Adjustor**********************************");
printf("%c[0m",27);
//printf("\nI got a call\n");
SaNameT rootName = { 0, "" };
 SaNameT objectNameSU;
 strncpy((char *)rootName.value,objectnameSG1.value , SA_MAX_NAME_LENGTH);
 rootName.length = strlen((char *)rootName.value);
 //SaImmHandleT immHandle;
 SaImmSearchParametersT_2 searchParam,searchParam1;
 SaImmScopeT scope = SA_IMM_SUBLEVEL;	/* default search scope */
 SaImmScopeT scope1 = SA_IMM_SUBTREE;	/* default search scope */
 SaImmSearchHandleT searchHandle,searchHandle1;
 SaImmAccessorHandleT accessorHandle,accessorHandle3;
 int rc = EXIT_SUCCESS;
 char string[100],string2[100],string3[100];
 SaImmAdminOwnerHandleT ownerHandle;
 char ch1;
 int objectNames_len = 1,no_standbySUs,no_activeSUs,noinsSUs,maxactSIsperSUs;
 SaNameT objectsity;
 unsigned l;
 
 SaImmAttrNameT  attributeNames[] ={"safSISU",NULL};
 //const SaNameT rootName =  objectnameSI;
 SaAisErrorT error;
 SaNameT objectName;
 SaImmAttrValuesT_2 **attributes;
 char * ch;
 SaNameT **objectNames = NULL;
  SaNameT *objectNamec;
  SaNameT *objectName1c;
  char *s;
 char nameccbattribute[256],noassg[256];
 SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
int len1,len2,len3,i,cntr1,cntr2,flag=0,noSIassg,noassgactSUs,noSIassgactSU,noSIassgstdSU,noassgSUs,maxactSIsperSU,no_SIs,activeSIssum,activeSIs,noSIassgactSUF,noSIassgstdSUF,noassginsSUs,noactassgSUs,noassgstdSUs,maxstdSIsperSU,DefStandbyNumCSIs;
int noSIassgcurractSU,flag1=0,DefActiveNumCSIs,noSIassgcurrstdSU;
loop25:
attributeNames[0]="saAmfSGMaxActiveSIsperSU";
   attributeNames[1]=NULL;
  	      error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe number of saAmfSGMaxActiveSIsperSU of %s SG are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  		attributeNames[0]="saAmfSGNumPrefActiveSUs";
  		attributeNames[1]=NULL;
  		error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgactSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefActiveSUs of %s SG are %d\n",objectnameSG1.value,noassgactSUs);
					               	                }
								  }

		///search number of SIs_start
  		printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
printf("%c[1;32mCheck active capaity\n",27);
printf("%c[0m",27);
if((noassgactSUs) * maxactSIsperSU >= no_SIs )
{
  printf("\n Active capacity handled check standby capacity");
}
else
{
   printf("\n Need to increase active capacity");
   

  
  printf("%c[1;32m\nNeed to increase saAmfSGMaxActiveSIsperSU\n",27);
 printf("%c[0m",27);
  	strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
   	objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
   	printf("The SI .. type is %s \n",objectsity.value);
  	    attributeNames[0]="saAmfCtDefNumMaxActiveCSIs";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefActiveNumCSIs = *((SaUint32T *)attrValue);

										
										printf("The saAmfCtDefNumMaxActiveCSIs : %d \n", DefActiveNumCSIs );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe cSI is %d \n",DefStandbyNumCSIs);
					               	                }
								 }

        ////get the maxactiveSIperSU attribute
         attributeNames[0]="saAmfSGMaxActiveSIsperSU";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxActiveSIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
					
   								 }
	/////////////////////////////////////////////find maxstandbySIperSU////////////////////////////
        
	if(DefActiveNumCSIs > maxactSIsperSU)   
	{ 
             printf("\nCreate a CCB to increase saAmfSGMaxActiveSIsperSU");  
              printf("%c[1;33m\nCreate a CCB to increase saAmfSGMaxStandbySIsperSU\n",27);    
          printf("Creating...\n");
              swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = maxactSIsperSU + 1; 
		     strcpy(nameccbattribute,"saAmfSGMaxActiveSIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in decrease the num of max standby SIs per SU",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27); 
           goto loop25;
         } 
         else
         {      
                loop27:
                 printf("We check to increase active SUs\n");
    // printf("\nWe can increase the maxstandbySIsperSU\n");
     attributeNames[0]="saAmfSGNumPrefStandbySUs";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										no_standbySUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,no_standbySUs);
					               	                }
					
   								 }
 attributeNames[0]="saAmfSGNumPrefActiveSUs";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										no_activeSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,no_activeSUs);
					               	                }
					
   								 }
if(preinstantiable == 1)
  {
     
	 attributeNames[0]="saAmfSGNumPrefInserviceSUs";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noinsSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The inservice SUs of %s SG  are %d\n",objectnameSG1.value,noinsSUs);
					               	                }
					
   								 }
  }
  else
  {
      noinsSUs = num_SUofSG(immHandle,objectnameSG1);          
  } 
if(noinsSUs > (no_activeSUs + no_standbySUs))
{
   //printf("\n Create a CCB to increase saAmfSGNumPrefStandbySUs");
    printf("%c[1;33m\nCreate a CCB to increase saAmfSGNumPrefActiveSUs\n",27);    
          printf("Creating...\n");
              swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = no_activeSUs + 1; 
		     strcpy(nameccbattribute,"saAmfSGNumPrefActiveSUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in decrease the num of max standby SIs per SU",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
/////////////////////////////////////////////////////////////////////////buffer_call/////////////////////////////////////////////////////////////////
        if(preinstantiable == 1)
  {
     buffer_checkker(immHandle,objectnameSG1,objectnameSI,1,argv);
  }
  else
  {
    buffer2_checkker(immHandle,objectnameSG1,objectnameSI,argv);
  }
/////////////////////////////////////////////////////////////////////////buffer_call/////////////////////////////////////////////////////////////////
                
                goto loop25;
      }
              else
              {
                     loop29:
                     attributeNames[0]="saAmfSGNumPrefStandbySUs";
 	 	     attributeNames[1]=NULL;
 	 	     error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										no_standbySUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,no_standbySUs);
					               	                }
					
   								 }
                        attributeNames[0]="saAmfSGMaxStandbySIsperSUs";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
					
   								 }
                 printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
                  if(no_SIs <= (maxactSIsperSU * (no_standbySUs -1)))
                   {
                          printf("%c[1;33m\nCreate a CCB to decrease saAmfSGNumPrefStandbySUs\n",27);    
          printf("Creating...\n");
              swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = no_standbySUs -1; 
		     strcpy(nameccbattribute,"saAmfSGNumPrefStandbySUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in decrease the num of max standby SIs per SU",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
                         
                         goto loop27; 
                        //Asynchronous call to buffer
                   }
                   else
                   {
                       // loop29:
                       //check to increase maxstandbySIsperSU
                          //IF(CONDITION)
                           { 

                           

                                      printf("\n Need to increase standby capacity");
                                      printf("%c[1;32m\nNeed to increase saAmfSGMaxstandbySIsperSU\n",27);
                                      printf("%c[0m",27);
  				      strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
   			      	      objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
   				      printf("The SI .. type is %s \n",objectsity.value);
  	    			      attributeNames[0]="saAmfCtDefNumMaxStandbyCSIs";
 		    		      attributeNames[1]=NULL;
  	            		      error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefStandbyNumCSIs = *((SaUint32T *)attrValue);

										
										printf("The saAmfCtDefNumMaxStandbyCSIs : %d \n", DefStandbyNumCSIs );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe cSI is %d \n",DefStandbyNumCSIs);
					               	                }
								 }

        ////get the maxactiveSIperSU attribute
         attributeNames[0]="saAmfSGMaxStandbySIsperSU";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
					
   								 }
	/////////////////////////////////////////////find maxstandbySIperSU////////////////////////////
        
	if(DefStandbyNumCSIs > maxstdSIsperSU)   
	{    
               printf("We can increase saAmfSGMaxStandbySIsperSU\n");
             /////////////////////////////////////////////////////////
                searchParam.searchOneAttr.attrName = NULL;
	        searchParam.searchOneAttr.attrValue = NULL;


	//printf("\n%s\n",rootName.value);
	error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle1);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	         printf("Searching SU in the SG having equal number of SIs to the atrribute saAmfSGMaxStandbySIsperSU\n");
                //printf("\nSearch hanadle Obtained\n");
                //rc=EXIT_FAILURE;
                
	}

	noSIassgcurractSU =0;flag1 =0;
	do {
		error = saImmOmSearchNext_2(searchHandle1, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			//exit(EXIT_FAILURE);
		}
                if (error != SA_AIS_OK) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			//exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                { 
                          //printf("\n%s\n",objectName.value);

                        ch =strstr(objectName.value,"safSu");
                      if(ch == NULL)
                      {   //printf("\n%s\n",objectName.value);
                      }
                      else
                      {   
			//printf("%s!!!!\n", objectName.value);
                        strcpy(string,objectName.value);
                        
                      
                                          

      
 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);

			//printf("\nstring:%s",objectNameSU.value);
		    
			attributeNames[0] = "saAmfSUNumCurrStandbySIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                	printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	rc= EXIT_FAILURE;
                     	}
       			if (error == SA_AIS_OK)
   			{
       				
       				error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
       				if(error == SA_AIS_OK)
       				{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgcurrstdSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of SU %s is %d\n",objectNameSU.value,noSIassgcurrstdSU);
                                                                                if(noSIassgcurrstdSU == maxstdSIsperSU)
                                                                                {
                                                                                       flag1 = 1;
                                                                                 }
					               	              
      				}
                       }
                    }//if safSu

                 }//serchNext

  	} while (error != SA_AIS_ERR_NOT_EXIST && flag1 != 1);              
     



error = saImmOmSearchFinalize(searchHandle1);
        ////////////////////////////
 
  if(flag1 == 1)
  {
      printf("The SG has an SU satisfying above condition\n");
    // printf("\nWe can increase the maxstandbySIsperSU\n");
     attributeNames[0]="saAmfSGMaxStandbySIsperSU";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
					
   								 }
          ///search number of SIs_start
  		printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
       if((maxstdSIsperSU + 1) <= no_SIs)
       {
          printf("%c[1;33m\nCreate a CCB to increase saAmfSGMaxStandbySIsperSU\n",27);    
          printf("Creating...\n");
              swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = maxstdSIsperSU + 1; 
		     strcpy(nameccbattribute,"saAmfSGMaxStandbySIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in decrease the num of max standby SIs per SU",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        goto loop29;
  
       }

       else
       {
              //cluster level
              rc =3; 
       }
     
  }
  else
  {
     //cluster level
              rc =3;
  }




                                
                           }
                           else
                          {
                                   //elasticity at cluster level
                                  rc = 3;
                          }
                           
                   }
              }
         }
    }
}
 loop26:
attributeNames[0]="saAmfSGMaxStandbySIsperSU";
   attributeNames[1]=NULL;
  	      error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe number of saAmfSGMaxActiveSIsperSU of %s SG are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
								 }
  		attributeNames[0]="saAmfSGNumPrefStandbySUs";
  		attributeNames[1]=NULL;
  		error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgstdSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefStandbySUs of %s SG are %d\n",objectnameSG1.value,noassgstdSUs);
					               	                }
								  }

		///search number of SIs_start
  		printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
printf("%c[1;32mCheck standby capacity\n",27);
printf("%c[0m",27);
if((noassgstdSUs) * maxstdSIsperSU >= no_SIs )
{
  printf("\n Standby capacity handled");
  rc =1;
}
else
{
  printf("\n Need to increase standby capacity");
  printf("%c[1;32m\nNeed to increase saAmfSGMaxstandbySIsperSU\n",27);
 printf("%c[0m",27);
  	strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
   	objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
   	printf("The SI .. type is %s \n",objectsity.value);
  	    attributeNames[0]="saAmfCtDefNumMaxStandbyCSIs";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefStandbyNumCSIs = *((SaUint32T *)attrValue);

										
										printf("The saAmfCtDefNumMaxStandbyCSIs : %d \n", DefStandbyNumCSIs );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe cSI is %d \n",DefStandbyNumCSIs);
					               	                }
								 }

        ////get the maxactiveSIperSU attribute
         attributeNames[0]="saAmfSGMaxStandbySIsperSU";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
					
   								 }
	/////////////////////////////////////////////find maxstandbySIperSU////////////////////////////
        
	if(DefStandbyNumCSIs > maxstdSIsperSU)   
	{    
               printf("We can increase saAmfSGMaxStandbySIsperSU\n");
             /////////////////////////////////////////////////////////
                searchParam.searchOneAttr.attrName = NULL;
	        searchParam.searchOneAttr.attrValue = NULL;


	//printf("\n%s\n",rootName.value);
	error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle1);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	         printf("Searching SU in the SG having equal number of SIs to the atrribute saAmfSGMaxStandbySIsperSU\n");
                //printf("\nSearch hanadle Obtained\n");
                //rc=EXIT_FAILURE;
                
	}

	noSIassgcurractSU =0;flag1 =0;
	do {
		error = saImmOmSearchNext_2(searchHandle1, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			//exit(EXIT_FAILURE);
		}
                if (error != SA_AIS_OK) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			//exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                { 
                          //printf("\n%s\n",objectName.value);

                        ch =strstr(objectName.value,"safSu");
                      if(ch == NULL)
                      {   //printf("\n%s\n",objectName.value);
                      }
                      else
                      {   
			//printf("%s!!!!\n", objectName.value);
                        strcpy(string,objectName.value);
                       
 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);

			//printf("\nstring:%s",objectNameSU.value);
		    
			attributeNames[0] = "saAmfSUNumCurrStandbySIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                	printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	rc= EXIT_FAILURE;
                     	}
       			if (error == SA_AIS_OK)
   			{
       				
       				error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
       				if(error == SA_AIS_OK)
       				{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgcurrstdSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of SU %s is %d\n",objectNameSU.value,noSIassgcurrstdSU);
                                                                                if(noSIassgcurrstdSU == maxstdSIsperSU)
                                                                                {
                                                                                       flag1 = 1;
                                                                                 }
					               	              
      				}
                       }
                    }//if safSu

                 }//serchNext

  	} while (error != SA_AIS_ERR_NOT_EXIST && flag1 != 1);              
     



error = saImmOmSearchFinalize(searchHandle1);
        ////////////////////////////
 
  if(flag1 == 1)
  {
      printf("The SG has an SU satisfying above condition\n");
    // printf("\nWe can increase the maxstandbySIsperSU\n");
     attributeNames[0]="saAmfSGMaxStandbySIsperSU";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
					
   								 }
          ///search number of SIs_start
  		printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
       if((maxstdSIsperSU + 1) <= no_SIs)
       {
          printf("%c[1;33m\nCreate a CCB to increase saAmfSGMaxStandbySIsperSU\n",27);    
          printf("Creating...\n");
              swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = maxstdSIsperSU + 1; 
		     strcpy(nameccbattribute,"saAmfSGMaxStandbySIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in decrease the num of max standby SIs per SU",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        goto loop26;
  
       }
       else
       {
              goto loop28; 
       }
     
  }
  else
  {
     goto loop28;
  }
             ////////////////////////////////////////////////////////
           
        }
        else
        {
           goto loop28;
        }
}      
 
loop28:
printf("We check to increase standby SUs\n");
    // printf("\nWe can increase the maxstandbySIsperSU\n");
     attributeNames[0]="saAmfSGNumPrefStandbySUs";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										no_standbySUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,no_standbySUs);
					               	                }
					
   								 }
 attributeNames[0]="saAmfSGNumPrefActiveSUs";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										no_activeSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,no_activeSUs);
					               	                }
					
   								 }
if(preinstantiable == 1)
  {
         attributeNames[0]="saAmfSGNumPrefInserviceSUs";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noinsSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The inservice SUs of %s SG  are %d\n",objectnameSG1.value,noinsSUs);
					               	                }
					
   								 }
    }
    else
   {
       noinsSUs =  num_SUofSG(immHandle,objectnameSG1);  
   }
if(noinsSUs > (no_activeSUs + no_standbySUs))
{
   //printf("\n Create a CCB to increase saAmfSGNumPrefStandbySUs");
    printf("%c[1;33m\nCreate a CCB to increase saAmfSGNumPrefStandbySUs\n",27);    
          printf("Creating...\n");
              swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = no_standbySUs + 1; 
		     strcpy(nameccbattribute,"saAmfSGNumPrefStandbySUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in decrease the num of max standby SIs per SU",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
if(preinstantiable == 1)
  {
     buffer_checkker(immHandle,objectnameSG1,objectnameSI,1,argv);
  }
  else
  {
    buffer2_checkker(immHandle,objectnameSG1,objectnameSI,argv);
  }
   goto loop26;
 //Asynchronous call to buffer_manager
}

else
{
  rc = 3;
}

return rc;
}
//N+M Multiple SI increase_end


//N+M Single SI decrease_start
int single_SIDecreaseNPM(SaImmHandleT immHandle,SaNameT objectnameSG1,char *argv[],SaNameT objectnameSI,int preinstantiable)
{
printf("%c[1;32m\n***********************************************************************************",27);
printf("\n**************************************RM Adjustor**********************************");
printf("%c[0m",27);
//printf("\nI got a call\n");
SaNameT rootName = { 0, "" };
 SaNameT objectNameSU;
 strncpy((char *)rootName.value,objectnameSG1.value , SA_MAX_NAME_LENGTH);
 rootName.length = strlen((char *)rootName.value);
 //SaImmHandleT immHandle;
 SaImmSearchParametersT_2 searchParam,searchParam1;
 SaImmScopeT scope = SA_IMM_SUBLEVEL;	/* default search scope */
 SaImmScopeT scope1 = SA_IMM_SUBTREE;	/* default search scope */
 SaImmSearchHandleT searchHandle,searchHandle1;
 SaImmAccessorHandleT accessorHandle,accessorHandle3;
 int rc = EXIT_SUCCESS;
 char string[100],string2[100],string3[100];
 SaImmAdminOwnerHandleT ownerHandle;
 char ch1;
 int objectNames_len = 1;
 SaNameT objectsity;
 unsigned l;
 
 SaImmAttrNameT  attributeNames[] ={"safSISU",NULL};
 //const SaNameT rootName =  objectnameSI;
 SaAisErrorT error;
 SaNameT objectName;
 SaImmAttrValuesT_2 **attributes;
 char * ch;
 SaNameT **objectNames = NULL;
  SaNameT *objectNamec;
  SaNameT *objectName1c;
  char *s;
 char nameccbattribute[256],noassg[256];
 SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
int len1,len2,len3,i,cntr1,cntr2,flag=0,noSIassg,noassgactSUs,noSIassgactSU,noSIassgstdSU,noassgSUs,maxactSIsperSU,no_SIs,activeSIssum,activeSIs,noSIassgactSUF,noSIassgstdSUF,noassginsSUs,noactassgSUs,noassgstdSUs,maxstdSIsperSU,DefStandbyNumCSIs;
int noSIassgcurractSU,flag1=0,DefActiveNumCSIs,noSIassgcurrstdSU;
loop15:
attributeNames[0]="saAmfSGMaxActiveSIsperSU";
   attributeNames[1]=NULL;
  	      error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe number of saAmfSGMaxActiveSIsperSU of %s SG are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  		attributeNames[0]="saAmfSGNumPrefActiveSUs";
  		attributeNames[1]=NULL;
  		error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgactSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefActiveSUs of %s SG are %d\n",objectnameSG1.value,noassgactSUs);
					               	                }
								  }

		///search number of SIs_start
  		printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
printf("%c[1;32mCheck to decrease number of active SUs\n",27);
printf("%c[0m",27);
if((noassgactSUs-1) * maxactSIsperSU >= no_SIs )
{
  printf("%c[1;33m\nCreate a CCB to decrease number of SUs\n",27);
  printf("Creating..\n");
              swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = noassgactSUs - 1; 
		     strcpy(nameccbattribute,"saAmfSGNumPrefActiveSUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in decrease the num of max standby SIs per SU",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        buffer_checkker_decrease(immHandle,objectnameSG1,objectnameSI,1,argv,preinstantiable);

        goto loop16;
 
}
else
{
   printf("%c[1;31m\nCheck if we can increase saAmfSGMaxActiveSIsperSU\n",27);
   printf("%c[0m",27);
 
   strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
   objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
   printf("The SI .. type is %s \n",objectsity.value);
  	    attributeNames[0]="saAmfCtDefNumMaxActiveCSIs";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefActiveNumCSIs = *((SaUint32T *)attrValue);

										
										printf("The saAmfCtDefNumMaxActiveCSIs : %d \n", DefActiveNumCSIs );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe cSI is %d \n",DefStandbyNumCSIs);
					               	                }
								 }

        ////get the maxactiveSIperSU attribute
         attributeNames[0]="saAmfSGMaxActiveSIsperSU";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxActiveSIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
					
   								 }
/////////////////////////////////////////////find maxstandbySIperSU////////////////////////////
if(DefActiveNumCSIs > maxactSIsperSU)   
{
   printf("We can increase saAmfSGMaxActiveSIsperSU\n");
	////////////////////////////
           searchParam.searchOneAttr.attrName = NULL;
	   searchParam.searchOneAttr.attrValue = NULL;


	//printf("\n%s\n",rootName.value);
	error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle1);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	
                printf("Searching SU in the SG having equal number of SIs to the atrribute saAmfSGMaxActiveSIsperSU\n");
                //rc=EXIT_FAILURE;
                
	}

	noSIassgcurractSU =0;flag1 =0;
	do {
		error = saImmOmSearchNext_2(searchHandle1, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			//exit(EXIT_FAILURE);
		}
                if (error != SA_AIS_OK) {
			//printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			//exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                { 
                          //printf("\n%s\n",objectName.value);

                        ch =strstr(objectName.value,"safSu");
                      if(ch == NULL)
                      {   //printf("\n%s\n",objectName.value);
                      }
                      else
                      {   
			//printf("%s!!!!\n", objectName.value);
                        strcpy(string,objectName.value);
                        
                      
                                          

      
 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);

			//printf("\nstring:%s",objectNameSU.value);
		    
			attributeNames[0] = "saAmfSUNumCurrActiveSIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                	printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	rc= EXIT_FAILURE;
                     	}
       			if (error == SA_AIS_OK)
   			{
       				
       				error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
       				if(error == SA_AIS_OK)
       				{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgcurractSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of SU %s is %d\n",objectNameSU.value,noSIassgcurractSU);
                                                                                if(noSIassgcurractSU == maxactSIsperSU)
                                                                                {
                                                                                       flag1 = 1;
                                                                                 }
					               	              
      				}
                       }
                    }//if safSu

                 }//serchNext

  	} while (error != SA_AIS_ERR_NOT_EXIST && flag1 != 1);              
     



error = saImmOmSearchFinalize(searchHandle1);
        ////////////////////////////
 
  if(flag1 == 1)
  {
   printf("The SG has an SU satisfying above condition\n");
   attributeNames[0]="saAmfSGMaxActiveSIsperSU";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxActiveSIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
					
   								 }
                                                      printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
  	if((maxactSIsperSU+1) <= no_SIs)
  	{
            printf("%c[1;33m\nCreate a CCB to increase saAmfSGMaxActiveSIsperSU\n",27);
            printf("Creating...\n");
              swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = maxactSIsperSU+1; 
		     strcpy(nameccbattribute,"saAmfSGMaxActiveSIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in decrease the num of max standby SIs per SU",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        goto loop15;
  	}
        else
        {
            goto loop16;
         }
   
  }
  else
  {
      goto loop16;
  }
}
else
{
  loop16:
  printf("%c[1;32m\nTrying to decrease saAmfSGNumPrefStandbySUs\n",27);
  printf("%c[0m",27);
 attributeNames[0]="saAmfSGMaxStandbySIsperSU";
   attributeNames[1]=NULL;
  	      error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of saAmfSGMaxStandbySIsperSU of %s SG are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
								 }
  		attributeNames[0]="saAmfSGNumPrefStandbySUs";
  		attributeNames[1]=NULL;
  		error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgstdSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefStandbySUs of %s SG are %d\n",objectnameSG1.value,noassgstdSUs);
					               	                }
								  }

		///search number of SIs_start
  		printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end

if((noassgstdSUs-1) * maxstdSIsperSU >= no_SIs )
{
  printf("%c[1;33m\nCreate a CCB to decrease saAmfSGNumPrefStandbySUs\n",27);
  //printf("\nCreate a CCB to decrease number of SUs\n");
  printf("Creating...\n");
              swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = noassgstdSUs - 1; 
		     strcpy(nameccbattribute,"saAmfSGNumPrefStandbySUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in decrease the num of max standby SIs per SU",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        rc =1;
buffer_checkker_decrease(immHandle,objectnameSG1,objectnameSI,1,argv,preinstantiable);


}
else
{
 printf("%c[1;32m\nNeed to increase saAmfSGMaxstandbySIsperSU\n",27);
 printf("%c[0m",27);
  	strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
   	objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
   	printf("The SI .. type is %s \n",objectsity.value);
  	    attributeNames[0]="saAmfCtDefNumMaxStandbyCSIs";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefStandbyNumCSIs = *((SaUint32T *)attrValue);

										
										printf("The saAmfCtDefNumMaxStandbyCSIs : %d \n", DefStandbyNumCSIs );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe cSI is %d \n",DefStandbyNumCSIs);
					               	                }
								 }

        ////get the maxactiveSIperSU attribute
         attributeNames[0]="saAmfSGMaxStandbySIsperSU";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
					
   								 }
	/////////////////////////////////////////////find maxstandbySIperSU////////////////////////////
        
	if(DefStandbyNumCSIs > maxstdSIsperSU)   
	{    
               printf("We can increase saAmfSGMaxStandbySIsperSU\n");
             /////////////////////////////////////////////////////////
                searchParam.searchOneAttr.attrName = NULL;
	        searchParam.searchOneAttr.attrValue = NULL;


	//printf("\n%s\n",rootName.value);
	error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle1);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	         printf("Searching SU in the SG having equal number of SIs to the atrribute saAmfSGMaxStandbySIsperSU\n");
                //printf("\nSearch hanadle Obtained\n");
                //rc=EXIT_FAILURE;
                
	}

	noSIassgcurractSU =0;flag1 =0;
	do {
		error = saImmOmSearchNext_2(searchHandle1, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			//exit(EXIT_FAILURE);
		}
                if (error != SA_AIS_OK) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			//exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                { 
                          //printf("\n%s\n",objectName.value);

                        ch =strstr(objectName.value,"safSu");
                      if(ch == NULL)
                      {   //printf("\n%s\n",objectName.value);
                      }
                      else
                      {   
			//printf("%s!!!!\n", objectName.value);
                        strcpy(string,objectName.value);
                        
                      
                                          

      
 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);

			//printf("\nstring:%s",objectNameSU.value);
		    
			attributeNames[0] = "saAmfSUNumCurrStandbySIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                	printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	rc= EXIT_FAILURE;
                     	}
       			if (error == SA_AIS_OK)
   			{
       				
       				error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
       				if(error == SA_AIS_OK)
       				{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgcurrstdSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of SU %s is %d\n",objectNameSU.value,noSIassgcurrstdSU);
                                                                                if(noSIassgcurrstdSU == maxstdSIsperSU)
                                                                                {
                                                                                       flag1 = 1;
                                                                                 }
					               	              
      				}
                       }
                    }//if safSu

                 }//serchNext

  	} while (error != SA_AIS_ERR_NOT_EXIST && flag1 != 1);              
     



error = saImmOmSearchFinalize(searchHandle1);
        ////////////////////////////
 
  if(flag1 == 1)
  {
      printf("The SG has an SU satisfying above condition\n");
    // printf("\nWe can increase the maxstandbySIsperSU\n");
     attributeNames[0]="saAmfSGMaxStandbySIsperSU";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
					
   								 }
          ///search number of SIs_start
  		printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
       if((maxstdSIsperSU + 1) <= no_SIs)
       {
          printf("%c[1;33m\nCreate a CCB to increase saAmfSGMaxStandbySIsperSU\n",27);    
          printf("Creating...\n");
              swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		     noassgSUs = maxstdSIsperSU + 1; 
		     strcpy(nameccbattribute,"saAmfSGMaxStandbySIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		//printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in decrease the num of max standby SIs per SU",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        goto loop16;
  
       }
       else
       {
              rc =2; 
       }
     
  }
  else
  {
     rc = 2;
  }
             ////////////////////////////////////////////////////////
           
        }
        else
        {
           rc =2;
        }
}      
 
}

}
return rc;
}
//N+M Single SI decrease_end

//Nway SI increase_start
int single_SIIncreaseNway(SaImmHandleT immHandle,SaNameT objectnameSG1,char *argv[],SaNameT objectnameSI,int preinstantiable)
{
  //printf("\nI got a call");
  printf("%c[1;32m\n***********************************************************************************",27);
printf("\n**************************************RM Adjustor**********************************");
printf("%c[0m",27);
 //printf("\nI got a call\n");
 /////SU handling the SI
  SaNameT rootName = { 0, "" };
 SaNameT objectNameSU;
 strncpy((char *)rootName.value,objectnameSI.value , SA_MAX_NAME_LENGTH);
 rootName.length = strlen((char *)rootName.value);
 //SaImmHandleT immHandle;
 SaImmSearchParametersT_2 searchParam,searchParam1;
 SaImmScopeT scope = SA_IMM_SUBLEVEL;	/* default search scope */
 SaImmScopeT scope1 = SA_IMM_SUBTREE;	/* default search scope */
 SaImmSearchHandleT searchHandle,searchHandle1;
 SaImmAccessorHandleT accessorHandle,accessorHandle3;
 int rc = EXIT_SUCCESS,noinserviceSUs=0;
 char string[100],string2[100],string3[100];
 SaImmAdminOwnerHandleT ownerHandle;
 char ch1;
 int objectNames_len = 1;
 SaNameT objectsity;
 unsigned l;
 
 SaImmAttrNameT  attributeNames[] ={"safSISU",NULL};
 //const SaNameT rootName =  objectnameSI;
 SaAisErrorT error;
 SaNameT objectName;
 SaImmAttrValuesT_2 **attributes;
 char * ch;
 SaNameT **objectNames = NULL;
  SaNameT *objectNamec;
  SaNameT *objectName1c;
  char *s;
 char nameccbattribute[256],noassg[256];
 SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
 int len1,len2,len3,i,cntr1,cntr2,flag=0,noSIassg,noSIassgactSU,noSIassgstdSU,noassgSUs,maxactSIsperSU,no_SIs,activeSIssum,activeSIs,flag1,noSIassgactSUF,noSIassgstdSUF,noassginsSUs,noactassgSUs,noassgstdSUs,maxstdSIsperSU,DefStandbyNumCSIs;

  /////
  searchParam.searchOneAttr.attrName = NULL;
searchParam.searchOneAttr.attrValue = NULL;


//printf("\n%s\n",rootName.value);
error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	
               // printf("\nSearch hanadle Obtained\n");
                rc=EXIT_FAILURE;
                
	}

noSIassgactSUF =0 ,noSIassgstdSUF=0;
do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                {
                                ch =strstr(objectName.value,"safSISU");
                      if(ch == NULL)
                      {
                      }
                      else
                      {   
			//printf("%s\n", objectName.value);
                        strcpy(string,objectName.value);
                        
                      }
                                          

      
 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);

			//printf("\nstring:%s",objectNameSU.value);
			attributeNames[0] = "saAmfSISUHAState";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                	printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	rc= EXIT_FAILURE;
                     	}
       			if (error == SA_AIS_OK)
   			{
       				
       				error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       				if(error == SA_AIS_OK)
       				{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassg = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of SU %s is %d\n",objectNameSU.value,noSIassg);
					               	              
      				}
//

				error = saImmOmAccessorFinalize(accessorHandle);
                   
                        strcpy(string2,"safSISU=");
                        len1=strlen(string);
      			len2=strlen(string2);   
      

      			for(i=0;(i+len2+1)<=len1;i++)
            		string[i]=string[len2+i];
            		string[i]='\0';
      			//printf("\nDeleted : %s\n",string);

    			strcpy(string3,",");
    			strcat(string3,rootName.value);
    			//printf("\nconcated :%s\n",string3);

      				//printf("\n%s\n",string);

      			len1=strlen(string);
  
      			len2=strlen(string3); 
      			len3=len1-len2;
    
      

      			for(i=0;i<len3;i++)
            		string[i]=string[i];
            		string[i]='\0';
      			//printf("\nDeleted2 : %s\n",string);


    			remove_all_chars(string, '\\');
    			//printf("'%s'\n", string);


 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);
                    if (noSIassg == 1)
                    {
                        attributeNames[0] = "saAmfSUNumCurrActiveSIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgactSUF = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current active SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgactSUF);
					               	                       // printf("\nThe SU : %s is handling active assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgactSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 
                 
                        attributeNames[0] = "saAmfSUNumCurrStandbySIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgstdSUF = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current standby SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgstdSUF);
					               	            // printf("The SU : %s is handling standby assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgstdSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 }
                 if(noSIassg !=1 && noSIassg !=2)
                 {
                     //printf("\n error : no association class\n");
                 }
                 

                      
             }
          }
 ////SU handling the SI

} while (error != SA_AIS_ERR_NOT_EXIST);
error = saImmOmSearchFinalize(searchHandle);
  /////
  printf("\nThe SU is handling %d active assignments and %d standby assignments",noSIassgactSUF,noSIassgstdSUF);
  if(noSIassgactSUF>=noSIassgstdSUF)
  {
    printf("\nWe will spread the active assignments");


    ////////////////
loop17:
 printf("%c[1;32m\nChecking to spread the SI assignment i.e decrease the saAmfSGMaxActiveSIsperSU\n",27);
printf("%c[0m",27);
 attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of saAmfSGMaxActiveSIsperSU of %s SG are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }

///search number of SIs_start
  printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			printf("error - saImmOmSearchInitialize_2 FAILED:%d",error);
                	   		 		 		        //printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
///search number of SIs_end
//////////// Decrease MaxActive SI per SU 
printf("Checking if we can decrease the saAmfSGMaxActiveSIsperSU\n");
   if((maxactSIsperSU-1)*noassgSUs >= no_SIs)
   {
      printf("\nWe can decrease maxactiveSIperSU");
      printf("%c[1;32m\nThere is enough capacity hence we can decrease saAmfSGMaxActiveSIsperSU\n",27);
      printf("%c[0m",27);
      printf("%c[1;33mCreating a CCB ....\n ",27);
      //////ccb to increase maxactiveSiperSU

            swapsi(immHandle,argv,2,objectnameSG1);  //lock SG

             //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		noassgSUs = maxactSIsperSU-1; 
		strcpy(nameccbattribute,"saAmfSGMaxActiveSIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        //noSIassgactSU=magicSicurractive(objectnameSI,immHandle);

for(i=0;i<2;i++)
{
//printf("\nsleeping\n");
usleep(60000);
//printf("\nGot up!\n");
}   


searchParam.searchOneAttr.attrName = NULL;
searchParam.searchOneAttr.attrValue = NULL;


//printf("\n%s\n",rootName.value);
error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	
               // printf("\nSearch hanadle Obtained\n");
                rc=EXIT_FAILURE;
                
	}

noSIassgactSU =0 ,noSIassgstdSU=0;
do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                {
                                ch =strstr(objectName.value,"safSISU");
                      if(ch == NULL)
                      {
                      }
                      else
                      {   
			//printf("%s\n", objectName.value);
                        strcpy(string,objectName.value);
                        
                      }
                                          

      
 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);

			//printf("\nstring:%s",objectNameSU.value);
			attributeNames[0] = "saAmfSISUHAState";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                	printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	rc= EXIT_FAILURE;
                     	}
       			if (error == SA_AIS_OK)
   			{
       				
       				error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       				if(error == SA_AIS_OK)
       				{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassg = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of SU %s is %d\n",objectNameSU.value,noSIassg);
					               	              
      				}
//

				error = saImmOmAccessorFinalize(accessorHandle);
                   
                        strcpy(string2,"safSISU=");
                        len1=strlen(string);
      			len2=strlen(string2);   
      

      			for(i=0;(i+len2+1)<=len1;i++)
            		string[i]=string[len2+i];
            		string[i]='\0';
      			//printf("\nDeleted : %s\n",string);

    			strcpy(string3,",");
    			strcat(string3,rootName.value);
    			//printf("\nconcated :%s\n",string3);

      				//printf("\n%s\n",string);

      			len1=strlen(string);
  
      			len2=strlen(string3); 
      			len3=len1-len2;
    
      

      			for(i=0;i<len3;i++)
            		string[i]=string[i];
            		string[i]='\0';
      			//printf("\nDeleted2 : %s\n",string);


    			remove_all_chars(string, '\\');
    			//printf("'%s'\n", string);


 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);
                    if (noSIassg == 1)
                    {
                        attributeNames[0] = "saAmfSUNumCurrActiveSIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgactSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current active SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgactSUF);
					               	                       // printf("\nThe SU : %s is handling active assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgactSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 
                 
                        attributeNames[0] = "saAmfSUNumCurrStandbySIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgstdSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current standby SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgstdSUF);
					               	            // printf("The SU : %s is handling standby assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgstdSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 }
                 if(noSIassg !=1 && noSIassg !=2)
                 {
                     //printf("\n error : no association class\n");
                 }
                 

                      
             }
          }
 ////SU handling the SI

} while (error != SA_AIS_ERR_NOT_EXIST);
error = saImmOmSearchFinalize(searchHandle);
  /////
  printf("\nThe SU is handling1 %d active assignments and %d standby assignments",noSIassgactSU,noSIassgstdSU);
if(noSIassgactSU < noSIassgactSUF)
{
  printf("\nJob well done");
  rc =1;
}   
else
{
  goto loop17;
}

        
   }
   else
   {
        loop18:
   	printf("\nWe cannot spread active so we try standby");
        printf("%c[1;32m\nChecking to spread the SI assignment i.e decrease the saAmfSGMaxStandbySIsperSU\n",27);
	printf("%c[0m",27);
 	attributeNames[0]="saAmfSGMaxStandbySIsperSU";
  	attributeNames[1]=NULL;
  	error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of saAmfSGMaxStandbySIsperSU of %s SG are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
								 }
  attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }

///search number of SIs_start
  printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			printf("error - saImmOmSearchInitialize_2 FAILED:%d",error);
                	   		 		 		        //printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
///search number of SIs_end
//////////// Decrease MaxActive SI per SU 
printf("Checking if we can decrease the saAmfSGMaxStandbySIsperSU\n");
   
        if((maxstdSIsperSU-1)*noassgSUs >= no_SIs)
        {
            
                  printf("\nWe can decrease maxstandbySIperSU");
      printf("%c[1;32m\nThere is enough capacity hence we can decrease saAmfSGMaxStandbySIsperSU\n",27);
      printf("%c[0m",27);
      printf("%c[1;33mCreating a CCB ....\n ",27);
      //////ccb to increase maxstandbySiperSU

            swapsi(immHandle,argv,2,objectnameSG1);  //lock SG

             //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		noassgSUs = maxstdSIsperSU-1; 
		strcpy(nameccbattribute,"saAmfSGMaxStandbySIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        //noSIassgactSU=magicSicurractive(objectnameSI,immHandle);

for(i=0;i<2;i++)
{
//printf("\nsleeping\n");
usleep(60000);
//printf("\nGot up!\n");
}   


searchParam.searchOneAttr.attrName = NULL;
searchParam.searchOneAttr.attrValue = NULL;


//printf("\n%s\n",rootName.value);
error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	
               // printf("\nSearch hanadle Obtained\n");
                rc=EXIT_FAILURE;
                
	}

noSIassgactSU =0 ,noSIassgstdSU=0;
do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                {
                                ch =strstr(objectName.value,"safSISU");
                      if(ch == NULL)
                      {
                      }
                      else
                      {   
			//printf("%s\n", objectName.value);
                        strcpy(string,objectName.value);
                        
                      }
                                          

      
 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);

			//printf("\nstring:%s",objectNameSU.value);
			attributeNames[0] = "saAmfSISUHAState";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                	printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	rc= EXIT_FAILURE;
                     	}
       			if (error == SA_AIS_OK)
   			{
       				
       				error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       				if(error == SA_AIS_OK)
       				{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassg = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of SU %s is %d\n",objectNameSU.value,noSIassg);
					               	              
      				}
//

				error = saImmOmAccessorFinalize(accessorHandle);
                   
                        strcpy(string2,"safSISU=");
                        len1=strlen(string);
      			len2=strlen(string2);   
      

      			for(i=0;(i+len2+1)<=len1;i++)
            		string[i]=string[len2+i];
            		string[i]='\0';
      			//printf("\nDeleted : %s\n",string);

    			strcpy(string3,",");
    			strcat(string3,rootName.value);
    			//printf("\nconcated :%s\n",string3);

      				//printf("\n%s\n",string);

      			len1=strlen(string);
  
      			len2=strlen(string3); 
      			len3=len1-len2;
    
      

      			for(i=0;i<len3;i++)
            		string[i]=string[i];
            		string[i]='\0';
      			//printf("\nDeleted2 : %s\n",string);


    			remove_all_chars(string, '\\');
    			//printf("'%s'\n", string);


 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);
                    if (noSIassg == 1)
                    {
                        attributeNames[0] = "saAmfSUNumCurrActiveSIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgactSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current active SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgactSUF);
					               	                       // printf("\nThe SU : %s is handling active assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgactSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 
                 
                        attributeNames[0] = "saAmfSUNumCurrStandbySIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgstdSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current standby SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgstdSUF);
					               	            // printf("The SU : %s is handling standby assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgstdSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 }
                 if(noSIassg !=1 && noSIassg !=2)
                 {
                     //printf("\n error : no association class\n");
                 }
                 

                      
             }
          }
 ////SU handling the SI

} while (error != SA_AIS_ERR_NOT_EXIST);
error = saImmOmSearchFinalize(searchHandle);
  /////
  printf("\nThe SU is handling1 %d active assignments and %d standby assignments",noSIassgactSU,noSIassgstdSU);
if(noSIassgstdSU < noSIassgstdSUF)
{
  printf("\nJob well done");
  rc =1;
}   
else
{
  goto loop18;
}
 
        }
        else
        {
            printf("\nNeed to increase assigned SU");
            



            printf("%c[1;31m\nWe check to increase saAmfSGNumPrefAssignedSUs\n",27);
  printf("%c[0m",27);
if(preinstantiable == 1)
{
	attributeNames[0]="saAmfSGNumPrefInserviceSUs";
	attributeNames[1]=NULL;
	error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noinserviceSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of inservice SUs of %s SG are %d\n",objectnameSG1.value,noinserviceSUs);
					               	                }
 //printf("To check to increase saAmfSGNumPrefAssignedSUs we check for enough inservice SUs\n");
  		                                        	 }
 }
 else
 {
       noassginsSUs =  num_SUofSG(immHandle,objectnameSG1);  
 }
 	if (noinserviceSUs > noassgSUs)
 	{
 	    printf("%c[1;31mThere is enough capacity ... \n",27);
            printf("%c[0m",27);
            printf("%c[1;33m\nCreate a CCB to increase saAmfSGNumPrefAssignedSUs\n",27);
            printf("Creating...\n");
            swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		noassgSUs = noassgSUs+1; 
		strcpy(nameccbattribute,"saAmfSGNumPrefAssignedSUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	    //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
	printf("%c[0m",27);
	//give a asynchronous call to  buffer manager functions//
if(preinstantiable == 1)
  {
     buffer_checkker(immHandle,objectnameSG1,objectnameSI,0,argv);
  }
  else
  {
    buffer2_checkker(immHandle,objectnameSG1,objectnameSI,argv);
  }
	//give a asynchronous call to  buffer manager functions//
  
        goto loop17;
      }
      else
      {
         rc = 2;
      }  
         
            
            
        }
   }

}
else
{

   printf("\nWe will spread the active assignments");


    ////////////////
loop20:
 printf("%c[1;32m\nChecking to spread the SI assignment i.e decrease the saAmfSGMaxStandbySIsperSU\n",27);
printf("%c[0m",27);
 attributeNames[0]="saAmfSGMaxStandbySIsperSU";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of saAmfSGMaxStandbySIsperSU of %s SG are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
								 }
  attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }

///search number of SIs_start
  printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			printf("error - saImmOmSearchInitialize_2 FAILED:%d",error);
                	   		 		 		        //printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
///search number of SIs_end
//////////// Decrease MaxActive SI per SU 
printf("Checking if we can decrease the saAmfSGMaxActiveSIsperSU\n");
   if((maxstdSIsperSU-1)*noassgSUs >= no_SIs)
   {
      printf("\nWe can decrease maxstandbySIperSU");
      printf("%c[1;32m\nThere is enough capacity hence we can decrease saAmfSGMaxActiveSIsperSU\n",27);
      printf("%c[0m",27);
      printf("%c[1;33mCreating a CCB ....\n ",27);
      //////ccb to increase maxactiveSiperSU

            swapsi(immHandle,argv,2,objectnameSG1);  //lock SG

             //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		noassgSUs = maxstdSIsperSU-1; 
		strcpy(nameccbattribute,"saAmfSGMaxStandbySIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        //noSIassgactSU=magicSicurractive(objectnameSI,immHandle);

for(i=0;i<2;i++)
{
//printf("\nsleeping\n");
usleep(60000);
//printf("\nGot up!\n");
}   


searchParam.searchOneAttr.attrName = NULL;
searchParam.searchOneAttr.attrValue = NULL;


//printf("\n%s\n",rootName.value);
error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	
               // printf("\nSearch hanadle Obtained\n");
                rc=EXIT_FAILURE;
                
	}

noSIassgactSU =0 ,noSIassgstdSU=0;
do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                {
                                ch =strstr(objectName.value,"safSISU");
                      if(ch == NULL)
                      {
                      }
                      else
                      {   
			//printf("%s\n", objectName.value);
                        strcpy(string,objectName.value);
                        
                      }
                                          

      
 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);

			//printf("\nstring:%s",objectNameSU.value);
			attributeNames[0] = "saAmfSISUHAState";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                	printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	rc= EXIT_FAILURE;
                     	}
       			if (error == SA_AIS_OK)
   			{
       				
       				error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       				if(error == SA_AIS_OK)
       				{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassg = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of SU %s is %d\n",objectNameSU.value,noSIassg);
					               	              
      				}
//

				error = saImmOmAccessorFinalize(accessorHandle);
                   
                        strcpy(string2,"safSISU=");
                        len1=strlen(string);
      			len2=strlen(string2);   
      

      			for(i=0;(i+len2+1)<=len1;i++)
            		string[i]=string[len2+i];
            		string[i]='\0';
      			//printf("\nDeleted : %s\n",string);

    			strcpy(string3,",");
    			strcat(string3,rootName.value);
    			//printf("\nconcated :%s\n",string3);

      				//printf("\n%s\n",string);

      			len1=strlen(string);
  
      			len2=strlen(string3); 
      			len3=len1-len2;
    
      

      			for(i=0;i<len3;i++)
            		string[i]=string[i];
            		string[i]='\0';
      			//printf("\nDeleted2 : %s\n",string);


    			remove_all_chars(string, '\\');
    			//printf("'%s'\n", string);


 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);
                    if (noSIassg == 1)
                    {
                        attributeNames[0] = "saAmfSUNumCurrActiveSIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgactSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current active SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgactSUF);
					               	                       // printf("\nThe SU : %s is handling active assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgactSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 
                 
                        attributeNames[0] = "saAmfSUNumCurrStandbySIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgstdSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current standby SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgstdSUF);
					               	            // printf("The SU : %s is handling standby assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgstdSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 }
                 if(noSIassg !=1 && noSIassg !=2)
                 {
                     //printf("\n error : no association class\n");
                 }
                 

                      
             }
          }
 ////SU handling the SI

} while (error != SA_AIS_ERR_NOT_EXIST);
error = saImmOmSearchFinalize(searchHandle);
  /////
  printf("\nThe SU is handling1 %d active assignments and %d standby assignments",noSIassgactSU,noSIassgstdSU);
if(noSIassgstdSU < noSIassgstdSUF)
{
  printf("\nJob well done");
  rc =1;
}   
else
{
  goto loop20;
}

        
   }
   else
   {
        loop21:
   	printf("\nWe cannot spread standby so we try active");
        printf("%c[1;32m\nChecking to spread the SI assignment i.e decrease the saAmfSGMaxActiveSIsperSU\n",27);
	printf("%c[0m",27);
 	attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  	attributeNames[1]=NULL;
  	error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of saAmfSGMaxActiveSIsperSU of %s SG are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }

///search number of SIs_start
  printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			printf("error - saImmOmSearchInitialize_2 FAILED:%d",error);
                	   		 		 		        //printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
///search number of SIs_end
//////////// Decrease MaxActive SI per SU 
printf("Checking if we can decrease the saAmfSGMaxStandbySIsperSU\n");
   
        if((maxactSIsperSU-1)*noassgSUs >= no_SIs)
        {
            
                  printf("\nWe can decrease maxactiveSIperSU");
      printf("%c[1;32m\nThere is enough capacity hence we can decrease saAmfSGMaxActiveSIsperSU\n",27);
      printf("%c[0m",27);
      printf("%c[1;33mCreating a CCB ....\n ",27);
      //////ccb to increase maxstandbySiperSU

            swapsi(immHandle,argv,2,objectnameSG1);  //lock SG

             //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		noassgSUs = maxstdSIsperSU-1; 
		strcpy(nameccbattribute,"saAmfSGMaxActiveSIsperSU=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	   
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
        printf("%c[0m",27);
        //noSIassgactSU=magicSicurractive(objectnameSI,immHandle);


for(i=0;i<2;i++)
{
//printf("\nsleeping\n");
usleep(60000);
//printf("\nGot up!\n");
}   


searchParam.searchOneAttr.attrName = NULL;
searchParam.searchOneAttr.attrValue = NULL;


//printf("\n%s\n",rootName.value);
error = saImmOmSearchInitialize_2(immHandle, &rootName, scope, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	
               // printf("\nSearch hanadle Obtained\n");
                rc=EXIT_FAILURE;
                
	}

noSIassgactSU =0 ,noSIassgstdSU=0;
do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                {
                                ch =strstr(objectName.value,"safSISU");
                      if(ch == NULL)
                      {
                      }
                      else
                      {   
			//printf("%s\n", objectName.value);
                        strcpy(string,objectName.value);
                        
                      }
                                          

      
 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);

			//printf("\nstring:%s",objectNameSU.value);
			attributeNames[0] = "saAmfSISUHAState";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                	printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	rc= EXIT_FAILURE;
                     	}
       			if (error == SA_AIS_OK)
   			{
       				
       				error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       				if(error == SA_AIS_OK)
       				{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassg = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of SU %s is %d\n",objectNameSU.value,noSIassg);
					               	              
      				}
//

				error = saImmOmAccessorFinalize(accessorHandle);
                   
                        strcpy(string2,"safSISU=");
                        len1=strlen(string);
      			len2=strlen(string2);   
      

      			for(i=0;(i+len2+1)<=len1;i++)
            		string[i]=string[len2+i];
            		string[i]='\0';
      			//printf("\nDeleted : %s\n",string);

    			strcpy(string3,",");
    			strcat(string3,rootName.value);
    			//printf("\nconcated :%s\n",string3);

      				//printf("\n%s\n",string);

      			len1=strlen(string);
  
      			len2=strlen(string3); 
      			len3=len1-len2;
    
      

      			for(i=0;i<len3;i++)
            		string[i]=string[i];
            		string[i]='\0';
      			//printf("\nDeleted2 : %s\n",string);


    			remove_all_chars(string, '\\');
    			//printf("'%s'\n", string);


 			strncpy((char *)objectNameSU.value, string, SA_MAX_NAME_LENGTH);
 			objectNameSU.length = strlen((char *)objectNameSU.value);
                    if (noSIassg == 1)
                    {
                        attributeNames[0] = "saAmfSUNumCurrActiveSIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgactSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current active SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgactSUF);
					               	                       // printf("\nThe SU : %s is handling active assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgactSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 
                 
                        attributeNames[0] = "saAmfSUNumCurrStandbySIs";
			attributeNames[1] = NULL;
			error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			if (error != SA_AIS_OK) {
                		printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                		rc= EXIT_FAILURE;
                      }
       		    if (error == SA_AIS_OK)
   		    {
       			if (error !=SA_AIS_OK)
       			{
	 			printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
       			} 
       			error= saImmOmAccessorGet_2(accessorHandle, &objectNameSU, attributeNames, &attributes);
       			if(error == SA_AIS_OK)
       			{
            
                                                                        
                                       					
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noSIassgstdSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe number of current standby SI assignment for %s SU are %d\n",objectNameSU.value,noSIassgstdSUF);
					               	            // printf("The SU : %s is handling standby assignment of SI :%s and the SI is sharing capacity with %d number of SIs\n",objectNameSU.value,objectnameSI.value,noSIassgstdSUF);
      			}
//

			error = saImmOmAccessorFinalize(accessorHandle);
      
   		   }
                 }
                 if(noSIassg !=1 && noSIassg !=2)
                 {
                     //printf("\n error : no association class\n");
                 }
                 

                      
             }
          }
 ////SU handling the SI

} while (error != SA_AIS_ERR_NOT_EXIST);
error = saImmOmSearchFinalize(searchHandle);
  /////
  printf("\nThe SU is handling1 %d active assignments and %d standby assignments",noSIassgactSU,noSIassgstdSU);
if(noSIassgactSU < noSIassgactSUF)
{
  printf("\nJob well done");
  rc =1;
}   
else
{
  goto loop21;
}
 
        }
        else
        {
            printf("\nNeed to increase assigned SU");
            



            printf("%c[1;31m\nWe check to increase saAmfSGNumPrefAssignedSUs\n",27);
  printf("%c[0m",27);
if(preinstantiable == 1)
  {
attributeNames[0]="saAmfSGNumPrefInserviceSUs";
attributeNames[1]=NULL;
error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noinserviceSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of inservice SUs of %s SG are %d\n",objectnameSG1.value,noinserviceSUs);
					               	                }
 //printf("To check to increase saAmfSGNumPrefAssignedSUs we check for enough inservice SUs\n");
  		                                        	 }
}
else
{
       noassginsSUs =  num_SUofSG(immHandle,objectnameSG1);  
}
 	if (noinserviceSUs > noassgSUs)
 	{
 	    printf("%c[1;31mThere is enough capacity ... \n",27);
            printf("%c[0m",27);
            printf("%c[1;33m\nCreate a CCB to increase saAmfSGNumPrefAssignedSUs\n",27);
            printf("Creating...\n");
            swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
             //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 		noassgSUs = noassgSUs+1; 
		strcpy(nameccbattribute,"saAmfSGNumPrefAssignedSUs=");
                         sprintf(noassg, "%d",noassgSUs);
               		 strcat(nameccbattribute,noassg);
             		// printf("\n%s",nameccbattribute);
                        s= objectnameSG1.value ;
                        //printf("\n%s\n",s);
                        objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 			objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
			objectNames[objectNames_len++] = NULL;
 			objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       //??
      		       
		       error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      if (error != SA_AIS_OK) {
		       printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		       rc = EXIT_FAILURE;
		      }
		     if (error == SA_AIS_OK)
             	     {   
			//printf("\n%s in increase the num of assigned SUs",objectNames[0]->value);
	 	       rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                       free(objectNamec);
                      
		        if(rc==0)
		        {
		           rc=1;
		        }
        
	             }
	
	error = saImmOmAdminOwnerFinalize(ownerHandle);
	if (SA_AIS_OK != error) {
		printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
		rc = EXIT_FAILURE;
	//	goto done_om_finalize;
	}
	    //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
	swapsi(immHandle,argv,1,objectnameSG1); //UNLOCK SG
	printf("%c[0m",27);
	//give a asynchronous call to  buffer manager functions//
if(preinstantiable == 1)
  {
     buffer_checkker(immHandle,objectnameSG1,objectnameSI,0,argv);
  }
  else
  {
    buffer2_checkker(immHandle,objectnameSG1,objectnameSI,argv);
  }
	//give a asynchronous call to  buffer manager functions//
  
        goto loop20;
      }
      else
      {
         rc = 2;
      }  
         
            
            
        }
   }
  
}

  

return rc;
}
//Nway single_SI increase_end


//Nway multiple SI increase_start
int multiple_SIIncreaseNway(SaImmHandleT immHandle,SaNameT objectnameSG1,char *argv[],SaNameT objectnameSI,int preinstantiable)
{
//printf("\nI got a call");
printf("%c[1;32m\n***********************************************************************************",27);
printf("\n**************************************RM Adjustor**********************************");
printf("%c[0m",27);
 //printf("\nI got a call\n");
 /////SU handling the SI
  SaNameT rootName = { 0, "" };
 SaNameT objectNameSU;
 strncpy((char *)rootName.value,objectnameSI.value , SA_MAX_NAME_LENGTH);
 rootName.length = strlen((char *)rootName.value);
 //SaImmHandleT immHandle;
 SaImmSearchParametersT_2 searchParam,searchParam1;
 SaImmScopeT scope = SA_IMM_SUBLEVEL;	/* default search scope */
 SaImmScopeT scope1 = SA_IMM_SUBTREE;	/* default search scope */
 SaImmSearchHandleT searchHandle,searchHandle1;
 SaImmAccessorHandleT accessorHandle,accessorHandle3;
 int rc = EXIT_SUCCESS,noinserviceSUs=0,DefActiveNumCSIs,maxctSIsperSUact,noinsSUs,noassSUs,maxstdSIsperSUact,maxctSIsperSUstd;
 char string[100],string2[100],string3[100];
 SaImmAdminOwnerHandleT ownerHandle;
 char ch1;
 int objectNames_len = 1,no_SIs1=0;
 SaNameT objectsity;
 unsigned l;
 //int maxactSIsperSU;
 SaImmAttrNameT  attributeNames[] ={"safSISU",NULL};
 //const SaNameT rootName =  objectnameSI;
 SaAisErrorT error;
 SaNameT objectName;
 SaImmAttrValuesT_2 **attributes;
 char * ch;
 SaNameT **objectNames = NULL;
  SaNameT *objectNamec;
  SaNameT *objectName1c;
  char *s;
 char nameccbattribute[256],noassg[256];
 SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
 int len1,len2,len3,i,cntr1,cntr2,flag=0,noSIassg,noSIassgactSU,noSIassgstdSU,noassgSUs,maxactSIsperSU,no_SIs,activeSIssum,activeSIs,flag1,noSIassgactSUF,noSIassgstdSUF,noassginsSUs,noactassgSUs,noassgstdSUs,maxstdSIsperSU,DefStandbyNumCSIs;

 printf("%c[1;32m\nChecking if the active assignment of the new SI created is assigned\n",27);
printf("%c[0m",27);
loop22:
 attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of saAmfSGMaxActiveSIsperSU of %s SG are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
noassgSUs=0;								 }
  attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }

///search number of SIs_start
  printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;
                                  
                                                                   

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			printf("error - saImmOmSearchInitialize_2 FAILED:%d",error);
                	   		 		 		        //printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							//flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
///search number of SIs_end
//////////// Decrease MaxActive SI per SU 
printf("Checking ..\n");
   if((maxactSIsperSU)*noassgSUs >= no_SIs)
   {
       printf("\n Already assigned");
       goto loop23;
   }
   else
   {
       //need to increase active capacity
       printf("%c[1;31m\nNeed to increase saAmfSGMaxActiveSIsperSU\n",27);
printf("%c[0m",27);
strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
printf("The SI .. type is %s \n",objectsity.value);
//printf("\n1\n");
  	    attributeNames[0]="saAmfCtDefNumMaxActiveCSIs";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	//printf("\n2\n");
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{       //printf("\n3\n");
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {       // printf("\n4\n");
	                                                              /*	
                                                                        	printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefActiveNumCSIs = *((SaUint32T *)attrValue);
                                                                        **/

										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxctSIsperSUact = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
										printf("The saAmfCtDefNumMaxActiveCSIs : %d \n", maxctSIsperSUact );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe cSI is %d \n",maxactSIsperSU);
					               	                }
								 }

 attributeNames[0]="saAmfSGMaxActiveSIsperSU";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of saAmfSGMaxActiveSIsperSU of %s SG are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
       if(maxctSIsperSUact > maxactSIsperSU )
       {
               //create a ccb to increase the maxactiveSIsperSU
                printf("Create a CCB to increase saAmfSGMaxActiveSIsperSU of %s SG\n",objectnameSG1.value);
 									 	     ////////////////////////create ccb to increase saAmfSGMaxActiveSIsperSU
  								         	     swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
 								        	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 								        	     noassgSUs = maxactSIsperSU+1; 
		    					                	     strcpy(nameccbattribute,"saAmfSGMaxActiveSIsperSU=");
                         							     sprintf(noassg, "%d",noassgSUs);
               									     strcat(nameccbattribute,noassg);
             									     // printf("\n%s",nameccbattribute);
                        							     s= objectnameSG1.value ;
                        							     //printf("\n%s\n",s);
                        							     objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 										     objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
										     objectNames[objectNames_len++] = NULL;
 										     objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       							//??
      		       
		      							 	      error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      						        	      if (error != SA_AIS_OK) {
		       										printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		      								 		rc = EXIT_FAILURE;
		     							 	      }
		     				         			      if (error == SA_AIS_OK)
             	    					                	      {   
			  
	 	      	  							 		rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                      								 		free(objectNamec);
                      
		       								 		if(rc==0)
		        							                {
		          	 									rc=1;
		      							        		}
        
	          						         	       }
	
										   error = saImmOmAdminOwnerFinalize(ownerHandle);
										   if (SA_AIS_OK != error) {
											printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
											rc = EXIT_FAILURE;
										//	goto done_om_finalize;
										}
								   		 //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
										swapsi(immHandle,argv,1,objectnameSG1);  //unlock SG
                                                   
          for(i=0;i<2;i++)
{
//printf("\nsleeping\n");
usleep(60000);
//printf("\nGot up!\n");
}
  goto loop22;
       }
       else
       {
              //check to increase assigned SUs
               printf("\nNeed to increase assigned SU");
               attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }

 if(preinstantiable == 1)
  {
	attributeNames[0]="saAmfSGNumPrefInserviceSUs";
  	attributeNames[1]=NULL;
  	error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noinsSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGNumPrefInsericeSUs of %s SG are %d\n",objectnameSG1.value,noinsSUs);
					               	                }
								  }
  }
  else
  {
       noinsSUs =  num_SUofSG(immHandle,objectnameSG1);  
  }


       if(noinsSUs > noassgSUs)
       {
           //printf("\n Create ccb to increase the no of Assigned SU");
           printf("Create a CCB to increase saAmfSGNumPrefAssignedSUs of %s SG\n",objectnameSG1.value);
 									 	     ////////////////////////create ccb to increase saAmfSGMaxActiveSIsperSU
  								         	     swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
 								        	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 								        	     noassgSUs = noassgSUs+1; 
		    					                	     strcpy(nameccbattribute,"saAmfSGNumPrefAssignedSUs=");
                         							     sprintf(noassg, "%d",noassgSUs);
               									     strcat(nameccbattribute,noassg);
             									     // printf("\n%s",nameccbattribute);
                        							     s= objectnameSG1.value ;
                        							     //printf("\n%s\n",s);
                        							     objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 										     objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
										     objectNames[objectNames_len++] = NULL;
 										     objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       							//??
      		       
		      							 	      error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      						        	      if (error != SA_AIS_OK) {
		       										printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		      								 		rc = EXIT_FAILURE;
		     							 	      }
		     				         			      if (error == SA_AIS_OK)
             	    					                	      {   
			  
	 	      	  							 		rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                      								 		free(objectNamec);
                      
		       								 		if(rc==0)
		        							                {
		          	 									rc=1;
		      							        		}
        
	          						         	       }
	
										   error = saImmOmAdminOwnerFinalize(ownerHandle);
										   if (SA_AIS_OK != error) {
											printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
											rc = EXIT_FAILURE;
										//	goto done_om_finalize;
										}
								   		 //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
										swapsi(immHandle,argv,1,objectnameSG1);  //unlock SG

for(i=0;i<2;i++)
{
//printf("\nsleeping\n");
usleep(60000);
//printf("\nGot up!\n");
}
if(preinstantiable == 1)
  {
     buffer_checkker(immHandle,objectnameSG1,objectnameSI,0,argv);
  }
  else
  {
    buffer2_checkker(immHandle,objectnameSG1,objectnameSI,argv);
  }
                                                     goto loop22;
       }
           
      }
        
   }
   
   loop23:
   printf("%c[1;32m\nChecking  the saAmfSGMaxStandbySIsperSU\n",27);
	printf("%c[0m",27);
 	attributeNames[0]="saAmfSGMaxStandbySIsperSU";
  	attributeNames[1]=NULL;
  	error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of saAmfSGMaxStandbySIsperSU of %s SG are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
								 }
  attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }

///search number of SIs_start
no_SIs= 0;
//printf("\n1\n");


printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam1.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam1.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam1.searchOneAttr.attrValue = &objectnameSG1;
                                  
                                                                   

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope1,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam1, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			printf("error - saImmOmSearchInitialize_2 FAILED:%d",error);
                	   		 		 		        //printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							//flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
 //printf("\n5");
							   attributeNames[0] ="saAmfSIPrefStandbyAssignments";
							   attributeNames[1] =NULL;
							   error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   if (error != SA_AIS_OK) {
                	       					printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       					rc= EXIT_FAILURE;
							//	flag = 1;
                      				   	     }
       			       			 	  if (error == SA_AIS_OK)
   						  	 {
                                                               // printf("\n6");
								error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        			if (error !=SA_AIS_OK)
                                        			{
                                               				printf("\nerror:%d",error);
									rc= EXIT_FAILURE;
                                                        //		flag = 1; 
                                        		         }
                        	 				if (error ==SA_AIS_OK)
                                 				{
			                     		         //       printf("\n7");
					     				SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    				no_SIs1 = *((SaUint32T *)attrValues);
                                                              	  //printf("\nthe end\n",activeSis);
                                                               		// printf("Active assignments :%d",no_SIs1);
                		   	               	 		error = saImmOmAccessorFinalize(accessorHandle);
							        }
                                                               // printf("\n8");
       								no_SIs = no_SIs + no_SIs1;
                        		                 }//accesor initialize
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
///search number of SIs_end
//////////// Decrease MaxActive SI per SU 
printf("Checking ..\n");




 
 
                                                          
                                               	

 	
        if((maxstdSIsperSU)*noassgSUs >= no_SIs)
        {
             printf("\n Already assigned");
             rc = 1;
        }
        else
        {
               printf("\nWe increase number of maxStandbySIperSU\n");
               // printf("%c[1;31m\nNeed to increase saAmfSGMaxActiveSIsperSU\n",27);
printf("%c[0m",27);
strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
printf("The SI .. type is %s \n",objectsity.value);
//printf("\n1\n");
  	    attributeNames[0]="saAmfCtDefNumMaxStandbyCSIs";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	//printf("\n2\n");
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{       //printf("\n3\n");
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {       // printf("\n4\n");
	                                                              /*	
                                                                        	printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefActiveNumCSIs = *((SaUint32T *)attrValue);
                                                                        **/

										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxctSIsperSUstd = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
										printf("The saAmfCtDefNumMaxStandbyCSIs : %d \n", maxctSIsperSUstd );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe cSI is %d \n",maxactSIsperSU);
					               	                }
								 }

 attributeNames[0]="saAmfSGMaxStandbySIsperSU";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxstdSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The number of saAmfSGMaxStandbySIsperSU of %s SG are %d\n",objectnameSG1.value,maxstdSIsperSU);
					               	                }
								 }
       if(maxctSIsperSUstd > maxstdSIsperSU )
       {
               //create a ccb to increase the maxactiveSIsperSU
                printf("Create a CCB to increase saAmfSGMaxStandbySIsperSU of %s SG\n",objectnameSG1.value);
 									 	     ////////////////////////create ccb to increase saAmfSGMaxActiveSIsperSU
  								         	     swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
 								        	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 								        	     noassgSUs = maxstdSIsperSU+1; 
		    					                	     strcpy(nameccbattribute,"saAmfSGMaxStandbySIsperSU=");
                         							     sprintf(noassg, "%d",noassgSUs);
               									     strcat(nameccbattribute,noassg);
             									     // printf("\n%s",nameccbattribute);
                        							     s= objectnameSG1.value ;
                        							     //printf("\n%s\n",s);
                        							     objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 										     objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
										     objectNames[objectNames_len++] = NULL;
 										     objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       							//??
      		       
		      							 	      error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      						        	      if (error != SA_AIS_OK) {
		       										printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		      								 		rc = EXIT_FAILURE;
		     							 	      }
		     				         			      if (error == SA_AIS_OK)
             	    					                	      {   
			  
	 	      	  							 		rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                      								 		free(objectNamec);
                      
		       								 		if(rc==0)
		        							                {
		          	 									rc=1;
		      							        		}
        
	          						         	       }
	
										   error = saImmOmAdminOwnerFinalize(ownerHandle);
										   if (SA_AIS_OK != error) {
											printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
											rc = EXIT_FAILURE;
										//	goto done_om_finalize;
										}
								   		 //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
										swapsi(immHandle,argv,1,objectnameSG1);  //unlock SG
                                                   
          for(i=0;i<2;i++)
{
//printf("\nsleeping\n");
usleep(60000);
//printf("\nGot up!\n");
}
  goto loop23;
       }
       else
       {
              //check to increase assigned SUs
               printf("\nNeed to increase assigned SU");
               attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }
if(preinstantiable == 1)
  {

attributeNames[0]="saAmfSGNumPrefInserviceSUs";
  attributeNames[1]=NULL;
  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noinsSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGNumPrefInsericeSUs of %s SG are %d\n",objectnameSG1.value,noinsSUs);
					               	                }
								  }
}
else
{
       noinsSUs =  num_SUofSG(immHandle,objectnameSG1);  
}


       if(noinsSUs > noassgSUs)
       {
           //printf("\n Create ccb to increase the no of Assigned SU");
           printf("Create a CCB to increase saAmfSGNumPrefAssignedSUs of %s SG\n",objectnameSG1.value);
 									 	     ////////////////////////create ccb to increase saAmfSGMaxActiveSIsperSU
  								         	     swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
 								        	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 								        	     noassgSUs = noassgSUs+1; 
		    					                	     strcpy(nameccbattribute,"saAmfSGNumPrefAssignedSUs=");
                         							     sprintf(noassg, "%d",noassgSUs);
               									     strcat(nameccbattribute,noassg);
             									     // printf("\n%s",nameccbattribute);
                        							     s= objectnameSG1.value ;
                        							     //printf("\n%s\n",s);
                        							     objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 										     objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
										     objectNames[objectNames_len++] = NULL;
 										     objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       							//??
      		       
		      							 	      error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      						        	      if (error != SA_AIS_OK) {
		       										printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		      								 		rc = EXIT_FAILURE;
		     							 	      }
		     				         			      if (error == SA_AIS_OK)
             	    					                	      {   
			  
	 	      	  							 		rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                      								 		free(objectNamec);
                      
		       								 		if(rc==0)
		        							                {
		          	 									rc=1;
		      							        		}
        
	          						         	       }
	
										   error = saImmOmAdminOwnerFinalize(ownerHandle);
										   if (SA_AIS_OK != error) {
											printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
											rc = EXIT_FAILURE;
										//	goto done_om_finalize;
										}
								   		 //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
										swapsi(immHandle,argv,1,objectnameSG1);  //unlock SG
for(i=0;i<2;i++)
{
//printf("\nsleeping\n");
usleep(60000);
//printf("\nGot up!\n");
}
if(preinstantiable == 1)
  {
     buffer_checkker(immHandle,objectnameSG1,objectnameSI,0,argv);
  }
  else
  {
    buffer2_checkker(immHandle,objectnameSG1,objectnameSI,argv);
  }
                                                     goto loop23;
       }
           
      }
        

        }
  return rc;
}
//Nway mutilple SI increase_end
//Nway single SI decrease_statrt

int single_SIDecreaseNway(SaImmHandleT immHandle,SaNameT objectnameSG1,char *argv[],SaNameT objectnameSI,int preinstantiable)
{

  //printf("\nI got a call");
  printf("%c[1;32m\n***********************************************************************************",27);
printf("\n**************************************RM Adjustor**********************************");
printf("%c[0m",27);
   SaImmAttrNameT  attributeNames[2] ={"saAmfSIPrefStandbyAssignments",NULL};
SaAisErrorT error;

SaImmScopeT scope = SA_IMM_SUBTREE;	/* default search scope */
int flag1=1,no_SUs=1,i;
SaUint32T noassgSI,assignmentactF;
 int objectNames_len = 1;
SaImmAccessorHandleT accessorHandle;
SaImmSearchHandleT searchHandle;
SaNameT objectName,objectsity,objectname,objectname1;
SaImmAttrValuesT_2 **attributes,**attributes1;
 SaImmAdminOwnerHandleT ownerHandle;
 SaNameT **objectNames = NULL;
SaNameT *objectNamec;
SaNameT *objectName1c;
SaNameT objtnamesgtype;
 SaNameT objectnamevalidsutype1;
int maxstdSIsperSU,noassgactSUs,no_SIs,DefStandbyNumCSIs,noSIassgcurractSU,flag=0;
char *s;
int l=0;
SaInt8T namevalidsutype[256];
 SaImmSearchParametersT_2 searchParam;
int no_of_SIs=0,curr_assg_SUs=0;

char nameccbattribute[256],noassg[256];

SaUint32T noSIassg,noassgSUs,noinserviceSUs,maxactSIsperSU,activeSIs,activeSIssum,maxactSIsperSUt,DefActiveNumCSIs;
int rc = EXIT_SUCCESS;
SaImmAdminOwnerNameT adminOwnerName = create_adminOwnerName(basename(argv[0]));
loop24:

								attributeNames[0]="saAmfSGNumPrefAssignedSUs";
 								attributeNames[1]=NULL;
  								error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe saAmfSGNumPrefAssignedSUs of %s SG is %d\n",objectnameSG1.value,noassgSUs);
					               	                }
								  }

searchParam.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    			searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								   			searchParam.searchOneAttr.attrValue = &objectnameSG1;

								    			error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					       	                    			SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 			   			if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					  		 }
                           				           		if (SA_AIS_OK == error)
        		    					  		 {
		         								//printf("\n\nWe got the search handle \n");
                                			               			 activeSIs=0;
            										do {                
                   										error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
												if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
												printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 					rc=EXIT_FAILURE;
					  							flag=1;
						    					   }
                                        


                                       		    						if(error == SA_AIS_ERR_NOT_EXIST )
               					    						{
		  	  									 	if(flag1==1)
                  	   			         						{ 
                     												//arraySU[counter]==NULL;
                     												//printf("\nobject does not exist"); 
        	    	   			       						 	}
		     	   										else
		     	  										{ 
                             											rc=EXIT_FAILURE;
                             											flag=1;
 		          										}        
						  					 	}
                
						 						if (error == SA_AIS_OK)
                  				  						{
 
                              
							   						attributeNames[0] ="saAmfSIPrefStandbyAssignments";
							  				        	attributeNames[1] =NULL;
							   						error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   						if (error != SA_AIS_OK) {
                	       										printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       										rc= EXIT_FAILURE;
													flag = 1;
                      				   	     						}
       			       			 	  						if (error == SA_AIS_OK)
   						  							{
														error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        									if (error !=SA_AIS_OK)
                                        									{
                                               										printf("\nerror18:%d",error);
															rc= EXIT_FAILURE;
                                                        								flag = 1; 
                                        		        					 	}
                        	 									 	if (error ==SA_AIS_OK)
                                 									 	{
			                     		
					     										SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    										activeSIs = *((SaUint32T *)attrValues);
                                                              	  
                		   	               	 								error = saImmOmAccessorFinalize(accessorHandle);
							       							}
       													        if(noassgSUs > (activeSIs+1))
                                         									{
                                                                                                                    
                                                                                                                }
													        else
                                                                                                                {
                                                                                                                       no_SUs=0;
                                                                                                                }
 														
                        		                 					        }//accesor initialize
                                               	

                                         						     }//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST && no_SUs==1);
                   								 //printf(" total number of SIs is %d\n",activeSIssum);
										//"Calculating the total required capacity i.e summing up the SI assignments");
                                                                              error = saImmOmSearchFinalize(searchHandle);
  									     }//search initialize
       										 ////////////										
                                                                               if(no_SUs==1)
                                                                               {
                                                                                       printf("\nThe number of SUs are greater than the nuber of standby assignments of each SI");
                                                                                      
attributeNames[0]="saAmfSGMaxActiveSIsperSU";
   attributeNames[1]=NULL;
  	      error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe number of saAmfSGMaxActiveSIsperSU of %s SG are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  		attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  		attributeNames[1]=NULL;
  		error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgactSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgactSUs);
					               	                }
								  }

		///search number of SIs_start
  		printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					no_SIs= no_SIs + 1;
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
printf("%c[1;32m\nCheck to decrease number of active capacity\n",27);
printf("%c[0m",27);
if((noassgactSUs-1) * maxactSIsperSU >= no_SIs )
{
    attributeNames[0]="saAmfSGMaxStandbySIsperSU";
   attributeNames[1]=NULL;
  	      error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe number of saAmfSGMaxStandbySIsperSU of %s SG are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  		attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  		attributeNames[1]=NULL;
  		error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgactSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgactSUs);
					               	                }
								  }

		///search number of SIs_start
  		printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {   
                                                                                        no_of_SIs=0;             
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
                                                                                              attributeNames[0] ="saAmfSIPrefStandbyAssignments";
							  				        	attributeNames[1] =NULL;
							   						error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   						if (error != SA_AIS_OK) {
                	       										printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       										rc= EXIT_FAILURE;
													flag = 1;
                      				   	     						}
       			       			 	  						if (error == SA_AIS_OK)
   						  							{
														error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        									if (error !=SA_AIS_OK)
                                        									{
                                               										printf("\nerror18:%d",error);
															rc= EXIT_FAILURE;
                                                        								flag = 1; 
                                        		        					 	}
                        	 									 	if (error ==SA_AIS_OK)
                                 									 	{
			                     		
					     										SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    										no_of_SIs = *((SaUint32T *)attrValues);
                                                              	  
                		   	               	 								error = saImmOmAccessorFinalize(accessorHandle);
							   					//printf("**** total number of SIs is %d\n",no_of_SIs);
                                                                                                //printf("**?? total number of SIs is %d\n",no_SIs);
                                               	                                                                 }
                                                                                                                }
                                         						}//search next
                                                                                    no_SIs= no_SIs + no_of_SIs;
                                                                                 //printf("** total number of SIs is %d\n",no_SIs);
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
printf("%c[1;32m\nCheck to decrease number of standby capacity\n",27);
printf("%c[0m",27);
    //printf("\nCheck standby capacity");

/////////check condition if((noassgstdtSUs-1) * maxstdSIsperSU >= no_SIs )
if( (noassgactSUs-1) * maxactSIsperSU >= no_SIs)
{
  ///decrease assigned SUs
   printf("Create a CCB to decrease saAmfSGNumPrefAssignedSUs of %s SG\n",objectnameSG1.value);
 									 	     ////////////////////////create ccb to increase saAmfSGMaxActiveSIsperSU
  								         	     swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
 								        	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 								        	     noassgSUs = noassgactSUs-1; 
		    					                	     strcpy(nameccbattribute,"saAmfSGNumPrefAssignedSUs=");
                         							     sprintf(noassg, "%d",noassgSUs);
               									     strcat(nameccbattribute,noassg);
             									     // printf("\n%s",nameccbattribute);
                        							     s= objectnameSG1.value ;
                        							     //printf("\n%s\n",s);
                        							     objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 										     objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
										     objectNames[objectNames_len++] = NULL;
 										     objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       							//??
      		       
		      							 	      error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      						        	      if (error != SA_AIS_OK) {
		       										printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		      								 		rc = EXIT_FAILURE;
		     							 	      }
		     				         			      if (error == SA_AIS_OK)
             	    					                	      {   
			  
	 	      	  							 		rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                      								 		free(objectNamec);
                      
		       								 		if(rc==0)
		        							                {
		          	 									rc=1;
		      							        		}
        
	          						         	       }
	
										   error = saImmOmAdminOwnerFinalize(ownerHandle);
										   if (SA_AIS_OK != error) {
											printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
											rc = EXIT_FAILURE;
										//	goto done_om_finalize;
										}
								   		 //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
										swapsi(immHandle,argv,1,objectnameSG1);  //unlock SG
/////////////////////////////////////////////////buffer_checker_dec
buffer_checkker_decrease(immHandle,objectnameSG1,objectnameSI,0,argv,preinstantiable);
////////////////////////////////////////////////buffer_checkker_dec

printf("%c[1;32m\nCheck if further capacity can be reduced\n",27);
printf("%c[0m",27);
 attributeNames[0]="saAmfSGMaxStandbySIsperSU";
   attributeNames[1]=NULL;
  	      error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("\nThe number of saAmfSGMaxStandbySIsperSU of %s SG are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
								 }
  		attributeNames[0]="saAmfSGNumPrefAssignedSUs";
  		attributeNames[1]=NULL;
  		error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noassgactSUs = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGNumPrefAssignedSUs of %s SG are %d\n",objectnameSG1.value,noassgactSUs);
					               	                }
								  }

		///search number of SIs_start
  		printf("Calculating the total required capacity i.e Total number of SIs");
        							    searchParam.searchOneAttr.attrName = "saAmfSIProtectedbySG";
 								    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam.searchOneAttr.attrValue = &objectnameSG1;

								    error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			                no_SIs=0;
            								do {   
                                                                                        no_of_SIs=0;             
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						//flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									
		     	  									      //printf("\nhi\n");
                             										
                             										//flag=1;
 		          									       
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
                                                                                              attributeNames[0] ="saAmfSIPrefStandbyAssignments";
							  				        	attributeNames[1] =NULL;
							   						error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   						if (error != SA_AIS_OK) {
                	       										printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       										rc= EXIT_FAILURE;
													flag = 1;
                      				   	     						}
       			       			 	  						if (error == SA_AIS_OK)
   						  							{
														error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        									if (error !=SA_AIS_OK)
                                        									{
                                               										printf("\nerror18:%d",error);
															rc= EXIT_FAILURE;
                                                        								flag = 1; 
                                        		        					 	}
                        	 									 	if (error ==SA_AIS_OK)
                                 									 	{
			                     		
					     										SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    										no_of_SIs = *((SaUint32T *)attrValues);
                                                              	  
                		   	               	 								error = saImmOmAccessorFinalize(accessorHandle);
							   					//printf("**** total number of SIs is %d\n",no_of_SIs);
                                                                                                //printf("**?? total number of SIs is %d\n",no_SIs);
                                               	                                                                 }
                                                                                                                }
                                         						}//search next
                                                                                    no_SIs= no_SIs + no_of_SIs;
                                                                                 //printf("** total number of SIs is %d\n",no_SIs);
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST);
                   								 printf(" total number of SIs is %d\n",no_SIs);
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
		///search number of SIs_end
printf("%c[1;32m\nCheck to decrease number of standby capacity\n",27);
printf("%c[0m",27);
    //printf("\nCheck standby capacity");

/////////check condition if((noassgstdtSUs-1) * maxstdSIsperSU >= no_SIs )
if( (noassgactSUs-1) * maxactSIsperSU > no_SIs)
{
   printf("\nFurther capacity can be reduced");
   goto loop24;
}
else
{
  printf("\nNo Further capacity can be reduced");
}
                                                          
}
else
{



///////////////////////////////////////////////////
//printf("\n Increase maxActiveSIsperSU");
   printf("%c[1;32m\nCheck to increase saAmfSGMaxStandbySIsperSU\n",27);
                    printf("%c[0m",27);



                     



//////////////////////////////////////////////find maxstandbySIperSU///////////////////////////
strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
printf("The SI .. type is %s \n",objectsity.value);
  	    attributeNames[0]="saAmfCtDefNumMaxStandbyCSIs";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefStandbyNumCSIs = *((SaUint32T *)attrValue);

										
										printf("The saAmfCtDefNumMaxStandbyCSIs : %d \n", DefStandbyNumCSIs );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe cSI is %d \n",DefStandbyNumCSIs);
					               	                }
								 }

        ////get the maxactiveSIperSU attribute
         attributeNames[0]="saAmfSGMaxStandbySIsperSU";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxStandbySIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
					
   								 }
/////////////////////////////////////////////find maxstandbySIperSU////////////////////////////
if(DefStandbyNumCSIs > maxactSIsperSU)
{
  printf("Checking if for any SU in the SG saAmfSGMaxStandbySIsperSU is equal to current number of SI Assignment\n");
                                                                        attributeNames[0]="saAmfSGType";
  								        attributeNames[1]=NULL;
                                                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 									if (error != SA_AIS_OK) {
                							printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                							rc= EXIT_FAILURE;
                      							}
       									if (error == SA_AIS_OK)
   									{
  										//     printf("\nWe got the accessor handle to access SG\n");
         
      											error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);
											if (error !=SA_AIS_OK)
											{
												 printf("\n%d",error);
         											 rc= EXIT_FAILURE;
											}


											if (error ==SA_AIS_OK)
											{
												//printf("\nsaImmOmAccessorGet_2 function returned the SG object and attribute\n");

												SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
												SaNameT *objtnamevalidsutype = (SaNameT *)attrValue;


 												strncpy(objtnamesgtype.value,objtnamevalidsutype->value, SA_MAX_NAME_LENGTH);
 												objtnamesgtype.length=strlen(objtnamesgtype.value);


											}
											error = saImmOmAccessorFinalize(accessorHandle);
                                                                      //    printf("\nThe saAmfSGType of %s SG  is %s\n",objectnameSG1.value,objtnamesgtype.value);
									}
									//>>>
                							attributeNames[0]="saAmfSgtValidSuTypes";
  								        attributeNames[1]=NULL;
 									error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 									if (error != SA_AIS_OK) {
                									printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                									rc= EXIT_FAILURE;
                      								}
       									if (error == SA_AIS_OK)
   									{
       										//printf("\nWe got the accessor handle to access SG\n");
  												// } 

										error= saImmOmAccessorGet_2(accessorHandle, &objtnamesgtype, attributeNames, &attributes);

										if(error == SA_AIS_OK)
										{
 											//printf("\nok ok");
 											SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
 											SaNameT *objtnamevalidsutype = (SaNameT *)attrValue;
 											strcpy(namevalidsutype,objtnamevalidsutype->value);
 											l=objtnamevalidsutype->length;
 											objectnamevalidsutype1.length=l;
//printf("\n\n%u\n\n",x);

											strncpy((char *)objectnamevalidsutype1.value, namevalidsutype, SA_MAX_NAME_LENGTH);

										}
									error = saImmOmAccessorFinalize(accessorHandle);
        								//printf("\nThe saAmfSgtValidSuTypes of %s SG  is %s\n",objectnameSG1.value,objectnamevalidsutype1.value);
                                                                         }
                                                                          searchParam.searchOneAttr.attrName = "saAmfSUType";
 								    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam.searchOneAttr.attrValue = &objectnamevalidsutype1;

								    error = saImmOmSearchInitialize_2(immHandle,&objectnameSG1 , scope,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							//flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			               curr_assg_SUs=0,activeSIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									 if(flag1==1)
                  	   			         					{ 
                     											//arraySU[counter]==NULL;
                     											//printf("\nobject does not exist"); 
        	    	   			       						 }
		     	   									else
		     	  									{ 
                             										rc=EXIT_FAILURE;
                             										//flag=1;
 		          									}        
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					attributeNames[0] ="saAmfSUNumCurrStandbySIs";
							  				        attributeNames[1] =NULL;
							   					error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   					if (error != SA_AIS_OK) {
                	       										printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       										rc= EXIT_FAILURE;
													//flag = 1;
                      				   	     					}
       			       			 	  					if (error == SA_AIS_OK)
   						  						{
													error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        								if (error !=SA_AIS_OK)
                                        								{
                                               									printf("\nerror:%d",error);
														rc= EXIT_FAILURE;
                                                        							flag = 1; 
                                        		        					 }
                        	 									 if (error ==SA_AIS_OK)
                                 									 {
			                     		                                                        //no_of_SIs++;
					     									SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    									activeSIs = *((SaUint32T *)attrValues);
                                                              	  
                		   	               	 							error = saImmOmAccessorFinalize(accessorHandle);
							       						 }
       													printf(" total number of SIs handled by SU %s is %d\n",objectName.value,activeSIs);
                                                                                                        if(activeSIs == maxactSIsperSU)
                                                                                                        {
                                                                                                             curr_assg_SUs =1;
                                                                                                        }
                        		                 					}//accesor initialize
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST && curr_assg_SUs != 1);
                   								 
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
									








//curr_assg_SUs=1;
                                                                           //  printf("\ncurr_assg_SUs:%d\n",curr_assg_SUs); 
                                                                             if(curr_assg_SUs==0)
                                                                             {
                                                                                 printf("\nElasticity action at Cluster Level");
                                                                                 rc=2;
                                                                             
                                                                            }
                                                                             else
                                                                             {
                                                                               printf("Checking if saAmfSGMaxStandbySIsperSU < total number of SIs:");
                                                                               printf("\n The total number of SIs is %d\n",no_SIs);
                                                                                  if(maxactSIsperSU<no_SIs )
                                                                                  {
                                                                                     printf("Increase the saAmfSGMaxStandbySIsperSU \n");
                                                                                     printf("Create a CCB to increase saAmfSGMaxActiveSIsperSU of %s SG\n",objectnameSG1.value);
 									 	     ////////////////////////create ccb to increase saAmfSGMaxActiveSIsperSU
  								         	     swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
 								        	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 								        	     noassgSUs = maxactSIsperSU+1; 
		    					                	     strcpy(nameccbattribute,"saAmfSGMaxStandbySIsperSU=");
                         							     sprintf(noassg, "%d",noassgSUs);
               									     strcat(nameccbattribute,noassg);
             									     // printf("\n%s",nameccbattribute);
                        							     s= objectnameSG1.value ;
                        							     //printf("\n%s\n",s);
                        							     objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 										     objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
										     objectNames[objectNames_len++] = NULL;
 										     objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       							//??
      		       
		      							 	      error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      						        	      if (error != SA_AIS_OK) {
		       										printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		      								 		rc = EXIT_FAILURE;
		     							 	      }
		     				         			      if (error == SA_AIS_OK)
             	    					                	      {   
			  
	 	      	  							 		rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                      								 		free(objectNamec);
                      
		       								 		if(rc==0)
		        							                {
		          	 									rc=1;
		      							        		}
        
	          						         	       }
	
										   error = saImmOmAdminOwnerFinalize(ownerHandle);
										   if (SA_AIS_OK != error) {
											printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
											rc = EXIT_FAILURE;
										//	goto done_om_finalize;
										}
								   		 //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
										swapsi(immHandle,argv,1,objectnameSG1);  //unlock SG
                                                                               goto loop24;









    

 }      
} 
}
///////////////////////////////////////////////////




}
}
else
{
   //printf("\n Increase maxActiveSIsperSU");
   printf("%c[1;32m\nCheck to increase saAmfSGMaxActiveSIsperSU\n",27);
                    printf("%c[0m",27);



                     



//////////////////////////////////////////////find maxstandbySIperSU///////////////////////////
strncpy((char *)objectsity.value, "safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2", SA_MAX_NAME_LENGTH);
objectsity.length= strlen("safSupportedCsType=safVersion=1\\,safCSType=AmfDemo2,safVersion=1,safCompType=AmfDemo2");
printf("The SI .. type is %s \n",objectsity.value);
  	    attributeNames[0]="saAmfCtDefNumMaxActiveCSIs";
 		    attributeNames[1]=NULL;
  	            error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	          
									error= saImmOmAccessorGet_2(accessorHandle, &objectsity, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										DefStandbyNumCSIs = *((SaUint32T *)attrValue);

										
										printf("The saAmfCtDefNumMaxActiveCSIs : %d \n", DefStandbyNumCSIs );


               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("\nThe cSI is %d \n",DefStandbyNumCSIs);
					               	                }
								 }

        ////get the maxactiveSIperSU attribute
         attributeNames[0]="saAmfSGMaxActiveSIsperSU";
 	 attributeNames[1]=NULL;
 	 error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										maxactSIsperSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						printf("The saAmfSGMaxActiveSIsperSU of %s SG  are %d\n",objectnameSG1.value,maxactSIsperSU);
					               	                }
					
   								 }
/////////////////////////////////////////////find maxstandbySIperSU////////////////////////////
if(DefStandbyNumCSIs > maxactSIsperSU)
{
  printf("Checking if for any SU in the SG saAmfSGMaxActiveSIsperSU is equal to current number of SI Assignment\n");
                                                                        attributeNames[0]="saAmfSGType";
  								        attributeNames[1]=NULL;
                                                                        error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 									if (error != SA_AIS_OK) {
                							printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                							rc= EXIT_FAILURE;
                      							}
       									if (error == SA_AIS_OK)
   									{
  										//     printf("\nWe got the accessor handle to access SG\n");
         
      											error= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNames, &attributes);
											if (error !=SA_AIS_OK)
											{
												 printf("\n%d",error);
         											 rc= EXIT_FAILURE;
											}


											if (error ==SA_AIS_OK)
											{
												//printf("\nsaImmOmAccessorGet_2 function returned the SG object and attribute\n");

												SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
												SaNameT *objtnamevalidsutype = (SaNameT *)attrValue;


 												strncpy(objtnamesgtype.value,objtnamevalidsutype->value, SA_MAX_NAME_LENGTH);
 												objtnamesgtype.length=strlen(objtnamesgtype.value);


											}
											error = saImmOmAccessorFinalize(accessorHandle);
                                                                      //    printf("\nThe saAmfSGType of %s SG  is %s\n",objectnameSG1.value,objtnamesgtype.value);
									}
									//>>>
                							attributeNames[0]="saAmfSgtValidSuTypes";
  								        attributeNames[1]=NULL;
 									error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 									if (error != SA_AIS_OK) {
                									printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                									rc= EXIT_FAILURE;
                      								}
       									if (error == SA_AIS_OK)
   									{
       										//printf("\nWe got the accessor handle to access SG\n");
  												// } 

										error= saImmOmAccessorGet_2(accessorHandle, &objtnamesgtype, attributeNames, &attributes);

										if(error == SA_AIS_OK)
										{
 											//printf("\nok ok");
 											SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
 											SaNameT *objtnamevalidsutype = (SaNameT *)attrValue;
 											strcpy(namevalidsutype,objtnamevalidsutype->value);
 											l=objtnamevalidsutype->length;
 											objectnamevalidsutype1.length=l;
//printf("\n\n%u\n\n",x);

											strncpy((char *)objectnamevalidsutype1.value, namevalidsutype, SA_MAX_NAME_LENGTH);

										}
									error = saImmOmAccessorFinalize(accessorHandle);
        								//printf("\nThe saAmfSgtValidSuTypes of %s SG  is %s\n",objectnameSG1.value,objectnamevalidsutype1.value);
                                                                         }
                                                                          searchParam.searchOneAttr.attrName = "saAmfSUType";
 								    searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
 								    searchParam.searchOneAttr.attrValue = &objectnamevalidsutype1;

								    error = saImmOmSearchInitialize_2(immHandle,&objectnameSG1 , scope,
					       	                    SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,&searchHandle);
					 			   if (SA_AIS_OK != error) {
			  		 	         			//printf("error - saImmOmSearchInitialize_2 FAILED");
                	   		 		 		        printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                	   			        		        rc=EXIT_FAILURE;
                	   							//flag=1;
			     					   }
                           				           if (SA_AIS_OK == error)
        		    					   {
		         						//printf("\n\nWe got the search handle \n");
                                			               curr_assg_SUs=0,activeSIs=0;
            								do {                
                   									error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
											if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       
											printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                      			 		 				rc=EXIT_FAILURE;
					  						flag=1;
						    					}
                                        


                                       		    					if(error == SA_AIS_ERR_NOT_EXIST )
               					    					{
		  	  									 if(flag1==1)
                  	   			         					{ 
                     											//arraySU[counter]==NULL;
                     											//printf("\nobject does not exist"); 
        	    	   			       						 }
		     	   									else
		     	  									{ 
                             										rc=EXIT_FAILURE;
                             										//flag=1;
 		          									}        
						  					 }
                
						 					if (error == SA_AIS_OK)
                  				  					{
 
                              
							   					attributeNames[0] ="saAmfSUNumCurrActiveSIs";
							  				        attributeNames[1] =NULL;
							   					error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			 	   					if (error != SA_AIS_OK) {
                	       										printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       										rc= EXIT_FAILURE;
													//flag = 1;
                      				   	     					}
       			       			 	  					if (error == SA_AIS_OK)
   						  						{
													error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);
                                        								if (error !=SA_AIS_OK)
                                        								{
                                               									printf("\nerror18:%d",error);
														rc= EXIT_FAILURE;
                                                        							flag = 1; 
                                        		        					 }
                        	 									 if (error ==SA_AIS_OK)
                                 									 {
			                     		                                                        //no_of_SIs++;
					     									SaImmAttrValueT *attrValues = attributes[0]->attrValues[0];
					    									activeSIs = *((SaUint32T *)attrValues);
                                                              	  
                		   	               	 							error = saImmOmAccessorFinalize(accessorHandle);
							       						 }
       													printf(" total number of SIs handled by SU %s is %d\n",objectName.value,activeSIs);
                                                                                                        if(activeSIs == maxactSIsperSU)
                                                                                                        {
                                                                                                             curr_assg_SUs =1;
                                                                                                        }
                        		                 					}//accesor initialize
                                               	

                                         						}//search next
                                       
                                   						 }while (error != SA_AIS_ERR_NOT_EXIST && curr_assg_SUs != 1);
                   								 
										//"Calculating the total required capacity i.e summing up the SI assignments");
  									     }//search initialize
									








//curr_assg_SUs=1;
                                                                           //  printf("\ncurr_assg_SUs:%d\n",curr_assg_SUs); 
                                                                             if(curr_assg_SUs==0)
                                                                             {
                                                                                 printf("\nElasticity action at Cluster Level");
                                                                                 rc=2;
                                                                             
                                                                            }
                                                                             else
                                                                             {
                                                                               printf("Checking if maxactSIsperSU < total number of SIs:");
                                                                               printf("\n The total number of SIs is %d\n",no_SIs);
                                                                                  if(maxactSIsperSU<no_SIs )
                                                                                  {
                                                                                     printf("Increase the saAmfSGMaxActiveSIsperSU \n");
                                                                                     printf("Create a CCB to increase saAmfSGMaxActiveSIsperSU of %s SG\n",objectnameSG1.value);
 									 	     ////////////////////////create ccb to increase saAmfSGMaxActiveSIsperSU
  								         	     swapsi(immHandle,argv,2,objectnameSG1);  //lock SG
 								        	     //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
 								        	     noassgSUs = maxactSIsperSU+1; 
		    					                	     strcpy(nameccbattribute,"saAmfSGMaxActiveSIsperSU=");
                         							     sprintf(noassg, "%d",noassgSUs);
               									     strcat(nameccbattribute,noassg);
             									     // printf("\n%s",nameccbattribute);
                        							     s= objectnameSG1.value ;
                        							     //printf("\n%s\n",s);
                        							     objectNames = realloc(objectNames, (objectNames_len + 1) * sizeof(SaNameT*));
 										     objectNamec = objectNames[0] = malloc(sizeof(SaNameT));
										     objectNames[objectNames_len++] = NULL;
 										     objectNamec->length = snprintf((char*)objectNamec->value, SA_MAX_NAME_LENGTH, "%s", s);
                       							//??
      		       
		      							 	      error = saImmOmAdminOwnerInitialize(immHandle, adminOwnerName, SA_TRUE, &ownerHandle);
		       
		      						        	      if (error != SA_AIS_OK) {
		       										printf("error9 - saImmOmAdminOwnerInitialize FAILED: %d\n", error);
		      								 		rc = EXIT_FAILURE;
		     							 	      }
		     				         			      if (error == SA_AIS_OK)
             	    					                	      {   
			  
	 	      	  							 		rc = object_modify((const SaNameT **)objectNames, ownerHandle,nameccbattribute);
                      								 		free(objectNamec);
                      
		       								 		if(rc==0)
		        							                {
		          	 									rc=1;
		      							        		}
        
	          						         	       }
	
										   error = saImmOmAdminOwnerFinalize(ownerHandle);
										   if (SA_AIS_OK != error) {
											printf("error - saImmOmAdminOwnerFinalize FAILED: %d\n",error);
											rc = EXIT_FAILURE;
										//	goto done_om_finalize;
										}
								   		 //CREATE A CCB TO INCREASE saAmfSGNumPrefAssignedSUs
										swapsi(immHandle,argv,1,objectnameSG1);  //unlock SG
                                                                               goto loop24;









    

 }      
}   
} 
  else
  {
     printf("\nElasticity at Cluster level\n");
     rc = 2;
  }
        
}

                                                                               }					    			       											
 return rc; 
}
//Nway singlE SI decrease_end
//Nway singlE SI decrease_end


int is_preinstantiable(SaImmHandleT immHandle,SaNameT objectnameSG1)
{
  

  int noofSU;
    SaNameT rootName = { 0, "" };
    
  SaImmAttrNameT  attributeNames[1];
 strncpy((char *)rootName.value, objectnameSG1.value, SA_MAX_NAME_LENGTH);
 rootName.length = strlen((char *)rootName.value);
 
 //SaImmHandleT immHandle;
 SaImmSearchParametersT_2 searchParam;
 SaImmScopeT scope = SA_IMM_SUBLEVEL;	/* default search scope */
 SaImmScopeT scope1 = SA_IMM_SUBTREE;
 SaImmSearchHandleT searchHandle;
 SaImmAccessorHandleT accessorHandle;
 int rc = EXIT_SUCCESS;
 char string[100],string2[100],string3[100];
 char ch1;
 
// SaImmAttrNameT  attributeNames[] ={"safSISU",NULL};
 //const SaNameT rootName =  objectnameSI;
 SaAisErrorT error;
 SaNameT objectNames,objectName;
 SaImmAttrValuesT_2 **attributes,**attributes1;
 char * s3;
 int long ch;
 int len1,len2,len3,i,cntr1,cntr2,flag=0,noSIassg;
 char *buff;
 int found =0;

 // printf("\nOOOOOOOOOOOOO\n");
  searchParam.searchOneAttr.attrName = NULL;
searchParam.searchOneAttr.attrValue = NULL;

//printf("\n%s\n",rootName.value);
error = saImmOmSearchInitialize_2(immHandle, &rootName, scope1, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	
           //     printf("\nSearch Hnadle Obtained\n");
                rc=EXIT_FAILURE;
                
	

noofSU=0,found=0;
do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                {
                    //  printf("\nthe object searched %s",objectName.value);
                             
                               buff = objectName.value;
                                  s3 =strstr(buff,"safSu=");
                             
              
                      if(s3 == NULL)
                      {
                         //  printf("\nno");
                      }
                      else
                      {  
                        ch =buff - s3;
                      //   printf("\nTHe position %ld",ch); 
                        if(ch == 0)
                        {
                        				 attributeNames[0]="saAmfSUPreInstantiable";
 	 						attributeNames[1]=NULL;
 	 					error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
                       						if (error != SA_AIS_OK)
		   						{	
                							printf("error - saImmOmAccessorInitialize FAILED:%d\n",error);
                							rc= EXIT_FAILURE;
                    						}
       		   						if (error == SA_AIS_OK)
   	           						{       
                                                        	        
                      							//printf("\nWe got the accessor handle to access SU for rank !!\n");
                                                        	        //error17= saImmOmAccessorGet_2(accessorHandle15, &objectsuname, attributeNamesSUrank,   		
									//&attributesSUrank);     
									error= saImmOmAccessorGet_2(accessorHandle, &objectName, attributeNames, &attributes);  
                                                                        if (error !=SA_AIS_OK)
 									{
										printf("error - saImmOmAccessorGet_2 FAILED:%d\n",error);					
									} 
                                       					if (error ==SA_AIS_OK)
                                                                        {
	                                  //                            		printf("\nRank!!!");
										SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                    		//printf("%drrr\n",*((SaUint32T *)attrValuecrk));
										noofSU = *((SaUint32T *)attrValue);
               						   			error = saImmOmAccessorFinalize(accessorHandle);
                                                                                
                                   						//printf("the preinstaniability is %d\n",noofSU);
                                                                                if(noofSU ==1)
                                                                                {
                                                                                    printf("It is a SG with pre-instantiable SUs");
                                                                                }
                                                                                else
                                                                                {
                                                                                     printf("It is a SG with non-pre-instantiable SUs");
                                                                                }
                                                                           found=1;
					               	                }
					
   								 }
                        
			
                        }
                        
                      }
                }
       if(found==1)
       break;
      } while (error != SA_AIS_ERR_NOT_EXIST);
error = saImmOmSearchFinalize(searchHandle);

}                       
//printf("\nThe number of SUs of SG is %d",noofSU);   

   return noofSU;


  
}

///switch
int switchcall(SaNameT objectnameSI,SaNameT objectnameSG1,int elasticityT,int elasticityact,SaImmHandleT immHandle,char *argv[])
{
  //printf("\nI got a call");

  int result =0;
  SaNameT objectnameSG3;
  int redmodT;
  SaInt8T nameSG3[256];
  const SaImmAttrNameT  attributeNamesSGT[] ={"saAmfSgtRedundancyModel",NULL};
  const SaImmAttrNameT  attributeNamesSG[] ={"saAmfSGType",NULL};
  SaAisErrorT error,error3;
  SaImmAccessorHandleT accessorHandle;
  SaImmAttrValuesT_2 **attributesSGT,**attributesSG;
  int preinstantiable=1;


  error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
  if (error != SA_AIS_OK) {
                printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                //rc= EXIT_FAILURE;
                      }
       if (error == SA_AIS_OK)
   {
       //printf("\nWe got the accessor handle to access SG object!!\n\n\n");
       //error3= saImmOmAccessorGet_2(accessorHandle1, &objectnameSG1, attributeNamesSG, &attributesSG);
       error3= saImmOmAccessorGet_2(accessorHandle, &objectnameSG1, attributeNamesSG, &attributesSG);
       if(error3 == SA_AIS_OK)
       {      
//         	
         // printf("\nsaImmOmAccessorGet_2 function returned the SG object and attribute\n");


          SaImmAttrValueT *attrValue1 = attributesSG[0]->attrValues[0];
	  SaNameT *objectnameSG2 = (SaNameT *)attrValue1;
			printf("The protecting SG type is : %s (%u) \n", objectnameSG2->value,objectnameSG2->length);
	  strcpy(nameSG3,objectnameSG2->value);
	  strncpy((char *)objectnameSG3.value, nameSG3, SA_MAX_NAME_LENGTH);
	  objectnameSG3.length=strlen(objectnameSG3.value);
        }
//
error = saImmOmAccessorFinalize(accessorHandle);
//        This accesses object Sg type and its attribute

     }

preinstantiable = is_preinstantiable(immHandle,objectnameSG1);
error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 if (error != SA_AIS_OK) {
                printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                //rc= EXIT_FAILURE;
                      }
       if (error == SA_AIS_OK)
   {
       error3= saImmOmAccessorGet_2(accessorHandle, &objectnameSG3, attributeNamesSGT, &attributesSGT);
       if(error3 == SA_AIS_OK)
       {      
       
	SaImmAttrValueT *attrValue3 = attributesSGT[0]->attrValues[0];
	redmodT= *((SaUint32T *)attrValue3);

       }
      error = saImmOmAccessorFinalize(accessorHandle);


    }




switch(redmodT)
{
 case 1 : printf("and is of 2N Redundancy Model\n");
          printf("\nCalling respective RM Adjustor method...");
          printf("\nThe request is for :");

          switch(elasticityact)
             {
              case 1 : //It is Increase
                         switch(elasticityT)
                         {
                          case 1 : printf("Single SI increase in 2N SG \n");
                                   //result=single_SIincrease2n(immHandle,nameSI,objectnameSG1,argv);
				   result=single_SIincrease2n(immHandle,objectnameSG1,argv,objectnameSI);
                                   break;
                          default : printf("Elasticity Engine does not support this type of increase in 2N SG\n");
                                    break;
             
                         }
                       break;
              case 2 : //printf("It is Decrease\n");
                         switch(elasticityT)
                         {
                          case 1 : printf("Single SI decrease in 2N SG \n");
				   //result=single_SIdecrease2n(immHandle,nameSI,objectnameSG1,argv);
				   result=single_SIdecrease2n(immHandle,objectnameSG1,argv,objectnameSI);
				   //printf("\n%d",result);
                                   break;
                          default : printf("Elasticity Engine does not support this type of increase in 2N SG\n");
                                    break;
             
                         }   
                      break;   
            }
          break;

 case 2 : printf("It is N+M Redundancy Model\n");
          printf("The request is for :");
          switch(elasticityact)
             {
              case 1 :// printf("It is Increase\n");
                         switch(elasticityT)
                         {
                          case 1 : printf("Single SI increase in N+M SG \n");
                                   result=single_SIIncreaseNPM(immHandle,objectnameSG1,argv,objectnameSI,preinstantiable);
                                   break;
                          case 2 : printf("Multiple SI increase in N+M SG \n");
                                   result=multiple_SIIncreaseNPM(immHandle,objectnameSG1,argv,objectnameSI,preinstantiable);
                                   break;
                          default: printf("Elasticity Engine does not support this type of increase in N+M SG\n");
                                    break;
             
                         }
                        break;             
           
              case 2 : //printf("It is Decrease\n");
                         switch(elasticityT)
                         {
                          case 1 : printf("Single SI decrease in N+M SG \n");
                                   result=single_SIDecreaseNPM(immHandle,objectnameSG1,argv,objectnameSI,preinstantiable);
                                   break;
                          case 2 : printf("Multiple SI decrease in N+M SG \n");
                                   result=single_SIDecreaseNPM(immHandle,objectnameSG1,argv,objectnameSI,preinstantiable);
                                   break;
                          default : printf("Elasticity Engine does not support this type of decrease\n");
                                    break;
             
                         }    
                         break;  
          }
            break;

 case 3 : printf("It is N-way Redundancy Model\n");
          printf("The request is for :");
             switch(elasticityact)
             {
              case 1 ://printf("It is increase");
                      switch(elasticityT)
                      {                          
                          case 1 : printf("Single SI increase in N-way SG");
                                   result=single_SIIncreaseNway(immHandle,objectnameSG1,argv,objectnameSI,preinstantiable);
                                   break;
                          case 2 : printf("Multiple SI increase in N-way SG");
                                   result=multiple_SIIncreaseNway(immHandle,objectnameSG1,argv,objectnameSI,preinstantiable);
                                   break;
                          default: printf("Elasticity Engine does not support this type of increase in Nway SG\n");
                                    break;
             
                      }
                      break;
              case 2 : //printf("It is Decrease\n");
                         switch(elasticityT)
                         {
                          case 1 : printf("Single SI decrease in N-way SG\n");
                                   result=single_SIDecreaseNway(immHandle,objectnameSG1,argv,objectnameSI,preinstantiable);
                                   break;
                          case 2 : printf("Multiple SI decrease in N-way SG");
                                   result=single_SIDecreaseNway(immHandle,objectnameSG1,argv,objectnameSI,preinstantiable);
                                   break;
                          default : printf("Elasticity Engine does not support this type of decrease in Nway SG\n");
                                    break;
             
                         }   
                         break;   
          }
          break;

 case 4 : printf("and is of N-way Active Redundancy Model\n");
          printf("\nCalling respective RM Adjustor method...");
          printf("\nThe request is for :");
        
          switch(elasticityact)
          {
             case 1 : //printf("It is increase in Nway Active SG");
                       switch(elasticityT)
                         {
                          case 1 : printf("Single SI increase in N-way Active SG\n");
				   result=single_SIincreaseNwayAct(immHandle,objectnameSG1,objectnameSI,argv,preinstantiable);
                                   break;
                          case 2 : printf("Multiple SI increase in N-way Active SG\n");
				   result=multiple_SIincreaseNwayAct(immHandle,objectnameSG1,objectnameSI,argv,preinstantiable);
                                   break;
                          default: printf("Elasticity Engine does not support this type of increase in Nway Active SG\n");
                                    break;
             
                         }
                         break;
              case 2 : //printf("It is Decrease\n");
                         switch(elasticityT)
                         {
                          case 1 : printf("Single SI decrease in N-way Active SG");
				   result=single_SIdecreaseNwayAct(immHandle,objectnameSG1,objectnameSI,argv,elasticityT,preinstantiable);
                                   break;
                          case 2 : printf("Multiple SI decrease in N-way Active SG");
                                   result=single_SIdecreaseNwayAct(immHandle,objectnameSG1,objectnameSI,argv,elasticityT,preinstantiable);
                                   break;
                          default : printf("Elasticity Engine does not support this type of decrease in Nway Active SG\n");
                                    break;
             
                         }   
                         break;   
          }
          break;
 
 case 5: printf("It is No Redundancy Model\n");
         printf("The request is for :");
         switch(elasticityact)
         {
          case 1:// printf("Its is Increase in Nway Active SG\n");  
                  switch(elasticityT)
                         {
                          case 1 : printf("Single SI increase in No Redundancy SG \n");
                                   break;
                          
                          default: printf("Elasticity Engine does not support this type of increase in No redundancy\n");
                                    break;
             
                         }
             break;
              case 2 : //printf("It is Decrease\n");
                         switch(elasticityT)
                         {
                          case 1 : printf("Single SI decrease in No Redundancy SG \n");
                                   break;
                        
                          default : printf("Elasticity Engine does not support this type of decrease in No Redundancy SG\n");
                                    break;
             
                         }
          break;      
          }
         break;
 //printf("\n**************************************************************************************");
 


}

//result =2;
return result;

}//switch function

///switch

static void create_pid_file(const char *directory, const char *filename_prefix)
{
       // printf("\n3\n");
	FILE *fp;
	char path[256];

	snprintf(path, sizeof(path), "%s/%s.pid", directory, filename_prefix);
	

	fp = fopen(path, "w");
	if (fp == NULL)	{
		syslog(LOG_ERR, "fopen '%s' failed: %s", path, strerror(errno));
		exit(EXIT_FAILURE);
	}
	fprintf(fp, "%d\n", getpid());
	fclose(fp);
}


static void sigterm_handler(int sig)
{
  printf("\n4\n");
 //syslog(LOG_NOTICE, "caught term signal");
 exit(EXIT_SUCCESS);
}



int elasticity_engine_main(int argc, char argv[])
{
/*
//initscr();
//clear();
//system("clear");
//int choice=0;
int choice=0;
char str[100];
int res;
while(1)
{
printf("%c[1;32m\n1. Enter the Elastiity Tool\n", 27);
printf("2. Exit ...\n");
printf("Please Select a Number: ");
scanf(" %d", &choice);

printf("%c[0m", 27);
getchar();
switch(choice)
          {
             case 1:
		system("clear");
		break;
	case 2:
		exit(0);
}

//variable declariations
SaInt8T nameSI[256];
*/
SaInt8T nameSG[256];
SaInt8T nameSG3[256];
//SaNameT objectnameSI;
SaNameT objectnameSG1,objectnameSG3;
int redmodT;


int result =0;
/**
typedef enum{
increase =1,
decrease =2
}elasticityaction;

typedef enum{
singleSI =1,
multipleSI =2
}elasticitytype;


elasticityaction elasticityact;
elasticitytype elasticityT;
**/
//strncpy((char *)objectnameSI.value, nameSI, SA_MAX_NAME_LENGTH);


int i = 0, j;
SaAisErrorT error,error1,error3;
//SaImmHandleT immHandle;
SaImmOiHandleT immOiHandle;
//const SaImmOiCallbacksT_2 immOiCallbacks;
SaImmOiCallbacksT_2 imm_callbacks = {0};
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
char *env_comp_name;
env_comp_name = "magic1";

const SaImmOiImplementerNameT implementerName = (SaImmOiImplementerNameT) "@safAmfService";
const SaImmClassNameT className = "SaAmfSI";
SaSelectionObjectT selectionObject;
struct pollfd fds[1];

if((signal(SIGTERM, sigterm_handler)) == SIG_ERR) {
	syslog(LOG_ERR, "signal TERM failed: %d", errno);
        return EXIT_FAILURE;
}
//printf("\n1\n");
create_pid_file("/tmp", env_comp_name);
//printf("\n2\n");

/*Cancel certain signals that would kill us*/


pidfile = getenv("PIDFILE");
 
//variable declariations
/*Initialize our callbacks*/
//imm_callbacks.saImmOiAdminOperationCallback = imm_objadminopr_set_callback;
imm_callbacks.saImmOiCcbObjectCreateCallback = imm_objcreate_set_callback;
imm_callbacks.saImmOiCcbObjectDeleteCallback = imm_objdel_set_callback;
imm_callbacks.saImmOiCcbObjectModifyCallback = imm_objmod_set_callback;
imm_callbacks.saImmOiCcbAbortCallback = imm_ccbAbort_callback;
imm_callbacks.saImmOiCcbApplyCallback = imm_ccbApply_callback;
imm_callbacks.saImmOiCcbCompletedCallback = imm_ccbcomplete_set_callback;
//register_callback(immOiCallbacks.saImmOiCcbObjectCreateCallback);
//immOiCallbacks.saImmOiCcbObjectDeleteCallbackT = imm_ccbobjectdelete_set_callback;


///immhandle
//////////////////////////////////oi register/////////////////////////////////////////////////////

    /* Version parameter is in/out i.e. must be mutable and should not be
       re-used from previous call in a retry loop. */
  

	error = saImmOiInitialize_2( &immOiHandle, &imm_callbacks, &immVersion);
	if (error != SA_AIS_OK )
		printf("error - saImmOiInitialize FAILED: %d", error);
        if (error == SA_AIS_OK )
{
		printf("OI handle gained\n");

 // printf("\n1\n");
	error =  saImmOiSelectionObjectGet(immOiHandle, &selectionObject);
	if (error != SA_AIS_OK )
        {
		printf("saImmOiSelectionObjectGet FAILED %d\n", error);
	}
        if (error == SA_AIS_OK )
        {
		printf("Operating system handle received\n");
	}
        
        
}

	error = saImmOiImplementerSet(immOiHandle, implementerName);
	if (error != SA_AIS_OK)
		printf("error - saImmOiImplementerSet FAILED: %d\n", error);
	if (error == SA_AIS_OK )
		printf("OI implementer set\n");
 

	error = saImmOiClassImplementerSet(immOiHandle, className);
	
	if (error != SA_AIS_OK )
        {
		printf("error - saImmOiClassImplementerSet FAILED : %d\n", error);
        }
 
	if (error == SA_AIS_OK )
        {
		printf("OI ClassImplementer set\n");
        }

//printf("\n11\n");
fds[0].fd = selectionObject;
fds[0].events = POLLIN;
//printf("\n12\n");
/////////////////////////////////oi register ////////////////////////////////////////////////////
/**
i=0;
while(i<=2)
{
   printf("\nThe Elasticity type is %d",elasticityT);
   sleep(60);
  i++;
  
}
**/
//printf("\n13\n");
while(i<1){
//sleep(60);
//printf("\n14\n");
	res = poll(fds,1,1000);
       // printf("\nres : %d\n",res);
	if(res == -1){
//              printf("\n15\n");
              if(errno == EINTR)
               continue; 
	     else{
               printf("\nPOLL Failed\n"); 
               return EXIT_FAILURE;
             }
       }
        if(fds[0].revents & POLLIN){
//         printf("\n16\n");
        //rc = saDispatch(immOiHandle, SA_DISPATCH_ONE);

        error = saImmOiDispatch(immOiHandle, SA_DISPATCH_ONE);
         //error = saImmOiDispatch(immOiHandle, );
        if(error !=SA_AIS_OK){
        	return EXIT_FAILURE;
        }

       }
i++;
}


//GUI
//printf("%c[1;32m\n**************Welcome to Elasticity Controller**********************************\n",27);
if(callbackflag !=1)
{
	if (flag_set ==3)
	{
	//	printf("\nPlease enter name of SI :\n");
	//	printf("%c[0m",27);
	//	fflush(stdout);
	//	gets(nameSI);
		objectnameSI.length= strlen(nameSI);
		strncpy((char *)objectnameSI.value, nameSI, SA_MAX_NAME_LENGTH);
	//	printf("%c[1;32m\nPlease enter elasticitytype (i)Enter 1 for Single SI (ii)Enter 2 for Multiple SI :",27);
	//	printf("%c[0m",27);
	//	scanf("%u",&elasticityT);	
	//elasticityT = 1;      //SingleSI
	//elasticityT = 2;      //MultipleSI
	//printf("%c[1;32m\nPlease enter elasticity action (i)Enter 1 for increase (ii)Enter 2 for decrease :",27); 
	//printf("%c[0m",27);
	//scanf("%u",&elasticityact);
	}
}
else
{
 printf("\nThe SI name is :%s",objectnameSI.value);
 printf("\nThe elasticity action is :%d",elasticityact);
 printf("\nThe elasticity type is %d",elasticityT);
}


//GUI
//printf("%c[1;32m\n***********************************************************************************",27);
//printf("%c[0m",27);

printf("%c[1;32m\n*****IMM Handle*****",27);
printf("%c[0m",27);
//HANDLE AQUIRED
error = saImmOmInitialize( &immHandle, NULL, &immVersion);
	if (error != SA_AIS_OK) {
		printf("error - saImmOmInitialize FAILED:%d",error);
		rc= EXIT_FAILURE;
                      }
       if (error == SA_AIS_OK)
          printf("%c[1;31m\nIMM handle gained\n",27);
          printf("%c[0m",27);

/**////////////////////////////////oi register/////////////////////////////////////////////////////

    /* Version parameter is in/out i.e. must be mutable and should not be
       re-used from previous call in a retry loop. 
  

	error = saImmOiInitialize_3( &immOiHandle, &immOiCallbacks, &immVersion);
	
	if (error != SA_AIS_OK )
		printf("error - saImmOiInitialize FAILED: %d", error);
        if (error == SA_AIS_OK )
		printf("\nOI handle gained\n");

	error = saImmOiImplementerSet(immOiHandle, implementerName);
	if (error != SA_AIS_OK)
		printf("error - saImmOiImplementerSet FAILED: %d", error);
	if (error == SA_AIS_OK )
		printf("\nOI implementer set\n");
 
        error= saImmOiClassImplementerRelease(immOiHandle,className);
        printf("error %d",error);
        
	error = saImmOiClassImplementerSet(immOiHandle, className);
	
	if (error != SA_AIS_OK )
        {
		printf("error : saImmOiClassImplementerSet FAILED, error = %d", error);
        }

/////////////////////////////////oi register ////////////////////////////////////////////////////*/



         
//printf("\n*****************************************************************************************************************************************************");
printf("%c[1;32m\n*****Access SI Object and its attributes*****",27);
printf("%c[0m",27);
error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 if (error != SA_AIS_OK) {
                printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                rc= EXIT_FAILURE;
                      }
       if (error == SA_AIS_OK)
   {
      // printf("\nAccessor handle gained\n");
   }
//HANDLE AQUIRED

//WE ACCESS OUR FIRST OBGECT SI AND THE REQUIRED PROTECTED SG ATTRIBUTE
error1= saImmOmAccessorGet_2(accessorHandle, &objectnameSI, attributeNames, &attributes);

if (error1 ==SA_AIS_OK)
{
	//printf("\nsaImmOmAccessorGet_2 function returned the SI object and attribute\n");


	}
//WE ACCESS OUR FIRST OBGECT SI AND THE REQUIRED PROTECTED SG ATTRIBUTE


//we initialize to attribute to acces our second object SG

SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
SaNameT *objectnameSG = (SaNameT *)attrValue;
			printf("%c[1;31m\nThe protecting SG is :",27);
                        printf("%c[0m",27);
                        printf("%s (%u) \n",objectnameSG->value,objectnameSG->length);
                        
strcpy(nameSG,objectnameSG->value);
x=objectnameSG->length;
objectnameSG1.length=x;
strncpy((char *)objectnameSG1.value, nameSG, SA_MAX_NAME_LENGTH);
error = saImmOmAccessorFinalize(accessorHandle);

strcpy(strs[0],objectnameSG1.value);
//printf("\n::%s\n",strs[0]);

//Array of the SGs in the cluster
attributeNames1[0] = "saAmfSGSuHostNodeGroup";
attributeNames1[1] = NULL;
error = saImmOmAccessorInitialize( immHandle, &accessorHandle1);
 if (error != SA_AIS_OK) {
                printf("error - saImmOmAccessorInitialize FAILED:%d",error);
                rc= EXIT_FAILURE;
                      }
       if (error == SA_AIS_OK)
   {
     
       error3= saImmOmAccessorGet_2(accessorHandle1, &objectnameSG1, attributeNames1, &attributesSG);
       if(error3 == SA_AIS_OK)
       {
          
	  SaImmAttrValueT *attrValue1 = attributesSG[0]->attrValues[0];
	  SaNameT *objectnameSG2 = (SaNameT *)attrValue1;
	//  printf("\nThe SG valid host node group is : %s (%u) \n", objectnameSG2->value,objectnameSG2->length);
          
	  
	  strncpy((char *)objectnameSG1.value, objectnameSG2->value, SA_MAX_NAME_LENGTH);
          objectnameSG1.length=strlen(objectnameSG1.value);
	 
      }
//

error = saImmOmAccessorFinalize(accessorHandle1);
      
       
   }

searchParam.searchOneAttr.attrName = "saAmfSGSuHostNodeGroup";
searchParam.searchOneAttr.attrValueType = SA_IMM_ATTR_SANAMET;
searchParam.searchOneAttr.attrValue = &objectnameSG1;

error = saImmOmSearchInitialize_2(immHandle, NULL, scope,
					  SA_IMM_SEARCH_ONE_ATTR | SA_IMM_SEARCH_GET_NO_ATTR, &searchParam, NULL,
					  &searchHandle);
	if (SA_AIS_OK != error) {
		//printf("error - saImmOmSearchInitialize_2 FAILED");
                printf("error - saImmOmSearchInitialize FAILED:%d",error);
                rc=EXIT_FAILURE;
                //flag=1;
	}
 
        

        if (SA_AIS_OK == error)
        {
		       //  printf("\nWe got the search handle\n");
		           

            do {
          	error = saImmOmSearchNext_2(searchHandle, &objectNameSU, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
                       // printf("\n\n How r u\n\n");
			printf("error8 - saImmOmSearchNext_2 FAILED: %d\n", error);
                        rc=EXIT_FAILURE;
			//flag=1;
		}
                if(error == SA_AIS_ERR_NOT_EXIST )
                {
		  //   printf("\n does not exist\n");
		 
                      
 		           
		}
                
		if (error == SA_AIS_OK)
                  {
                        //flag1=1;
			//printf("\n%s!!!\n", objectNameSU.value);
                        if(strcmp(objectNameSU.value,strs[0])==0)
                        {
                          //   printf("\nits the same");
                        }
                        else
                        {
                           counter++;
                           //strs[counter]= objectNameSU.value;
                           strcpy(strs[counter],objectNameSU.value);
                        }
                   	//strncpy((char *)objectname.value, objectNameSU.value, SA_MAX_NAME_LENGTH);
			//objectname.length= strlen(objectname.value);
                  }//search next
                
             }while(error != SA_AIS_ERR_NOT_EXIST);
       error= saImmOmSearchFinalize(searchHandle);
         }//search initialize
//Array of the SGs in the cluster

//printf("\n*****Access protecting SG Object and its attributes*****");

/////call from the switch
//printf("\n%d",counter);
strncpy((char *)objectname.value, strs[0], SA_MAX_NAME_LENGTH);
objectname.length= strlen(objectname.value);
i=0;
loop1:
result = switchcall(objectnameSI,objectname,elasticityT,elasticityact,immHandle,argv);


if(result ==2)
{
  
  i++;
  if(i<=counter)
  {
   printf("%c[1;32m\nElasticity action at cluster level\n",27);
   printf("%c[0m",27);
   strncpy((char *)objectname.value, strs[i], SA_MAX_NAME_LENGTH);
   objectname.length= strlen(objectname.value);
   goto loop1; 
   //result = switchcall(objectname,elasticityT,elasticityact,immHandle,argv);  
  }
 else
  {
    if(elasticityact==1)
    {
      result=3;
    }
    else
    {
        printf("\n%c[1;32mThis is minimum configuration\n",27);
        printf("%c[0m",27);
    }
  }
  
  
}

if(result ==1)
{
  printf("%c[1;32m\nElasticity action was succesfull\n",27);
  printf("%c[0m",27);
}

if(result ==3)
{
 printf("\nElasticity engine requests to increase the number of nodes\n");
("************************************************************************************************\n");
}
//error = saImmOiFinalize(immOiHandle);
//error = saImmOmFinalize(immHandle);

//exit(rc);
error = saImmOiClassImplementerRelease(immOiHandle, className);
error = saImmOiImplementerClear(immOiHandle);
error = saImmOiFinalize(immOiHandle);
error = saImmOmFinalize(immHandle);
return result;
//exit(rc);
//}//if
}//while(1)
//}

}

SaAisErrorT imm_objcreate_set_callback(SaImmOiHandleT immOiHandle,SaImmOiCcbIdT ccbId,const SaImmClassNameT class_Name,const SaNameT *parent_Name,const SaImmAttrValuesT_2 **attr)
{
   printf("\n Callback received for SI creation");
   int i=0,j;
   SaImmAttrValuesT_2 *attributes;
   SaAisErrorT error;
   
   SaNameT *myNameT;
   char str[100];
  
   //strncpy(class_Name1,class_Name);
   //i = my_func();
   //printf("\nI got a call\n");
  // strncpy(class_Name1->value,parent_Name->value,100);
  /**
   strncpy((char *)class_Name1.value, parent_Name->value, SA_MAX_NAME_LENGTH);
          class_Name1.length=strlen(class_Name1.value);
  **/

   elasticityT = 2;
   elasticityact =1;
   callbackflag =1;
  while((attr[i]) != NULL)
  {
     
     if (strcmp(attr[i]->attrName, "safSi") == 0)
    {

         	//printf("\n%d)%s:%s\n",i,attr[i]->attrName,*(char **)attr[i]->attrValues[0]);
                 strcpy(str,*(char **)attr[i]->attrValues[0]); 
                 strcat(str,",");
                 strcat(str,parent_Name->value);  
               printf("\nThe name of SI ::%s\n", str);  
                 objectnameSI.length= strlen(str);
		strncpy((char *)objectnameSI.value, str, SA_MAX_NAME_LENGTH);        
             printf("\nThe name of SI ::%s\n",objectnameSI.value ); 
    } 
    if (strcmp(attr[i]->attrName, "saAmfSIProtectedbySG") == 0)
    {
     myNameT = (SaNameT *)attr[i]->attrValues[0];
     //printf("\n%d)%s:%s\n",i,attr[i]->attrName,myNameT->value);
    } 
     i++;
     
  }
 
  return SA_AIS_OK;
//   return 0;
}

static SaAisErrorT imm_objdel_set_callback(SaImmOiHandleT immOiHandle,SaImmOiCcbIdT ccbId,const SaNameT *object_Name)
{
   SaNameT objectName;
   SaImmAttrNameT  attributeNames[1];
   printf("\nSI name deleted %s\n",object_Name->value);
   SaImmAccessorHandleT accessorHandle;
   elasticityT = 2;
   elasticityact =2;
   callbackflag =1;
   objectnameSI.length= strlen(object_Name->value);
		strncpy((char *)objectnameSI.value, object_Name->value, SA_MAX_NAME_LENGTH);        
   printf("\nSI name deleted %s\n",objectnameSI.value);
      printf("\n Callback received for SI deletion");
   int i=0,j;
   SaImmAttrValuesT_2 **attributes;
  
   SaAisErrorT error;
   
   SaNameT *objectnameSG;
   char str[100];
   attributeNames[0] ="saAmfSIProtectedbySG";
   attributeNames[1] =NULL;
						   error = saImmOmAccessorInitialize( immHandle, &accessorHandle);
 			       			   if (error != SA_AIS_OK) {
                	       					printf("error - saImmOmAccessorInitialize FAILED:%d to acces the Valid SU type",error);
                	       					//rc= EXIT_FAILURE;
								//flag = 1;
                      				   }
       			       			   if (error == SA_AIS_OK)
   						   {
							error= saImmOmAccessorGet_2(accessorHandle, &objectnameSI, attributeNames, &attributes);
                                        		if (error !=SA_AIS_OK)
                                        		{
                                               			printf("\nerror18:%d",error);
								//rc= EXIT_FAILURE;
                                                        	//flag = 1; 
                                        		}
                        	 			if (error ==SA_AIS_OK)
                                 			{
			                     		
					     			SaImmAttrValueT *attrValue = attributes[0]->attrValues[0];
                                                                objectnameSG = (SaNameT *)attrValue;
			printf("The protecting SG is : %s (%u) \n", objectnameSG->value,objectnameSG->length);
                		   	               	 	error = saImmOmAccessorFinalize(accessorHandle);
							}//if not standby
       			
                        		           }//accesor initialize
     

  return SA_AIS_OK;
}


static SaAisErrorT imm_objmod_set_callback(SaImmOiHandleT immOiHandle,SaImmOiCcbIdT ccbId,const SaNameT *object_Name,const SaImmAttrModificationT_2 **attrMods)
{
   //printf("\nSI name deleted %s\n",object_Name->value);
   return SA_AIS_OK;
}
/*
static void imm_objadminopr_set_callback(SaImmOiHandleT immOiHandle,SaInvocationT invocation,const SaNameT *object_Name,SaImmAdminOperationIdT operationId,const SaImmAdminOperationParamsT_2 **params)
{
   //printf("\nSI name deleted %s\n",object_Name->value);
   
}
*/

static SaAisErrorT imm_ccbcomplete_set_callback(SaImmOiHandleT immOiHandle,SaImmOiCcbIdT ccbId)
{
   
   //printf("\nSI name deleted %s\n",object_Name->value);
   return SA_AIS_OK;
}

static void imm_ccbApply_callback(SaImmOiHandleT immOiHandle,SaImmOiCcbIdT ccbId)
{
   //printf("\nSI name deleted %s\n",object_Name->value);
   //return SA_AIS_OK;
   printf("\nI got CCB Apply callback\n");
}

static void imm_ccbAbort_callback(SaImmOiHandleT immOiHandle,SaImmOiCcbIdT ccbId)
{
   //printf("\nSI name deleted %s\n",object_Name->value);
   //return SA_AIS_OK;
   printf("\nI got a callback to abort\n");
}

int num_SUofSG(SaImmHandleT immHandle,SaNameT objectnameSG1)
{
 //  printf("\n I got a call");
 int noofSU;
    SaNameT rootName = { 0, "" };
    
  SaImmAttrNameT  attributeNames[1];
 //strncpy((char *)rootName.value, "safSg=AmfDemo,safApp=AmfDemo2", SA_MAX_NAME_LENGTH);
 strncpy((char *)rootName.value, objectnameSG1.value, SA_MAX_NAME_LENGTH);
 rootName.length = strlen((char *)rootName.value);
 
 //SaImmHandleT immHandle;
 SaImmSearchParametersT_2 searchParam;
 SaImmScopeT scope = SA_IMM_SUBLEVEL;	/* default search scope */
 SaImmScopeT scope1 = SA_IMM_SUBTREE;
 SaImmSearchHandleT searchHandle;
 SaImmAccessorHandleT accessorHandle;
 int rc = EXIT_SUCCESS;
 char string[100],string2[100],string3[100];
 char ch1;
 
// SaImmAttrNameT  attributeNames[] ={"safSISU",NULL};
 //const SaNameT rootName =  objectnameSI;
 SaAisErrorT error;
 SaNameT objectNames,objectName;
 SaImmAttrValuesT_2 **attributes,**attributes1;
 char * s3;
 int long ch;
 int len1,len2,len3,i,cntr1,cntr2,flag=0,noSIassg;
 char *buff;

 // printf("\nOOOOOOOOOOOOO\n");
  searchParam.searchOneAttr.attrName = NULL;
searchParam.searchOneAttr.attrValue = NULL;

//printf("\n%s\n",rootName.value);
error = saImmOmSearchInitialize_2(immHandle, &rootName, scope1, SA_IMM_SEARCH_GET_SOME_ATTR , &searchParam, attributeNames , &searchHandle);
	if (SA_AIS_OK != error) {
	
                printf("error - saImmOmSearchInitialize FAILED:%d\n",error);
                rc=EXIT_FAILURE;
                
	}
        if (SA_AIS_OK == error) {
	
           //     printf("\nSearch Hnadle Obtained\n");
                rc=EXIT_FAILURE;
                
	

noofSU=0;
do {
		error = saImmOmSearchNext_2(searchHandle, &objectName, &attributes);
		if (error != SA_AIS_OK && error != SA_AIS_ERR_NOT_EXIST) {
			printf("error - saImmOmSearchNext_2 FAILED: %d\n", error);
			exit(EXIT_FAILURE);
		}
		if (error == SA_AIS_OK)
                {
                    //  printf("\nthe object searched %s",objectName.value);
                             
                               buff = objectName.value;
                                  s3 =strstr(buff,"safSu=");
                             
              
                      if(s3 == NULL)
                      {
                         //  printf("\nno");
                      }
                      else
                      {  
                        ch =buff - s3;
                      //   printf("\nTHe position %ld",ch); 
                        if(ch == 0)
                        {
			//printf("\nSU%s\n", objectName.value);
                        noofSU++;
                        }
                        
                      }
                }
      } while (error != SA_AIS_ERR_NOT_EXIST);
error = saImmOmSearchFinalize(searchHandle);

}                       
printf("\nThe number of SUs of SG is %d",noofSU);   

   return noofSU;
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

