#ifndef	__BITSET_H
#define __BITSET_H

namespace Klever {

class CBitSet	: public CObject {
public:
	CWordArray m_Bits;
	ULONG m_BitsInSet;
	enum	{
		bitsPerWord = sizeof(WORD)*8
	};

	CBitSet(ULONG size = 0) : m_BitsInSet(0) { SetSize(size); }
	CBitSet(CBitSet& o) : m_BitsInSet(0) { CopyFrom(o); }

	void SetSize(ULONG size,BOOL bFillOnes=FALSE) {
	UINT os = m_Bits.GetSize();
	UINT ns = (size+bitsPerWord-1)/bitsPerWord;
		if(os==ns){
			if(os){
				if(bFillOnes)
					m_Bits[m_BitsInSet/bitsPerWord]|=(~(WORD)0)<<(m_BitsInSet%bitsPerWord);
				else
					m_Bits[m_BitsInSet/bitsPerWord]&=~((~(WORD)0)<<(m_BitsInSet%bitsPerWord));
			}
			m_BitsInSet=size;
		}else{
			// *?* ASSERT((!os) || (((os-1)*bitsPerWord)<=m_BitsInSet && m_BitsInSet<(os*bitsPerWord)));
			if(os<ns){
				m_Bits.SetSize(ns);
				if(bFillOnes)
					m_Bits[m_BitsInSet/bitsPerWord]|=(~(WORD)0)<<(m_BitsInSet%bitsPerWord);
				else
					m_Bits[m_BitsInSet/bitsPerWord]&=~((~(WORD)0)<<(m_BitsInSet%bitsPerWord));
			WORD* ws = m_Bits.GetData();
				ASSERT(ws);
				memset(&ws[os],bFillOnes?0xFF:0,(ns-os)*sizeof(WORD));
				m_BitsInSet=size;
			}else{
				m_Bits.SetSize(ns);
				m_BitsInSet=size;
			}
		}
	}
	BOOL BitSet(UINT bit,BOOL bGrow=TRUE) { return SetBit(bit,TRUE,bGrow); }
	BOOL BitClear(UINT bit,BOOL bGrow=TRUE) { return SetBit(bit,FALSE,bGrow); }
	BOOL SetBit(UINT bit,BOOL state,BOOL bGrow=TRUE) {
		if(m_BitsInSet<=bit){
			if(!bGrow)
				return FALSE;
			SetSize(bit+1);
		}
	WORD mask = ((WORD)1)<<(bit%bitsPerWord);
		if(state)
			m_Bits[bit/bitsPerWord]|=mask;
		else
			m_Bits[bit/bitsPerWord]&=~mask;
		return TRUE;
	}
	BOOL IsSet(UINT bit) {
		if(m_BitsInSet<=bit)
			return FALSE;
		return (m_Bits[bit/bitsPerWord]&(((WORD)1)<<(bit%bitsPerWord)))!=0;
	}
	void Invert() {
		for(int i=m_Bits.GetUpperBound();i>=0;i--)
			m_Bits[i]=~m_Bits[i];
	}
	CBitSet& operator&=(CBitSet& o) {
		if(o.m_BitsInSet<m_BitsInSet)
			SetSize(o.m_BitsInSet);
		for(int i=m_Bits.GetUpperBound();i>=0;i--)
			m_Bits[i]&=o.m_Bits[i];
		return *this;
	}
	CBitSet& operator|=(CBitSet& o) {
		if(o.m_BitsInSet>m_BitsInSet)
			SetSize(o.m_BitsInSet);
		for(int i=o.m_Bits.GetUpperBound();i>=0;i--)
			m_Bits[i]|=o.m_Bits[i];
		return *this;
	}
	CBitSet& operator=(CBitSet& o) {
		CopyFrom(o);
		return *this;
	}
	void CopyFrom(CBitSet& o) {
		m_BitsInSet=o.m_BitsInSet;
		m_Bits.Copy(o.m_Bits);
	}
	void Serialize(CArchive& ar) {
		if(ar.IsStoring()){
			ar << m_BitsInSet;
			m_Bits.Serialize(ar);
		}else{
			ar >> m_BitsInSet;
			m_Bits.Serialize(ar);
		}
	}
};

};

#endif // __BITSET_H
