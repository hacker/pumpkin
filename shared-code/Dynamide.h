#ifndef __DYNAMIDE_H
#define __DYNAMIDE_H

#include "LRUCache.h"

namespace Klever {

template<int fbSize,int bSize>
class CDynamide : public CObject {
	struct	firstBlock {
		LONG	freeFile;
		BYTE	crap[fbSize-sizeof(LONG)];
	};
	struct	theBlock {
		LONG	next;
		BYTE	data[bSize-sizeof(LONG)];
	};
public:
	static firstBlock FirstBlock;
	static theBlock TheBlock;
private:
	class	CDynaCache : public CLRUCache<DWORD,DWORD,theBlock> {
	public:
		CFile* m_File;
		BOOL m_bAutodelete;
		CDynaCache(CFile* file,BOOL bAutodelete=TRUE) : CLRUCache<DWORD,DWORD,theBlock>(64) {
			m_File=file;
			m_bAutodelete=bAutodelete;
		}
		virtual ~CDynaCache() {
			Flush();
			if(m_bAutodelete){
				m_File->Close();
				delete m_File;
			}
		}
		virtual BOOL _ReadIn(DWORD idx,theBlock* data) {
		LONG p = sizeof(firstBlock)+idx*sizeof(theBlock);
		LONG s = m_File->Seek(p,CFile::begin);
			if(p==s){
			UINT rb = m_File->Read(data,sizeof(*data));
				if(rb==sizeof(*data))
					return TRUE;
				if(rb)
					return FALSE;
				memset(data,0,sizeof(*data));
				data->next=-1;
				try{
					m_File->Write(data,sizeof(*data));
				}catch(CException* e){
					e->Delete();
					return FALSE;
				}
			}else{
			LONG togo = p-s;
				ASSERT(togo>0);
				ASSERT(!(togo%sizeof(theBlock)));
				memset(data,0,sizeof(*data));
				data->next=-1;
				while(togo>=0){
					try{
						m_File->Write(data,sizeof(*data));
					}catch(CException* e){
						e->Delete();
						return FALSE;
					}
				}
			}
			return TRUE;
		}
		virtual BOOL _WriteOut(DWORD idx,theBlock* data) {
		LONG p = sizeof(firstBlock)+idx*sizeof(theBlock);
		LONG s = m_File->Seek(p,CFile::begin);
			if(p!=s)
				return FALSE;
			try{
				m_File->Write(data,sizeof(*data));
			}catch(CException* e){
				e->Delete();
				return FALSE;
			}
			return TRUE;
		}
		DWORD AllocateNode() {
		LONG l = m_File->GetLength();
			ASSERT(!((l-sizeof(firstBlock))%sizeof(theBlock)));
			m_File->SetLength(l+sizeof(theBlock));
			return (l-sizeof(firstBlock))/sizeof(theBlock);
		}
		BOOL Read1stBlock(firstBlock* fb) {
			m_File->SeekToBegin();
		UINT rb = m_File->Read(fb,sizeof(*fb));
			if(rb==sizeof(*fb))
				return TRUE;
			if(rb)
				return FALSE;
			memset(fb,0,sizeof(*fb));
			fb->freeFile = -1;
			try{
				m_File->Write(fb,sizeof(*fb));
			}catch(CException* e){
				e->Delete();
				return FALSE;
			}
			return TRUE;
		}
		BOOL Write1stBlock(firstBlock* fb) {
			m_File->SeekToBegin();
			try{
				m_File->Write(fb,sizeof(*fb));
			}catch(CException* e){
				e->Delete();
				return FALSE;
			}
			return  TRUE;
		}
	};
public:
	class	CDynaFile	: public CFile	{
	public:
		CDynamide<fbSize,bSize>* m_Daddy;
		CArray<LONG,LONG> m_Blocks;
		LONG m_Position;

		CDynaFile(CDynamide<fbSize,bSize>* daddy) : m_Daddy(NULL) { AttachToDaddy(daddy); }
		virtual ~CDynaFile() { Close(); DetachFromDaddy(); }

		void AttachToDaddy(CDynamide<fbSize,bSize>* daddy) {
			ASSERT(!m_Daddy);
			ASSERT(daddy);
			m_Daddy=daddy;
			m_Daddy->AttachDynaFile(this);
		}
		void DetachFromDaddy() {
			ASSERT(m_Daddy);
			ASSERT(!IsOpened());
			m_Daddy->DetachDynaFile(this);
			m_Daddy=NULL;
		}

		BOOL Create() {
			if(IsOpened())
				return FALSE;
			m_Blocks.SetAtGrow(0,m_Daddy->Allocate());
			ASSERT(m_Blocks[0]>=0);
			m_Position=0;
			return TRUE;
		}
		BOOL Open(LONG fb) {
			if(IsOpened())
				return FALSE;
			ASSERT(fb>=0);
		theBlock* b;
			do{
				m_Blocks.Add(fb);
				b = m_Daddy->m_File->GetCached(fb);
				ASSERT(b);
				fb=b->next;
				if(m_Blocks[m_Blocks.GetUpperBound()]==fb)
					return FALSE;
			}while(fb>=0);
			m_Position=0;
			return TRUE;
		}

		LONG GetFile() {
			if(!IsOpened())
				return -1;
			return m_Blocks[0];
		}
		virtual DWORD GetPosition() const {
			if(!IsOpened())
				return 0;
			if(m_Position<0)
				return 0;
			if(m_Position>GetLength())
				return GetLength();
			return m_Position;
		}
		virtual CString GetFileName() {
		CString rv;
			if(IsOpened())
				rv.Format("0x%08lX",m_Blocks[0]);
			else
				rv.Format("None");
			return rv;
		}
		virtual CString GetFileTitle() { return GetFileName(); }
		virtual CString GetFilePath() { return GetFileName(); }
		virtual void SetFilePath(LPCTSTR lpszNewName) { ASSERT(FALSE); }

		virtual BOOL Open(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError = NULL) { ASSERT(FALSE); return FALSE; }
        virtual CFile* Duplicate() { ASSERT(FALSE); return NULL; }

        virtual LONG Seek(LONG lOff, UINT nFrom) {
			if(!IsOpened())
				return -1;
			switch(nFrom){
			case CFile::begin:
				m_Position=lOff;
				break;
			case CFile::current:
				m_Position+=lOff;
				break;
			case CFile::end:
				m_Position=GetLength()+lOff;
				break;
			default:
				ASSERT(FALSE);
				return -1;
			}
			if(m_Position<0)
				m_Position=0;
			if(m_Position>GetLength())
				m_Position=GetLength();
			return m_Position;
		}
        virtual void SetLength(DWORD dwNewLen) {
			if(!IsOpened())
				return;
			if(dwNewLen<GetLength()){
				dwNewLen=dwNewLen+sizeof(TheBlock.data);
				dwNewLen-=dwNewLen%sizeof(TheBlock.data);
				while(dwNewLen<GetLength() && m_Blocks.GetSize()>1){
				LONG mb = m_Blocks[m_Blocks.GetUpperBound()];
				LONG mb1 = m_Blocks[m_Blocks.GetUpperBound()-1];
				theBlock* b = m_Daddy->m_File->GetCached(mb1);
					ASSERT(b);
					ASSERT(b->next==mb);
					b->next=-1;
					m_Daddy->m_File->MakeDirty(mb1);
					m_Daddy->Deallocate(mb);
					m_Blocks.SetSize(m_Blocks.GetSize()-1);
				}
			}else{
				while(dwNewLen>GetLength()){
				LONG mb = m_Blocks[m_Blocks.GetUpperBound()];
				LONG newBlock = m_Daddy->Allocate();
					ASSERT(newBlock>=0);
				theBlock* b = m_Daddy->m_File->GetCached(mb);
					ASSERT(b);
					ASSERT(b->next<0);
					b->next=newBlock;
					m_Daddy->m_File->MakeDirty(mb);
					m_Blocks.Add(newBlock);
				}
			}
		}
        virtual DWORD GetLength() const {
			return ((long)m_Blocks.GetSize())*((long)sizeof(TheBlock.data));
		}

        virtual UINT Read(void* lpBuf, UINT nCount) {
		UINT rv = 0;
			ASSERT(m_Position>=0 && m_Position<=GetLength());
		LPBYTE data = (LPBYTE)lpBuf;
			while(nCount && m_Position<GetLength()){
			UINT bn = m_Position/sizeof(TheBlock.data);
			UINT bo = m_Position%sizeof(TheBlock.data);
			theBlock* b = m_Daddy->m_File->GetCached(m_Blocks[bn]);
				ASSERT(b);
			UINT bc = min(nCount,sizeof(TheBlock.data)-bo);
				memmove(data,&b->data[bo],bc);
				nCount-=bc;
				data=&data[bc];
				rv+=bc;
				m_Position+=bc;
			}
			return rv;
		}
        virtual void Write(const void* lpBuf, UINT nCount) {
			ASSERT(m_Position>=0 && m_Position<=GetLength());
		LPBYTE data = (LPBYTE)lpBuf;
			while(nCount){
			UINT bn = m_Position/sizeof(TheBlock.data);
			UINT bo = m_Position%sizeof(TheBlock.data);
				while(bn>=m_Blocks.GetSize()){
				LONG mb = m_Blocks[m_Blocks.GetUpperBound()];
				LONG newBlock = m_Daddy->Allocate();
					ASSERT(newBlock>=0);
				theBlock* b = m_Daddy->m_File->GetCached(mb);
					ASSERT(b);
					ASSERT(b->next<0);
					b->next=newBlock;
					m_Daddy->m_File->MakeDirty(mb);
					m_Blocks.Add(newBlock);
				}
			theBlock* b = m_Daddy->m_File->GetCached(m_Blocks[bn]);
				ASSERT(b);
			UINT bc = min(nCount,sizeof(TheBlock.data)-bo);
				memmove(&b->data[bo],data,bc);
				m_Daddy->m_File->MakeDirty(m_Blocks[bn]);
				nCount-=bc;
				data=&data[bc];
				m_Position+=bc;
			}
		}

        virtual void LockRange(DWORD dwPos, DWORD dwCount) { ASSERT(FALSE); }
        virtual void UnlockRange(DWORD dwPos, DWORD dwCount) { ASSERT(FALSE); }

        virtual void Abort() { ASSERT(FALSE); }
        virtual void Flush() {
			m_Daddy->m_File->Flush();
		}
        virtual void Close() {
			m_Blocks.RemoveAll();
		}

		BOOL IsOpened() const { return m_Blocks.GetSize()!=0; }
	};

	CDynaCache* m_File;
	firstBlock m_FB;

	CDynamide() : m_File(NULL) {}
	~CDynamide() { Close(); }

	BOOL AttachDynaFile(CDynaFile* f) {
	//	ASSERT(!m_Files.Find(f));
	//	m_Files.AddHead(f);
		return TRUE;
	}
	BOOL DetachDynaFile(CDynaFile* f) {
	//POSITION p  = m_Files.Find(f);
	//	ASSERT(p);
	//	m_Files.RemoveAt(p);
		return TRUE;
	}

	BOOL Open(LPCTSTR file,BOOL bRO=FALSE) {
		Close();
		try{
		CFile* f = new CFile(file,CFile::typeBinary|(bRO?CFile::modeRead|CFile::shareDenyWrite:CFile::modeReadWrite|CFile::shareDenyRead));
			return Attach(f,TRUE);
		}catch(CException* e){
			e->Delete();
			return FALSE;
		}
	}
	BOOL Create(LPCTSTR file) {
		Close();
		try{
		CFile* f = new CFile(file,CFile::typeBinary|CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyRead);
			return Attach(f,TRUE);
		}catch(CException* e){
			e->Delete();
			return FALSE;
		}
	}
	BOOL Attach(CFile* file,BOOL bAutodelete=TRUE) {
		if(IsOpened())
			return FALSE;
		m_File = new CDynaCache(file,bAutodelete);
		if(!m_File->Read1stBlock(&m_FB)){
			memset(&m_FB,0,sizeof(m_FB));
			m_FB.freeFile=-1;
			VERIFY(m_File->Write1stBlock(&m_FB));
		}	
		return IsOpened();
	}
	// CFile* Detach();
	BOOL Close() {
		if(!IsOpened())
			return FALSE;
		m_File->Write1stBlock(&m_FB);
		delete m_File;
		m_File=NULL;
		return TRUE;
	}
	BOOL IsOpened() {
		return m_File != NULL;
	}
	BOOL Write1stBlock(void) {
		if(!IsOpened())
			return FALSE;
		VERIFY(m_File->Write1stBlock(&m_FB));
		return TRUE;
	}

	CDynaFile* CreateFile() {
	CDynaFile* rv = new CDynaFile(this);
		if(rv->Create())
			return rv;
		delete rv;
		return NULL;
	}
	CDynaFile* OpenFile(LONG fb) {
	CDynaFile* rv = new CDynaFile(this);
		if(rv->Open(fb))
			return rv;
		delete rv;
		return NULL;
	}
	BOOL DeleteFile(LONG fb) {
		while(fb>=0){
		theBlock* b = m_File->GetCached(fb);
		LONG nb = b->next;
			Deallocate(fb);
			fb=nb;
		}
		return TRUE;
	}

	LONG Allocate() {
		if(!IsOpened())
			return -1;
		if(m_FB.freeFile<0){
		LONG rv = m_File->AllocateNode();
		theBlock *b = m_File->GetCached(rv);
			b->next=-1;
			m_File->MakeDirty(rv);
			return rv;
		}
	LONG rv = m_FB.freeFile;
	theBlock *b = m_File->GetCached(rv);
		m_FB.freeFile=b->next;
		b->next=-1;
		m_File->MakeDirty(rv);
		m_File->Write1stBlock(&m_FB);
		return rv;
	}
	BOOL Deallocate(LONG bk) {
		if(!IsOpened())
			return FALSE;
	theBlock* b = m_File->GetCached(bk);
		ASSERT(b);
		if(m_FB.freeFile<0){
			b->next=-1;
			m_FB.freeFile=bk;
		}else{
			b->next=m_FB.freeFile;
			m_FB.freeFile=bk;
		}
		m_File->MakeDirty(bk);
		m_File->Write1stBlock(&m_FB);
		return TRUE;
	}
};

};

#endif // __DYNAMIDE_H
