#include "resource.h"
#include "../shared-code/install.h"

#define	KINAME "PumpKIN 2.5"
#define SKINAME "PumpKIN"

BOOL Install(void)
{
STRING dPath = strFETCH_REG_KEY(HKEY_LOCAL_MACHINE,"Software\\Klever Group, Inc.",SKINAME "Path");
STRING kPath = strFETCH_REG_KEY(HKEY_LOCAL_MACHINE,"Software\\Klever Group, Inc.","KINPath");
LPCSTR qPath = ((LPCSTR)dPath)?(LPCSTR)dPath:(((LPCSTR)kPath)?(LPSTR)kPath:"C:\\Program Files\\Klever\\Nothings");
STRING path = REQUESTPATH("  " KINAME,"\nEnter destination path:",qPath);
	if(!path)
		return NULL;

#ifdef	K_ANNED
STRING sysDir(_MAX_PATH);
	GetSystemDirectory(sysDir,_MAX_PATH);
	INSTALLFILE("mfc42.dl_",sysDir,"mfc42.dll");
#endif

	MAKE_PATH(path);
STRING shortPath = GET_SHORT_PATH(path);
	if(!shortPath){
		MessageBox(NULL,"Failed to install " KINAME " in specified directory",NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}

	if(!(
		INSTALLFILE(SKINAME ".ex_",path,SKINAME ".exe") &&
		INSTALLFILE(SKINAME ".hl_",path,SKINAME ".hlp") && 
		INSTALLFILE(SKINAME ".cn_",path,SKINAME ".cnt")
	)){
		MessageBox(NULL,"Failed to install " KINAME " in specified directory",NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	ADDMENU("Accessories\\Klever Co.",SKINAME,path, SKINAME ".exe");

	strSET_REG_KEY(HKEY_LOCAL_MACHINE,"Software\\Klever Group, Inc.",SKINAME "Path",path);
	strSET_REG_KEY(HKEY_LOCAL_MACHINE,"Software\\Klever Group, Inc.","KINPath",path);

FILE* inf=CREATE_INF_FILE(path,SKINAME ".INF");
	if(!inf){
		MessageBox(NULL,"Failed to install " KINAME,NULL,MB_ICONERROR|MB_OK);
		return FALSE;
	}
	INF_FILE_HEADER(inf);
	INF_FILE_SECTION(inf,"Uninstall");
	fprintf(inf,"AddReg=kFiles\nDelReg=kReg\nUpdateInis=kMenu\n");
	INF_FILE_SECTION(inf,"kFiles");
	INF_REMOVE_ROOT(inf,SKINAME "Files",shortPath);
	INF_REMOVE_FILE(inf,SKINAME "Files",SKINAME ".exe");
	INF_REMOVE_HELP_FILE(inf,SKINAME "Files",SKINAME);
	INF_REMOVE_FILE(inf,SKINAME "Files",SKINAME ".inf");
	INF_FILE_SECTION(inf,"kReg");
	INF_UNINSTALL_REG(inf,SKINAME);
	INF_FILE_SECTION(inf,"kMenu");
	INF_MENU_GROUP(inf,1,"Accessories\\Klever Co.");
	INF_MENU_ITEM(inf,1,SKINAME);
	fclose(inf);

	REG_UNINSTALL_COMMAND(SKINAME,"Klever " KINAME,shortPath,SKINAME ".INF","Uninstall");

	MessageBox(NULL,KINAME " installed successfully, you may now run it from Programs/Accessories/Klever Co. menu or remove it using Control Panel Add/Remove Programs applet."," Rejoice!",MB_ICONINFORMATION|MB_OK);

	return TRUE;
}
