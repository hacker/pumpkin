#ifndef	__BTREENDEX_H
#define	__BTREENDEX_H

#include "Dynamide.h"

namespace Klever {

template<class key,class value,int treeOrder,int cluster>
class	CBTreendex	: public CObject	{
public:
	typedef LONG CBTPageRef;
	struct CBTRecordRef {
		CBTPageRef m_Page;
		INT m_Offset;
		CBTRecordRef(CBTPageRef page=-1,INT offset=-1) : m_Page(page), m_Offset(offset) {}
	};
	class CBTRecord : public CObject {
	public:
		CBTPageRef m_ptrLeft;
		CBTPageRef m_ptrRight;
		key m_Key;
		value m_Value;
		CBTRecord() : m_ptrLeft(-1), m_ptrRight(-1) {}
		CBTRecord(key& _key,value& _value,CBTPageRef left=-1,CBTPageRef right=-1) : m_Key(_key), m_Value(_value), m_ptrLeft(left), m_ptrRight(right) {}
		CBTRecord(CBTRecord& r) : m_Key(r.m_Key), m_Value(r.m_Value), m_ptrLeft(r.m_ptrLeft), m_ptrRight(r.m_ptrRight) {}

		CBTRecord& operator=(CBTRecord& r) {m_Key=r.m_Key, m_Value=r.m_Value, m_ptrLeft=r.m_ptrLeft, m_ptrRight=r.m_ptrRight;return *this;}

		void Serialize(CArchive& ar) {
			if(ar.IsStoring()){
				ar << m_ptrLeft;
				ar << m_ptrRight;
			}else{
				ar >> m_ptrLeft;
				ar >> m_ptrRight;
			}
			SerializeElements(ar,&m_Key,1);
			SerializeElements(ar,&m_Value,1);
		}
	};
	class CBTPage : public CArray<CBTRecord,CBTRecord&> {
	public:
		void Serialize(CArchive& ar) {
		int nCount = -1;
			if(ar.IsStoring()){
				nCount = GetSize();
				ar << nCount;
			}else{
				nCount = 0;
				ar >> nCount;
				RemoveAll();
				SetSize(nCount);
			}
			for(int tmp=0;tmp<nCount;tmp++)
				ElementAt(tmp).Serialize(ar);
		}
	};
	typedef CDynamide<256,cluster> CBTDyna;
	typedef CBTDyna::CDynaFile CBTDynaFile;
	typedef CArray<CBTRecordRef,CBTRecordRef&> CBTRStack;

	CBTDyna m_BT;
	struct	_btCrap {
		BOOL m_bRootSet;
		CBTPageRef m_Root;
	} *m_BTCrap;
	BOOL m_bRO;
	CBTRStack m_btStack;
	CBTPage m_stackTop;

	CBTreendex() {}
	~CBTreendex() { Close(); }
	BOOL Attach(CFile* file,BOOL bAutodelete) {
		m_bRO = FALSE;
		if(!m_BT.Attach(file,bAutodelete))
			return FALSE;
		return Attach();
	}
	BOOL Open(LPCTSTR file,BOOL bReadOnly) {
		if(!m_BT.Open(file,bReadOnly))
			return FALSE;
		m_bRO = bReadOnly;
		return Attach();
	}
	BOOL Create(LPCTSTR file) {
		try{
		CFile* f = new CFile(file,CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyRead|CFile::shareDenyWrite|CFile::typeBinary);
			ASSERT(f);
			return Attach(f,TRUE);
		}catch(CException* e){
			e->Delete();
			return FALSE;
		}
	}
	BOOL Attach() {
		ASSERT(m_BT.IsOpened());
		m_BTCrap = (_btCrap*)m_BT.m_FB.crap;
		if(!m_BTCrap->m_bRootSet){
			m_BTCrap->m_Root = AllocatePage();
			if(m_BTCrap->m_Root<0)
				return FALSE;
			m_BTCrap->m_bRootSet = TRUE;
			m_BT.Write1stBlock();
		}
		return TRUE;
	}
	BOOL Close() {
		m_BT.Close();
		return TRUE;
	}
	BOOL IsOpened() {
		return m_BT.IsOpened();
	}

	BOOL Lookup(key& _key,value& value) {
		if(!IsOpened())
			return FALSE;
		ASSERT(m_BTCrap->m_bRootSet);
		if(!SeekToPage(_key))
			return FALSE;
	CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
		if(rr.m_Offset<0)
			return FALSE;
		ASSERT(rr.m_Offset<m_stackTop.GetSize());
		if(_key != m_stackTop[rr.m_Offset].m_Key)
			return FALSE;
		value = m_stackTop[rr.m_Offset].m_Value;
		return TRUE;
	}
	BOOL Add(key& _key,value& _value) {
		if(!IsOpened())
			return FALSE;
		ASSERT(m_BTCrap->m_bRootSet);
		if(!SeekToPage(_key))
			return FALSE;
	CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
	CBTRecord nuRecord(_key,_value);
		if(rr.m_Offset<0){
			if(m_stackTop.GetSize())
				nuRecord.m_ptrLeft = m_stackTop[m_stackTop.GetUpperBound()].m_ptrRight;
		}else if(rr.m_Offset==0){
			nuRecord.m_ptrRight = m_stackTop[0].m_ptrLeft;
		}else{
			nuRecord.m_ptrLeft = m_stackTop[rr.m_Offset-1].m_ptrRight;
			nuRecord.m_ptrRight = m_stackTop[rr.m_Offset].m_ptrLeft;
		}
//		ASSERT(rr.m_Offset==0 || (m_stackTop[rr.m_Offset-1].m_Key<_key && m_stackTop[rr.m_Offset-1].m_ptrRight<0));
//		ASSERT(rr.m_Offset<0 || m_stackTop[rr.m_Offset].m_Key>=_key && m_stackTop[rr.m_Offset].m_ptrLeft<0);
		if(rr.m_Offset>=0 && m_stackTop[rr.m_Offset].m_Key==_key){
			// Exact match found - just replace.
			m_stackTop[rr.m_Offset].m_Value = _value;
			if(!SavePage(rr.m_Page,m_stackTop))
				return FALSE;
			return TRUE;
		}
		// Split the page and propagate the split if needed..
		// Insert new element at rr.m_Offset..
	BOOL nuisnew = TRUE;
		for(int sp=m_btStack.GetUpperBound();sp>=0;sp--){
		CBTPageRef opr = m_btStack[sp].m_Page;
		int iAt = m_btStack[sp].m_Offset;
		CBTPage op;
			VERIFY(LoadPage(opr,op));
			if(iAt<0)
				iAt = op.GetSize();
			else{
				if(op[iAt].m_Key<nuRecord.m_Key)
					iAt++;
				ASSERT(iAt==op.GetSize() || op[iAt].m_Key > nuRecord.m_Key);
			}
			op.InsertAt(iAt,nuRecord);
			if(iAt>0)
				op[iAt-1].m_ptrRight=nuRecord.m_ptrLeft;
			if(iAt<op.GetUpperBound())
				op[iAt+1].m_ptrLeft=nuRecord.m_ptrRight;
			if(op.GetSize()<=treeOrder*2){
				// This isn't causing overflow
				VERIFY(SavePage(opr,op));
				return TRUE;
			}
			TRACE0("Split\n");
			ASSERT(op.GetSize()==(treeOrder*2+1));
		CBTPageRef npr = AllocatePage();
			ASSERT(npr>=0);
		CBTPage np;
			ASSERT(LoadPage(npr,np));
			ASSERT(!np.GetSize());
			nuRecord = op[treeOrder];
			if(iAt==treeOrder){
				// We're inserting central element! - drop out the stack top if this is still new one
				for(int tmp=0;tmp<treeOrder;tmp++)
					np.InsertAt(tmp,op[tmp]);
				op.RemoveAt(0,treeOrder+1);
				nuRecord.m_ptrLeft = npr;
				nuRecord.m_ptrRight = opr;
				if(nuisnew)
					m_btStack.RemoveAt(m_btStack.GetUpperBound());
			}else{
				if(iAt<treeOrder){
					// We're inserting in the left subtree.
					// Make newpage the right one and forget it.
					for(int tmp=0;tmp<treeOrder;tmp++)
						np.InsertAt(tmp,op[tmp+treeOrder+1]);
					op.RemoveAt(treeOrder,treeOrder+1);
					nuRecord.m_ptrLeft = opr;
					nuRecord.m_ptrRight = npr;
				}else{
					// We're inserting in the right subtree.
					// Make newpage the left one, forget it, but also adjust offset in the stack
					for(int tmp=0;tmp<treeOrder;tmp++)
						np.InsertAt(tmp,op[tmp]);
					op.RemoveAt(0,treeOrder+1);
					nuRecord.m_ptrLeft = npr;
					nuRecord.m_ptrRight = opr;
					m_btStack[sp].m_Offset-=treeOrder+1;
				}
				// Note that we're not inserting new element anymore.
				nuisnew = FALSE;
			}
			// Do, excessive sanity checks and save pages
			ASSERT(op.GetSize());
			ASSERT(np.GetSize());
			VERIFY(SavePage(opr,op));
			VERIFY(SavePage(npr,np));
		}
		// Here we have root page overflow, which means that we're simply putting new
		// record in this brand new root page and also inserting this page on the bottom of the stack
	CBTPageRef nuroot = AllocatePage();
		ASSERT(nuroot>=0);
	CBTPage nurpa;
		ASSERT(LoadPage(nuroot,nurpa));
		ASSERT(!nurpa.GetSize());
		nurpa.Add(nuRecord);
		VERIFY(SavePage(nuroot,nurpa));
		m_btStack.InsertAt(0,CBTRecordRef(nuroot,0));
		m_BTCrap->m_Root = nuroot;
		m_BT.Write1stBlock();
		return TRUE;
	}
	BOOL Delete(key& _key) {
		if(!IsOpened())
			return FALSE;
		ASSERT(m_BTCrap->m_bRootSet);
	value _value;
		if(!Lookup(_key,_value))
			return FALSE;
		// Found key, check if it's a leaf
		{
		CBTRecordRef* rr = &m_btStack[m_btStack.GetUpperBound()];
		int rrIdx = m_btStack.GetUpperBound();
			if(m_stackTop[rr->m_Offset].m_ptrLeft>=0){
				ASSERT(m_stackTop[rr->m_Offset].m_ptrRight>=0);
				// It isn't - scan for the _next_ key and do dirty deeds
				m_btStack.Add(CBTRecordRef(m_stackTop[rr->m_Offset].m_ptrRight,0));
				for(;;){
				CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
					if(!LoadPage(rr.m_Page,m_stackTop))
						return FALSE;
					if(m_stackTop[0].m_ptrLeft<0)
						break;
					m_btStack.Add(CBTRecordRef(m_stackTop[0].m_ptrLeft,0));
				}
				// We have a leaf node here, replace victim with the first element and kill it.
			CBTPage uppage;
				rr = &m_btStack[rrIdx];
				if(!LoadPage(rr->m_Page,uppage))
					return FALSE;
				uppage[rr->m_Offset].m_Key=m_stackTop[0].m_Key; uppage[rr->m_Offset].m_Value=m_stackTop[0].m_Value;
				m_stackTop.RemoveAt(0);
				if(!(SavePage(rr->m_Page,uppage) && SavePage(m_btStack[m_btStack.GetUpperBound()].m_Page,m_stackTop)))
					return FALSE;
			}else{
				ASSERT(m_stackTop[rr->m_Offset].m_ptrRight<0);
				m_stackTop.RemoveAt(rr->m_Offset);
				if(!SavePage(rr->m_Page,m_stackTop))
					return FALSE;
			}
		}
		// We have a page to check for underflow at the top of the stack now.
		for(;;){
		CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
			if(!LoadPage(rr.m_Page,m_stackTop))
				return FALSE;
			if(m_stackTop.GetSize()>=treeOrder || m_btStack.GetSize()==1)
				return TRUE;
		CBTRecordRef& rr1 = m_btStack[m_btStack.GetUpperBound()-1];
		CBTPage daddy;
			if(!LoadPage(rr1.m_Page,daddy))
				return FALSE;
		CBTPageRef nPage = daddy[rr1.m_Offset].m_ptrRight;
		BOOL bRight=TRUE;
			if(nPage<0 || nPage==rr.m_Page){
				nPage = daddy[rr1.m_Offset].m_ptrLeft;
				bRight = FALSE;
			}
			ASSERT(nPage>=0 && nPage!=rr.m_Page);
		CBTPage neighbor;
			if(!LoadPage(nPage,neighbor))
				return FALSE;
			// Here we have possibly two cases:
			// 1. Neighboring page can share some data with use, then do share and leave
			// 2. Neighboring page is of treeorder in size, then merge and propagate
			if(neighbor.GetSize()>treeOrder){
				TRACE0("Redistributing..\n");
				// Borrow some data from there.
			int toBorrow = neighbor.GetSize()-treeOrder;
				toBorrow=toBorrow/2+1;
				ASSERT(toBorrow);
				if(bRight)
					m_stackTop.Add(CBTRecord(daddy[rr1.m_Offset].m_Key,daddy[rr1.m_Offset].m_Value,m_stackTop[m_stackTop.GetUpperBound()].m_ptrRight,neighbor[0].m_ptrLeft));
				else
					m_stackTop.InsertAt(0,CBTRecord(daddy[rr1.m_Offset].m_Key,daddy[rr1.m_Offset].m_Value,neighbor[neighbor.GetUpperBound()].m_ptrRight,m_stackTop[0].m_ptrLeft));
				for(toBorrow--;toBorrow;toBorrow--){
					if(bRight){
						m_stackTop.Add(neighbor[0]);
						neighbor.RemoveAt(0);
					}else{
						m_stackTop.InsertAt(0,neighbor[neighbor.GetUpperBound()]);
						neighbor.RemoveAt(neighbor.GetUpperBound());
					}
				}
				daddy[rr1.m_Offset].m_Key = neighbor[bRight?0:neighbor.GetUpperBound()].m_Key; daddy[rr1.m_Offset].m_Value = neighbor[bRight?0:neighbor.GetUpperBound()].m_Value;
				neighbor.RemoveAt(bRight?0:neighbor.GetUpperBound());
				if(!(SavePage(rr1.m_Page,daddy) && SavePage(nPage,neighbor) && SavePage(rr.m_Page,m_stackTop)))
					return FALSE;
				rr.m_Offset = -1;	// *** Point to the next??
				return TRUE;
			}
			TRACE0("Merging..\n");
			// We need to merge pages here..
			// We will merge them at stacktop, then we'll discard neighbor guy..
			if(bRight)
				m_stackTop.Add(CBTRecord(daddy[rr1.m_Offset].m_Key,daddy[rr1.m_Offset].m_Value,m_stackTop[m_stackTop.GetUpperBound()].m_ptrRight,neighbor[0].m_ptrLeft));
			else
				m_stackTop.InsertAt(0,CBTRecord(daddy[rr1.m_Offset].m_Key,daddy[rr1.m_Offset].m_Value,neighbor[neighbor.GetUpperBound()].m_ptrRight,m_stackTop[0].m_ptrLeft));
			if(bRight){
				while(neighbor.GetSize()){
					m_stackTop.Add(neighbor[0]);
					neighbor.RemoveAt(0);
				}
			}else{
				while(neighbor.GetSize()){
					m_stackTop.InsertAt(0,neighbor[neighbor.GetUpperBound()]);
					neighbor.RemoveAt(neighbor.GetUpperBound());
				}
			}
			if(rr1.m_Offset>0)
				daddy[rr1.m_Offset-1].m_ptrRight=rr.m_Page;
			if(rr1.m_Offset<daddy.GetUpperBound())
				daddy[rr1.m_Offset+1].m_ptrLeft=rr.m_Page;
			daddy.RemoveAt(rr1.m_Offset);
			if(!(SavePage(rr1.m_Page,daddy) && SavePage(rr.m_Page,m_stackTop)))
				return FALSE;
			VERIFY(DeallocatePage(nPage));
			m_btStack.RemoveAt(m_btStack.GetUpperBound());
		}
		return FALSE;
	}
	BOOL GoFirst() {
		if(!IsOpened())
			return FALSE;
		ASSERT(m_BTCrap->m_bRootSet);
		m_btStack.RemoveAll();
		m_btStack.Add(CBTRecordRef(m_BTCrap->m_Root,-1));
		for(;;){
		CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
			if(!LoadPage(rr.m_Page,m_stackTop))
				return FALSE;
			if(!m_stackTop.GetSize()){
				ASSERT(m_btStack.GetSize()==1);
				return FALSE;
			}
			rr.m_Offset = 0;
			if(m_stackTop[rr.m_Offset].m_ptrLeft<0)
				return TRUE;
			m_btStack.Add(CBTRecordRef(m_stackTop[rr.m_Offset].m_ptrLeft,-1));
		}
	}
	BOOL GoLast() {
		if(!IsOpened())
			return FALSE;
		ASSERT(m_BTCrap->m_bRootSet);
		m_btStack.RemoveAll();
		m_btStack.Add(CBTRecordRef(m_BTCrap->m_Root,-1));
		for(;;){
		CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
			if(!LoadPage(rr.m_Page,m_stackTop))
				return FALSE;
			if(!m_stackTop.GetSize()){
				ASSERT(m_btStack.GetSize()==1);
				return FALSE;
			}
			rr.m_Offset = m_stackTop.GetUpperBound();
			if(m_stackTop[rr.m_Offset].m_ptrRight<0)
				return TRUE;
			m_btStack.Add(CBTRecordRef(m_stackTop[rr.m_Offset++].m_ptrRight,-1));
		}
	}
	BOOL GoNext() {
		if(!IsOpened())
			return FALSE;
		if(!(m_btStack.GetSize() && m_btStack[m_btStack.GetUpperBound()].m_Offset>=0))
			return FALSE;
	CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
		if(!LoadPage(rr.m_Page,m_stackTop))
			return FALSE;
		ASSERT(rr.m_Offset<m_stackTop.GetSize());
		if(m_stackTop[rr.m_Offset].m_ptrRight>=0){
			// Advance pointer in this page and dive into subtree
			// going left and left until we have nowhere to go.
//			TRACE1("Dive into page %lu",m_stackTop[rr.m_Offset].m_ptrRight);
			m_btStack.Add(CBTRecordRef(m_stackTop[rr.m_Offset++].m_ptrRight,0));
			for(;;){
			CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
				ASSERT(rr.m_Offset==0);
				if(!LoadPage(rr.m_Page,m_stackTop))
					return FALSE;
				if(m_stackTop[rr.m_Offset].m_ptrLeft<0)
					break;
//				TRACE1(", %lu",m_stackTop[rr.m_Offset].m_ptrLeft);
				m_btStack.Add(CBTRecordRef(m_stackTop[rr.m_Offset].m_ptrLeft,0));
			}
//			TRACE0("\n");
			return TRUE;
		}else if(rr.m_Offset<m_stackTop.GetUpperBound()){
			rr.m_Offset++;
			return TRUE;
		}
		// We're at the end of page go up until we're done or we have data.
		m_btStack.RemoveAt(m_btStack.GetUpperBound());
//		TRACE0("Go up");
		while(m_btStack.GetSize()){
		CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
			if(!LoadPage(rr.m_Page,m_stackTop))
				return FALSE;
			if(rr.m_Offset<m_stackTop.GetSize()){
//				TRACE0("\n");
				return TRUE;
			}
//			TRACE0(", up");
			m_btStack.RemoveAt(m_btStack.GetUpperBound());
		}
//		TRACE0("\nBtree is done\n");
		return FALSE;
	}
	BOOL GoPrev() {
		if(!IsOpened())
			return FALSE;
		if(!(m_btStack.GetSize() && m_btStack[m_btStack.GetUpperBound()].m_Offset>=0))
			return FALSE;
	CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
		if(!LoadPage(rr.m_Page,m_stackTop))
			return FALSE;
		ASSERT(rr.m_Offset<m_stackTop.GetSize());
		if(m_stackTop[rr.m_Offset].m_ptrLeft>=0){
			// Dive in and go right and right from the rightmost until
			// we have nowhere to go.
			m_btStack.Add(CBTRecordRef(m_stackTop[rr.m_Offset].m_ptrLeft,-1));
			for(;;){
			CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
				if(!LoadPage(rr.m_Page,m_stackTop))
					return FALSE;
				rr.m_Offset = m_stackTop.GetUpperBound();
				if(m_stackTop[rr.m_Offset].m_ptrRight<0)
					return TRUE;
				m_btStack.Add(CBTRecordRef(m_stackTop[rr.m_Offset++].m_ptrRight,-1));
			}
			return TRUE;
		}else if(rr.m_Offset>0){
			rr.m_Offset--;
			return TRUE;
		}
		// We're at the leftmost element in page - go up and left until we're
		// done or we have data.
		m_btStack.RemoveAt(m_btStack.GetUpperBound());
		while(m_btStack.GetSize()){
		CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
			if(!LoadPage(rr.m_Page,m_stackTop))
				return FALSE;
			rr.m_Offset--;
			if(rr.m_Offset>=0)
				return TRUE;
			m_btStack.RemoveAt(m_btStack.GetUpperBound());
		}
		// No more data - we were at the first element in tree.
		return FALSE;
	}
	BOOL GetThis(key& _key,value& _value) {
		if(!IsOpened())
			return FALSE;
		// *** MORE CHECKING
	CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
		if(!LoadPage(rr.m_Page,m_stackTop))
			return FALSE;
		_key = m_stackTop[rr.m_Offset].m_Key;
		_value = m_stackTop[rr.m_Offset].m_Value;
		return TRUE;
	}

	BOOL SeekToPage(const key& _key) {
		ASSERT(IsOpened());
		ASSERT(m_BTCrap->m_bRootSet);
		m_btStack.RemoveAll();
		m_btStack.Add(CBTRecordRef(m_BTCrap->m_Root,-1));
		for(;;){
		CBTRecordRef& rr = m_btStack[m_btStack.GetUpperBound()];
			if(!LoadPage(rr.m_Page,m_stackTop))
				return FALSE;
			ASSERT(m_stackTop.GetSize() || !m_btStack.GetUpperBound());
			if(!m_stackTop.GetSize()){
				rr.m_Offset=-1;
				return TRUE;
			}
			for(rr.m_Offset=0;rr.m_Offset<m_stackTop.GetSize();rr.m_Offset++){
				if(_key == m_stackTop[rr.m_Offset].m_Key)
					return TRUE;
				if(_key < m_stackTop[rr.m_Offset].m_Key){
					ASSERT(rr.m_Offset==0 || m_stackTop[rr.m_Offset].m_ptrLeft==m_stackTop[rr.m_Offset-1].m_ptrRight);
					if(m_stackTop[rr.m_Offset].m_ptrLeft<0)
						return TRUE;
					m_btStack.Add(CBTRecordRef(m_stackTop[rr.m_Offset].m_ptrLeft,-1));
					break;
				}
				if(rr.m_Offset==m_stackTop.GetUpperBound()){
					if(m_stackTop[rr.m_Offset].m_ptrRight<0){
						rr.m_Offset=-1;
						return TRUE;
					}
					m_btStack.Add(CBTRecordRef(m_stackTop[rr.m_Offset].m_ptrRight,-1));
					break;
				}
			}
		}
	}

	BOOL LoadPage(CBTPageRef ref,CBTPage& page) {
	CFile* pageFile = m_BT.OpenFile(ref);
		if(!pageFile)
			return FALSE;
	BOOL rv = TRUE;
		try{
		CArchive ar(pageFile,CArchive::load);
			page.Serialize(ar);
			if(m_bRO)
				page.FreeExtra();	// ** ???
			ar.Close();
		}catch(CException* e){
			e->Delete();
			rv = FALSE;
		}
		delete pageFile;
		return rv;
	}
	BOOL SavePage(CBTPageRef ref,CBTPage& page) {
	CFile* pageFile = m_BT.OpenFile(ref);
		if(!pageFile)
			return FALSE;
	BOOL rv = TRUE;
		try{
		CArchive ar(pageFile,CArchive::store);
			page.Serialize(ar);
			ar.Close();
		}catch(CException* e){
			e->Delete();
			rv = FALSE;
		}
		delete pageFile;
		return rv;
	}
	CBTPageRef AllocatePage() {
	CBTDynaFile* pageFile = m_BT.CreateFile();
		if(!pageFile)
			return -1;
	CBTPage nothing;
	CBTPageRef rv = pageFile->GetFile();
		try{
		CArchive ar(pageFile,CArchive::store);
			nothing.Serialize(ar);
			ar.Close();
		}catch(CException* e){
			e->Delete();
			rv = -1;
		}
		delete pageFile;
		return rv;
	}
	BOOL DeallocatePage(CBTPageRef ref) {
		return m_BT.DeleteFile(ref);
	}

};

};

#endif	// __BTREENDEX_H
