// PropsNetwork.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropsNetwork dialog

class CPropsNetwork : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropsNetwork)

// Construction
public:
	CPropsNetwork();
	~CPropsNetwork();

// Dialog Data
	//{{AFX_DATA(CPropsNetwork)
	enum { IDD = IDD_PROPS_NETWORK };
	CSpinButtonCtrl	m_BSizeSpinCtl;
	CSpinButtonCtrl	m_TimeSpinCtl;
	CSpinButtonCtrl	m_SpeakSpinCtl;
	CSpinButtonCtrl	m_ListenSpinCtl;
	UINT	m_ListenPort;
	UINT	m_SpeakPort;
	UINT	m_TimeOut;
	UINT	m_BlockSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropsNetwork)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropsNetwork)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
