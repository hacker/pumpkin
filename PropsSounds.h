// PropsSounds.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropsSounds dialog

class CPropsSounds : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropsSounds)

// Construction
public:
	void Play(CComboBox& ctl);
	void Browse(CComboBox& ctl);
	CBellsNWhistles* m_bnw;
	CPropsSounds();
	~CPropsSounds();

// Dialog Data
	//{{AFX_DATA(CPropsSounds)
	enum { IDD = IDD_PROPS_SOUNDS };
	CButton	m_RequestPlayCtl;
	CButton	m_RequestBrowseCtl;
	CComboBox	m_RequestCtl;
	CButton	m_SuccessPlayCtl;
	CButton	m_SuccessBrowseCtl;
	CComboBox	m_SuccessCtl;
	CButton	m_AbortPlayCtl;
	CButton	m_AbortBrowseCtl;
	CComboBox	m_AbortCtl;
	CString	m_Abort;
	CString	m_Success;
	CString	m_Request;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropsSounds)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropsSounds)
	virtual BOOL OnInitDialog();
	afx_msg void OnAbortedBrowse();
	afx_msg void OnFinishedBrowse();
	afx_msg void OnRingBrowse();
	afx_msg void OnAbortedPlay();
	afx_msg void OnFinishedPlay();
	afx_msg void OnRingPlay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
