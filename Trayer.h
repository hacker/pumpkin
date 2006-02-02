// Trayer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTrayer window

class CPumpKINDlg;
class CTrayer : public CWnd
{
// Construction
public:
	int m_inMenu;
	CPumpKINDlg* m_Daddy;
	CTrayer(CPumpKINDlg* daddy);
	CTrayer();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTrayer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTrayer();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTrayer)
	afx_msg LRESULT OnTray(WPARAM,LPARAM);
	afx_msg void OnTrayAboutpumpkin();
	afx_msg void OnTrayExit();
	afx_msg void OnTrayFetchfile();
	afx_msg void OnTrayHelp();
	afx_msg void OnTrayOpenfilesfolder();
	afx_msg void OnTrayOptions();
	afx_msg void OnTraySendfile();
	afx_msg void OnTrayShowpumpkinwindow();
	afx_msg void OnTrayListen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
