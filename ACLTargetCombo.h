// ACLTargetCombo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CACLTargetCombo window

class CACLTargetCombo : public CComboBox
{
// Construction
public:
	void SetTarget(int t,int op=-1);
	int GetTarget();
	void SetOp(int op);
	int m_op;
	CACLTargetCombo();

// Attributes
public:
	CMap<int,int,int,int> m_tmap;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CACLTargetCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CACLTargetCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(CACLTargetCombo)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
