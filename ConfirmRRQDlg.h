// ConfirmRRQDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConfirmRRQDlg dialog

class CRRQSocket;
class CConfirmRRQDlg : public CDialog
{
// Construction
public:
	CRRQSocket * m_Daddy;
	CConfirmRRQDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfirmRRQDlg)
	enum { IDD = IDD_CONFIRM_RRQ };
	CString	m_File;
	CString	m_Host;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfirmRRQDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfirmRRQDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
