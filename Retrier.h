// Retrier.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRetrier window

class CPumpKINDlg;
class CRetrier : public CWnd
{
// Construction
public:
	CPumpKINDlg* m_Daddy;
	CRetrier(CPumpKINDlg *daddy);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRetrier)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRetrier();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRetrier)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
