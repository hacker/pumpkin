// PumpKINDlg.h : header file
//

//#define	REVERSEBYTES(w)	MAKEWORD(HIBYTE(w),LOBYTE(w));
#define	REVERSEBYTES(w) (htons((WORD)w))
#define	tftpoBSize	"blksize"
#define	tftpoTSize	"tsize"
#define	tftpoTOut	"timeout"

#define	tftpoXResume	"x-resume"

struct	tftp	{
	typedef CMapStringToString tftpOptions;
	typedef WORD tftpLength;
	typedef WORD tftpOpcode;
	tftpLength	length;
	tftpOpcode opcode;
	enum	{
		opRRQ = 1,
		opWRQ = 2,
		opDATA = 3,
		opACK = 4,
		opERROR = 5,
		opOACK = 6,

		errUndefined = 0,
		errNotFound = 1,
		errAccessViolation = 2,
		errDiskFull = 3,
		errIllegalOp = 4,
		errUnknownTID = 5,
		errFileExists = 6,
		errNoUser = 7,
		errOption = 8
	};
	struct	tftpUNKNOWN {
		BYTE data[1];
	};
	struct	tftpRQ {
		BYTE data[1];
public:
	void Set(LPCTSTR file,LPCTSTR type,tftpOptions* ops=NULL);
	static UINT tftpSize(LPCTSTR file,LPCTSTR type,tftpOptions* ops=NULL);
	};
	struct	tftpRRQ {
		BYTE data[1];
public:
	void Set(LPCTSTR file,LPCTSTR type,tftpOptions* ops=NULL);
	static UINT tftpSize(LPCTSTR file,LPCTSTR type,tftpOptions* ops=NULL);
	};
	struct	tftpWRQ {
		BYTE data[1];
public:
	void Set(LPCTSTR file,LPCTSTR type,tftpOptions* ops=NULL);
	static UINT tftpSize(LPCTSTR file,LPCTSTR type,tftpOptions* ops=NULL);
	};
	struct	tftpDATA {
		WORD block;
		BYTE data[1];
	public:
	WORD Block();
	void SetBlock(WORD b);
	static UINT tftpSize(UINT blkSize=512);
	};
	struct tftpACK {
		WORD block;
	public:
	static UINT tftpSize();
	void SetBlock(WORD b);
	WORD Block();
	};
	struct	tftpERROR {
		typedef WORD tftpErrorCode;
		tftpErrorCode code;
		BYTE data[1];
	public:
	void SetCode(WORD c);
	WORD Code();
		static UINT tftpSize(LPCTSTR msg);
	};
	struct tftpOACK {
		BYTE data[1];
	public:
	void Set(tftpOptions* ops);
		static UINT tftpSize(tftpOptions* ops);
	};
	union	tftpPacket {
		tftpUNKNOWN m_UNKNOWN;
		tftpRQ m_RQ;
		tftpRRQ m_RRQ;
		tftpWRQ m_WRQ;
		tftpDATA m_DATA;
		tftpACK m_ACK;
		tftpERROR m_ERROR;
		tftpOACK m_OACK;
	}	data;

	void	*udpBase(void)	{ return &opcode; }
public:
	static tftp* Copy(tftp *src);
	CString errMessage();
	void SetOpcode(WORD op);
	void errSet(UINT code,LPCTSTR msg);
	static tftp* Allocate(UINT tftpSize=0);
	BOOL Send(CAsyncSocket *socket,SOCKADDR_IN* saddr);
	CString rqType();
	CString rqFileName();
	UINT GetOptions(tftpOptions* ops);
	tftp();
	UINT Opcode();
	BOOL Receive(CAsyncSocket* socket,UINT maxLength,SOCKADDR_IN *sin=NULL);
};
#define	tftpHdrSize	(sizeof(tftp)-sizeof(tftp::tftpPacket))
#define	tftpSlackSize sizeof(tftp::tftpLength)

struct acl_rule {
 enum {
	 opRRQ=tftp::opRRQ, opWRQ=tftp::opWRQ
 };
 int op;
 DWORD addr, mask;
 enum {
	rrqGrant=0, rrqPrompt, rrqDeny,
	rrqRules,
	rrqNone=-1
 };
 enum {
	 wrqGrant=0, wrqPromptIfExists, wrqPrompt, wrqDeny,
	 wrqRules,
	 wrqNone=-1
 };
 int target;

 acl_rule()
	 : op(-1), addr(0), mask(0), target(-1) { }
 acl_rule(int o,DWORD a,DWORD m,int t)
	 : op(o), addr(a), mask(m), target(t) { }
 acl_rule(const acl_rule& s)
	 : op(s.op), addr(s.addr), mask(s.mask), target(s.target) { }

 BOOL IsValid() {
	 if(op==opRRQ) {
		 if(target<rrqNone || target>=rrqRules)
			 return FALSE;
	 }else if(op==opWRQ) {
		 if(target<wrqNone || target>=wrqRules)
			 return FALSE;
	 }else
		 return FALSE;
	 return TRUE;
 }

 BOOL IsMatch(int o,DWORD a) {
	 if(o!=op) return FALSE;
	 if( (a&mask) != (addr&mask)) return FALSE;
	 return TRUE;
 }

 CString str_addr() {
	 return inet_ntoa(*(struct in_addr*)&addr);
 }
 CString str_mask() {
	 return inet_ntoa(*(struct in_addr*)&mask);
 }
 CString str_target() {
	 if(op==opRRQ) {
		 switch(target) {
		 case rrqNone: return "fallback";
		 case rrqGrant: return "grant";
		 case rrqPrompt: return "prompt";
		 case rrqDeny: return "deny";
		 default: return "?";
		 }
	 }else if(op==opWRQ) {
		 switch(target) {
		 case wrqNone: return "fallback";
		 case wrqGrant: return "grant";
		 case wrqPromptIfExists: return "prompt if exists";
		 case wrqPrompt: return "prompt";
		 case wrqDeny: return "deny";
		 default: return "?";
		 }
	 }else
		 return "?";
 }

 void SaveProfile(CWinApp* app,int i) {
	 CString n; n.Format("%d",i);
	 app->WriteProfileInt("ACL","op_"+n,op);
	 app->WriteProfileString("ACL","addr_"+n,str_addr());
	 app->WriteProfileString("ACL","mask_"+n,str_mask());
	 app->WriteProfileInt("ACL","target_"+n,target);
 }

 void LoadProfile(CWinApp* app,int i) {
	 CString n; n.Format("%d",i);
	 op=app->GetProfileInt("ACL","op_"+n,-1);
	 addr=inet_addr(app->GetProfileString("ACL","addr_"+n));
	 mask=inet_addr(app->GetProfileString("ACL","mask_"+n));
	 target=app->GetProfileInt("ACL","target_"+n,-1);
 }

};

class acl_rules_t : public CArray<acl_rule,acl_rule&> {
public:

	acl_rules_t& operator=(const acl_rules_t& s) {
		// Copy(s); XXX: unsuprisingly, there's a bug in MFC Copy, *pDst++=*pSrc (no ++ for Src)
		RemoveAll();
		int ns = s.GetSize();
		SetSize(ns);
		for(int i=0;i<ns;++i)
			m_pData[i]=s.m_pData[i];
		return *this;
	}

	int AppendRule(acl_rule& r) {
		return Add(r);
	}

	void DeleteRule(int r) {
		RemoveAt(r);
	}

	int FindRule(int op,DWORD ip) {
		for(int i=0;i<GetSize();++i)
			if(m_pData[i].IsMatch(op,ip))
				return i;
		return -1;
	}

	int FindTarget(int op,DWORD ip) {
		int r=FindRule(op,ip);
		if(r<0) return -1;
		return m_pData[r].target;
	}

	void SaveProfile(CWinApp* app) {
		int s=GetSize();
		for(int i=0;i<s;++i)
			m_pData[i].SaveProfile(app,i);
		app->WriteProfileInt("ACL","rules",s);
	}
	void LoadProfile(CWinApp* app) {
		RemoveAll();
		int s=app->GetProfileInt("ACL","rules",0);
		for(int i=0;i<s;++i) {
			acl_rule r;
			r.LoadProfile(app,i);
			if(r.IsValid())
				Add(r);
		}
	}
};

class CPumpKINDlg;
class CListenSocket : public CAsyncSocket	{
public:
	CPumpKINDlg* m_Daddy;
	BOOL m_bListen;

	CListenSocket()
		: m_bListen(FALSE), m_Daddy(0) { }

	BOOL SetListen(BOOL b);
	virtual void OnReceive(int nErrorCode);
};

typedef	CList<tftp*,tftp*>	CTFTPList;
class CResolver;
class CXferSocket : public CAsyncSocket	{
public:
	UINT m__timeOut;
	UINT m__blkSize;
	tftp::tftpOptions m_Options;
	LONG m_xferSize;
	UINT m_timeOut;
	UINT m_blkSize;
	BOOL m_bRetry;
	void SetTry(tftp *p=NULL);
	tftp* m_Retry;
	void OnRetry();
	BOOL CheckBadRelativeness(LPCTSTR file);
	CString ApplyRootGently(LPCTSTR fn);
	CString m_HostName;
	virtual void OnResolved();
	virtual void OnFailedToResolve();
	CResolver *m_wndResolver;
	BYTE m_ResolveBuff[MAXGETHOSTSTRUCT];
	virtual void Abort();
	void ResetTimeout();
	enum	{
		stateNone, stateDeny, stateInit, stateOACK, stateXfer, stateFinish, stateClosing
	};
	int state;
	void Deny(UINT errCode,UINT errID);
	void Deny(CFileException* e);
	CString ApplyRoot(LPCTSTR fileName);
	void TurnSlashes(CString& fn,BOOL bBack=TRUE);
	virtual void Destroy(BOOL success=TRUE);
	void PostError(CFileException* e);
	void PostError(UINT errCode,UINT errID);
	void PostTFTP(tftp* p,BOOL retryable=FALSE);
	CXferSocket(CPumpKINDlg *daddy,LPCTSTR fileName,LPCTSTR type,SOCKADDR_IN* sin);
	CFile m_File;
	virtual ULONG GetACK();
	CString m_Type;
	CString m_FileName;
	CXferSocket();
	void UpdateList();
	void SetPeer(SOCKADDR_IN *sin);
	virtual BOOL OnTFTP(tftp* p) = 0;
	virtual void OnReceive(int nErrorCode);
	void DoSelect(BOOL do_select=FALSE);
	SOCKADDR_IN m_Peer;
	virtual void OnSend(int nErrorCode);
	CPumpKINDlg* m_Daddy;
	CTFTPList m_Queue;

	DECLARE_DYNAMIC(CXferSocket)
};
class CWRQSocket : public CXferSocket {
public:
	BOOL m_bResume;
	void OnHostKnown();
	virtual void OnResolved();
	UINT m_LastSlack;
	ULONG GetACK();
	void DoXfer();
	UINT m_ACK;
	BOOL SaveAs(CString& fn);
	BOOL RenameFile(CString& fn);
	BOOL m_Rename;
	BOOL ConfirmRequest();
	BOOL Create(LPCTSTR localFile=NULL,LPCTSTR hostName=NULL);
	CWRQSocket(CPumpKINDlg* daddy,LPCTSTR fileName,LPCTSTR type,SOCKADDR_IN *sin);
	BOOL OnTFTP(tftp* p);

	DECLARE_DYNAMIC(CWRQSocket)
};
class CRRQSocket : public CXferSocket {
public:
	UINT m_ACKtoClose;
	void OnHostKnown();
	virtual void OnResolved();
	BOOL ConfirmRequest();
	WORD m_LastSlack;
	UINT m_ACK;
	BOOL OnTFTP(tftp* p);
	ULONG GetACK(void);
	void DoXfer();
	CRRQSocket(CPumpKINDlg *daddy,LPCTSTR fileName,LPCTSTR type,SOCKADDR_IN *sin);
	BOOL Create(LPCTSTR localFile=NULL,LPCTSTR hostName=NULL);

	DECLARE_DYNAMIC(CRRQSocket)
};

typedef	CMap<SOCKET,SOCKET,CXferSocket*,CXferSocket*>	CTIDMap;
typedef CMap<CTime*,CTime*,CTime*,CTime*> CTimeMap;

/////////////////////////////////////////////////////////////////////////////
// CPumpKINDlg dialog

class CTrayer;
class CRetrier;
class CPumpKINDlg : public CDialog
{
// Construction
public:
	void AdjustButton(CWnd& w,CRect& wrc);
	void RecalcLayout(int,int);
	CSize m_MinSize;
	UINT m_bottomGapLog;
	UINT m_bottomGapListen;
	UINT m_rightGapList;
	UINT m_rightGapButtons;
	CString m_lastlogerr;
	void LogLine(LPCTSTR str);
	CString m_LogFile;
	BOOL m_bListen;
	acl_rules_t m_aclRules;
	CString m_bnwRequest;
	CString m_bnwSuccess;
	CString m_bnwAbort;
	CBellsNWhistles m_bnw;
	CTrayer *m_Trayer;
	CTimeSpan m_RetryTimeOut;
	virtual ~CPumpKINDlg();
	CRetrier* m_Retrier;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL m_bShown;
	BOOL m_bExiting;
	void SaveSettings();
	void LoadSettings();
	void SetupButtons();
	BITMAP m_bitmapBack;
	CBitmap m_bmpBack;
	UINT m_PromptTimeOut;
	UINT m_SpeakPort;
	void LogLine(UINT msgID);
	CTimeMap m_LogTimes;
	void LogLineToFile(LPCTSTR str);
	void LogLineToScreen(LPCTSTR str);
	int m_LogLength;
	enum	{
		subitemFile=0, subitemType, subitemPeer, subitemBytes, subitemTSize
	};
	int m_iWRQ;
	int m_iRRQ;
	CImageList m_Images;
	CTIDMap m_Xfers;
	CTimeSpan m_TFTPTimeOut;
	enum	{
		rrqGiveAll=acl_rule::rrqGrant,
		rrqAlwaysConfirm=acl_rule::rrqPrompt,
		rrqDenyAll=acl_rule::rrqDeny,
		rrqFallback=acl_rule::rrqNone,
		rrqGrant=rrqGiveAll, rrqDeny=rrqDenyAll, rrqPrompt=rrqAlwaysConfirm
	};
	enum	{
		wrqTakeAll=acl_rule::wrqGrant,
		wrqConfirmIfExists=acl_rule::wrqPromptIfExists,
		wrqAlwaysConfirm=acl_rule::wrqPrompt,
		wrqDenyAll=acl_rule::wrqDeny,
		wrqFallback=acl_rule::wrqNone,
		wrqGrant=wrqTakeAll,wrqDeny=wrqDenyAll, wrqPrompt=wrqAlwaysConfirm
	};
	UINT m_RRQMode;
	UINT m_WRQMode;
	BOOL m_bTFTPSubdirs;
	CString m_TFTPRoot;
	UINT m_ListenPort;
	UINT m_BlockSize;
	CListenSocket m_Listener;
	CPumpKINDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPumpKINDlg)
	enum { IDD = IDD_PUMPKIN_DIALOG };
	CButton	m_HelpCtl;
	CButton	m_PutCtl;
	CButton	m_GetCtl;
	CButton	m_ExitCtl;
	CButton	m_ListenCtl;
	CButton	m_AbortCtl;
	CButton	m_OptionsCtl;
	CListBox	m_Log;
	CListCtrl	m_List;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPumpKINDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_tooltip;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPumpKINDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnOptions();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnExit();
	afx_msg void OnPut();
	afx_msg void OnGet();
	afx_msg void OnDeleteallitemsConnections(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemConnections(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInsertitemConnections(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedConnections(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAbort();
	afx_msg void OnClose();
	afx_msg void OnTrayShowpumpkinwindow();
	afx_msg void OnTrayListen();
	afx_msg void OnTrayExit();
	afx_msg void OnTrayAboutpumpkin();
	afx_msg void OnTrayFetchfile();
	afx_msg void OnTrayHelp();
	afx_msg void OnTrayOptions();
	afx_msg void OnTraySendfile();
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnSelchangeLog();
	afx_msg void OnTrayOpenfilesfolder();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	virtual void OnCancel();
	afx_msg void OnHelp();
	afx_msg void OnListening();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
