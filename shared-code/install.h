#define	WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define WIN32_EXTRALEAN
#include <windows.h>
#include <shlobj.h>
#include <winver.h>
#include <crtdbg.h>
#include <string.h>
#include <stdio.h>

extern "C" WINSHELLAPI void WINAPI SHFree( LPVOID);

template<class T> class Smart	{
public:
	T *pT;

	Smart() : pT(NULL) {}
	Smart(int cb) : pT(new T[cb]) {}
	Smart(T* p) : pT(p) {}
	~Smart() { if(pT)delete pT; }

	Smart& operator=(T* p) { if(pT)delete pT; pT=p; return *this; }
	operator T* () { return pT; }

//	T& operator[](int ndx) { return pT[ndx]; }

	T* Detach() { T* rv = pT; pT=NULL; return rv; }
};
typedef	Smart<char> STRING;

#define APPEND_SLASH(str)		if((str)[strlen(str)-1]!='\\')strcat(str,"\\")

HINSTANCE theInstance;

LPSTR strFETCH_REG_KEY(HKEY hRoot,LPCSTR subKey,LPCSTR val)
{
HKEY hkey;
	if(RegOpenKeyEx(hRoot,subKey,0,KEY_QUERY_VALUE,&hkey)!=ERROR_SUCCESS)
		return NULL;
DWORD kType,cb=0;
STRING rv;
	if(RegQueryValueEx(hkey,val,NULL,&kType,NULL,&cb)==ERROR_SUCCESS && kType==REG_SZ){
		rv= new char[cb];
		_ASSERT(rv!=NULL);
		if(RegQueryValueEx(hkey,val,NULL,&kType,(LPBYTE)(LPSTR)rv,&cb)!=ERROR_SUCCESS)
			rv=NULL;
	}
	RegCloseKey(hkey);
	return rv.Detach();
}

BOOL strSET_REG_KEY(HKEY hRoot,LPCSTR subKey,LPCSTR valName,LPCSTR val)
{
HKEY hkey;
DWORD dw;
	if(RegCreateKeyEx(hRoot,subKey,0,REG_NONE,REG_OPTION_NON_VOLATILE,KEY_READ|KEY_WRITE,NULL,&hkey,&dw)!=ERROR_SUCCESS)
		return FALSE;
BOOL rv = (RegSetValueEx(hkey,valName,0,REG_SZ,(LPBYTE)val,strlen(val)+1)==ERROR_SUCCESS);
	RegCloseKey(hkey);
	return rv;
}

void MAKE_PATH(LPCSTR path)
{
STRING tmp(strlen(path)+1);
LPCSTR t0=path;
LPSTR t1=tmp;
	while(*t0){
		if((*t0)=='\\'){
			*t1=0;
			CreateDirectory(tmp,NULL);
		}
		*(t1++)=*(t0++);
	}
	*t1=0;
	CreateDirectory(tmp,NULL);
}

BOOL ADDMENU(LPCSTR menu,LPCSTR item,LPCSTR path,LPCSTR program)
{
STRING stm = strFETCH_REG_KEY(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders","Programs");
	if(!stm)
		return FALSE;
int pil = 1+strlen(path)+1+strlen(program)+1+1;
STRING pi(pil);
	strcpy(pi,path);
	APPEND_SLASH(pi);
	strcat(pi,program);
int ipl = strlen(stm)+1+strlen(menu)+1+strlen(item)+4+1;
STRING ip(ipl);
	memmove(ip,stm,strlen(stm)+1);
	APPEND_SLASH(ip);
	strcat(ip,menu);
	MAKE_PATH(ip);
	APPEND_SLASH(ip);
	strcat(ip,item);
	strcat(ip,".lnk");
IShellLink* sl = NULL;
IPersistFile* pf = NULL;
BOOL rv = FALSE;
	do{
	HRESULT hrv = CoCreateInstance(CLSID_ShellLink,NULL,CLSCTX_INPROC_SERVER,IID_IShellLink,(LPVOID*)&sl);
		if(!SUCCEEDED(hrv))
			break;
		sl->SetDescription(item);
		sl->SetPath(pi);
		hrv = sl->QueryInterface(IID_IPersistFile,(LPVOID*)&pf);
		if(!SUCCEEDED(hrv))
			break;
	WORD wsz[MAX_PATH];
		MultiByteToWideChar(CP_ACP,0,ip,-1,wsz,MAX_PATH);
		hrv = pf->Save(wsz,TRUE);
		if(SUCCEEDED(hrv))
			rv=TRUE;
	}while(FALSE);
	if(pf)
		pf->Release();
	if(sl)
		sl->Release();
	return rv;
}

FILE* CREATE_INF_FILE(LPCSTR path,LPCSTR file)
{
STRING fn(strlen(path)+1+strlen(file)+1);
	strcpy(fn,path);
	APPEND_SLASH(fn);
	strcat(fn,file);
	return fopen(fn,"wt");
}

BOOL INSTALLFILE(LPCSTR res,LPCSTR path,LPCSTR file)
{
STRING temp(MAX_PATH);
	if(!GetTempPath(MAX_PATH,temp)) return FALSE;
STRING tf(MAX_PATH);
	if(!GetTempFileName(temp,"KGI",0,tf)) return FALSE;
HRSRC hrsrc = FindResource(NULL,res,MAKEINTRESOURCE(RT_RCDATA));
	if(!hrsrc) return FALSE;
DWORD sor = SizeofResource(NULL,hrsrc);
	if(!sor) return FALSE;
HGLOBAL hglobal = LoadResource(NULL,hrsrc);
	if(!hglobal) return FALSE;
LPVOID lpv = LockResource(hglobal);
	if(!lpv) return FALSE;
HANDLE hf = CreateFile(tf,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_TEMPORARY,NULL);
	if(!hf) return FALSE;
DWORD written = 0;
	if(!WriteFile(hf,lpv,sor,&written,NULL) || written!=sor){
		CloseHandle(hf);
		return FALSE;
	}
	CloseHandle(hf);
STRING toKill(strlen(tf)+1);
	strcpy(toKill,tf);
	for(int tmp=strlen(tf)-1;tmp>0 && ((tf[tmp])!='\\');tmp--);
	if(tf[tmp]=='\\')
		tf[tmp++]=0;
STRING nothing(_MAX_PATH);
UINT nothingLength=_MAX_PATH;
	if(VerInstallFile(0,&tf[tmp],(LPSTR)file,tf,(LPSTR)path,NULL,nothing,&nothingLength)){
		DeleteFile(toKill);
		return FALSE;
	}
	DeleteFile(toKill);
	return TRUE;
}

LPCSTR pdTitle, pdPrompt;
char pdPath[_MAX_PATH];
BOOL CALLBACK pathDlgProc(HWND hwnd,UINT uMsg,WPARAM wP,LPARAM lP)
{
	switch(uMsg){
	case WM_INITDIALOG:
		SetWindowText(hwnd,pdTitle);
		SetDlgItemText(hwnd,IDC_PROMPT,pdPrompt);
		SetDlgItemText(hwnd,IDC_PATH,pdPath);
		SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wP)){ // ID
		case IDC_BROWSE:
			switch(HIWORD(wP)){
			case BN_CLICKED:
			{
			BROWSEINFO bi;
				memset(&bi,0,sizeof(bi));
				bi.hwndOwner=hwnd;
				bi.pszDisplayName=pdPath;
				bi.lpszTitle="Select Folder..";
				bi.ulFlags=BIF_RETURNONLYFSDIRS;
			LPITEMIDLIST lpidl=SHBrowseForFolder(&bi);
				if(lpidl){
					SHGetPathFromIDList(lpidl,pdPath);
					SHFree(lpidl);
					SetDlgItemText(hwnd,IDC_PATH,pdPath);
				}
			}
				return 1;
			}
			break;
		case IDOK:
			switch(HIWORD(wP)){
			case BN_CLICKED:
				if(GetDlgItemText(hwnd,IDC_PATH,pdPath,sizeof(pdPath)))
					EndDialog(hwnd,IDOK);
				else
					// *** Error message
					EndDialog(hwnd,IDCANCEL);
				return 1;
			}
			break;
		case IDCANCEL:
			switch(HIWORD(wP)){
			case BN_CLICKED:
				EndDialog(hwnd,IDCANCEL);
				return 1;
			}
			break;
		};
		break;
	}
	return 0;
}

LPSTR REQUESTPATH(LPCSTR title,LPCSTR prompt,LPCSTR defPath)
{
	pdTitle=title;pdPrompt=prompt;
	strcpy(pdPath,defPath);
	if(DialogBox(NULL,MAKEINTRESOURCE(IDD_PATH),NULL/*Parent*/,(DLGPROC)&pathDlgProc)!=IDOK)
		return NULL;
STRING rv(strlen(pdPath)+1);
	strcpy(rv,pdPath);
	return rv.Detach();
}

HKEY uninstallKey(LPCSTR regKey) {
	STRING rk(100+strlen(regKey)+1);
	sprintf(rk,"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s",regKey);
	HKEY rv = NULL;
	DWORD dw;
	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE,rk,0,REG_NONE,REG_OPTION_NON_VOLATILE,KEY_READ|KEY_WRITE,NULL,&rv,&dw)!=ERROR_SUCCESS)
		return NULL;
	return rv;
}

BOOL REG_UNINSTALL_COMMAND(LPCSTR regKey,LPCSTR dName,LPCSTR iPath,LPCSTR iFile,LPCSTR iSection)
{
	HKEY hKey = uninstallKey(regKey);
	if(!hKey)
		return FALSE;
	BOOL rv=FALSE;
	do{
		if(RegSetValueEx(hKey,"DisplayName",0,REG_SZ,(LPBYTE)dName,strlen(dName)+1)!=ERROR_SUCCESS)
			break;
	STRING us(50+strlen(iPath)+1+strlen(iFile)+strlen(iSection)+1);
		strcpy(us,"RunDll32 setupapi.dll,InstallHinfSection ");
		strcat(us,iSection);
		strcat(us," 132 ");
		strcat(us,iPath);
		APPEND_SLASH(us);
		strcat(us,iFile);
		if(RegSetValueEx(hKey,"UninstallString",0,REG_SZ,(LPBYTE)(LPCSTR)us,strlen(us)+1)!=ERROR_SUCCESS)
			break;
		rv=TRUE;
	}while(FALSE);
	RegCloseKey(hKey);
	return rv;
}
BOOL REG_UNINSTALL_ICON(LPCSTR regKey,LPCSTR path,LPCSTR file,int n) {
	HKEY hKey = uninstallKey(regKey);
	if(!hKey)
		return FALSE;
	STRING uis(strlen(path)+1+strlen(file)+7);
	strcpy(uis,path);
	APPEND_SLASH(uis);
	strcat(uis,file);
	char tmp[8];
	sprintf(tmp,";%d",n);
	strcat(uis,tmp);
	BOOL rv = TRUE;
	if(RegSetValueEx(hKey,"DisplayIcon",0,REG_SZ,(LPBYTE)(LPCSTR)uis,strlen(uis)+1)!=ERROR_SUCCESS)
		rv = FALSE;
	RegCloseKey(hKey);
	return rv;
}
BOOL REG_UNINSTALL_COMMENT(LPCSTR regKey,LPCSTR comment) {
	HKEY hKey = uninstallKey(regKey);
	if(!hKey)
		return FALSE;
	BOOL rv = TRUE;
	if(RegSetValueEx(hKey,"lComment",0,REG_SZ,(LPBYTE)comment,strlen(comment)+1)!=ERROR_SUCCESS)
		rv = FALSE;
	RegCloseKey(hKey);
	return rv;
}
BOOL REG_UNINSTALL_VERSION(LPCSTR regKey,LPCSTR version) {
	HKEY hKey = uninstallKey(regKey);
	if(!hKey)
		return FALSE;
	BOOL rv = TRUE;
	if(RegSetValueEx(hKey,"DisplayVersion",0,REG_SZ,(LPBYTE)version,strlen(version)+1)!=ERROR_SUCCESS)
		rv = FALSE;
	RegCloseKey(hKey);
	return rv;
}
BOOL REG_UNINSTALL_LOCATION(LPCSTR regKey,LPCSTR location) {
	HKEY hKey = uninstallKey(regKey);
	if(!hKey)
		return FALSE;
	BOOL rv = TRUE;
	if(RegSetValueEx(hKey,"InstallLocation",0,REG_SZ,(LPBYTE)location,strlen(location)+1)!=ERROR_SUCCESS)
		rv = FALSE;
	RegCloseKey(hKey);
	return rv;
}
BOOL REG_UNINSTALL_PUBLISHER(LPCSTR regKey,LPCSTR publisher) {
	HKEY hKey = uninstallKey(regKey);
	if(!hKey)
		return FALSE;
	BOOL rv = TRUE;
	if(RegSetValueEx(hKey,"Publisher",0,REG_SZ,(LPBYTE)publisher,strlen(publisher)+1)!=ERROR_SUCCESS)
		rv = FALSE;
	RegCloseKey(hKey);
	return rv;
}
BOOL REG_UNINSTALL_URLS(LPCSTR regKey,LPCSTR about,LPCSTR update) {
	HKEY hKey = uninstallKey(regKey);
	if(!hKey)
		return FALSE;
	BOOL rv = TRUE;
	if(RegSetValueEx(hKey,"URLInfoAbout",0,REG_SZ,(LPBYTE)about,strlen(about)+1)!=ERROR_SUCCESS)
		rv = FALSE;
	if(RegSetValueEx(hKey,"URLUpdateInfo",0,REG_SZ,(LPBYTE)update,strlen(update)+1)!=ERROR_SUCCESS)
		rv = FALSE;
	RegCloseKey(hKey);
	return rv;
}

#define	INF_FILE_HEADER(i) fprintf(i,"[Version]\nSignature=\"$CHICAGO$\"\n\n")
#define INF_FILE_SECTION(i,s) fprintf(i,"\n[%s]\n",s)
#define	INF_UNINSTALL_REG(i,p) fprintf(i,"HKLM,Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s\nHKLM,Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s,DisplayName\nHKLM,Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\%s,UninstallString\n",p,p,p)
#define INF_MENU_GROUP(i,n,m) fprintf(i,"setup.ini, progman.groups,,\"group%d=%s\"\n",n,m)
#define INF_MENU_ITEM(i,n,m) fprintf(i,"setup.ini, group%d,, \"\"\"%s\"\"\"\n",n,m);
#define INF_REMOVE_ROOT(i,g,r) fprintf(i,"HKLM,Software\\Microsoft\\Windows\\CurrentVersion\\DeleteFiles\\%s,,,\"%s\"\n",g,r)
#define INF_REMOVE_FILE(i,g,f) fprintf(i,"HKLM,Software\\Microsoft\\Windows\\CurrentVersion\\DeleteFiles\\%s,%s,,\"%s\"\n",g,f,f)
#define INF_REMOVE_HELP_FILE(i,g,f) {INF_REMOVE_FILE(i,g,f".hlp");INF_REMOVE_FILE(i,g,f".cnt");INF_REMOVE_FILE(i,g,f".GID");INF_REMOVE_FILE(i,g,f".FTS");}

LPSTR GET_SHORT_PATH(LPCSTR path)
{
char tmp;
DWORD len = GetShortPathName(path,&tmp,1);
	if(!len)
		return NULL;
STRING rv(len+1);
	if(!GetShortPathName(path,rv,len+1))
		return NULL;
	return rv.Detach();
}

BOOL Install(void);

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE,LPSTR,int)
{
	theInstance=hInstance;
	CoInitialize(NULL);
	Install();
	CoUninitialize();
	return 0;
}
