#ifndef	__BELLSNWHISTLES_H
#define __BELLSNWHISTLES_H

class CBellsNWhistles	{
public:
	class CBang {
	public:
		CString m_codeName;
		enum	_bangType {
			bangNone, bangSpeaker, bangSystem, bangResource,
			bangWaveform
		}	m_type;
		BOOL m_bLoop;
		union	{
			UINT system;
			LPCTSTR resource;
			LPCTSTR wave;

			LPCTSTR str;
			UINT id;
		};
		CString m_strWave;
		CBang() : m_type(bangNone), m_bLoop(FALSE) {}
	};
	typedef CTypedPtrMap<CMapStringToPtr,CString,CBang*> CBangs;
	struct	CBelling {
		LPCSTR snd;
		HMODULE hm;
		DWORD flags;
		CBelling(LPCSTR snd,HMODULE hm,DWORD flags) : snd(snd), hm(hm),
			flags(flags) {}
		CBelling(CBelling& s) : snd(s.snd), hm(s.hm), flags(s.flags) {}
		CBelling& operator=(CBelling& s) {
			snd=s.snd; hm=s.hm; flags=s.flags;
			return *this;
		}
	};
	typedef CBelling* Whistling;

	CBangs m_bangs;

	~CBellsNWhistles() {
	POSITION p = m_bangs.GetStartPosition();
		while(p){
		CString s; CBang* b;
			m_bangs.GetNextAssoc(p,s,b);
			delete b;
		}
		m_bangs.RemoveAll();
	}

	BOOL AssignSound(LPCTSTR codeName,LPCTSTR id,CBang::_bangType type=CBang::bangWaveform) {
	CString cn = codeName;
		cn.MakeLower();
	CBang* b;
		if(!m_bangs.Lookup(cn,b)) {
			b = new CBang;
			b->m_codeName=cn;
			m_bangs[cn]=b;
		}
		b->m_type=type;
		b->str = id;
		if(type==CBang::bangWaveform){
			b->m_strWave=id; b->str = b->m_strWave;
		}
		return TRUE;
	}
	BOOL AssignSound(LPCTSTR codeName,UINT nID,CBang::_bangType type=CBang::bangResource) {
	CString cn = codeName;
		cn.MakeLower();
	CBang* b;
		if(!m_bangs.Lookup(cn,b)) {
			b = new CBang;
			b->m_codeName=cn;
			m_bangs[cn]=b;
		}
		b->m_type=type;
		b->id = nID;
		ASSERT(type!=CBang::bangWaveform);
		return TRUE;
	}
	BOOL UnassignSound(LPCTSTR codeName) {
	CString cn = codeName;
		cn.MakeLower();
	CBang* b;
		if(m_bangs.Lookup(cn,b)) {
			m_bangs.RemoveKey(cn);
			delete b;
			return TRUE;
		}
		return FALSE;
	}

	Whistling StartSound(LPCTSTR codeName) {
	Whistling* rv = NULL;
	CString cn = codeName;
	CBang* b;
		if(!m_bangs.Lookup(cn,b)){
			::PlaySound(cn,AfxGetInstanceHandle(),SND_ASYNC|SND_NODEFAULT|SND_NOWAIT|SND_FILENAME);
			return NULL;
		}
	UINT flags = SND_ASYNC|SND_NODEFAULT|SND_NOWAIT;
	LPCSTR snd = NULL;
		switch(b->m_type){
		case CBang::bangNone: return NULL;
		case CBang::bangSpeaker: MessageBeep(0xFFFFFFFF); return NULL;
		case CBang::bangSystem: MessageBeep(b->system); return NULL;
		case CBang::bangResource:
			snd = b->resource;
			flags|=SND_RESOURCE; break;
		case CBang::bangWaveform:
			snd = b->wave;
			flags|=SND_FILENAME; break;
#ifdef _DEBUG
		default:
			ASSERT(FALSE); return NULL;
#endif
		}
		if(b->m_bLoop)
			flags|=SND_LOOP;
	HMODULE hm = AfxGetInstanceHandle();
		if(!::PlaySound(snd,hm,flags))
			return NULL;
		return b->m_bLoop?new CBelling(snd,hm,flags):NULL;
	}
	BOOL StopSound(Whistling whistle) {
		if(!whistle)
			return FALSE;
		::PlaySound(whistle->snd,whistle->hm,whistle->flags|SND_PURGE);
		delete whistle;
		return TRUE;
	}
	UINT FillInCombo(CComboBox* combo) {
	POSITION p = m_bangs.GetStartPosition();
	UINT rv = 0;
		while(p) {
		CString s;
		CBang* b;
			m_bangs.GetNextAssoc(p,s,b);
			combo->AddString(s);
		}
		return rv;
	}
};

#endif	// _BELLSNWHISTLES_H