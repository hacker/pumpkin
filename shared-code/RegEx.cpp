#include "../stdafx.h"
#include "RegEx.h"

#define	isWordableChar(c) (isalnum(c) || (c)=='_')

BOOL CRegEx::Compile(LPCTSTR regex,int flags)
{
	ASSERT(!((flags&regExtended) && (flags&regLiteral)));
	m_Matches.RemoveAll();
	m_Strip.RemoveAll();
	m_Strip.SetSize(0,15);
	m_Pattern=regex;
	m_ParsePointer=0;
	m_Error=0;
	m_Sets.RemoveAll();
	m_Flags=flags;
	m_iFlags=0;
	m_BOLs=m_EOLs=0;
	m_Subexps = 0;
	m_Categories=1;	// 0 is 'everything else'
	m_bBackRefs=FALSE;
	memset(m_Category,0,sizeof(m_Category));
	
	// Go ahead.
	m_Error || m_Strip.Add(CSop(CSop::opEnd));
	if(flags&regExtended){
		ParseERE();
	}else if(flags&regLiteral){
		ParseLiteral();
	}else{
		ParseBRE();
	}
	m_Error || m_Strip.Add(CSop(CSop::opEnd));
	Categorize();
	m_Strip.FreeExtra();
	FigureMust();
	m_Pluses=CountPluses();
	if(m_iFlags&iflagsBad){
		m_Error || (m_Error=regeAssert);
		// ??? point to nuls? ;-)
	}
	// We may wish to free some memory here if we're erroneous (ie. m_Error..)
	m_ParseParens.RemoveAll();
#ifdef _DEBUG
	if(m_Error){
	CString tmp;
		tmp.Format("RE: ParseError: %d\n",m_Error);
		TRACE0(tmp);
	}
//	DumpStrip(afxDump);
#endif
	return (m_bCompiled=(!m_Error));
}

BOOL CRegEx::Match(LPCTSTR src,int flags)
{
	if(!m_bCompiled)
		return FALSE;
	if(m_iFlags&iflagsBad)
		return FALSE;
	m_Input=src;
	m_mFlags=flags;
	m_mPointer=m_Input;
	m_mBegin=m_Input;
	m_mEnd=&m_mBegin[m_Input.GetLength()];
	ASSERT(m_mPointer<=m_mEnd);
	m_Matches.RemoveAll();
	if(!m_Must.IsEmpty()){
		if(m_Input.Find(m_Must)<0)
			return FALSE;
	}
	// Go ahead..
int stripLen = m_Strip.GetSize();
	m_mLastPos.SetSize(0);
	for(int tmp=0;tmp<stripLen;tmp++)
		m_Strip[tmp].m_MatchData=0;
LPCTSTR beginp = m_mBegin;
LPCTSTR endp;
	for(;;){
		endp = MatchFast(beginp);
		if(!endp)
			return FALSE;
		if((m_mFlags&regNoSubExpressions) && !m_bBackRefs)
			break;
		ASSERT(m_cOldP);
		for(;;){
			endp = MatchSlow(m_cOldP,m_mEnd,1,stripLen-1);
			if(endp)
				break;
			ASSERT(m_cOldP<m_mEnd);
			m_cOldP++;
		}
		if((m_mFlags&regOneMatch) && !m_bBackRefs)
			break;
		// Oh, his, we want the subexpression..
		m_Matches.SetSize(m_Subexps+1);
	LPCTSTR dp;
		if(!m_bBackRefs && !(m_mFlags&regBackRefs)){
			dp = MatchDissect(m_cOldP,endp,1,stripLen-1);
		}else{
			if(m_Pluses>0 && !m_mLastPos.GetSize())
				m_mLastPos.SetSize(m_Pluses);
			dp = MatchBackRef(m_cOldP,endp,1,stripLen-1,0);
		}
		if(dp)
			break;
		// Uh.. oh..  we couldn't find a subexpression-level match
		ASSERT(m_bBackRefs);
		ASSERT(m_Pluses==0 || m_mLastPos.GetSize());
		for(;;){
			if(dp || endp <= m_cOldP)
				break;	// defeat.. ?
			endp = MatchSlow(m_cOldP,endp-1,1,stripLen-1);
			if(!endp)
				break;	// defeat.. ?
			// Try it on a shorter possibility..
#ifdef _DEBUG
			for(tmp=1;tmp<=m_Subexps;tmp++)
				ASSERT(m_Matches[tmp].m_Begin<0 && m_Matches[tmp].m_End<0);
#endif
			dp = MatchBackRef(m_cOldP,endp,1,stripLen-1,0);
		}
		ASSERT((!dp) || dp==endp);
		if(dp)	// Found a shorter one..
			break;
		// Despite initial appearances, there is no match here
		beginp = m_cOldP+1;
		ASSERT(beginp<=m_mEnd);
	}
	// Fill in the detail if so requested..
	if(!(m_mFlags&regNoSubExpressions)){
		if(!m_Matches.GetSize())
			m_Matches.SetSize(1);
		m_Matches[0].m_Begin=m_cOldP-m_mBegin;
		m_Matches[0].m_End=endp-m_mBegin;
	}
	m_mLastPos.RemoveAll();
	return TRUE;
}

CString CRegEx::Replace(LPCTSTR src,LPCTSTR rep,int flags)
{
	// ***
	return CString();
}

void CRegEx::ParseERE(int stop)
{
UCHAR c;
BOOL first=TRUE;
int prevF, prevB;
	for(;;){
	int co = m_Strip.GetSize();
		while(m_ParsePointer < m_Pattern.GetLength() && ((c=m_Pattern[m_ParsePointer])!='|') && c!=stop)
			ParseEREexp();
		if(m_Strip.GetSize()==co){
			m_Error || (m_Error=regeEmpty);
			// ??? point to nuls?
		}
		if(m_ParsePointer>=m_Pattern.GetLength() || m_Pattern[m_ParsePointer]!='|')
			break;
		else
			m_ParsePointer++;
		if(first){
			StripInsert(co,CSop(CSop::opChoice0,m_Strip.GetSize()-co+1));
			prevF = prevB = co;
			first=FALSE;
		}
		m_Error || m_Strip.Add(CSop(CSop::opOr0,m_Strip.GetSize()-prevB));
		prevB = m_Strip.GetSize()-1;
		m_Error || (m_Strip[prevF].m_Operand=m_Strip.GetSize()-prevF);
		prevF = m_Strip.GetSize();
		m_Error || m_Strip.Add(CSop(CSop::opOr1,0));	// offset isn't really right.. very so..
	}
	if(!first){
		m_Error || (m_Strip[prevF].m_Operand=m_Strip.GetSize()-prevF);
		m_Error || m_Strip.Add(CSop(CSop::opChoice1,m_Strip.GetSize()-prevB));
	}
	ASSERT(m_ParsePointer >= m_Pattern.GetLength() || m_Pattern[m_ParsePointer]==stop);
}

void CRegEx::ParseEREexp()
{
	ASSERT(m_ParsePointer < m_Pattern.GetLength());
UCHAR c = m_Pattern[m_ParsePointer++];
int pos = m_Strip.GetSize();
int subno;
int count, count2;
BOOL wascaret=FALSE;
	switch(c){
	case '(':
		if(!(m_ParsePointer<m_Pattern.GetLength())){
			TRACE0("RE: '(' at the end of the pattern\n");
			if(!m_Error)
				m_Error = regeParen;
			// ??? point to nuls?
		}
		m_Subexps++;
		subno=m_Subexps;
		m_ParseParens.SetAtGrow(m_Subexps,CParenthesis(m_Strip.GetSize()));
		m_Error || m_Strip.Add(CSop(CSop::opLeftParen,subno));
		if(m_ParsePointer>=m_Pattern.GetLength() || m_Pattern[m_ParsePointer]!=')')
			ParseERE(')');
		VERIFY(m_ParseParens[m_Subexps].m_End = m_Strip.GetSize());
		m_Error || m_Strip.Add(CSop(CSop::opRightParen,subno));
		if(m_ParsePointer >= m_Pattern.GetLength() || m_Pattern[m_ParsePointer++]!=')'){
			TRACE0("RE: No matching ')'\n");
			if(!m_Error)
				m_Error = regeParen;
			// ??? point to nuls?
		}
		break;
	case '^':
		m_Error || m_Strip.Add(CSop(CSop::opBOL));
		m_iFlags|=iflagsUseBOL;
		m_BOLs++;
		wascaret=TRUE;
		break;
	case '$':
		m_Error || m_Strip.Add(CSop(CSop::opEOL));
		m_iFlags|=iflagsUseEOL;
		m_EOLs++;
		break;
	case '|':
		TRACE0("RE: '|' outside of expression\n");
		if(!m_Error)
			m_Error = regeEmpty;
		// ??? point to nuls?
		break;
	case '*':
	case '+':
	case '?':
		TRACE0("RE: '*'/'+'/'?' with no previous expression\n");
		if(!m_Error)
			m_Error = regeBadRepeat;
		// ??? point to nuls?
		break;
	case '.':
		if(m_Flags&regNewLine)
			EmitNonNewLineAny();
		else
			m_Error || m_Strip.Add(CSop(CSop::opAny));
		break;
	case '[':
		ParseBracket();
		break;
	case '\\':
		if(m_ParsePointer >= m_Pattern.GetLength()){
			TRACE0("RE: '\\' at the end of the pattern\n");
			if(!m_Error)
				m_Error = regeEscape;
			// ??? point to nuls?
		}else{
			c = m_Pattern[m_ParsePointer++];
			EmitOrdinary(c);
		}
		break;
	case '{':
		if(m_ParsePointer >= m_Pattern.GetLength() || !isdigit(m_Pattern[m_ParsePointer])){
			TRACE0("RE: '{' with no repeat count\n");
			if(!m_Error)
				m_Error = regeBadRepeat;
			// ??? point to nuls?
		}
		// Fallthrough..
	default:
		EmitOrdinary(c);
		break;
	}
	if(m_ParsePointer >= m_Pattern.GetLength())
		return;
	c = m_Pattern[m_ParsePointer];
	// Call a '{' repetition if followed by a digit
	if (!(c=='*' || c=='+' || c=='?' || ( c=='{' && (m_ParsePointer+1) < m_Pattern.GetLength() && isdigit(m_Pattern[m_ParsePointer+1])) ))
		return; // No repetitor - done.
	m_ParsePointer++;
	if(wascaret){
		TRACE0("RE: repetitive '^' detected\n");
		if(!m_Error)
			m_Error = regeBadRepeat;
		// ??? point to nuls?
	}
	switch(c){
	case '*':	// Implemeted as +?
		// + expression
		StripInsert(pos,CSop(CSop::opPlus0,m_Strip.GetSize()-pos+1));
		m_Error || m_Strip.Add(CSop(CSop::opPlus1,m_Strip.GetSize()-pos));
		// ? expression
		StripInsert(pos,CSop(CSop::opQuest0,m_Strip.GetSize()-pos+1));
		m_Error || m_Strip.Add(CSop(CSop::opQuest1,m_Strip.GetSize()-pos));
		break;
	case '+':
		// + expression
		StripInsert(pos,CSop(CSop::opPlus0,m_Strip.GetSize()-pos+1));
		m_Error || m_Strip.Add(CSop(CSop::opPlus1,m_Strip.GetSize()-pos));
		break;
	case '?':
		// Kludge - emit y? as (y|) until subtle bug gets fixed :-)
		StripInsert(pos,CSop(CSop::opChoice0,m_Strip.GetSize()-pos+1));
		m_Error || m_Strip.Add(CSop(CSop::opOr0,m_Strip.GetSize()-pos));
		m_Error || (m_Strip[pos].m_Operand=m_Strip.GetSize()-pos);
		m_Error || m_Strip.Add(CSop(CSop::opOr1,1));
		m_Error || m_Strip.Add(CSop(CSop::opChoice1,2));
		break;
	case '{':
		count = ParseCount();
		if(m_ParsePointer < m_Pattern.GetLength() && m_Pattern[m_ParsePointer]==','){
			m_ParsePointer++;
			if(isdigit(m_Pattern[m_ParsePointer])){ // HHH Personally, I doubt it is always available
				count2=ParseCount();
				if(!(count<=count2)){
					TRACE0("RE: Disbalanced counts in '{}' repeater\n");
					m_Error || (m_Error=regeBadBrace);
					// ??? point to nuls?
				}
			}else // Single number with comma
				count2=256; // Infinity
		}else // Single number
			count2=count;
		EmitRepeat(pos,count,count2);
		if(m_ParsePointer >= m_Pattern.GetLength() || m_Pattern[m_ParsePointer]!='}'){
			// No '}'..
			TRACE0("RE: No immediately following '}' of '{' expression\n");
			while(m_ParsePointer < m_Pattern.GetLength() && m_Pattern[m_ParsePointer]!='}')
				m_ParsePointer++;
			if(m_ParsePointer >= m_Pattern.GetLength()){
				TRACE0("RE: No closing '}' found\n");
				m_Error || (m_Error=regeBrace);
			}else
				m_Error || (m_Error=regeBadBrace);
			// ??? point to nuls?
		}else
			m_ParsePointer++;
		break;
	}
	if(m_ParsePointer >= m_Pattern.GetLength())
		return;
	c = m_Pattern[m_ParsePointer];
	if(!(c=='*' || c=='+' || c=='?' || (c=='{' && (m_ParsePointer+1)<m_Pattern.GetLength() && isdigit(m_Pattern[m_ParsePointer+1]))))
		return;
	TRACE0("RE: Double repeater\n");
	m_Error || (m_Error=regeBadRepeat);
	// ??? point to nuls?
}

void CRegEx::StripInsert(int pos,CSop& sop)
{
	if(m_Error)
		return;
int sn = m_Strip.GetSize();
	m_Strip.InsertAt(pos,sop);
	for(int tmp=1;tmp<m_ParseParens.GetSize();tmp++){
		if(m_ParseParens[tmp].m_Begin>=pos)
			m_ParseParens[tmp].m_Begin++;
		if(m_ParseParens[tmp].m_End>=pos)
			m_ParseParens[tmp].m_End++;
	}
}

void CRegEx::EmitOrdinary(UCHAR c)
{
	if(m_Flags&regIgnoreCase && isalpha(c) && (tolower(c) !=toupper(c))){
		// Emit both cases
	CString savePattern = m_Pattern;
	int savePointer = m_ParsePointer;
		m_Pattern=c;
		m_Pattern+=']';
		m_ParsePointer=0;
		ParseBracket();
		m_Pattern=savePattern;
		m_ParsePointer=savePointer;
	}else{
		m_Error || m_Strip.Add(CSop(CSop::opChar,c));
		if(!m_Category[(BYTE)c])
			m_Category[(BYTE)c]=m_Categories++;
	}
}

void CRegEx::EmitNonNewLineAny()
{
	// Kludges're going on and on..
CString savePattern = m_Pattern;
int savePointer = m_ParsePointer;
	m_Pattern="^\n]";
	m_ParsePointer=0;
	ParseBracket();
	m_Pattern=savePattern;
	m_ParsePointer=savePointer;
}

int CRegEx::ParseCount()
{
BOOL nonEmpty=FALSE;
int rv = 0;
UCHAR c;
	while(m_ParsePointer < m_Pattern.GetLength() && isdigit(c=m_Pattern[m_ParsePointer]) && rv <=255){
		rv = rv*10 + c-'0';
		nonEmpty=TRUE;
		m_ParsePointer++;
	}
	if(rv>255 || !nonEmpty){
		m_Error || (m_Error=regeBadBrace);
		// ??? point to nuls?
	}
	return rv;
}

void CRegEx::ParseBracket()
{
	// Dept. of truly sickening special case kludges
	if((m_ParsePointer+5) < m_Pattern.GetLength() && !m_Pattern.Mid(m_ParsePointer,6).Compare("[:<]]")){
		m_Error || m_Strip.Add(CSop(CSop::opBOW));
		m_ParsePointer+=6;
		return;
	}
	if((m_ParsePointer+5) < m_Pattern.GetLength() && !m_Pattern.Mid(m_ParsePointer,6).Compare("[:>]]")){
		m_Error || m_Strip.Add(CSop(CSop::opEOW));
		m_ParsePointer+=6;
		return;
	}
BOOL invert=TRUE;
	if(m_ParsePointer < m_Pattern.GetLength() && m_Pattern[m_ParsePointer]=='^')
		m_ParsePointer++;
	else
		invert=FALSE;
CSet cset;
	if(m_ParsePointer < m_Pattern.GetLength()){
		switch(m_Pattern[m_ParsePointer]){
		case ']':
		case '-':
			cset.Add(m_Pattern[m_ParsePointer]);
			m_ParsePointer++;
			break;
		}
	}
	while(m_ParsePointer < m_Pattern.GetLength() && m_Pattern[m_ParsePointer]!=']' && !((m_ParsePointer+1)<m_Pattern.GetLength() && !m_Pattern.Mid(m_ParsePointer,2).Compare("-]")))
		ParseBracketTerm(cset);
	if(m_ParsePointer < m_Pattern.GetLength() && m_Pattern[m_ParsePointer]=='-'){
		m_ParsePointer++;
		cset.Add('-');
	}
	if(m_ParsePointer < m_Pattern.GetLength() && m_Pattern[m_ParsePointer]==']')
		m_ParsePointer++;
	else{
		m_Error || (m_Error=regeBracket);
		// ???  point to nuls?
		return;
	}
	if(m_Flags&regIgnoreCase){
		for(int tmp=CSet::size-1;tmp>=0;tmp--){
			if(cset.IsIn(tmp) && isalpha(tmp) && (toupper(tmp)!=tolower(tmp)))
				cset.Add(isupper(tmp)?tolower(tmp):toupper(tmp));
		}
		/*
		if(!cset->m_Multis.IsEmpty())
			cset.CollatingCase();
		 */
	}
	if(invert){
		for(int tmp=CSet::size-1;tmp>=0;tmp--)
			if(cset.IsIn(tmp))
				cset.Sub(tmp);
			else
				cset.Add(tmp);
		if(m_Flags&regNewLine)
			cset.Sub('\n');
		/*
		if(!cset.m_Multis.IsEmpty())
			cset.CollatingInvert();
		*/
	}
UCHAR c = cset.GetOnly();
	if(c){
		EmitOrdinary(c);
	}else
		m_Error || m_Strip.Add(CSop(CSop::opAnyOf,StoreSet(cset)));
}

void CRegEx::CSet::Add(UCHAR c)
{
	m_Set[(BYTE)c]=TRUE;
	m_Hash+=c;
}

BOOL CRegEx::CSet::IsIn(UCHAR c)
{
	return m_Set[(BYTE)c];
}

void CRegEx::CSet::Sub(UCHAR c)
{
	m_Set[(BYTE)c]=FALSE;
	m_Hash-=c;
}

UCHAR CRegEx::CSet::GetOnly()
{
int rv = 0;
UCHAR only = 0;
	for(int tmp=0;tmp<size;tmp++){
		rv+=m_Set[tmp]?(only=tmp,1):0;
	}
	return (rv==1)?only:0;
}

int CRegEx::StoreSet(CSet& cset)
{
	for(int tmp=0;tmp<m_Sets.GetSize();tmp++)
		if(m_Sets[tmp]==cset)
			return tmp;
	return m_Sets.Add(cset);
}

void CRegEx::ParseBracketTerm(CSet& cset)
{
UCHAR c;
	switch((m_ParsePointer<m_Pattern.GetLength())?m_Pattern[m_ParsePointer]:0){
	case '[':
		c = ((m_ParsePointer+1)<m_Pattern.GetLength())?m_Pattern[m_ParsePointer+1]:0;
		break;
	case '-':
		m_Error || (m_Error=regeRange);
		// ??? point to nuls?
		return;
	default:
		c = 0;
		break;
	}
	switch(c){
	case ':':	// Character class
		m_ParsePointer+=2;
		if(m_ParsePointer>=m_Pattern.GetLength()){
			m_Error || (m_Error=regeBracket);
			// ??? point to nuls?
		}
		c = m_Pattern[m_ParsePointer];
		if(c== '-' || c==']'){
			m_Error || (m_Error=regeCType);
			// ??? point to nuls?
		}
		ParseBracketCClass(cset);
		if(m_ParsePointer>=m_Pattern.GetLength()){
			m_Error || (m_Error=regeBracket);
			// ??? point to nuls?
		}
		if((m_ParsePointer+1)>=m_Pattern.GetLength() || (m_Pattern.Mid(m_ParsePointer,2).Compare(":]"))){
			m_Error || (m_Error=regeCType);
			// ??? point to nuls?
		}else
			m_ParsePointer+=2;
		break;
	case '=':	// Equivalence class
		m_ParsePointer+=2;
		if(m_ParsePointer >= m_Pattern.GetLength()){
			m_Error || (m_Error=regeBracket);
			// ??? point to nuls?
		}
		c = m_Pattern[m_ParsePointer];
		if(c== '-' || c==']'){
			m_Error || (m_Error=regeCollate);
			// ??? point to nuls?
		}
		ParseBracketEClass(cset);
		if((m_ParsePointer+1)>=m_Pattern.GetLength() || (m_Pattern.Mid(m_ParsePointer,2).Compare("=]"))){
			m_Error || (m_Error=regeCollate);
			// ??? point to nuls?
		}else
			m_ParsePointer+=2;
		break;
	default:	// Symbol, character or range
		{
		UCHAR start, finish;
			start =  ParseBracketSymbol();
			if((m_ParsePointer<m_Pattern.GetLength() && m_Pattern[m_ParsePointer]=='-') /*&& (m_ParsePointer+1)<m_Pattern.GetLength() && m_Pattern[m_ParsePointer+1]==']'*/){
				// I believe the expression above is seetwo..
				// range.
				m_ParsePointer++;
				if(m_ParsePointer<m_Pattern.GetLength() && m_Pattern[m_ParsePointer]=='-'){
					m_ParsePointer++;
					finish='-';
				}else
					finish=ParseBracketSymbol();
			}else
				finish=start;
			if(((BYTE)start)>((BYTE)finish)){
				m_Error || (m_Error=regeRange);
				// ??? point to nuls?
			}
			for(int tmp=start;tmp<=(BYTE)finish;tmp++)
				cset.Add(tmp);
		}
		break;
	}
}

void CRegEx::ParseBracketCClass(CSet& cset)
{
static struct	{
	char *className;
	char *classChars;
}	cc[] = {
	{"alnum","ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"},
	{"alpha","ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"},
	{"blank"," \t"},
	{"cntrl","\007\b\t\n\v\f\r\1\2\3\4\5\6\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37\177"},
	{"digit","0123456789"},
	{"graph","ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"},
	{"lower","abcdefghijklmnopqrstuvwxyz"},
	{"print","ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~ "},
	{"punct","!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"},
	{"space","\t\n\v\f\r "},
	{"upper","ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
	{"xdigit","0123456789ABCDEFabcdef"}
};
CString cclass;
UCHAR c;
	while(m_ParsePointer < m_Pattern.GetLength() && isalpha(c=m_Pattern[m_ParsePointer])){
		cclass+=c;
		m_ParsePointer++;
	}
char *classChars = NULL;
	for(int tmp=0;tmp<(sizeof(cc)/sizeof(cc[0]));tmp++){
		if(!cclass.CompareNoCase(cc[tmp].className)){
			classChars=cc[tmp].classChars;
			break;
		}
	}
	if(!classChars){
		m_Error || (m_Error=regeCType);
		// ??? point to nuls?
		return;
	}
	while(*classChars)
		cset.Add(*(classChars++));
	// --- multis
}

void CRegEx::ParseBracketEClass(CSet& cset)
{
	cset.Add(ParseBracketCollatingElement('='));;
}

UCHAR CRegEx::ParseBracketCollatingElement(UCHAR term)
{
static struct	{
	char *entityName;
	char entity;
}	cc[] = { {"NUL",'\0'},{"SOH",'\001'},{"STX",'\002'},{"ETX",'\003'},{"EOT",'\004'},{"ENQ",'\005'},{"ACK",'\006'},{"BEL",'\007'},{"alert",'\007'},{"BS",'\010'},{"backspace",'\b'},{"HT",'\011'},{"tab",'\t'},{"LF",'\012'},{"newline",'\n'},{"VT",'\013'},{"vertical-tab",'\v'},{"FF",'\014'},{"form-feed",'\f'},{"CR",'\015'},{"carriage-return",'\r'},{"SO",'\016'},{"SI",'\017'},{"DLE",'\020'},{"DC1",'\021'},{"DC2",'\022'},{"DC3",'\023'},{"DC4",'\024'},{"NAK",'\025'},{"SYN",'\026'},{"ETB",'\027'},{"CAN",'\030'},{"EM",'\031'},{"SUB",'\032'},{"ESC",'\033'},{"IS4",'\034'},{"FS",'\034'},{"IS3",'\035'},{"GS",'\035'},{"IS2",'\036'},{"RS",'\036'},{"IS1",'\037'},{"US",'\037'},{"space",' '},{"exclamation-mark",'!'},{"quotation-mark",'"'},{"number-sign",'#'},{"dollar-sign",'$'},{"percent-sign",'%'},{"ampersand",'&'},{"apostrophe",'\''},{"left-parenthesis",'('},{"right-parenthesis",')'},{"asterisk",'*'},{"plus-sign",'+'},{"comma",','},{"hyphen",'-'},{"hyphen-minus",'-'},{"period",'.'},{"full-stop",'.'},{"slash",'/'},{"solidus",'/'},{"zero",'0'},{"one",'1'},{"two",'2'},{"three",'3'},{"four",'4'},{"five",'5'},{"six",'6'},{"seven",'7'},{"eight",'8'},{"nine",'9'},{"colon",':'},{"semicolon",';'},{"less-than-sign",'<'},{"equals-sign",'='},{"greater-than-sign",'>'},{"question-mark",'?'},{"commercial-at",'@'},{"left-square-bracket",'['},{"backslash",'\\'},{"reverse-solidus",'\\'},{"right-square-bracket",']'},{"circumflex",'^'},{"circumflex-accent",'^'},{"underscore",'_'},{"low-line",'_'},{"grave-accent",'`'},{"left-brace",'{'},{"left-curly-bracket",'{'},{"vertical-line",'|'},{"right-brace",'}'},{"right-curly-bracket",'}'},{"tilde",'~'},{"DEL",'\177'} };
CString seeTwo;
	seeTwo=term;
	seeTwo+=']';
CString entityName;
	while(m_ParsePointer<m_Pattern.GetLength() && !((m_ParsePointer+1)<m_Pattern.GetLength() && !m_Pattern.Mid(m_ParsePointer,2).Compare(seeTwo)))
		entityName+=m_Pattern[m_ParsePointer++];
	if(m_ParsePointer>=m_Pattern.GetLength()){
		m_Error || (m_Error=regeBracket);
		// ??? point to nuls?
		return 0;
	}
	for(int tmp=0;tmp<(sizeof(cc)/sizeof(cc[0]));tmp++)
		if(!entityName.CompareNoCase(cc[tmp].entityName))
			return cc[tmp].entity;
	if(entityName.GetLength()==1)
		return entityName[0];
	m_Error || (m_Error=regeCollate);
	// ??? point to nuls?
	return 0;
}

UCHAR CRegEx::ParseBracketSymbol()
{
	if(m_ParsePointer>=m_Pattern.GetLength()){
		m_Error || (m_Error=regeBracket);
		// ??? point to nuls?
	}
	if((m_ParsePointer+1)<m_Pattern.GetLength() && !m_Pattern.Mid(m_ParsePointer,2).Compare("[."))
		m_ParsePointer+=2;
	else
		return m_Pattern[m_ParsePointer++];
	// Collating symbol
UCHAR rv = ParseBracketCollatingElement('.');
	if((m_ParsePointer+1)<m_Pattern.GetLength() && !m_Pattern.Mid(m_ParsePointer,2).Compare("[."))
		m_ParsePointer+=2;
	else{
		m_Error || (m_Error=regeCollate);
		// ??? point to nuls?
	}
	return rv;
}

void CRegEx::EmitRepeat(int pos,int from,int to)
{
	if(m_Error)
		return;
	ASSERT(from<=to);
int finish = m_Strip.GetSize();
int copy;
#define	N 2
#define INF 3
#define REP(f,t) ((f)*8+(t))
#define MAP(n) (((n)<=1)?(n):((n)==256)?INF:N)
	switch(REP(MAP(from),MAP(to))){
	case REP(0,0):	// must be user doing ths??
		m_Strip.SetSize(pos);
		break;
	case REP(0,1):	// as in '?'
	case REP(0,N):	// as in '{1,n}?'
	case REP(0,INF):	// as in '{1,}?'
		// Kludge - emit y? as (y|) until something gets fixed..
		StripInsert(pos,CSop(CSop::opChoice0,pos));
		EmitRepeat(pos+1,1,to);
		m_Error || m_Strip.Add(CSop(CSop::opOr0,m_Strip.GetSize()-pos));
		m_Error || (m_Strip[pos].m_Operand=m_Strip.GetSize()-pos);
		m_Error || m_Strip.Add(CSop(CSop::opOr1,1));
		m_Error || m_Strip.Add(CSop(CSop::opChoice1,2));
		break;
	case REP(1,1):
		break;
	case REP(1,N):	// as in 'x?x{1,n-1}'
		// Kludge again..
		StripInsert(pos,CSop(CSop::opChoice0,pos));
		m_Error || m_Strip.Add(CSop(CSop::opOr0,m_Strip.GetSize()-pos));
		m_Error || (m_Strip[pos].m_Operand=m_Strip.GetSize()-pos);
		m_Error || m_Strip.Add(CSop(CSop::opOr1,1));
		m_Error || m_Strip.Add(CSop(CSop::opChoice1,2));
		copy = StripDuplicate(pos+1,finish+1);
		ASSERT(copy==(finish+4));
		EmitRepeat(copy,1,to-1);
		break;
	case REP(1,INF): // as in '+'
		StripInsert(pos,CSop(CSop::opPlus0,pos));
		m_Error || m_Strip.Add(CSop(CSop::opPlus1,m_Strip.GetSize()-pos));
		break;
	case REP(N,N):	// as in 'xx{from-1,to-1}'
		copy = StripDuplicate(pos,finish);
		EmitRepeat(copy,from-1,to-1);
		break;
	case REP(N,INF): // as in 'xx{n-1,}'
		copy = StripDuplicate(pos,finish);
		EmitRepeat(copy,from-1,to);
		break;
#ifndef NDEBUG
	default:
		ASSERT(FALSE);
		break;
#endif
	}
#undef MAP
#undef REP
#undef INF
#undef N
}

int CRegEx::StripDuplicate(int from,int to)
{
int rv = m_Strip.GetSize();
	ASSERT(from<=to);
	if(from==to)
		return rv;
	//  Maybe should be optimized for copying the whole thing.
	for(int tmp=from;tmp<to;tmp++)
		m_Strip.Add(m_Strip[tmp]);
	return rv;
}

void CRegEx::Categorize()
{
	if(m_Error)
		return;
	for(int tmp=0;tmp<(sizeof(m_Category)/sizeof(m_Category[0]));tmp++)
		if((!m_Category[tmp]) && IsInSets(tmp)){
		int cat = m_Categories++;
			m_Category[tmp]=cat;
			for(int c=tmp+1;c<(sizeof(m_Category)/sizeof(m_Category[0]));c++)
				if((!m_Category[c]) && IsInSameSets(tmp,c))
					m_Category[c]=cat;
		}
}

BOOL CRegEx::IsInSets(UCHAR c)
{
	for(int tmp=0;tmp<m_Sets.GetSize();tmp++)
		if(m_Sets[tmp].IsIn(c))
			return TRUE;
	return FALSE;
}

BOOL CRegEx::IsInSameSets(UCHAR c1,UCHAR c2)
{
	for(int tmp=0;tmp<m_Sets.GetSize();tmp++)
		if(m_Sets[tmp].IsIn(c1)!=m_Sets[tmp].IsIn(c2))
			return FALSE;
	return TRUE;
}

void CRegEx::FigureMust()
{
	if(m_Error)
		return;
	m_Must.Empty();
int stripLen = m_Strip.GetSize();
int seqStart, seqLength = 0;
int mustStart, mustLength = 0;
	for(int tmp=1;tmp<stripLen;tmp++){
		switch(m_Strip[tmp].m_Operator){
		case CSop::opChar:
			if(!seqLength)
				seqStart=tmp;
			seqLength++;
			break;
		case CSop::opPlus0:
		case CSop::opLeftParen:
		case CSop::opRightParen:
			break;	// Break, since they don't break the sequence
		case CSop::opQuest0:
		case CSop::opChoice0:
			// These ones we skip.
			do{
				tmp+=m_Strip[tmp].m_Operand;
				// I still think it could be ASSERTed..
				if(m_Strip[tmp].m_Operator!=CSop::opQuest1 && m_Strip[tmp].m_Operator!=CSop::opChoice1 && m_Strip[tmp].m_Operator!=CSop::opOr1){
					m_iFlags|=iflagsBad;
					return;
				}
			}while(m_Strip[tmp].m_Operator!=CSop::opQuest1 && m_Strip[tmp].m_Operator!=CSop::opChoice1);
			// Fallthrough..
		default:
			// End of sequence
			if(seqLength>mustLength){
				mustStart=seqStart;
				mustLength=seqLength;
			}
			seqLength=0;
			break;
		}
	}	// Hmm.. originally it's meant to be do while not opEnd.. 
	if(!mustLength)
		return;
	// Turn into string, but, wait, personally I'm sure it could be put in the main loop.. or maybe not..
	for(tmp=0;tmp<seqLength;tmp++){
		while(m_Strip[seqStart+tmp].m_Operator!=CSop::opChar)
			ASSERT(tmp<seqLength);
		m_Must+=m_Strip[tmp].m_Operand;
	}
}

int CRegEx::CountPluses()
{
	if(m_Error)
		return 0;
int stripLen = m_Strip.GetSize();
int rv = 0;
int nest = 0;
	for(int tmp=0;tmp<stripLen;tmp++){
		switch(m_Strip[tmp].m_Operator){
		case CSop::opPlus0:
			nest++;
			break;
		case CSop::opPlus1:
			if(nest>rv)
				rv = nest;
			nest--;
			break;
		}
	}	// Again, originally we were supposed to scan till opEnd..
	if(nest)
		m_iFlags|=iflagsBad;	// Could this be an ASSERTion?
	return rv;
}

void CRegEx::ParseLiteral()
{
	if(!m_Pattern.GetLength()){
		m_Error || (m_Error=regeEmpty);
		// ??? point to nuls?
	}
	while(m_ParsePointer < m_Pattern.GetLength())
		EmitOrdinary(m_Pattern[m_ParsePointer++]);
}

void CRegEx::ParseBRE(int stopa,int stopb)
{
int start = m_Strip.GetSize();
BOOL first=TRUE;
BOOL wasDollar=FALSE;
	if(m_ParsePointer<m_Pattern.GetLength() && m_Pattern[m_ParsePointer]=='^'){
		m_ParsePointer++;
		m_Error || m_Strip.Add(CSop(CSop::opBOL,0));
		m_iFlags|=iflagsUseBOL;
		m_BOLs++;
	}
CString stopstr;
	if(stopa){
		stopstr+=stopa;
		if(stopb)
			stopstr+=stopb;
	}
	while(m_ParsePointer < m_Pattern.GetLength() && !((m_ParsePointer+1)<m_Pattern.GetLength() && !m_Pattern.Mid(m_ParsePointer,2).Compare(stopstr))){
		wasDollar = ParseBREexp(first);
		first=FALSE;
	}
	if(wasDollar){	// Trailing anchor that was..
		m_Strip.SetSize(m_Strip.GetSize()-1);
		m_Error || m_Strip.Add(CSop(CSop::opEOL,0));
		m_iFlags|=iflagsUseEOL;
		m_EOLs++;
	}
	if(m_Strip.GetSize()==start){
		m_Error || (m_Error=regeEmpty);
		// ??? point to nuls?
	}
}

BOOL CRegEx::ParseBREexp(BOOL ordinaryStar)
{
int subno;
int pos = m_Strip.GetSize();
	ASSERT(m_ParsePointer<m_Pattern.GetLength());
int c = m_Pattern[m_ParsePointer++];
	if(c=='\\'){
		if(m_ParsePointer>=m_Pattern.GetLength()){
			m_Error || (m_Error=regeEscape);
			// ??? point to nuls
		}else
			c = 0x100|m_Pattern[m_ParsePointer++];
	}
	switch(c){
	case '.':
		if(m_Flags&regNewLine)
			EmitNonNewLineAny();
		else
			m_Error || m_Strip.Add(CSop(CSop::opAny,0));
		break;
	case '[':
		ParseBracket();
		break;
	case 0x100|'{':
		m_Error || (m_Error=regeBadRepeat);
		// ??? point to nuls?
		break;
	case 0x100|'(':
		m_Subexps++;
		subno=m_Subexps;
		m_ParseParens.SetAtGrow(m_Subexps,CParenthesis(m_Strip.GetSize()));
		m_Error || m_Strip.Add(CSop(CSop::opLeftParen,subno));
		if(m_ParsePointer<m_Pattern.GetLength() && !((m_ParsePointer+1)<m_Pattern.GetLength() && !m_Pattern.Mid(m_ParsePointer,2).Compare("\\)")))
			ParseBRE('\\',')');
		VERIFY(m_ParseParens[m_Subexps].m_End = m_Strip.GetSize());
		m_Error || m_Strip.Add(CSop(CSop::opRightParen,subno));
		if((m_ParsePointer+1) < m_Pattern.GetLength() && !m_Pattern.Mid(m_ParsePointer,2).Compare("\\)"))
			m_ParsePointer+=2;
		else{
			m_Error || (m_Error=regeParen);
			// ??? point to nuls?
		}
		break;
	case 0x100|')':
	case 0x100|'}':
		// Can this possibly happen?!
		m_Error || (m_Error=regeParen);
		// ??? point to nuls?
		break;
	case 0x100|'1':
	case 0x100|'2':
	case 0x100|'3':
	case 0x100|'4':
	case 0x100|'5':
	case 0x100|'6':
	case 0x100|'7':
	case 0x100|'8':
	case 0x100|'9':
		{
		int i = (c&0xFF)-'0';
			if(i < m_ParseParens.GetSize() && m_ParseParens[i].m_End){
				m_Error || m_Strip.Add(CSop(CSop::opBackRef0,i));
				ASSERT(m_ParseParens[i].m_Begin);
				ASSERT(m_Strip[m_ParseParens[i].m_Begin].m_Operator==CSop::opLeftParen);
				ASSERT(m_Strip[m_ParseParens[i].m_End].m_Operator==CSop::opRightParen);
				StripDuplicate(m_ParseParens[i].m_Begin+1,m_ParseParens[i].m_End);
				m_Error || m_Strip.Add(CSop(CSop::opBackRef1,i));
			}else{
				m_Error || (m_Error=regeSubReg);
				// ??? point to nuls?
			}
			m_bBackRefs=TRUE;
		}
		break;
	case '*':
		if(!ordinaryStar){
			m_Error || (m_Error=regeBadRepeat);
			// ??? point to nuls?
		}
		// Fallthrough..
	default:
		EmitOrdinary(c&0xFF);
		break;
	}
	if(m_ParsePointer<m_Pattern.GetLength() && m_Pattern[m_ParsePointer]=='*'){
		m_ParsePointer++;
		// as in '+?'
		StripInsert(pos,CSop(CSop::opPlus0,m_Strip.GetSize()-pos+1));
		m_Error || m_Strip.Add(CSop(CSop::opPlus1,m_Strip.GetSize()-pos));
		StripInsert(pos,CSop(CSop::opQuest0,m_Strip.GetSize()-pos+1));
		m_Error || m_Strip.Add(CSop(CSop::opQuest1,m_Strip.GetSize()-pos));
	}else if ((m_ParsePointer+1)<m_Pattern.GetLength() && !m_Pattern.Mid(m_ParsePointer,2).Compare("\\{")){
		m_ParsePointer+=2;
	int count = ParseCount();
	int count2;
		if(m_ParsePointer<m_Pattern.GetLength() && m_Pattern[m_ParsePointer]==','){
			m_ParsePointer++;
			if(m_ParsePointer<m_Pattern.GetLength() && isdigit(m_Pattern[m_ParsePointer])){
				count2=ParseCount();
				if(count>count2){
					m_Error || (m_Error=regeBadBrace);
					// ??? poin to nuls?
				}
			}else // Single number with comma
				count2=256;
		}else // Single number
			count2=count;
		EmitRepeat(pos,count,count2);
		if((m_ParsePointer+1)>=m_Pattern.GetLength() || m_Pattern.Mid(m_ParsePointer,2).Compare("\\}")){
			while(m_ParsePointer<m_Pattern.GetLength() && !((m_ParsePointer+1)<m_Pattern.GetLength() && !m_Pattern.Mid(m_ParsePointer,2).Compare("\\}")))
				m_ParsePointer++;
			if(m_ParsePointer>=m_Pattern.GetLength()){
				m_Error || (m_Error=regeBrace);
				// ??? point to nuls?
			}
			m_Error || (m_Error=regeBadBrace);
		}else
			m_ParsePointer+=2;
	}else if(c=='$')
		return TRUE;
	return FALSE;
}

CRegEx::CRegEx()
{
	m_bCompiled=FALSE;
}

LPCTSTR CRegEx::MatchFast(LPCTSTR begin)
{
	MatchStatesClear(CSop::stCurrent);
	m_Strip[1].m_MatchData|=CSop::stCurrent;
int stripLen = m_Strip.GetSize();
	MatchStep(1,stripLen-1,CSop::stCurrent,charNothing,CSop::stCurrent);
	MatchStatesCopy(CSop::stFresh,CSop::stCurrent);
LPCTSTR coldp = NULL;
LPCTSTR p = begin;
int c = (begin==m_mBegin)?charOut:((int)(BYTE)m_mPointer[-1]);
	for(;;){
		// next character..
	int lastc = c;
		c = (p==m_mEnd)?charOut:(int)*(BYTE*)p;
		if(MatchStatesEqual(CSop::stCurrent,CSop::stFresh))
			coldp=p;
		// Is there an EOL and/or BOL between lastc and c? - they ask..
	int	flagc=0;
	int	i = 0;
		if((lastc=='\n' && m_Flags&regNewLine) || (lastc==charOut && !(m_mFlags&regNotBOL))){
			flagc=charBOL;
			i=m_BOLs;
		}
		if((c=='\n' && m_Flags&regNewLine) || (c==charOut && !(m_mFlags&regNotEOL))){
			flagc=(flagc==charBOL)?charBOLEOL:charEOL;
			i+=m_EOLs;
		}
		if(i){
			for(;i>0;i--)
				MatchStep(1,stripLen-1,CSop::stCurrent,flagc,CSop::stCurrent);
		}
		// What about a word boundary? - they wonder..
		if((flagc==charBOL || (lastc!=charOut && !isWordableChar(c))) && (c!=charOut && isWordableChar(c)))
			flagc = charBOW;
		if((lastc!=charOut && isWordableChar(lastc)) && (flagc==charEOL || (c!=charOut && !isWordableChar(c))))
			flagc = charEOW;
		if(flagc==charBOW || flagc==charEOW){
			MatchStep(1,stripLen-1,CSop::stCurrent,flagc,CSop::stCurrent);
		}
		// Are we done? Now WE wonder..
		if((m_Strip[stripLen-1].m_MatchData&CSop::stCurrent) || p==m_mEnd)
			break;	// They insist I need to note break out.. Okay, I do..
		// Nope, we're not done. We have to face this character..
		MatchStatesCopy(CSop::stTemp,CSop::stCurrent);
		MatchStatesCopy(CSop::stCurrent,CSop::stFresh);
		ASSERT(c!=charOut);
		MatchStep(1,stripLen-1,CSop::stTemp,c,CSop::stCurrent);
		p++;
	}
	ASSERT(coldp);
	m_cOldP=coldp;	// *** I believe this variable can be changed 'in-place'
	if(m_Strip[stripLen-1].m_MatchData&CSop::stCurrent)
		return &p[1];
	else
		return NULL;
}

void CRegEx::MatchStatesClear(BYTE mask)
{
int stripLen = m_Strip.GetSize();
	for(int tmp=0;tmp<stripLen;tmp++)
		m_Strip[tmp].m_MatchData&=~mask;
}

void CRegEx::MatchStep(int from,int to,BYTE maskBefore,int charCode,BYTE maskAfter)
{
BOOL i;
int look;
int here = from;
	for(int pc=from;pc!=to;pc++,here++){
	CSop s=m_Strip[pc];
		switch(s.m_Operator){
		case CSop::opEnd:
			ASSERT(pc==(to-1));
			break;
		case CSop::opChar:
			// Only characters can match..
			ASSERT((charCode<charOut) || charCode!=s.m_Operand);
			if(charCode==s.m_Operand)
				m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskBefore)?maskAfter:0;
			break;
		case CSop::opBOL:
			if(charCode==charBOL || charCode==charBOLEOL)
				m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskBefore)?maskAfter:0;
			break;
		case CSop::opEOL:
			if(charCode==charEOL || charCode==charBOLEOL)
				m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskBefore)?maskAfter:0;
			break;
		case CSop::opBOW:
			if(charCode==charBOW)
				m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskBefore)?maskAfter:0;
			break;
		case CSop::opEOW:
			if(charCode==charEOW)
				m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskBefore)?maskAfter:0;
			break;
		case CSop::opAny:
			if(charCode<charOut)
				m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskBefore)?maskAfter:0;
			break;
		case CSop::opAnyOf:
			if(charCode<charOut && m_Sets[s.m_Operand].m_Set[charCode])
				m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskBefore)?maskAfter:0;
			break;
		case CSop::opBackRef0:	// Ignored here..
		case CSop::opBackRef1:
			m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			break;
		case CSop::opPlus0:
			// Forward, this is just an empty, comment says..
			m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			break;
		case CSop::opPlus1:
			m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			i =(m_Strip[here-s.m_Operand].m_MatchData&maskAfter)!=0;
			m_Strip[here-s.m_Operand].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			if(!i && (m_Strip[here-s.m_Operand].m_MatchData&maskAfter)){
				// oho, must reconsider loop body, comment says.. what's so 'oho' about it?
				pc-=s.m_Operand+1;
				here=pc;
			}
			break;
		case CSop::opQuest0:
			// two branches, both forward..
			m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			m_Strip[here+s.m_Operand].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			break;
		case CSop::opQuest1:
			// just an empty.. aren't we tired of justanempties?
			m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			break;
		case CSop::opLeftParen: // they say it's not significan there..
		case CSop::opRightParen:
			m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			break;
		case CSop::opChoice0:	// mark the first two branches..
			m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			ASSERT(m_Strip[pc+s.m_Operand].m_Operator==CSop::opOr1);
			m_Strip[here+s.m_Operand].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			break;
		case CSop::opOr0:	// done a branch, find the end of choice..
			if(m_Strip[here].m_MatchData&maskAfter){
				for(look=1;(s=m_Strip[pc+look]).m_Operator!=CSop::opChoice1;look+=s.m_Operand)
					ASSERT(s.m_Operator==CSop::opOr1);
				m_Strip[here+look].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			}
			break;
		case CSop::opOr1: // Propagate Choice's marking..
			m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			if(m_Strip[pc+s.m_Operand].m_Operator!=CSop::opChoice1){
				ASSERT(m_Strip[pc+s.m_Operand].m_Operator==CSop::opOr1);
				m_Strip[here+s.m_Operand].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			}
			break;
		case CSop::opChoice1: // Just empty.. :-)..
			m_Strip[here+1].m_MatchData|=(m_Strip[here].m_MatchData&maskAfter)?maskAfter:0;
			break;
#ifdef _DEBUG
		default:
			ASSERT(FALSE);
			break;
#endif
		}
	}
}

void CRegEx::MatchStatesCopy(BYTE dst,BYTE src)
{
int stripLen = m_Strip.GetSize();
	for(int tmp=0;tmp<stripLen;tmp++){
		// I believe this can be optimized, easily..
		m_Strip[tmp].m_MatchData&=~dst;
		m_Strip[tmp].m_MatchData|=(m_Strip[tmp].m_MatchData&src)?dst:0;
	}
}

BOOL CRegEx::MatchStatesEqual(BYTE m1,BYTE m2)
{
int stripLen = m_Strip.GetSize();
	for(int tmp=0;tmp<stripLen;tmp++){
	BYTE mm = m_Strip[tmp].m_MatchData;
		if(((mm&m1) && (mm&m2)) || !(mm&(m1|m2)))
			continue;
		return FALSE;
	}
	return TRUE;
}

LPCTSTR CRegEx::MatchSlow(LPCTSTR begin,LPCTSTR end,int from,int to)
{
	MatchStatesClear(CSop::stCurrent);
	m_Strip[from].m_MatchData|=CSop::stCurrent;
	MatchStep(from,to,CSop::stCurrent,charNothing,CSop::stCurrent);
LPCTSTR mp = NULL;
int c = (m_mBegin==m_mPointer)?charOut:(int)(BYTE)begin[-1];
LPCTSTR p = begin;
	for(;;){
		// next character..
	int lastc = c;
	c = (p==m_mEnd)?charOut:(int)*(BYTE*)p;
		// Now we start to wonder if there is an EOL and/or BOL between lastc and c
	int flagc = 0;
	int i = 0;
		if((lastc=='\n' && m_Flags&regNewLine) || (lastc==charOut && !(m_mFlags&regNotBOL))){
			flagc = charBOL;
			i = m_BOLs;
		}
		if((c=='\n' && m_Flags&regNewLine) || (c==charOut && !(m_mFlags&regNotEOL))){
			flagc = (flagc==charBOL)?charBOLEOL:charEOL;
			i+=m_EOLs;
		}
		if(i){
			for(;i>0;i--)
				MatchStep(from,to,CSop::stCurrent,flagc,CSop::stCurrent);
		}
		// Now we wonder about word boundaries..
		if((flagc==charBOL || (lastc!=charOut && !isWordableChar(lastc))) && (c!=charOut && isWordableChar(c)))
			flagc=charBOW;
		if((lastc!=charOut && isWordableChar(lastc)) && (flagc==charEOL || (c!=charOut && !isWordableChar(c))))
			flagc=charEOW;
		if(flagc==charBOW || flagc==charEOW){
			MatchStep(from,to,CSop::stCurrent,flagc,CSop::stCurrent);
		}
		// Are we done we all wonder??
		if(m_Strip[to].m_MatchData&CSop::stCurrent)
			mp=p;
		if(MatchStatesEqual(CSop::stCurrent,CSop::stEmpty) || p==end)
			break;	// Again, we're obliged to note break out. We do.
		// Sucks.. we have to face this character..
		MatchStatesCopy(CSop::stTemp,CSop::stCurrent);
		MatchStatesCopy(CSop::stCurrent,CSop::stEmpty);
		ASSERT(c!=charOut);
		MatchStep(from,to,CSop::stTemp,c,CSop::stCurrent);
		MatchStep(from,to,CSop::stCurrent,charNothing,CSop::stCurrent);
		p++;
	}
	return mp;
}

LPCTSTR CRegEx::MatchDissect(LPCTSTR begin,LPCTSTR end,int from,int to)
{
LPCTSTR sp = begin, dp;
LPCTSTR stp, rest, tail, ssp, oldssp, sep;
int ssub, esub;
int es;
int i;
	for(int ss=from;ss<to;ss = es){
		// Identify end of SubRE
		es = ss;
		switch(m_Strip[es].m_Operator){
		case CSop::opPlus0:
		case CSop::opQuest0:
			es+=m_Strip[es].m_Operand;
			break;
		case CSop::opChoice0:
			while(m_Strip[es].m_Operator!=CSop::opChoice1)
				es+=m_Strip[es].m_Operand;
			break;
		}
		es++;
		// Figure out what it matched
		switch(m_Strip[ss].m_Operator){
		case CSop::opEnd:
			ASSERT(FALSE);
			break;
		case CSop::opChar:
			sp++;
			break;
		case CSop::opBOL:
		case CSop::opEOL:
		case CSop::opBOW:
		case CSop::opEOW:
			break;
		case CSop::opAny:
		case CSop::opAnyOf:
			sp++;
			break;
		case CSop::opBackRef0:
		case CSop::opBackRef1:
			ASSERT(FALSE);
			break;
			// Cases where lenght of match is hard to find..
		case CSop::opQuest0:
			stp=end;
			for(;;){
				// How long could this one be??
				rest = MatchSlow(sp,stp,ss,es);
				ASSERT(rest);	// It did match.. It should've..
				// Could the rest match the rest? (good question)
				tail = MatchSlow(rest,end,es,to);
				if(tail==end)
					break;	// yup.
				// nope, try a shorter match for this one..
				stp=rest-1;
				ASSERT(stp>=sp);	// It did work.. It should've..
			}
			ssub=ss+1;
			esub=es-1;
			// Did innards match?
			if(MatchSlow(sp,rest,ssub,esub)){
				dp = MatchDissect(sp,rest,ssub,esub);
				ASSERT(dp==rest);
			}else	// nope..
				ASSERT(sp==rest);
			sp = rest;
			break;
		case CSop::opPlus0:
			stp=end;
			for(;;){
				// How long could this one be??
				rest = MatchSlow(sp,stp,ss,es);
				ASSERT(rest);	// It did.. It should've..
				// Could the rest match the rest?
				tail = MatchSlow(rest,end,es,to);
				if(tail==end)
					break;	// yup.
				// nope..
				stp=rest-1;
				ASSERT(stp>=sp);	// It should've worked, we believe..
			}
			ssub=ss+1;
			esub=es-1;
			ssp=sp;
			oldssp=ssp;
			for(;;){	// Find last match of innards..
				sep  = MatchSlow(ssp,rest,ssub,esub);
				if((!sep) || sep==ssp)
					break; // Failed or matched nothin'
				oldssp=ssp;
				ssp=sep;
			}
			if(!sep){
				// Last successfull..
				sep=ssp;
				ssp=oldssp;
			}
			ASSERT(sep=rest);	// Must exhaust substring they say..
			VERIFY(MatchSlow(ssp,sep,ssub,esub)==rest);	// Assert or verify - that is the question..
			dp = MatchDissect(ssp,sep,ssub,esub);
			ASSERT(dp==sep);
			sp=rest;
			break;
		case CSop::opChoice0:
			stp = end;
			for(;;){
				// how long..
				rest = MatchSlow(sp,stp,ss,es);
				ASSERT(rest);
				// Could it..
				tail = MatchSlow(rest,end,es,to);
				if(tail==end)
					break;	// y
				// n
				stp = rest-1;
				ASSERT(stp>=sp);
			}
			ssub=ss+1;
			esub=ss+m_Strip[ss].m_Operand-1;
			ASSERT(m_Strip[esub].m_Operator==CSop::opOr0);
			for(;;){	// Find first matching branch..
				if(MatchSlow(sp,rest,ssub,esub)==rest)
					break;
				// this one missed, try next..
				ASSERT(m_Strip[esub].m_Operator==CSop::opOr0);
				esub++;
				ASSERT(m_Strip[esub].m_Operator==CSop::opOr1);
				ssub=esub+1;
				esub+=m_Strip[esub].m_Operand;
				if(m_Strip[esub].m_Operator==CSop::opOr1)
					esub--;
				else
					ASSERT(m_Strip[esub].m_Operator==CSop::opChoice1);
			}
			dp = MatchDissect(sp,rest,ssub,esub);
			ASSERT(dp==rest);
			sp=rest;
			break;
		case CSop::opPlus1:
		case CSop::opQuest1:
		case CSop::opOr0:
		case CSop::opOr1:
		case CSop::opChoice1:
			ASSERT(FALSE);
			break;
		case CSop::opLeftParen:
			i = m_Strip[ss].m_Operand;
			ASSERT(0<i && i<=m_Subexps);
			m_Matches[i].m_Begin=sp-m_mBegin;
			break;
		case CSop::opRightParen:
			i = m_Strip[ss].m_Operand;
			ASSERT(0<i && i<=m_Subexps);
			m_Matches[i].m_End=sp-m_mBegin;
			break;
#ifdef _DEBUG
		default:	// Uh.. oh..
			ASSERT(FALSE);
			break;
#endif
		}
	}
	ASSERT(sp==end);
	return sp;
}

LPCTSTR CRegEx::MatchBackRef(LPCTSTR begin,LPCTSTR end,int from,int to,int level)
{
LPCTSTR sp = begin;
BOOL hard = FALSE;
	// Get as far as we can as long as it's easy
	for(int ss=from;!hard && ss<to;ss++){
	CSop s = m_Strip[ss];
		switch(s.m_Operator){
		case CSop::opChar:
			if(sp==end || *sp++ != s.m_Operand)
				return NULL;
			break;
		case CSop::opAny:
			if(sp==end)
				return NULL;
			sp++;	// I'm sure this ++ could be embedded in previous expression..
			break;
		case CSop::opAnyOf:
			if(sp==end || !m_Sets[s.m_Operand].IsIn(*sp++))
				return NULL;
			break;
		case CSop::opBOL:
			if(!((sp==m_mBegin && !(m_mFlags&regNotBOL)) || (sp<m_mEnd && *(sp-1)=='\n' && (m_Flags&regNewLine))))
				return NULL;
			break;
		case CSop::opEOL:
			if(!((sp==m_mEnd && !(m_mFlags&regNotEOL)) || (sp<m_mEnd && *sp=='\n' && (m_Flags&regNewLine))))
				return NULL;
			break;
		case CSop::opBOW:
			if(!(((sp==m_mBegin && !(m_mFlags&regNotBOL)) || (sp<m_mEnd && *(sp-1)=='\n' && (m_Flags&regNewLine)) || (sp>m_mBegin && !isWordableChar(*(sp-1)))) && (sp<m_mEnd && isWordableChar(*sp))))
				return NULL;
			break;
		case CSop::opEOW:
			if(!(((sp==m_mEnd && !(m_mFlags&regNotEOL)) || (sp<m_mEnd && *sp=='\n' && (m_Flags&regNewLine)) || (sp<m_mEnd && !isWordableChar(*sp))) && (sp>m_mBegin && isWordableChar(*(sp-1)))))
				return NULL;
			break;
		case CSop::opQuest1:
			break;
		case CSop::opOr0:	// Matches null, but needs to skip
			ss++;
			s = m_Strip[ss];
			do{
				ASSERT(s.m_Operator==CSop::opOr1);
				ss+=s.m_Operand;
			}while((s=m_Strip[ss]).m_Operator!=CSop::opChoice1);
			// Now we should note that ss++ gets us past the Choice1..
			break;
		default:
			// Have to make a choice..
			hard=TRUE;
			break;
		}
	}
	if(!hard){	// That was it..
		if(sp!=end)
			return NULL;
		return sp;
	}
	ss--;	// Adjust for ther for's final increment..
	// Hard stuff.. is going on and on..
CSop s = m_Strip[ss];
int i, len, offsave;
int ssub,esub;
LPCTSTR ssp, dp;
	switch(s.m_Operator){
	case CSop::opBackRef0:	// The vilest depths they say.. If I only knew what the 'viles' stands for..
		i = s.m_Operand;
		ASSERT(0<i && i<=m_Subexps);
		if(m_Matches[i].m_End<0)
			return NULL;
		ASSERT(m_Matches[i].m_Begin>=0);
		len = m_Matches[i].GetLength();
		ASSERT((end-m_mBegin)>=len);
		if(sp>end-len)
			return NULL;	// Not enough left to match..
		ssp = m_mBegin+m_Matches[i].m_Begin;
		if(memcmp(sp,ssp,len))
			return NULL;
		while(m_Strip[ss]!=CSop(CSop::opBackRef1,i))
			ss++;
		return MatchBackRef(sp+len,end,ss+1,to,level-1);
		break;
	case CSop::opQuest0:	// to null or not they wonder..
		dp = MatchBackRef(sp,end,ss+1,to,level);
		if(dp)
			return dp;	// not..
		return MatchBackRef(sp,end,ss+s.m_Operand+1,to,level-1);
		break;
	case CSop::opPlus0:
		ASSERT(m_mLastPos.GetSize());
		ASSERT(level+1 <= m_Pluses);
		m_mLastPos[level+1]=sp;
		return MatchBackRef(sp,end,ss+1,to,level+1);
		break;
	case CSop::opPlus1:
		if(sp == m_mLastPos[level])	// Last pass matched null
			return MatchBackRef(sp,end,ss+1,to,level-1);
		// Try another pass..
		m_mLastPos[level]=sp;
		dp = MatchBackRef(sp,end,ss-s.m_Operand+1,to,level);
		if(dp)
			return dp;
		return MatchBackRef(sp,end,ss+1,to,level-1);
		break;
	case CSop::opChoice0:	// find the right one, ifany
		ssub = ss+1;
		esub = ss+s.m_Operand-1;
		ASSERT(m_Strip[esub].m_Operator==CSop::opOr0);
		for(;;){	// Find first matching branch.
			dp = MatchBackRef(sp,end,ssub,esub,level);
			if(dp)
				return dp;
			// This one missed, try next one..
			if(m_Strip[esub].m_Operator==CSop::opChoice1)
				return NULL;	// There is none..
			esub++;
			ASSERT(m_Strip[esub].m_Operator==CSop::opOr1);
			ssub=esub+1;
			esub+=m_Strip[esub].m_Operand;
			if(m_Strip[esub].m_Operator==CSop::opOr1)
				esub--;
			else
				ASSERT(m_Strip[esub].m_Operator==CSop::opChoice1);
		}
		break;
	case CSop::opLeftParen:	// Must undo assignment if rest fails..
		i = s.m_Operand;
		ASSERT(0<i && i<=m_Subexps);
		offsave = m_Matches[i].m_Begin;
		m_Matches[i].m_Begin = sp-m_mBegin;
		dp = MatchBackRef(sp,end,ss+1,to,level);
		if(dp)
			return dp;
		m_Matches[i].m_Begin=offsave;
		return NULL;
		break;
	case CSop::opRightParen: // Must undo assignment if rest fails..
		i = s.m_Operand;
		ASSERT(0<i && i<=m_Subexps);
		offsave = m_Matches[i].m_End;
		m_Matches[i].m_End = sp-m_mBegin;
		dp = MatchBackRef(sp,end,ss+1,to,level);
		if(dp)
			return dp;
		m_Matches[i].m_End = offsave;
		return NULL;
		break;
#ifdef	_DEBUG
	default:
		ASSERT(FALSE);
		break;
#endif
	}
	ASSERT(FALSE);
	return NULL;	// Anyway - we can never get here..
}

#ifdef	_DEBUG
void CRegEx::CSop::Dump(CDumpContext& dc)
{
	switch(m_Operator){
	case opEnd:
		dc << "end";
		break;
	case opChar:
		dc << "char('" << (char)m_Operand << "')";
		break;
	case opBOL:
		dc << "BOL";
		break;
	case opEOL:
		dc << "EOL";
		break;
	case opAny:
		dc << "any";
		break;
	case opAnyOf:
		dc << "anyOf(" << m_Operand << ")";
		break;
	case opBackRef0:
		dc << "[ backref(" << m_Operand << ")";
		break;
	case opBackRef1:
		dc << "] backref(" << m_Operand << ")";
		break;
	case opPlus0:
		dc << "[ + (" << m_Operand << ")";
		break;
	case opPlus1:
		dc << "] + (" << m_Operand << ")";
		break;
	case opQuest0:
		dc << "[ ? (" << m_Operand << ")";
		break;
	case opQuest1:
		dc << "] ? (" << m_Operand << ")";
		break;
	case opLeftParen:
		dc << "[ (  (" << m_Operand << ")";
		break;
	case opRightParen:
		dc << "] )  (" << m_Operand << ")";
		break;
	case opChoice0:
		dc << "[ choice (" << m_Operand << ")";
		break;
	case opOr0:
		dc << "[ | (" << m_Operand << ")";
		break;
	case opOr1:
		dc << "] | (" << m_Operand << ")";
		break;
	case opChoice1:
		dc << "] choice (" << m_Operand << ")";
		break;
	case opBOW:
		dc << "BOW";
		break;
	case opEOW:
		dc << "EOW";
		break;
	}
}
void CRegEx::DumpStrip(CDumpContext& dc)
{
	for(int tmp=0;tmp<m_Strip.GetSize();tmp++)
		dc << tmp << ": " << m_Strip[tmp] << ";\n";
}
#endif


CString CRegEx::GetMatch(int match)
{
CString rv;
	if(!m_Matches.GetSize())
		return rv;
	ASSERT(m_Matches[0].m_Begin<m_Input.GetLength() && m_Matches[0].m_End<=m_Input.GetLength());
	if(match==matchPreMatch)
		return m_Input.Left(m_Matches[0].m_Begin);
	if(match==matchPostMatch)
		return m_Input.Mid(m_Matches[0].m_End);
	if(match<0 || match >= m_Matches.GetSize())
		return rv;
	if(m_Matches[match].m_Begin<0 || m_Matches[match].m_End<0)
		return rv;
	ASSERT(m_Matches[match].m_Begin<m_Input.GetLength() && m_Matches[match].m_End<=m_Input.GetLength());
	rv = m_Input.Mid(m_Matches[match].m_Begin,m_Matches[match].m_End-m_Matches[match].m_Begin);
	return rv;
}
