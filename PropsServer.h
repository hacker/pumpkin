// PropsServer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropsServer dialog

class CPropsServer : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropsServer)

// Construction
public:
	UINT m_PromptTimeOut;
	CPropsServer();
	~CPropsServer();

// Dialog Data
	//{{AFX_DATA(CPropsServer)
	enum { IDD = IDD_PROPS_SERVER };
	CButton	m_LogBrowseCtl;
	CButton	m_BrowseCtl;
	CSliderCtrl	m_PromptTimeoutCtl;
	int		m_RRQMode;
	CString	m_TFTPRoot;
	BOOL	m_TFTPSubdirs;
	int		m_WRQMode;
	CString	m_LogFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropsServer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropsServer)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowse();
	afx_msg void OnLogfileBrowse();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
