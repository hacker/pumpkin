// PumpKIN.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "PumpKINDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPumpKINApp

BEGIN_MESSAGE_MAP(CPumpKINApp, CWinApp)
	//{{AFX_MSG_MAP(CPumpKINApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPumpKINApp construction

CPumpKINApp::CPumpKINApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPumpKINApp object

CPumpKINApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPumpKINApp initialization

BOOL CPumpKINApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	SetRegistryKey(IDS_REGISTRYKEY);
	m_HelpFile = m_pszHelpFilePath;
	m_HelpFile+=">Standard";
	m_pszHelpFilePath=(LPCTSTR)m_HelpFile;

	CPumpKINDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
