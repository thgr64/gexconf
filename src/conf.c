

#ifndef WIN32
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>
#else
#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#endif

#include "../include/gexconf.h"
#include "internalConf.h"
#include "config.h"




char * own_strtolower(char * str)
{
	int len = strlen(str);
	int i;
	for(i = 0; i < len; i++)
		str[i] = tolower(str[i]);

	return str;
}


wchar_t * own_wstrtolower(wchar_t * str)
{
	int len = wcslen(str);
	int i;
	for(i = 0; i < len; i++)
		str[i] = towlower(str[i]);
	
	return str;
}

int convertEndian(char *str, int len)
{
	char a;
	int i;
	for (i=0;i<len;i++)
	{
		a = str[i];
		str[i] = str[i+1];
		str[i+1] = a;
	}
	return i;
}

int shrinkbuffer(wchar_t *str, int charcount, char *output)
{
	int i;
	for (i=0;i<charcount;i++)
	{
		output[i] = str[i];
	}
	return i;
}

int expandbuffer(char* str, int charcount, wchar_t *output)
{
	int i;
	for (i=0;i<charcount;i++)
	{
		output[i] = str[i];
	}
	return i;
}


int SetGlobalConfigDefinition(CONFENTRY *entrydef, int entrycount)
{
	glEntry = entrydef;
	glEntryCount = entrycount;

	return 0;
}


/**
	THis Function exists because C doesn't allow to initialize the union to every type declared. Only the first can be initialized
*/
int SetConfigEntryDefString(CONFENTRY *entry, char* defstring)
{
	if (entry->flags & TYPE_STRING)
	{
		entry->def.c = defstring;
		return TRUE;
	}
	return FALSE;
}
/**
	THis Function exists because C doesn't allow to initialize the union to every type declared. Only the first can be initialized
*/
int SetConfigEntryDefStruct(CONFENTRY *entry, void* defstruct)
{
	if (entry->flags & TYPE_LIST)
	{
		entry->def.v = defstruct;
		return TRUE;
	}
	return FALSE;
}

int SetConfigEntryFloat(CONFENTRY *entry, double from, double to, double def)
{
	if (entry->flags & TYPE_FLOAT)
	{
		entry->from.d = from;
		entry->to.d = to;
		entry->def.d = def;
		return TRUE;
	}
	return FALSE;
}

/**
	This function sets all values not specified in the config file to the default values
*/
int FillRemaining(const CONFENTRY *entrydef, const int entrycount, int *res, unsigned char *config)
{
	int i;
	for (i=0;i<entrycount;i++)
	{
		if (res==NULL)
		{
			if (config==NULL)
				ProcessValue("", (LIST_TYPE*)&entrydef[i], entrydef[i].value);
			else
				ProcessValue("", (LIST_TYPE*)&entrydef[i], (void**) &config[entrydef[i].offset]);
		}
		else
			if (res[i]==0)
				if (config==NULL)
					ProcessValue("", (LIST_TYPE*)&entrydef[i], entrydef[i].value);
				else
					ProcessValue("", (LIST_TYPE*)&entrydef[i], (void**) &config[entrydef[i].offset]);
		
	}
	return 0;
}
/**
	The *config buffer must be large enough to hold all data that is specified in the CONFENTRY struct
	or it must be NULL
	the *result buffer must be the size specified by size - or it can be NULL - if you are not interested in the result of each key-parse-operation
*/
int LoadConfigurationA(const char *filename, const char *section, const CONFENTRY *entrydef, const int entrycount, int* result, unsigned char* config, int maxbufsize)
{
	int res;
	unsigned char *buffer;
	if (maxbufsize!=ALLOCATE_BUFFER)
	{
		buffer = (unsigned char*) malloc(maxbufsize+4);
		if (*buffer==NULL)
			return -1;
	}

	res = LoadConfigFileA(filename, &buffer, maxbufsize); 


	if (res == 0)
	{
		ProcessBuffer(buffer, section, entrydef, entrycount, result, config, maxbufsize);
	}
	free(buffer);
	return res;
}

int LoadConfigurationW(const wchar_t *filename, const char *section, const CONFENTRY *entrydef, const int entrycount, int* result, unsigned char* config, int maxbufsize)
{
	int res;
	unsigned char *buffer;

	if (maxbufsize!=ALLOCATE_BUFFER)
	{
		buffer = (unsigned char*) malloc(maxbufsize+4);
		if (buffer==NULL)
			return -1;
	}

	res = LoadConfigFileW(filename, &buffer, maxbufsize); 

	if (res == 0)
	{
		if (maxbufsize==ALLOCATE_BUFFER)
			maxbufsize = wcslen((wchar_t*)buffer);

		ProcessBuffer(buffer, section, entrydef, entrycount, result, config, maxbufsize);
	}
	free(buffer);
	return res;
}

int LoadConfigurationC(const char *filename, const char *section, const CONFENTRY *entrydef, const int entrycount, int* result, unsigned char* config, int maxbufsize)
{
	int res;
	unsigned char *buffer;

	if (maxbufsize!=-1)
	{
		buffer = (unsigned char*) malloc(maxbufsize+4);
		if (buffer==NULL)
			return -1;
	}

	res = LoadConfigFileC(filename, &buffer, maxbufsize); 

	if (res == 0)
	{
		if (maxbufsize==ALLOCATE_BUFFER)
			maxbufsize = strlen((char*)buffer);
		
		ProcessBuffer(buffer, section, entrydef, entrycount, result, config, maxbufsize);
	}
	free(buffer);
	return res;
}



/** 
	Loads all data into the specified buffer, the buffer must be large enough to hold MAX_CONF bytes
	It will return 0 or the OS dependent error code
*/
int LoadConfigFileW(const wchar_t *filename, unsigned char **buffer, int bufsize)
{
#ifdef WIN32
	HANDLE file;
	DWORD fileSize;
	DWORD read, res;

	file = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (file==INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	fileSize = GetFileSize(file, NULL);
	if (bufsize==ALLOCATE_BUFFER)
		*buffer = (unsigned char*) malloc(fileSize+4);
	else
		if (fileSize > bufsize)
			fileSize = bufsize;

	if (!ReadFile(file, *buffer, fileSize, &read, NULL))
	{
		res = GetLastError();
		CloseHandle(file);
		return res;
	}

	(*buffer)[read] = 0;
	(*buffer)[read+1] = 0;
	(*buffer)[read+2] = 0;
	(*buffer)[read+3] = 0;
	
	CloseHandle(file);
	return 0;
#endif
	return -1;
}

int LoadConfigFileA(const char *filename, unsigned char **buffer, int bufsize)
{
#ifdef WIN32
	HANDLE file;
	DWORD fileSize;
	DWORD read, res;

	file = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (file==INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	fileSize = GetFileSize(file, NULL);
	if (bufsize==ALLOCATE_BUFFER)
		*buffer = (unsigned char*) malloc(fileSize+4);
	else
		if (fileSize > bufsize)
			fileSize = bufsize;

	if (!ReadFile(file, *buffer, fileSize, &read, NULL))
	{
		res = GetLastError();
		CloseHandle(file);
		return res;
	}

	(*buffer)[read] = 0;
	(*buffer)[read+1] = 0;
	(*buffer)[read+2] = 0;
	(*buffer)[read+3] = 0;
	
	CloseHandle(file);
	return 0;
#endif
	return -1;
}

int LoadConfigFileC(const char *filename, unsigned char **buffer, int bufsize)
{
#ifndef WIN32
	int fd;
	int res;
	fd = open(filename, O_RDONLY);
	if (fd==-1)
	{
		return errno;
	}

	struct stat buf;
	if (stat(filename, &buf))
	{
		res = errno;
		close(fd);
		return res;
	}
	int size = buf.st_size;

	if (!read(fd, &buffer, size))
	{
		res = errno;
		close(fd);
		return res;
	}

	(*buffer)[size] = 0;
	(*buffer)[size+1] = 0;
	(*buffer)[size+2] = 0;
	(*buffer)[size+3] = 0;

	close(fd);
	return 0;
#endif
	return -1;
}

/**
	This function writes MAX_CONF bytes to the specified file if possible.
	It will return 0 or the OS dependent error code
*/
int SaveConfigFileW(const wchar_t *filename, const char *buffer, int size)
{
#ifdef WIN32
	HANDLE file;
	DWORD written, res;

	file = CreateFileW(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
	if (file==INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	/*if (size > MAX_CONF)
		size = MAX_CONF;
		*/
	SetFilePointer(file,size, NULL, FILE_BEGIN);
	SetEndOfFile(file);
	SetFilePointer(file, 0, NULL, FILE_BEGIN);

	if (!WriteFile(file, buffer, size, &written, NULL))
	{
		res = GetLastError();
		CloseHandle(file);
		return res;
	}

	CloseHandle(file);
	return 0;
#endif
	return -1;
}

int SaveConfigFileA(const char *filename, const char *buffer, int size)
{
#ifdef WIN32
	HANDLE file;
	DWORD written, res;

	file = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
	if (file==INVALID_HANDLE_VALUE)
	{
		return GetLastError();
	}

	/*if (size > MAX_CONF)
		size = MAX_CONF;
		*/
	SetFilePointer(file,size, NULL, FILE_BEGIN);
	SetEndOfFile(file);
	SetFilePointer(file, 0, NULL, FILE_BEGIN);

	if (!WriteFile(file, buffer, size, &written, NULL))
	{
		res = GetLastError();
		CloseHandle(file);
		return res;
	}

	CloseHandle(file);
	return 0;
#endif
	return -1;
}

int SaveConfigFileC(const char *filename, const char *buffer, int size)
{
#ifndef WIN32
	int fd;
	fd = open(filename, O_RDWR);
	if (fd==-1)
	{
		return -1;
	}

	if (size > MAX_CONF)
		size = MAX_CONF;

	ftruncate(fd, size);

	if (write(fd, buffer, size)==0)
	{
		close(fd);
		return -1;
	}

	close(fd);
#endif
	
	return -1;
}


int ProcessBuffer(unsigned char* buffer, const char *section, const CONFENTRY *entrydef, const int entrycount, int* result, unsigned char* config, int maxbufsize)
{
	int done = FALSE;
	int len;

	if (TestBuffer(buffer)==UTF_16)
	{
		//TODO Do wide char with the ANSI 
		wchar_t *result1, *result2;
		result1 = (wchar_t*)buffer;
		if (section!=NULL)
		{
			//TODO Find a solution to the problem of different charactersets
			//result1 = FindSectionW(result1, section);
			if (result2==NULL)
				return NO_SECTION;
			result2 = FindSectionEndW(result1);
			if (result2!=NULL)
				result2[0] = 0;
		}
		while(!done)
		{
			result1 = wcschr(result1, L'*');
			if (result1!=NULL)
			{
				//TODO maybe do more checks

				result1+=1;
				result2 = wcschr(result1, L'=');
				if (result2!=NULL)
				{
					*result2 = 0;
					result2++;
				}
				else
					done = TRUE;

				result1 = ProcessKeyW(entrydef, entrycount, result, result1, result2, config);
				if (result1==NULL)
					done = TRUE;
			}
		}
	}
	else
	{
		char *result1, *result2;
		result1 = (char*)buffer;
		if (section != NULL)
		{
			result1 = FindSectionA(result1, section, NULL);
			if (result2==NULL)
				return NO_SECTION;
			result2 = FindSectionEndA(result1);
			if (result2 != NULL)
				result2[0] = 0;
		}

		/*
			Processes every key - a key starts with a * and ends with a =
			till it reaches the end of the buffer - or there is no data to be processed
			The key should be in ASCII - for highes compatibility
		*/
		while(!done)
		{
			result1 = strchr(result1, L'*');
			if (result1!=NULL)
			{
				//TODO maybe do more checks
				if (result1[-1]=='\n')
				{
					result1+=1;
					result2 = strchr(result1, L'=');
					if (result2!=NULL)
					{
						*result2 = 0;
						result2++;
					}
					else
						done = TRUE;

					result1 = ProcessKeyA(entrydef, entrycount, result, own_strtolower(result1), result2, config);
					if (result1==NULL)
						done = TRUE;
				}
				else
					result1++;
			}
			else
				break;
		}
	}
	return 0;
}

char* FindSectionA(char* buffer, const char* section, char** begin)
{
	char* result1;
	int len;
	result1 = buffer;
	while (TRUE)
	{
		result1 = strstr(result1, "//[");
		len = strlen((char*)section);
		if (result1!=NULL)
		{
			if (begin!=NULL) 
				*begin = result1;
			result1+=3;
			if (strncmp((char*)section, result1, len)==0 && result1[len]==']')
			{
				result1+=len+1; //Begin of section
				return result1;
			}
		}
		else 
			return NULL;
	}
}
wchar_t* FindSectionW(wchar_t *buffer, const wchar_t* section, wchar_t** begin)
{
	wchar_t* result1;
	int len;
	result1 = buffer;
	while (TRUE)
	{
		result1 = wcsstr(result1, L"//[");
		len = wcslen((wchar_t*)section);
		if (result1!=NULL)
		{
			if (begin!=NULL) 
				*begin = result1;
			result1+=3;
			if (wcsncmp((wchar_t*)section, result1, len)==0 && result1[len]==L']')
			{
				result1+=len+1; //Begin of section
				return result1;
			}
		}
		else 
			return NULL;
	}
	return NULL;
}
char* FindSectionEndA(char* buffer)
{
	return strstr(buffer, "//[");
}
wchar_t* FindSectionEndW(wchar_t *buffer)
{
	return wcsstr(buffer, L"//[");
}


int TestBuffer(unsigned char *buffer)
{
	buffer+=4; /* so wie skip BOM, if it exists*/

	if (buffer[0]>0 && buffer[1]==0 && buffer[2]>0 && buffer[3]==0)
		return UTF_16;
	else if (buffer[0]==0 && buffer[1]>0 && buffer[2]==0 && buffer[3]>0)
	{
		//TODO Convert to little endian
		convertEndian((char*)buffer, strlen((char*)buffer));
		return UTF_16;
	}
	else
	{
		//TODO more checks
		return ANSI;
	}
}

wchar_t *ProcessKeyW(const CONFENTRY *entrydef, const int size, int* result, const wchar_t *key, wchar_t *value, unsigned char* config)
{
	int i;
	for (i=0; i< size;i++)
	{
		//TODO use ProcessKeyA, when tested
	}
	return NULL;
}

char *ProcessKeyA(const CONFENTRY *entrydef, const int size, int* result, const char *key, char *value, unsigned char* config)
{
	int i;
	char *ret;
	int len;
	int res;
	void**retval;

	for (i=0; i< size;i++)
	{
		int res;
	
		res = strcasecmp((char*)entrydef[i].key, key);
		
		if (res==0)
		{
			/*
				process data 
			*/
			if (entrydef[i].flags & TYPE_STRING || entrydef[i].flags & TYPE_LIST)
			{
				ret = strstr(value, ",,#");
				if (ret!=NULL)
				{
					*ret=0;
					*(ret+1)=0;
					*(ret+2)=0;
					ret+=3;
				}
				else
					return NULL;
				
				if (config==NULL)
					res=ProcessValue(value, (LIST_TYPE*)&entrydef[i], entrydef[i].value);
				else
					res=ProcessValue(value, (LIST_TYPE*)&entrydef[i], (void**) &config[entrydef[i].offset]);
				if (result!=NULL)
					result[i] = res;
				
				
			}
			else if (entrydef[i].flags & TYPE_USERDEF)
			{
				//TODO Call additionalData
				//ret = (char*)(FUNCTION_TYPE(entrydef[i].additionaldata))(&entrydef[i], (char*)value, entrydef[i].value);
				return ret;
			}
			else
			{
				ret = strchr(value,'#');
				if (ret!=NULL)
					*ret=0;
				else
					return NULL;
				ret+=1;
				if (config==NULL)
					res=ProcessValue(value, (LIST_TYPE*)&entrydef[i], entrydef[i].value);
				else
					res=ProcessValue(value, (LIST_TYPE*)&entrydef[i], (void**) &config[entrydef[i].offset]);
				
				if (result!=NULL)
					result[i] = res;
			}

			
			return ret;
		}
	}
	return NULL;
}

int ProcessValue(const char *value, LIST_TYPE *entry, void **retval)
{
	int val, len;
	int i;
	int ret=0;
	double vald;
	char *result1, *result2;

	if (entry->flags & TYPE_FLOAT)
	{
		vald = atof(value);
		if (!(entry->flags & DONT_CHECK))
			if ((vald < entry->from.d) || (vald > entry->to.d))
			{
				vald = entry->def.d;
				ret = DEFAULT;
			}
			else
				ret = PASSED;
	
		*(double*)(retval) = vald;

	}
	else if (entry->flags & TYPE_INTEGER)
	{
		val = atoi(value);
		if (!(entry->flags & DONT_CHECK))
		{
			if ((val < entry->from.i) || (val > entry->to.i))
			{
				val = entry->def.i;
				ret = DEFAULT;
			}
			else
				ret = PASSED;
		}
		else 
			ret = PASSED;
	
		*(int*)(retval) = val;
	
	}
	else if (entry->flags & TYPE_BOOLEAN)
	{
		if (strcmp(value,"TRUE")==0)
		{
			*(int*)(retval) = TRUE;
			ret = PASSED;
		}
		else if (strcmp(value, "FALSE")==0)
		{
			*(int*)(retval) = FALSE;
			ret = PASSED;
		}
		else
		{
			*(int*)(retval) = entry->def.i;
			ret = DEFAULT;
		}
	}
	else if (entry->flags & TYPE_BINARY)
	{
		//TODO 
	}
	else if (entry->flags & TYPE_STRING)
	{
		len = strlen(value);
		if (!(entry->flags & DONT_CHECK))
			if ((len < entry->from.i) || (len > entry->to.i))
			{
				len = strlen((char*)entry->def.c);
				value = (char*)entry->def.c;
				ret = DEFAULT;
			}
			else
				ret = PASSED;
						
		// it is valid
		if (value!=NULL)
		{
			//if (entry->flags & ALLOC_DYNAMIC)
			*retval = malloc(len+1);
			strcpy((char*)(*retval),value);
		}
	}
	else if (entry->flags & TYPE_LIST)
	{
		//TODO
		/*result1 = value;
		for (i = 0; i < entry->to.i;i++)
		{
			result2 = strstr(result1, ",,");
			if (result2==NULL)
				break;
			*result2 = 0;
			result2+=2;
			//ProcessValue(result1, entry, NULL);
			result1 = result2;
		}*/
	}
	else 
	{
		/*ERROR No type set*/
		ret = NOT_A_TYPE;
	}
	return ret;
}
/**
	This function writes the configuration file. Depending on the parameters supplied.
*//*
int WriteConfiguration(const XCHAR *filename, const XCHAR *section, const CONFENTRY *entrydef, const int size, unsigned char* config, const int CharType)
{
	if (section==NULL)
		return WriteCompleteConfiguration(filename, entrydef, size, config, CharType, DEF_BUF_SIZE);

	return 0;
}
*/
/**
	This function writes the complete config, it will remove all other data - all other sections, which are not declared
	in your project. This will leave a clean config, in the character format specified.
	
*/

int WriteCompleteConfigurationW(const wchar_t *filename, const CONFENTRY *entrydef, const int size, unsigned char* config, const int CharType, int maxbufsize)
{
	
	char *buffer;
	int ret;

	buffer = (char*) malloc(maxbufsize+4);
	if (buffer==NULL)
		return -1;

	if (CharType==UTF_16)
	{
		//ret = PrepareConfigBufferW
	}
	else
		ret = PrepareConfigBufferA(entrydef, size, buffer, maxbufsize);

	if (ret>BUFFER_TOO_SMALL)
		ret = SaveConfigFileW(filename, buffer, maxbufsize -ret);
	else
		ret = SaveConfigFileW(filename, buffer, maxbufsize);

	free(buffer);

	return ret;
}

int WriteCompleteConfigurationA(const char *filename, const CONFENTRY *entrydef, const int size, unsigned char* config, const int CharType, int maxbufsize)
{
	
	char *buffer;
	int ret;

	buffer = (char*) malloc(maxbufsize+4);
	if (buffer==NULL)
		return -1;

	if (CharType==UTF_16)
	{
		//ret = PrepareConfigBufferW
	}
	else
		ret = PrepareConfigBufferA(entrydef, size, buffer, maxbufsize);

	if (ret>BUFFER_TOO_SMALL)
		ret = SaveConfigFileA(filename, buffer, maxbufsize - ret);
	else
		ret = SaveConfigFileA(filename, buffer, maxbufsize);

	free(buffer);

	return ret;
}

int WriteCompleteConfigurationC(const char *filename, const CONFENTRY *entrydef, const int size, unsigned char* config, const int CharType, int maxbufsize)
{
	
	char *buffer;
	int ret;

	buffer = (char*) malloc(maxbufsize+4);
	if (buffer==NULL)
		return -1;

	if (CharType==UTF_16)
	{
		//ret = PrepareConfigBufferW
	}
	else
		ret = PrepareConfigBufferA(entrydef, size, buffer, maxbufsize);

	if (ret>BUFFER_TOO_SMALL)
		ret = SaveConfigFileC(filename, buffer, maxbufsize - ret);
	else
		ret = SaveConfigFileC(filename, buffer, maxbufsize);

	free(buffer);

	return ret;
}


/**
	This function will only update the section specified and leaves the rest untouched.
*/

int WriteSectionA(const char *filename, const char *section, const CONFENTRY *entrydef, const int size, const int CharType, int maxbufsize)
{
	int ret;
	int len;
	int nsize=0;
	int remaining=maxbufsize;

	unsigned char **buffer;
	unsigned char **newbuf;
	
	*buffer = (unsigned char*) malloc(maxbufsize);
	if (*buffer==NULL)
		return -1;

	*newbuf = (unsigned char*) malloc(maxbufsize);
	if (*newbuf==NULL)
	{
		free(*buffer);
		return -1;
	}


	if((ret=LoadConfigFileA(filename, buffer, maxbufsize))==0)
	{
		/*
			Convert the buffer from ASCII to UTF-16/32 or vice versa
		*/
		if (TestBuffer(*buffer)==ASCII && CharType!=ASCII)
			expandbuffer((char*)*buffer, ret, (wchar_t*) newbuf);
		else
			shrinkbuffer((wchar_t*)*buffer, ret/sizeof(wchar_t),(char*)newbuf);

		if (CharType==ASCII)
		{
			char* result1, *result2;
			char* begin, *buf, *nbuf;
			buf = (char*) *buffer;
			nbuf = (char*) *newbuf;

			result1 = FindSectionA(buf, section, &begin);
			if (result1!=NULL)
				result2 = FindSectionEndA(result1);
			else
			{
				free(*buffer);
				free(*newbuf);
				return -1;
			}

			/* Copy everything in front of the section */
			len = begin - result1;
			if (remaining-len > 16)
				strncpy(nbuf, buf, len);
			
			remaining -= len;
			
			/* Fill buffer */
			nsize = PrepareConfigBufferA(entrydef, size, result1, remaining);
			if (nsize>2)
				nsize += len;

			/* Copy the rest of the file */
			if (result2!=NULL)
			{
				strncpy(nbuf, result2, remaining);
			}
			else
			{
				/* Write End of File */
				strcpy(nbuf,"\\\\END OF FILE//");
				nsize+=16;
			}
			SaveConfigFileA(filename, nbuf, nsize);
		}
		else
		{

		}

	}

	free(*buffer);
	free(*newbuf);
	return ret;
}

int PrepareConfigBufferA(const CONFENTRY *entrydef, const int size, char* buffer, int remaining)
{
	int i;
	int res;

	strcpy(buffer, "//CONFIGURATION FILE\\\\\r\n");
	buffer+=24;
	remaining-=24;
	for (i = 0;i<size;i++)
	{
		if (entrydef[i].flags == TYPE_SECTION)
			res = snprintf(buffer, remaining, SAVE_SECTION_A, entrydef[i].key, entrydef[i].description);
		else if (entrydef[i].flags & TYPE_INTEGER)
			res = snprintf(buffer, remaining, SAVE_INTEGER_A, entrydef[i].description, PrintTypeA(entrydef[i].flags), entrydef[i].from.i, entrydef[i].to.i, entrydef[i].def.i, entrydef[i].flags, entrydef[i].key, (int)*entrydef[i].value);
		else if (entrydef[i].flags & TYPE_STRING)
			res = snprintf(buffer, remaining, SAVE_STRING_A, entrydef[i].description, PrintTypeA(entrydef[i].flags), entrydef[i].from.i, entrydef[i].to.i, entrydef[i].def.c, entrydef[i].flags, entrydef[i].key, entrydef[i].value);
		else if (entrydef[i].flags & TYPE_BOOLEAN)
			res = snprintf(buffer, remaining, SAVE_BOOLEAN_A, entrydef[i].description, PrintTypeA(entrydef[i].flags), PrintValueA(entrydef[i].from.i), PrintValueA(entrydef[i].to.i), PrintValueA(entrydef[i].def.i), entrydef[i].flags, entrydef[i].key, PrintValueA((int)*entrydef[i].value));
		else if (entrydef[i].flags & TYPE_FLOAT)
			res = snprintf(buffer, remaining, SAVE_FLOAT_A, entrydef[i].description, PrintTypeA(entrydef[i].flags), entrydef[i].from.d, entrydef[i].to.d, entrydef[i].def.d, entrydef[i].flags, entrydef[i].key, *(double*)entrydef[i].value);
		else
			res = snprintf(buffer, remaining, "// %s \\V:%s,%d,%d,%d \\H:%d\r\n*%s=%d#\r\n\r\n", entrydef[i].description, PrintTypeA(entrydef[i].flags), entrydef[i].from.i, entrydef[i].to.i, entrydef[i].def.i, entrydef[i].flags, entrydef[i].key, entrydef[i].value);
		
		buffer+=res;
		remaining-=res;
		*buffer = 0;
		if (remaining<=16)
			return BUFFER_TOO_SMALL;
	}

	strcpy(buffer,"\\\\END OF FILE//");
	remaining-=16;

	return remaining;
}

int PrepareConfigBuffer(const CONFENTRY *entries, const int size, unsigned char *buf, int maxbuf, unsigned char* config)
{
	int i;

	if (config!=NULL)
	{
		/* uses offset */
		for (i=0;i<size;i++)
		{
			/* Write */
		}
	}
	else
	{
		for (i=0;i<size;i++)
		{

		}
	}
	return 0;
}

/*
		
*/
char* PrintTypeA(int flags)
{
	if (flags & TYPE_INTEGER)
		return "TYPE_INTEGER";
	else if (flags & TYPE_BOOLEAN)
		return "TYPE_BOOLEAN";
	else if (flags & TYPE_STRING)
		return "TYPE_STRING";
	else if (flags & TYPE_BINARY)
		return "TYPE_BINARY";
	else if (flags & TYPE_FLOAT)
		return "TYPE_FLOAT";
	else if (flags & TYPE_USERDEF)
		return "TYPE_USERDEF";
	else if (flags & TYPE_LIST)
		return "TYPE_LIST";

	return "TYPE_ERROR";
}

/*

*/
wchar_t* PrintTypeW(int flags)
{
	if (flags & TYPE_INTEGER)
		return L"TYPE_INTEGER";
	else if (flags & TYPE_BOOLEAN)
		return L"TYPE_BOOLEAN";
	else if (flags & TYPE_STRING)
		return L"TYPE_STRING";
	else if (flags & TYPE_BINARY)
		return L"TYPE_BINARY";
	else if (flags & TYPE_FLOAT)
		return L"TYPE_FLOAT";
	else if (flags & TYPE_USERDEF)
		return L"TYPE_USERDEF";
	else if (flags & TYPE_LIST)
		return L"TYPE_LIST";

	return L"TYPE_ERROR";
}

/*

*/
char* PrintValueA(int val)
{
	if (val==0)
		return "FALSE";
	else
		return "TRUE";
}

/*

*/
wchar_t* PrintValueW(int val)
{
	if (val==0)
		return L"FALSE";
	else
		return L"TRUE";
}



int tobase64(const char* str, int len, char* buf)
{
	int a=0, i=0;

	do
	{
		encodeblock(&str[i], &buf[a], min(len,3));
		i+=3;
		a+=4;
		len -= 3;
	}while(len > 0);

	return a;
}

void encodeblock( const char* in, char* out, int len )
{
	out[0] = cb64[ in[0] >> 2 ];
	out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
	out[2] = (char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
	out[3] = (char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}
