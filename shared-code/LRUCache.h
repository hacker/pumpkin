#ifndef	__LRUCACHE_H
#define	__LRUCACHE_H

namespace Klever {

template<class IDX,class ARGIDX,class DATA>
class CLRUCache : public CObject {
public:
	struct	CacheEntry	{
		enum	cacheState	{
			cacheClean, cacheDirty, cacheEmpty
		};
		cacheState	m_State;
		UINT m_hits;

		IDX m_idx;
		DATA* m_pData;

		CacheEntry() { m_State=cacheEmpty; VERIFY(m_pData=new DATA); m_hits=0; }
		virtual ~CacheEntry() { delete m_pData; }
	};
	typedef CMap<IDX,ARGIDX,CacheEntry*,CacheEntry*> CCacheMap;
	typedef CList<CacheEntry*,CacheEntry*> CCacheList;

	CCacheList m_Cache;
	CCacheMap m_Map;

	CLRUCache(UINT cacheSize)	{
		for(int tmp=0;tmp<cacheSize;tmp++){
		CacheEntry* p = new CacheEntry;
			m_Cache.AddTail(p);
		}
	}
	virtual ~CLRUCache()	{
		Flush();
	POSITION p = m_Cache.GetHeadPosition();
		while(p){
		CacheEntry* c = m_Cache.GetNext(p);
			delete c;
		}
	}

	DATA* GetCached(ARGIDX idx,BOOL bLoad=TRUE) {
	CacheEntry* rv;
		if(m_Map.Lookup(idx,rv)){
			rv->m_hits++;
			PopUp(rv);
			return rv->m_pData;
		}
		if(!bLoad)
			return NULL;
		rv = m_Cache.GetTail();
		ASSERT(rv);
		switch(rv->m_State){
		case CacheEntry::cacheDirty:
			FlushEntry(rv);
		case CacheEntry::cacheClean:
			m_Map.RemoveKey(rv->m_idx);
			rv->m_State=CacheEntry::cacheEmpty;
		case CacheEntry::cacheEmpty:
			break;
		default:
			ASSERT(FALSE);
		}
		if(!_ReadIn(idx,rv->m_pData))
			return NULL;
		rv->m_hits=1;
		rv->m_State=CacheEntry::cacheClean;
		rv->m_idx=idx;
		m_Map[idx]=rv;
		PopUp(rv);
		return rv->m_pData;
	}
	BOOL MakeDirty(ARGIDX idx) {
	CacheEntry* pEntry;
		if(m_Map.Lookup(idx,pEntry)){
			ASSERT(pEntry->m_State==CacheEntry::cacheClean || pEntry->m_State==CacheEntry::cacheDirty);
			pEntry->m_State=CacheEntry::cacheDirty;
			return TRUE;
		}
		return FALSE;
	}
	BOOL Flush() {
	POSITION p = m_Cache.GetHeadPosition();
		while(p){
		CacheEntry* pEntry = m_Cache.GetNext(p);
			ASSERT(pEntry);
			FlushEntry(pEntry);
		}
		return TRUE;
	}
	BOOL FlushEntry(CacheEntry* pEntry) {
		if(pEntry->m_State==CacheEntry::cacheDirty){
		BOOL rv = _WriteOut(pEntry->m_idx,pEntry->m_pData);
			if(rv)
				pEntry->m_State=CacheEntry::cacheClean;
		}
		return FALSE;
	}
	void PopUp(CacheEntry* pEntry) {
	POSITION p = m_Cache.Find(pEntry);
		ASSERT(p);
		m_Cache.RemoveAt(p);
		VERIFY(m_Cache.AddHead(pEntry));
	}

	virtual	BOOL _ReadIn(ARGIDX idx,DATA* data) = 0;
	virtual BOOL _WriteOut(ARGIDX idx,DATA* data) = 0;
};

};

#endif	// __LRUCACHE_H
