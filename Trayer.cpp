// Trayer.cpp : implementation file
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "Trayer.h"

#include "PumpKINDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrayer

CTrayer::CTrayer()
{
	m_inMenu=0;
}

CTrayer::~CTrayer()
{
}


BEGIN_MESSAGE_MAP(CTrayer, CWnd)
	//{{AFX_MSG_MAP(CTrayer)
	ON_MESSAGE(WM_TRAYICON, OnTray)
	ON_COMMAND(ID_TRAY_ABOUTPUMPKIN, OnTrayAboutpumpkin)
	ON_COMMAND(ID_TRAY_EXIT, OnTrayExit)
	ON_COMMAND(ID_TRAY_FETCHFILE, OnTrayFetchfile)
	ON_COMMAND(ID_TRAY_HELP, OnTrayHelp)
	ON_COMMAND(ID_TRAY_OPENFILESFOLDER, OnTrayOpenfilesfolder)
	ON_COMMAND(ID_TRAY_OPTIONS, OnTrayOptions)
	ON_COMMAND(ID_TRAY_SENDFILE, OnTraySendfile)
	ON_COMMAND(ID_TRAY_SHOWPUMPKINWINDOW, OnTrayShowpumpkinwindow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTrayer message handlers

CTrayer::CTrayer(CPumpKINDlg* daddy)
	: m_Daddy(daddy)
{
	m_inMenu=0;
}

LRESULT CTrayer::OnTray(WPARAM wP,LPARAM lP)
{
	ASSERT(wP==IDC_TRAYICON);
	switch(lP){
	case WM_RBUTTONDOWN:
		{
		CMenu menu;
			VERIFY(menu.LoadMenu(IDM_POPUPS));
		CMenu *popUp = menu.GetSubMenu(0);
			ASSERT(popUp);
		CPoint pt;
			VERIFY(::GetCursorPos(&pt));
			m_inMenu++;
			SetForegroundWindow();
			popUp->CheckMenuItem(ID_TRAY_SHOWPUMPKINWINDOW,MF_BYCOMMAND|(IsWindowVisible()?MF_CHECKED:MF_UNCHECKED));
			popUp->TrackPopupMenu(TPM_RIGHTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,pt.x,pt.y,this);
			m_inMenu--;
			SendMessage(WM_NULL);
		}
		break;
	case WM_LBUTTONDOWN:
		m_Daddy->SendMessage(WM_COMMAND,ID_TRAY_SHOWPUMPKINWINDOW);
		break;
	}
	return 0;
}

void CTrayer::OnTrayAboutpumpkin() 
{
	m_Daddy->SendMessage(WM_COMMAND,ID_TRAY_ABOUTPUMPKIN);
}

void CTrayer::OnTrayExit() 
{
	m_Daddy->SendMessage(WM_COMMAND,ID_TRAY_EXIT);
}

void CTrayer::OnTrayFetchfile() 
{
	m_Daddy->SendMessage(WM_COMMAND,ID_TRAY_FETCHFILE);
}

void CTrayer::OnTrayHelp() 
{
	m_Daddy->SendMessage(WM_COMMAND,ID_TRAY_HELP);
}

void CTrayer::OnTrayOpenfilesfolder() 
{
	m_Daddy->SendMessage(WM_COMMAND,ID_TRAY_OPENFILESFOLDER);
}

void CTrayer::OnTrayOptions() 
{
	m_Daddy->SendMessage(WM_COMMAND,ID_TRAY_OPTIONS);
}

void CTrayer::OnTraySendfile() 
{
	m_Daddy->SendMessage(WM_COMMAND,ID_TRAY_SENDFILE);
}

void CTrayer::OnTrayShowpumpkinwindow() 
{
	m_Daddy->SendMessage(WM_COMMAND,ID_TRAY_SHOWPUMPKINWINDOW);
}
