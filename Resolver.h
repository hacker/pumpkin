// Resolver.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CResolver window

class CXferSocket;
class CResolver : public CWnd
{
// Construction
public:
	BOOL Resolve();
	HANDLE m_hAsync;
	CResolver(CXferSocket *socket);
	CXferSocket* socket;
	CResolver();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResolver)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CResolver();

	// Generated message map functions
protected:
	//{{AFX_MSG(CResolver)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnResolved(WPARAM wP,LPARAM lP);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
