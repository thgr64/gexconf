
#include <stddef.h>

/**
	These are hints for an GUI editor that might be used to edit a configfile
	These values have no other meaning
*/
#define HINT_BUTTON		1
#define HINT_COMBOBOX	2
#define HINT_SLIDER		3
#define HINT_MULTILINE	4
#define HINT_DATE		5

/**
	These are the fundamentel data types
	TYPE_BOOLEAN	is a simple integer but it contains TRUE or FALSE
	TYPE_INTEGER	is a signed integer (32BIT)
	TYPE_FLOAT		is a double
	TYPE_BINARY		is a pointer to a data buffer, the buffer will be encoded in BASE64
	TYPE_LIST		is a pointer to a struct, array of pointers, depending on the types it contains

	other types must be implemented by using TYPE_USERDEF and suppling a function, which handles the data
*/
#define TYPE_BOOLEAN 	8
#define TYPE_FLOAT		16
#define TYPE_INTEGER	32
#define TYPE_STRING		64
#define TYPE_BINARY		128
#define TYPE_LIST		256
#define TYPE_USERDEF	512

/**
	DONT_CHECK		doesn't verify the lower and upper limit of a value
*/
#define ALLOC_DYNAMIC	4096
#define DONT_CHECK		8192

/**
	TYPE_SECTION is not a real type, it is just specified to create a section
	it might contain a language id for the following section
*/
#define TYPE_SECTION	-1

/**
	character formats
*/
#define ASCII	7
#define ANSI	1
#define UTF_8	8
#define UTF_16	2
#define UTF_32	4

/**
	Error codes produced by this lib
*/
#define DEFAULT		1
#define PASSED		4
#define NOT_A_TYPE 42
#define NO_SECTION	8
#define BUFFER_TOO_SMALL 2

#ifndef WIN32
	
#define TRUE	1
#define FALSE	0

#endif


#define ALLOCATE_BUFFER 0

/**
	Types to be used by the CONFENTRIES
*/
typedef union DATA
{
	int 	i;
	double 	d;
	char	*c;
	void    *v;
}DATA;


typedef struct LIST_TYPE
{
	DATA from;
	DATA to;
	DATA def;
	int flags;
	int offset;
}LIST_TYPE;

/**
	This type defines a configuration entry
	from		-	the lower bound of a value -
					if TYPE_STRING - it is the minimum string length
	to			-	the upper bound of a value
					if TYPE_STRING - it is the maximum string length
	def			-	the default value for this entry
	flags		-	the TYPE_* of this entry
	offset		-	the position where to store the data, if a extra buffer is supplied
	key			-	the name of the key stored in the config file
	description -	the description of the key
	
	additionaldata  
	value		-	pointer to a location where the data can be stored
*/
typedef struct CONFENTRY
{
	DATA from;
	DATA to;
	DATA def;
	int flags;
	int offset;

	char *key;
	char *description;
	void *additionaldata;
	void **value;
} CONFENTRY;

/**
	User-defined function prototype
*/
typedef char* (*FUNCTION_TYPE) (const CONFENTRY*, char*, void**);

int SetGlobalConfigDefinition(CONFENTRY *entrydef, int entrycount);

int SetConfigEntryDefString(CONFENTRY *entry, char* defstring);
int SetConfigEntryDefStruct(CONFENTRY *entry, void* defstruct);
int SetConfigEntryFloat(CONFENTRY *entry, double from, double to, double def);

int FillRemaining(const CONFENTRY *entrydef, const int entrycount, int *res, unsigned char *config);

/**
	Loads the configuration file or parts of it
*/

int LoadConfigurationA(const char *filename, const char *section, const CONFENTRY *entrydef, const int entrycount, int* result, unsigned char* config, int maxbufsize);
int LoadConfigurationW(const wchar_t *filename, const char *section, const CONFENTRY *entrydef, const int entrycount, int* result, unsigned char* config, int maxbufsize);
int LoadConfigurationC(const char *filename, const char *section, const CONFENTRY *entrydef, const int entrycount, int* result, unsigned char* config, int maxbufsize);

#ifdef WIN32
	#ifdef UNICODE
		#define LoadConfiguration LoadConfigurationW
	#else
		#define LoadConfiguration LoadConfigurationA
	#endif
#else
	#define LoadConfiguration LoadConfigurationC
#endif

/** 
	Writes the complete configuration file to disk
*/

int WriteCompleteConfigurationA(const char *filename, const CONFENTRY *entrydef, const int size, unsigned char* config, const int CharType,int maxbufsize);
int WriteCompleteConfigurationW(const wchar_t *filename, const CONFENTRY *entrydef, const int size, unsigned char* config, const int CharType,int maxbufsize);
int WriteCompleteConfigurationC(const char *filename, const CONFENTRY *entrydef, const int size, unsigned char* config, const int CharType,int maxbufsize);

#ifdef WIN32
	#ifdef UNICODE
		#define WriteCompleteConfiguration WriteCompleteConfigurationW
	#else
		#define WriteCompleteConfiguration WriteCompleteConfigurationA
	#endif
#else
	#define WriteCompleteConfiguration WriteCompleteConfigurationC
#endif

int WriteConfigurationA(const char *filename, const char *section, const CONFENTRY *entrydef, const int size, unsigned char* config, const int CharType,int maxbufsize);
int WriteConfigurationW(const wchar_t *filename, const wchar_t *section, const CONFENTRY *entrydef, const int size, unsigned char* config, const int CharType,int maxbufsize);
int WriteConfigurationC(const char *filename, const char *section,const CONFENTRY *entrydef, const int size, unsigned char* config, const int CharType,int maxbufsize);

#ifdef WIN32
	#ifdef UNICODE
		#define WriteConfiguration WriteConfigurationW
	#else
		#define WriteConfiguration WriteConfigurationA
	#endif
#else
	#define WriteConfiguration WriteConfigurationC
#endif



/* global conf entry TODO comment */
CONFENTRY *glEntry;
int glEntryCount;
