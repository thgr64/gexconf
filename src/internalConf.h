

#define DEF_BUF_SIZE 0x8000



/**
	Declarations to create OS-dependet linefeeds

	maybe a bit redundant
*/
#ifdef WIN32

#define snprintf _snprintf
#define strcasecmp _stricmp

#define SAVE_INTEGER_A	"// %s \\V:%s,%d,%d,%d \\H:%d\r\n*%s=%d#\r\n\r\n"
#define SAVE_BOOLEAN_A	"// %s \\V:%s,%s,%s,%s \\H:%d\r\n*%s=%s#\r\n\r\n"
#define SAVE_FLOAT_A	"// %s \\V:%s,%f,%f,%f \\H:%d\r\n*%s=%f#\r\n\r\n"
#define SAVE_STRING_A	"// %s \\V:%s,%d,%d,%s \\H:%d\r\n*%s=%s,,#\r\n\r\n"
#define SAVE_BINARY_A	"// %s \\V:%s,%d,%d,%s \\H:%d\r\n*%s=%s#\r\n\r\n"
#define SAVE_SECTION_A	"//[%s] %s\\\\\r\n"

#define SAVE_INTEGER_W	L"// %s \\V:%s,%d,%d,%d \\H:%d\r\n*%s=%d#\r\n\r\n"
#define SAVE_BOOLEAN_W	L"// %s \\V:%s,%s,%s,%s \\H:%d\r\n*%s=%s#\r\n\r\n"
#define SAVE_FLOAT_W	L"// %s \\V:%s,%f,%f,%f \\H:%d\r\n*%s=%f#\r\n\r\n"
#define SAVE_STRING_W	L"// %s \\V:%s,%d,%d,%s \\H:%d\r\n*%s=%s,,#\r\n\r\n"
#define SAVE_BINARY_W	L"// %s \\V:%s,%d,%d,%s \\H:%d\r\n*%s=%s#\r\n\r\n"
#define SAVE_SECTION_W	L"//[%s] %s\\\\\r\n"

#else
#define SAVE_INTEGER_A	"// %s \\V:%s,%d,%d,%d \\H:%d\n*%s=%d#\n\n"
#define SAVE_BOOLEAN_A	"// %s \\V:%s,%s,%s,%s \\H:%d\n*%s=%s#\n\n"
#define SAVE_FLOAT_A	"// %s \\V:%s,%f,%f,%f \\H:%d\n*%s=%f#\n\n"
#define SAVE_STRING_A	"// %s \\V:%s,%d,%d,%s \\H:%d\n*%s=%s,,#\n\n"
#define SAVE_BINARY_A	"// %s \\V:%s,%d,%d,%s \\H:%d\n*%s=%s#\n\n"
#define SAVE_SECTION_A	"//[%s] %s\\\\\n"


#endif



/** I/O operations for Configuration files
	buffer - must be large enough to contain MAX_CONF bytes
*/

int LoadConfigFileA(const char *filename, unsigned char **buffer, int bufsize);
int LoadConfigFileW(const wchar_t *filename, unsigned char **buffer, int bufsize);
int LoadConfigFileC(const char *filename, unsigned char **buffer, int bufsize);

#ifdef WIN32
	#ifdef UNICODE
		#define LoadConfigFile LoadConfigFileW
	#else
		#define LoadConfigFile LoadConfigFileA
	#endif
#else
	#define LoadConfigFile LoadConfigFileC
#endif

int SaveConfigFileA(const char *filename, const char *buffer, int size);
int SaveConfigFileW(const wchar_t *filename, const char *buffer, int size);
int SaveConfigFileC(const char *filename, const char *buffer, int size);

#ifdef WIN32
	#ifdef UNICODE
		#define SaveConfigFile SaveConfigFileW
	#else
		#define SaveConfigFile SaveConfigFileA
	#endif
#else
	#define SaveConfigFile SaveConfigFileC
#endif

/** Output helper functions*/

/** Types to Strings 
	Converts the TYPE-* identifiers to strings
*/
char* PrintTypeA(int flags);
wchar_t* PrintTypeW(int flags);

/** Values to Strings 
	BOOLEAN: TRUE and FALSE
*/
char* PrintValueA(int val);
wchar_t* PrintValueW(int val);


/* String helper function*/
char * own_strtolower(char * str);
wchar_t * own_wstrtolower(wchar_t * str);

int convertEndian(char *str, int len);

/** 
	Converts a wchar_t buffer to a char buffer
	UTF-16/32 to ASCII/ANSI UNICODE characters will be lost
*/
int shrinkbuffer(wchar_t *str, int charcount, char *output);
/*
	Converts ASCII/ANSI to UTF-16/UTF-32
*/
int expandbuffer(char* str, int charcount, wchar_t *output);

int expandbufferex(char* str, int charcount, int size_wchar, wchar_t *output);

/**
	For BASE64 encoding
*/
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
void encodeblock( const char* in, char* out, int len );
int tobase64(const char*, int, char*);


int ProcessBuffer(unsigned char* buffer, const char *section, const CONFENTRY *entrydef, const int entrycount, int* result, unsigned char* config, int maxbufsize);

/** Tests buffer for character formats 
	like ASCII, ANSI, UTF-8, UTF_16, UTF-32
	- supported ASCII/ANSI, (UTF-16)
	
*/
int TestBuffer(unsigned char *buffer);

char* FindSectionA(char* buffer, const char* section, char** begin);
wchar_t* FindSectionW(wchar_t *buffer, const wchar_t* section, char** begin);

char* FindSectionEndA(char* buffer);
wchar_t* FindSectionEndW(wchar_t *buffer);

wchar_t *ProcessKeyW(const CONFENTRY *entrydef, const int size, int* result, const wchar_t *key, wchar_t *value, unsigned char* config);
char *ProcessKeyA(const CONFENTRY *entrydef, const int size, int* result, const char *key, char *value, unsigned char* config);
int ProcessValue(const char *value, LIST_TYPE *entry, void **retval);

int PrepareConfigBufferA(const CONFENTRY *entrydef, const int size, char* buffer, int remaining);



int WriteSectionA(const char *filename, const char *section, const CONFENTRY *entrydef, const int size, const int CharType, int maxbufsize);
int WriteSectionW(const wchar_t *filename, const char *section, const CONFENTRY *entrydef, const int size, const int CharType, int maxbufsize);
int WriteSectionc(const char *filename, const char *section, const CONFENTRY *entrydef, const int size, const int CharType, int maxbufsize);


/*
	int LoadConfiguration(char* filename);
	int LoadConfiguration(char* filename, CONFENTRY *entrydef, int size, void* conf);
	int SaveConfiguration(char* filename);
	int SaveConfiguration(char* filename, void* conf);
*/

/**
 * Conf file format
 * 
 * // any text \V:TRUE/FALSE
 * key=value
 * // any text \V:0 to 255
 * key=value
 * // any text \V:String\termination
 * key=value

*/

/*
// Configuration

// Use SharedMemory \V:16 to 1024 MB \DEF:1/16 RAM
SharedMemory=DEFAULT

// Hide GUI \V:TRUE/FALSE \DEF:TRUE
HideGUI=FALSE

// Auto load Disks 1 = A: ... 26 = Z:
// \V: 0 to 26 \DEF:0
AutoLoad=MAX

// Include deleted files \V:TRUE/FALSE \DEF:TRUE
DeletedFiles=TRUE


\\End of File
*/