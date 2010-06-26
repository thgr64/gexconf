// ConfHelper.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "Conf.h"
#include <tchar.h>
#include <math.h>
#include <Windows.h>

struct Filename
{
	char* data;
	int size;
};

LIST_TYPE entries[] = 
{
	{0,260,4, TYPE_STRING, offsetof(Filename, data)},
	{0,260,0,TYPE_INTEGER, offsetof(Filename, size)}
};


int glShared=0;
int glCase = -1;
int glDisks = 3;
int glGUI = FALSE;
// This is not allowed use SetConfigEntryDerfString instead
//char glPath[] = "C:\\windows\\blablabla\\interface\\bla\\";
char *glPath;
double PI = atan(1.0)*4.0;
struct myconfig
{
	int a;
	int b;
	int c;
	char *d;
	int e;
	double f;
};

CONFENTRY entry[] = 
{
	{0,10,0,TYPE_SECTION, 0,"Internal", "Data related to the internal structures like memory and services", NULL , NULL},
	{16,1024,32,TYPE_INTEGER,offsetof(myconfig,a), "SharedMemory", "Defines the amount of shared memory", NULL, (void**)&glShared},
	{0,0x9fffffff,0x9fffffff,TYPE_INTEGER,offsetof(myconfig,b), "AutoLoadDisk", "Automatically load all disks specified Disk A: = 1 B:=2 C: = 4 and so on", NULL, (void**)&glDisks},
	{TRUE,FALSE,FALSE,TYPE_BOOLEAN,offsetof(myconfig,c), "HideGUI", "Sets the status of the graphical user interace", NULL , (void**)&glGUI},

	//{0,10,0,TYPE_LIST | ALLOC_DYNAMIC,0, "Filenames", "Cool thing", entries , NULL},
	{0,10,0,TYPE_SECTION, 0,"GUI", "Everything that can be changed using the default GUI", NULL , NULL},
	{3,260,0,TYPE_STRING /*| ALLOC_DYNAMIC*/,offsetof(myconfig,d), "Path", "Path to the user directory", NULL , (void**) &glPath},
	{TRUE,FALSE,TRUE,TYPE_BOOLEAN,offsetof(myconfig,e), "CaseSensitive", "Should the case be regarded", NULL , (void**)&glCase},
	{0,0,0,TYPE_FLOAT,offsetof(myconfig,f), "PI", "atan(1)*4", NULL , (void**)&PI},

};

int _tmain(int argc, _TCHAR* argv[])
{
	SetConfigEntryDefString(&entry[5], "/Hello/world/path/C");
	SetConfigEntryFloat(&entry[7],3.0,3.2,PI);
	FillRemaining(&entry[0], 8,NULL,NULL);
	WriteCompleteConfiguration(L"test.conf",entry, 8, NULL, ANSI, 4096); 
	//LoadConfiguration(L"test.conf",NULL, entry, 8, NULL, NULL, 4096); 
	
	return 0;
}

