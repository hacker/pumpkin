#ifndef __REGEX_H
#define __REGEX_H

class CRegEx	{
public:
	CString GetMatch(int match=0);
	CString m_Input;
	struct	CMatch	{
		CMatch() : m_Begin(-1), m_End(-1) {}
		int GetLength() { return m_End-m_Begin; }
		int m_Begin;
		int m_End;
	};
	typedef	CArray<CMatch,CMatch&> CMatchBox;
	enum	{
		matchMatch = 0,
		matchPreMatch = -1,
		matchPostMatch = -2
	};
	CMatchBox m_Matches;
	enum	{
		charOut = 256,
		charBOL, charEOL, charBOLEOL, charNothing, charBOW, charEOW,
		charMaxCode = charEOW,
		charNNChars = (charMaxCode-255)
	};
	int m_mFlags;
	enum	{
		regeSuccess = 0,
		regeNoMatch = 1, regeBadPattern, regeCollate, regeCType, regeEscape, regeSubReg, regeBracket,
		regeParen, regeBrace, regeBadBrace, regeRange, regeSpace, regeBadRepeat, regeEmpty, regeAssert,
		regeInvArg
	};
	int m_Error;
	CRegEx();
	BOOL m_bCompiled;
	CString m_Pattern;
	BOOL m_bBackRefs;
	int m_Pluses;
	CString m_Must;
	BYTE m_Category[CHAR_MAX-CHAR_MIN+1];
	int m_Categories;
	int m_EOLs;
	int m_BOLs;
	int m_iFlags;
	int m_Subexps;
	struct CSop {
		void Dump(CDumpContext& dc);
		CSop()	{}
		CSop(BYTE op,DWORD opnd=0) : m_Operator(op), m_Operand(opnd) {}
		BOOL operator==(CSop& other) {return m_Operator==other.m_Operator && m_Operand==other.m_Operand;}
		BOOL operator!=(CSop& other) { return !((*this)==other);}
		enum	{
			opEnd = 1, opChar, opBOL, opEOL, opAny, opAnyOf, opBackRef0, opBackRef1,
			opPlus0, opPlus1, opQuest0, opQuest1, opLeftParen, opRightParen, opChoice0,
			opOr0, opOr1, opChoice1, opBOW, opEOW
		};
		BYTE m_Operator;
		DWORD m_Operand;
		enum	{
			stCurrent = 1, stFresh = 2, stTemp = 4, stEmpty = 8
		};
		BYTE m_MatchData;
	};
	typedef CArray<CSop,CSop&> CStrip;
	CStrip m_Strip;
	int m_Flags;
	struct CSet	{
		CSet() : m_Hash(0) { memset(m_Set,0,sizeof(m_Set)); }
		CSet(CSet& src) { (*this)=src; }
		CSet& operator=(CSet& src) { memmove(this,&src,sizeof(*this)); return *this; }
		BOOL operator==(CSet& other) { if(m_Hash!=other.m_Hash)return FALSE;return !memcmp(m_Set,other.m_Set,sizeof(m_Set)); }
		enum	{
			size = (CHAR_MAX-CHAR_MIN+1)
		};
		BOOL m_Set[size];
		BYTE m_Hash;
	public:
		UCHAR GetOnly();
		void Sub(UCHAR c);
		BOOL IsIn(UCHAR c);
		void Add(UCHAR c);
	};
	typedef CArray<CSet,CSet&> CSets;
	CSets m_Sets;
	enum	{
		// Compile flags
		regBasic = 0, regExtended = 1,
		regIgnoreCase = 2,
		regNoSubExpressions = 4,	// Also works for matching.
		regNewLine = 16,
		regLiteral = 32,
		// Match Flags
		regNotBOL = 1,
		regNotEOL = 2,
		regOneMatch=64,
		regBackRefs=128,
		// iFlags
		iflagsUseBOL=1, iflagsUseEOL=2, iflagsBad=4
	};
	CString Replace(LPCTSTR src,LPCTSTR rep,int flags=0);
	BOOL Match(LPCTSTR src,int flags=0);
	BOOL Compile(LPCTSTR regex,int flags=0);
private:
#ifdef	_DEBUG
	void DumpStrip(CDumpContext& dc);
#endif
	LPCTSTR MatchBackRef(LPCTSTR begin,LPCTSTR end,int from,int to,int level);
	typedef CArray<LPCTSTR,LPCTSTR> CStrPtrArray;
	CStrPtrArray m_mLastPos;
	LPCTSTR MatchDissect(LPCTSTR begin,LPCTSTR end,int from,int to);
	LPCTSTR MatchSlow(LPCTSTR begin,LPCTSTR end,int from,int to);
	LPCTSTR m_cOldP;
	BOOL MatchStatesEqual(BYTE m1,BYTE m2);
	LPCTSTR m_mBegin;
	void MatchStatesCopy(BYTE dst,BYTE src);
	void MatchStep(int from,int to,BYTE maskBefore,int charCode,BYTE maskAfter);
	void MatchStatesClear(BYTE mask);
	LPCTSTR MatchFast(LPCTSTR begin);
	LPCTSTR m_mEnd;
	LPCTSTR m_mPointer;
	BOOL ParseBREexp(BOOL ordinaryStar);
	void ParseBRE(int stopa=0,int stopb=0);
	void ParseLiteral();
	int CountPluses();
	void FigureMust();
	BOOL IsInSameSets(UCHAR c1,UCHAR c2);
	BOOL IsInSets(UCHAR c);
	void Categorize();
	int StripDuplicate(int from,int to);
	void EmitRepeat(int pos,int from,int to);
	UCHAR ParseBracketSymbol();
	UCHAR ParseBracketCollatingElement(UCHAR term);
	void ParseBracketEClass(CSet& cset);
	void ParseBracketCClass(CSet& cset);
	void ParseBracketTerm(CSet& cset);
	int StoreSet(CSet& cset);
	void ParseBracket();
	int ParseCount();
	void EmitNonNewLineAny();
	void EmitOrdinary(UCHAR c);
	void StripInsert(int pos,CSop& sop);
	void ParseEREexp();
	void ParseERE(int stop=0);
	struct	CParenthesis	{
		long m_Begin;
		long m_End;
		CParenthesis(long b=0,long e=0) : m_Begin(b), m_End(e) {}
	};
	typedef CArray<CParenthesis,CParenthesis&> CParens;
	CParens m_ParseParens;
	int m_ParsePointer;
};
#ifdef	_DEBUG
inline CDumpContext& operator<<(CDumpContext& dc, CRegEx::CSop& sop) { sop.Dump(dc); return dc; }
#endif

#endif // __REGEX_H