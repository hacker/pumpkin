#ifndef	__KHELPERS_H
#define __KHELPERS_H

#include <shlobj.h>

extern "C" WINSHELLAPI void WINAPI SHFree( LPVOID);

namespace Klever {

	inline BOOL BrowseForFolder(CString& folder,LPCTSTR title=NULL,CWnd* pParent=NULL) {
	BROWSEINFO bi;
		memset(&bi,0,sizeof(bi));
		if(pParent)
			bi.hwndOwner=pParent->GetSafeHwnd();
	CString rv;
		bi.pszDisplayName=rv.GetBuffer(MAX_PATH);
		bi.lpszTitle=title;
		bi.ulFlags=BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST lpidl = SHBrowseForFolder(&bi);
		if(lpidl){
			SHGetPathFromIDList(lpidl,bi.pszDisplayName);
			SHFree(lpidl);
			rv.ReleaseBuffer();
			folder=rv;
			return TRUE;
		}
		rv.ReleaseBuffer();
		return FALSE;
	}
	inline BOOL BrowseForFolder(CString& folder,UINT idTitle,CWnd* pParent=NULL) {
	CString title;
		VERIFY(title.LoadString(idTitle));
		return BrowseForFolder(folder,title,pParent);
	}
	inline CString GluePathAndFile(LPCTSTR path,LPCTSTR file) {
	CString rv = path;
		while((!rv.IsEmpty()) && rv[rv.GetLength()-1]=='\\')
			rv=rv.Left(rv.GetLength()-1);
		rv+='\\';
		while(*file && *file=='\\')
			file++;
		rv+=file;
		return rv;
	}
	inline UINT TokenizeString(CStringList& rv,LPCTSTR string,LPCTSTR delimiter) {
	CString s = string;
	int found;
	int delength = strlen(delimiter);
	int rvc = 0;
		while((found=s.Find(delimiter))>=0){
			rv.AddTail(s.Left(found));
			rvc++;
			s=s.Mid(found+delength);
		}
		if(!s.IsEmpty()){
			rv.AddTail(s);
			rvc++;
		}
		return rvc;
	}
	inline BOOL LogRecord(LPCTSTR logFile,LPCTSTR logRecord) {
		try{
		CFile f(logFile,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyWrite);
			f.SeekToEnd();
		CString s = CTime::GetCurrentTime().Format("[%c] ")+logRecord+"\r\n";
			f.Write((LPCTSTR)s,s.GetLength());
		}catch(CException* e){
			e->Delete();
			return FALSE;
		}
		return TRUE;
	}
	inline BOOL ReadString(CFile* file,CString& rv) {
		rv.Empty();
	int nBuffer = 256;
	TCHAR* ch = rv.GetBuffer(nBuffer);
	int nPos = 0;
	BOOL bRV = FALSE;
		for(;;){
		TCHAR c;
			try{
				if(file->Read(&c,sizeof(c))!=sizeof(c))
					break;
				bRV=TRUE;
			}catch(CException* e){
				e->Delete();
				TRACE0("Exception in ReadString\n");
				return FALSE;
			}
			if(nPos>=(nBuffer-1)){
				rv.ReleaseBuffer();
				ch = rv.GetBuffer(nBuffer=nBuffer+256);
				ASSERT(ch);
			}
			if(c=='\n')
				break;
			ch[nPos++]=c;
		}
		ch[nPos]=0;
		for(;;){
			nPos--;
			if(nPos<0)
				break;
			if(ch[nPos]!='\r')
				break;
			ch[nPos]=0;
		}
		rv.ReleaseBuffer();
		rv.FreeExtra();
		return bRV;
	}

	inline int LoadStringList(CStringList& list,LPCTSTR section) {
	CString n;
		list.RemoveAll();
	CWinApp* app = AfxGetApp();
		ASSERT(app);
		for(int tmp=0;;tmp++){
			n.Format("%d",tmp);
		CString str = app->GetProfileString(section,n,NULL);
			if(str.IsEmpty())
				break;
			list.AddTail(str);
		}
		return tmp;
	}
	inline int SaveStringList(CStringList& list,LPCTSTR section) {
	CString n;
	CWinApp* app = AfxGetApp();
		ASSERT(app);
	POSITION p = list.GetHeadPosition();
		for(int tmp=0;p;tmp++){
			n.Format("%d",tmp);
			app->WriteProfileString(section,n,list.GetNext(p));
		}
		n.Format("%d",tmp);
		app->WriteProfileString(section,n,NULL);
		return tmp;
	}

	inline BOOL WriteProfileString(LPCTSTR section,LPCTSTR entry,LPCTSTR str) {
	CWinApp* app = AfxGetApp();
		return app->WriteProfileBinary(section,entry,(LPBYTE)str,strlen(str)+1);
	}
	inline CString GetProfileString(LPCTSTR section,LPCTSTR entry,LPCTSTR defval) {
	CWinApp* app = AfxGetApp();
	LPBYTE pData;
	UINT nCount;
	CString rv = defval;
		if(app->GetProfileBinary(section,entry,&pData,&nCount)){
			rv = (LPCTSTR)pData;
			delete pData;
		}
		return rv;
	}

};

#endif // __KHELPERS_H
