// PumpKINDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "PumpKINDlg.h"

#include "ACLTargetCombo.h"
#include "PropsServer.h"
#include "PropsNetwork.h"
#include "PropsSounds.h"
#include "PropsACL.h"
#include "ConfirmRRQDlg.h"
#include "ConfirmWRQDlg.h"
#include "RequestDlg.h"
#include "Resolver.h"
#include "Retrier.h"
#include "Trayer.h"

#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXferSocket, CAsyncSocket)
IMPLEMENT_DYNAMIC(CWRQSocket, CXferSocket)
IMPLEMENT_DYNAMIC(CRRQSocket, CXferSocket)

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnKlevernet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_KLEVERNET, OnKlevernet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPumpKINDlg dialog

CPumpKINDlg::CPumpKINDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPumpKINDlg::IDD, pParent)
{
	m_Listener.m_Daddy = this;

	m_bListen = TRUE;

	m_ListenPort = 69;
	m_bTFTPSubdirs = TRUE;
	m_RRQMode = rrqAlwaysConfirm;
	m_WRQMode = wrqAlwaysConfirm;
	m_TFTPTimeOut = CTimeSpan(0,0,0,30);
	m_RetryTimeOut = CTimeSpan(0,0,0,10);
	m_LogLength = 100;
	m_SpeakPort = 69;
	m_PromptTimeOut=30;
	m_bShown=TRUE;
	m_bExiting=FALSE;
	m_BlockSize=1024;
	m_bnw.AssignSound("(bang)",IDR_WAVE_RING,CBellsNWhistles::CBang::bangResource);
	m_bnw.AssignSound("(done)",IDR_WAVE_FINISHED,CBellsNWhistles::CBang::bangResource);
	m_bnw.AssignSound("(oops)",IDR_WAVE_ABORTED,CBellsNWhistles::CBang::bangResource);
	m_bnw.AssignSound("(none)",(int)0,CBellsNWhistles::CBang::bangNone);
	m_bnwRequest="(bang)"; m_bnwSuccess="(done)";
	m_bnwAbort="(oops)";
	//{{AFX_DATA_INIT(CPumpKINDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bmpBack.LoadBitmap(IDB_BACKGROUND);
	m_bmpBack.GetBitmap(&m_bitmapBack);
	m_Retrier = new CRetrier(this);
	ASSERT(m_Retrier);
	m_Trayer = new CTrayer(this);
	ASSERT(m_Trayer);
	/* Ensure we're backwards compatible */
	ASSERT(CPumpKINDlg::rrqGiveAll==0);
	ASSERT(CPumpKINDlg::rrqAlwaysConfirm==1);
	ASSERT(CPumpKINDlg::rrqDenyAll==2);
	ASSERT(CPumpKINDlg::wrqTakeAll==0);
	ASSERT(CPumpKINDlg::wrqConfirmIfExists==1);
	ASSERT(CPumpKINDlg::wrqAlwaysConfirm==2);
	ASSERT(CPumpKINDlg::wrqDenyAll==3);
	/* -- */
	LoadSettings();
}

void CPumpKINDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPumpKINDlg)
	DDX_Control(pDX, ID_HELP, m_HelpCtl);
	DDX_Control(pDX, IDC_PUT, m_PutCtl);
	DDX_Control(pDX, IDC_GET, m_GetCtl);
	DDX_Control(pDX, IDC_EXIT, m_ExitCtl);
	DDX_Control(pDX, IDC_LISTENING, m_ListenCtl);
	DDX_Control(pDX, IDC_ABORT, m_AbortCtl);
	DDX_Control(pDX, IDC_OPTIONS, m_OptionsCtl);
	DDX_Control(pDX, IDC_LOG, m_Log);
	DDX_Control(pDX, IDC_CONNECTIONS, m_List);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPumpKINDlg, CDialog)
	//{{AFX_MSG_MAP(CPumpKINDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(IDC_PUT, OnPut)
	ON_BN_CLICKED(IDC_GET, OnGet)
	ON_NOTIFY(LVN_DELETEALLITEMS, IDC_CONNECTIONS, OnDeleteallitemsConnections)
	ON_NOTIFY(LVN_DELETEITEM, IDC_CONNECTIONS, OnDeleteitemConnections)
	ON_NOTIFY(LVN_INSERTITEM, IDC_CONNECTIONS, OnInsertitemConnections)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CONNECTIONS, OnItemchangedConnections)
	ON_BN_CLICKED(IDC_ABORT, OnAbort)
	ON_WM_CLOSE()
	ON_COMMAND(ID_TRAY_SHOWPUMPKINWINDOW, OnTrayShowpumpkinwindow)
	ON_COMMAND(ID_TRAY_LISTEN, OnTrayListen)
	ON_COMMAND(ID_TRAY_EXIT, OnTrayExit)
	ON_COMMAND(ID_TRAY_ABOUTPUMPKIN, OnTrayAboutpumpkin)
	ON_COMMAND(ID_TRAY_FETCHFILE, OnTrayFetchfile)
	ON_COMMAND(ID_TRAY_HELP, OnTrayHelp)
	ON_COMMAND(ID_TRAY_OPTIONS, OnTrayOptions)
	ON_COMMAND(ID_TRAY_SENDFILE, OnTraySendfile)
	ON_WM_WINDOWPOSCHANGING()
	ON_LBN_SELCHANGE(IDC_LOG, OnSelchangeLog)
	ON_COMMAND(ID_TRAY_OPENFILESFOLDER, OnTrayOpenfilesfolder)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(ID_HELP, OnHelp)
	ON_BN_CLICKED(IDC_LISTENING, OnListening)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPumpKINDlg message handlers

BOOL CPumpKINDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	VERIFY(m_Retrier->Create(NULL,"PumpKIN-Retrier",WS_CHILD,CRect(0,0,0,0),this,0));

	m_Images.Create(16,16,TRUE,2,1);
	m_iRRQ = m_Images.Add(AfxGetApp()->LoadIcon(IDI_RRQ));
	m_iWRQ = m_Images.Add(AfxGetApp()->LoadIcon(IDI_WRQ));
	ASSERT(m_iRRQ>=0);
	ASSERT(m_iWRQ>=0);
	m_List.SetImageList(&m_Images,LVSIL_NORMAL);
	m_List.SetImageList(&m_Images,LVSIL_SMALL);
	m_List.SetImageList(&m_Images,LVSIL_STATE);
	m_List.SetTextColor(RGB(255,255,0));	// Yellow
	m_List.SetTextBkColor(RGB(12,167,0));	// Green
	m_List.SetBkColor(RGB(12,167,0));	// Green
CRect listrc;
	m_List.GetClientRect(&listrc);
	m_List.InsertColumn(0,"File",LVCFMT_LEFT,listrc.Width()-((listrc.Width()/7)*3+listrc.Width()*2/7),subitemFile);
	m_List.InsertColumn(1,"type",LVCFMT_CENTER,listrc.Width()/7,subitemType);
	m_List.InsertColumn(2,"peer",LVCFMT_LEFT,listrc.Width()*2/7,subitemPeer);
	m_List.InsertColumn(3,"ACK",LVCFMT_RIGHT,listrc.Width()/7,subitemBytes);
	m_List.InsertColumn(4,"tsize",LVCFMT_RIGHT,listrc.Width()/7,subitemTSize);

	LogLine(IDS_LOG_START);

	SetupButtons();

CRect wrci, wrco;
	GetWindowRect(&wrco);
	GetClientRect(&wrci);
CRect brc;
	m_GetCtl.GetWindowRect(&brc); ScreenToClient(&brc);
	m_rightGapButtons = wrci.right-brc.right;
	m_List.GetWindowRect(&brc); ScreenToClient(&brc);
	m_rightGapList = wrci.right-brc.right;
	m_ListenCtl.GetWindowRect(&brc); ScreenToClient(&brc);
	m_bottomGapListen = wrci.bottom-brc.bottom;
	m_Log.GetWindowRect(&brc);	ScreenToClient(&brc);
	m_bottomGapLog = wrci.bottom-brc.bottom;
	m_MinSize.cx = wrco.Width(); m_MinSize.cy=wrco.Height();

CRect rc, drc;
	GetWindowRect(rc);
	GetDesktopWindow()->GetWindowRect(drc);
	SetWindowPos(NULL,drc.right-6-rc.Width(),6,0,0,SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);

	if(m_bShown)
		ShowWindow(SW_SHOW);
	else
		ShowWindow(SW_HIDE);

	m_ListenCtl.SetCheck(m_Listener.m_bListen?1:0);

	// CG: The following block was added by the ToolTips component.
	{
		// Create the ToolTip control.
		m_tooltip.Create(this);
		m_tooltip.Activate(TRUE);

		m_tooltip.AddTool(&m_List,IDC_CONNECTIONS);
		m_tooltip.AddTool(GetDlgItem(IDC_PUT),IDC_PUT);
		m_tooltip.AddTool(GetDlgItem(IDC_GET),IDC_GET);
		m_tooltip.AddTool(&m_AbortCtl,IDC_ABORT);
		m_tooltip.AddTool(GetDlgItem(IDC_OPTIONS),IDC_OPTIONS);
		m_tooltip.AddTool(GetDlgItem(IDC_EXIT),IDC_EXIT);
		m_tooltip.AddTool(GetDlgItem(ID_HELP),ID_HELP);
		m_tooltip.AddTool(GetDlgItem(IDC_LOG),IDC_LOG);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPumpKINDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CPumpKINDlg::OnDestroy()
{
	SaveSettings();

NOTIFYICONDATA nid;
	memset(&nid,0,sizeof(nid));
	nid.cbSize=sizeof(nid);
	nid.hWnd=m_Trayer->m_hWnd;
	nid.uID=IDC_TRAYICON;
	nid.uFlags=0;
	VERIFY(Shell_NotifyIcon(NIM_DELETE,&nid));

	WinHelp(0L, HELP_QUIT);
	CDialog::OnDestroy();
POSITION p = m_LogTimes.GetStartPosition();
	while(p){
	CTime *t,*tt;
		m_LogTimes.GetNextAssoc(p,t,tt);
		ASSERT(t && tt && t==tt);
		delete t;
	}
	// *** Abort and cleanup transfers
	m_LogTimes.RemoveAll();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPumpKINDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
	CPaintDC pDC(this);
	CDC bmpDC;
		bmpDC.CreateCompatibleDC(&pDC);
		bmpDC.SelectObject(&m_bmpBack);
	CRect rc;
		GetClientRect(&rc);
		for(int x=-m_bitmapBack.bmWidth*2/4;x<rc.Width();x+=m_bitmapBack.bmWidth)
			for(int y=-m_bitmapBack.bmHeight*2/4;y<rc.Height();y+=m_bitmapBack.bmHeight)
				pDC.BitBlt(x,y,m_bitmapBack.bmWidth,m_bitmapBack.bmHeight,&bmpDC,0,0,SRCCOPY);
		bmpDC.DeleteDC();
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPumpKINDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

int CPumpKINDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if(!m_Listener.SetListen(m_bListen)) {
		m_bListen=FALSE;
		TRACE0("Failed to create socket\n");
		AfxMessageBox(IDS_BOX_CANTBIND,MB_OK|MB_ICONEXCLAMATION);
	}

	if(!m_Trayer->Create(NULL,"PumpKIN TrayIcon",WS_CHILD,CRect(0,0,0,0),this,0)){
		TRACE0("Failed to create trayer\n");
		return -1;
	}

NOTIFYICONDATA nid;
	memset(&nid,0,sizeof(nid));
	nid.cbSize=sizeof(nid);
	nid.hWnd=m_Trayer->m_hWnd;
	nid.uID=IDC_TRAYICON;
	nid.uFlags=NIF_MESSAGE|NIF_ICON|NIF_TIP;
	nid.uCallbackMessage=WM_TRAYICON;
	nid.hIcon=AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// *** Load from resource
	strcpy(nid.szTip,"PumpKIN");
	VERIFY(Shell_NotifyIcon(NIM_ADD,&nid));

	return 0;
}

void CListenSocket::OnReceive(int nErrorCode)
{
	ASSERT(m_Daddy);
	if(nErrorCode){
		m_Daddy->LogLine(IDS_LOG_LISTENRECEIVEERROR);
		return;
	}
DWORD fionread = 0;
	VERIFY(IOCtl(FIONREAD,&fionread));	// *** Do some checking on the value acquired
tftp *tftpRQ = tftp::Allocate(fionread);
	ASSERT(tftpRQ);
SOCKADDR_IN sin;
	if(!tftpRQ->Receive(this,fionread,&sin)){
		m_Daddy->LogLine(IDS_LOG_LISTENACCEPTERROR);
		delete tftpRQ;
		return;
	}
#ifndef	NDEBUG
CString tmp;
	tmp.Format("%u - %s - %u\n",tftpRQ->Opcode(),inet_ntoa(sin.sin_addr),sin.sin_port);
	TRACE0(tmp);
#endif
POSITION p = m_Daddy->m_Xfers.GetStartPosition();
	while(p){
	SOCKET key;
	CXferSocket *sock;
		m_Daddy->m_Xfers.GetNextAssoc(p,key,sock);
		ASSERT(sock);
		if(sock->m_Peer.sin_addr.s_addr==sin.sin_addr.s_addr && sock->m_Peer.sin_port==sin.sin_port){
			TRACE0("Ignoring request which we are already processing\n");
			delete tftpRQ;
			return;
		}
	}
	switch(tftpRQ->Opcode()){
	case tftp::opRRQ:
		// Read Request
		{
		CString tmp;
			tmp.Format(IDS_LOG_RRQSERVE,tftpRQ->rqFileName(),tftpRQ->rqType(),inet_ntoa(sin.sin_addr));
			m_Daddy->LogLine(tmp);
		CRRQSocket *s = new CRRQSocket(m_Daddy,tftpRQ->rqFileName(),tftpRQ->rqType(),&sin);
			ASSERT(s);
			tftpRQ->GetOptions(&s->m_Options);
			if(!s->Create())
				s->Destroy(FALSE);
		}
		break;
	case tftp::opWRQ:
		// Write Request
		{
		CString tmp;
			tmp.Format(IDS_LOG_WRQSERVE,tftpRQ->rqFileName(),tftpRQ->rqType(),inet_ntoa(sin.sin_addr));
			m_Daddy->LogLine(tmp);
		CWRQSocket *s = new CWRQSocket(m_Daddy,tftpRQ->rqFileName(),tftpRQ->rqType(),&sin);
			ASSERT(s);
			tftpRQ->GetOptions(&s->m_Options);
			if(!s->Create(NULL,NULL))
				s->Destroy(FALSE);
		}
		break;
	default:
		m_Daddy->LogLine(IDS_LOG_LISTENOPCODE);
		delete tftpRQ;
		return;
	}
	delete tftpRQ;
}

BOOL tftp::Receive(CAsyncSocket* socket,UINT maxLength,SOCKADDR_IN *sin)
{
	ASSERT(socket);
int saddrLen = sizeof(SOCKADDR_IN);
	length = sin ? 
			socket->ReceiveFrom(udpBase(),maxLength,(SOCKADDR*)sin,&saddrLen)
		:
			socket->Receive(udpBase(),maxLength)
	;
	if(!length)
		return FALSE;
	if(length==(tftpLength)SOCKET_ERROR)
		return FALSE;
	return TRUE;
}

UINT tftp::Opcode()
{
	return REVERSEBYTES(opcode);
}

CString tftp::rqFileName()
{
	ASSERT(length);
	ASSERT(Opcode()==opRRQ || Opcode()==opWRQ);
CString rv;
	if(memchr(&data.m_RQ.data,0,length-sizeof(opcode)))
		rv = (LPCTSTR)data.m_RQ.data;
	return rv;
}

CString tftp::rqType()
{
	ASSERT(length);
	ASSERT(Opcode()==opRRQ || Opcode()==opWRQ);
CString rv;
char *tmp = (char*)memchr(&data.m_RQ.data,0,length-sizeof(opcode));
	if(tmp++)
		rv = (LPCTSTR)tmp;
	return rv;
}

UINT tftp::GetOptions(tftp::tftpOptions* ops)
{
	ASSERT(length);
	ASSERT(Opcode()==opRRQ || Opcode()==opWRQ || Opcode()==opOACK);
	ASSERT(ops);
tftpOptions& o = *ops;
LPSTR base = (LPSTR)&data.m_RQ.data;
UINT basePtr = 0;
	if(Opcode()==opRRQ || Opcode()==opWRQ){
		base = (LPSTR)memchr(&data.m_RQ.data,0,length-sizeof(opcode));
		if(!base)
			return 0;
		base++;
		basePtr = (base-(LPSTR)&data.m_RQ.data);
		base = (LPSTR)memchr(base,0,length-basePtr);
		if(!base)
			return 0;
		base++;
		basePtr = (base-(LPSTR)&data.m_RQ.data);
	}
	ops->RemoveAll();
UINT rv = 0;
	while(basePtr<(length-sizeof(opcode))){
	CString onam = (LPSTR)&data.m_RQ.data[basePtr];
		basePtr+=onam.GetLength()+1;
	CString oval = (LPSTR)&data.m_RQ.data[basePtr];
		basePtr+=oval.GetLength()+1;
		onam.MakeLower();
		o[onam]=oval;
		rv++;
	}
	return rv;
}

tftp::tftp()
{
	length=0;
}


void CXferSocket::OnSend(int nErrorCode)
{
	if(nErrorCode){
		ASSERT(m_Daddy);
		m_Daddy->LogLine(IDS_LOG_XFERSEND);
		return;
	}
	if(!m_Queue.IsEmpty()){
	tftp *p = m_Queue.GetHead();
		ASSERT(p);
		m_Queue.RemoveHead();
		if(!p->Send(this,&m_Peer)){
			ASSERT(m_Daddy);
			m_Daddy->LogLine(IDS_LOG_XFERUDPSEND);
		}
		delete p;
	}
	DoSelect();
	if(m_Queue.IsEmpty()){
		switch(state){
		case stateDeny:
			Destroy(FALSE);
			break;
		case stateFinish:
			Destroy(TRUE);
			break;
		}
	}
}

BOOL tftp::Send(CAsyncSocket *socket,SOCKADDR_IN* saddr)
{
	ASSERT(socket);
int rv = socket->SendTo(udpBase(),length,(SOCKADDR*)saddr,sizeof(SOCKADDR_IN));
	if(rv!=length)
		return FALSE;
	return TRUE;
}

void CXferSocket::DoSelect()
{
	if(m_Peer.sin_addr.s_addr!=INADDR_NONE)
		AsyncSelect(FD_CLOSE|FD_READ|(m_Queue.IsEmpty()?0:FD_WRITE));
}

void CXferSocket::OnReceive(int nErrorCode)
{
	if(nErrorCode){
		ASSERT(m_Daddy);
		m_Daddy->LogLine(IDS_LOG_XFERRECEIVE);
		return;
	}
	ASSERT(m_Daddy);
DWORD fionread = 0;
	VERIFY(IOCtl(FIONREAD,&fionread));
tftp *p = tftp::Allocate(fionread);
	ASSERT(p);
SOCKADDR_IN sin;
	if(!p->Receive(this,fionread,&sin)){
		m_Daddy->LogLine(IDS_LOG_XFERUDPRECEIVE);
		delete p;
	}else
		if(m_Peer.sin_addr.s_addr==INADDR_NONE){
			m_Peer.sin_addr=sin.sin_addr;
			m_Peer.sin_port=sin.sin_port;
	}
BOOL alive = TRUE;
	if(state==stateInit){
		state=stateXfer;
		m_Peer.sin_port=sin.sin_port;
		UpdateList();
	}
	if(sin.sin_addr.s_addr!=m_Peer.sin_addr.s_addr || sin.sin_port!=m_Peer.sin_port){
		m_Daddy->LogLine(IDS_LOG_XFERSOURCETID);
		// *** Bounce it!
	}else{
		alive = OnTFTP(p);
	}
	delete p;
	if(alive){
		DoSelect();
		ResetTimeout();
	}
}

void CXferSocket::SetPeer(SOCKADDR_IN *sin)
{
	ASSERT(sin);
	memmove(&m_Peer,sin,sizeof(m_Peer));
}

void CXferSocket::UpdateList()
{
	ASSERT(m_Daddy);
LV_FINDINFO lvf;
	memset(&lvf,0,sizeof(lvf));
	lvf.flags=LVFI_PARAM;
	lvf.lParam=(LPARAM)this;
int i = m_Daddy->m_List.FindItem(&lvf);
	if(i<0){
		ASSERT(IsKindOf(RUNTIME_CLASS(CRRQSocket)) || IsKindOf(RUNTIME_CLASS(CWRQSocket)));
		i=m_Daddy->m_List.InsertItem(0,m_FileName,IsKindOf(RUNTIME_CLASS(CRRQSocket))?m_Daddy->m_iRRQ:m_Daddy->m_iWRQ);
		ASSERT(!(i<0));
		m_Daddy->m_List.SetItemData(i,(DWORD)this);
	}
	m_Daddy->m_List.SetItemText(i,CPumpKINDlg::subitemFile,m_FileName);
	m_Daddy->m_List.SetItemText(i,CPumpKINDlg::subitemType,m_Type);
	m_Daddy->m_List.SetItemText(i,CPumpKINDlg::subitemPeer,inet_ntoa(m_Peer.sin_addr));
CString tmp;
	tmp.Format(IDS_FMT_BYTES,GetACK());
	m_Daddy->m_List.SetItemText(i,CPumpKINDlg::subitemBytes,tmp);
	if(m_xferSize>=0){
		tmp.Format(IDS_FMT_BYTES,m_xferSize);
		m_Daddy->m_List.SetItemText(i,CPumpKINDlg::subitemTSize,tmp);
	}
}

CXferSocket::CXferSocket()
	: m_wndResolver(NULL), m_Retry(NULL), m_bRetry(FALSE),
	m_blkSize(512), m_timeOut(30), m_xferSize(-1),
	m__blkSize(512), m__timeOut(30)
{
	m_Daddy=NULL;
	m_Peer.sin_addr.s_addr=INADDR_NONE;
	m_Peer.sin_family=AF_INET;
	state=stateNone;
}

ULONG CXferSocket::GetACK()
{
	return 0;
}

CXferSocket::CXferSocket(CPumpKINDlg *daddy,LPCTSTR fileName,LPCTSTR type,SOCKADDR_IN* sin)
	: m_wndResolver(NULL), m_Retry(NULL), m_bRetry(FALSE),
	m_blkSize(512), m_timeOut(30), m_xferSize(-1),
	m__blkSize(512), m__timeOut(30)
{
	m_Peer.sin_family=AF_INET;
	state=stateNone;
	ASSERT(daddy);
	m_Daddy=daddy;
	m_timeOut=m__timeOut=m_Daddy->m_TFTPTimeOut.GetTotalSeconds();
	if(sin){
		m_Peer.sin_addr.s_addr=sin->sin_addr.s_addr;
		m_Peer.sin_port=sin->sin_port;
	}else
		m_Peer.sin_addr.s_addr=INADDR_NONE;
	m_FileName=fileName;
	m_Type=type;
}

BOOL CRRQSocket::Create(LPCTSTR localFile,LPCTSTR hostName)
{
	if(!CAsyncSocket::Create(0,SOCK_DGRAM))
		return FALSE;
	ASSERT(m_Daddy);
	ASSERT(m_Peer.sin_addr.s_addr!=INADDR_NONE || hostName);
	m_Daddy->m_Xfers[m_hSocket]=this;
CString lFile = localFile?localFile:m_FileName;
	TurnSlashes(lFile,TRUE);
	UpdateList();
	if(!localFile){	// Check only for incoming requests
		if(CheckBadRelativeness(m_FileName)){
			Deny(tftp::errAccessViolation,IDS_TFTP_ERROR_ACCESS);
			return TRUE;
		}
		int atar=m_Daddy->m_aclRules.FindTarget(acl_rule::opRRQ,m_Peer.sin_addr.s_addr);
		if(atar<0)
			atar = m_Daddy->m_RRQMode;
		switch(atar){
		case CPumpKINDlg::rrqGiveAll:
			break;
		case CPumpKINDlg::rrqAlwaysConfirm:
			if(ConfirmRequest())
				break;
		default:
			TRACE1("Unexpected access target: %d\n",atar);
		case CPumpKINDlg::rrqDenyAll:
			Deny(tftp::errAccessViolation,IDS_TFTP_ERROR_ACCESS);
			return TRUE;
		}
	}
CString fn = localFile?ApplyRootGently(lFile):ApplyRoot(lFile);
CFileException e;
	if(!m_File.Open(fn,CFile::modeRead|CFile::shareDenyWrite,&e)){
		if(localFile){
		CString tmp;
			tmp.Format(IDS_LOG_FAILEDLOCALFILE,fn);
			m_Daddy->LogLine(tmp);
			return FALSE;
		}
		Deny(&e);
		return TRUE;
	}
	m_xferSize=m_File.GetLength();	// *** HANDLE EXCEPTION
	if(hostName){
		m_HostName=hostName;
	
	CString tmp;
		tmp.Format(IDS_LOG_SENDING,m_FileName,m_HostName);
		m_Daddy->LogLine(tmp);

		CString inAddr = hostName;
	int at = inAddr.Find('@');
		if(at>=0)
			inAddr=inAddr.Mid(at+1);
		if((m_Peer.sin_addr.s_addr=inet_addr((LPCTSTR)inAddr))==INADDR_NONE){
			ASSERT(!m_wndResolver);
			m_wndResolver = new CResolver(this);
			ASSERT(m_wndResolver);
			return m_wndResolver->Resolve();
		}
		else
			OnHostKnown();
	}else{
	tftp::tftpOptions o;
	CString v;
		if(m_Options.Lookup(tftpoBSize,v)){
			m__blkSize=atoi(v);
			if(m__blkSize){
				m_blkSize=m__blkSize;
				v.Format("%u",m_blkSize);
				o[tftpoBSize]=v;
			}
		}
		if(m_Options.Lookup(tftpoTSize,v)){
			v.Format("%lu",m_xferSize);
			o[tftpoTSize]=v;
		}
		if(m_Options.Lookup(tftpoTOut,v)){
			m__timeOut=atoi(v);
			if(m__timeOut){
				m_timeOut=m__timeOut;
				v.Format("%u",m_timeOut);
				o[tftpoTOut]=v;
			}
		}
		state = stateXfer;
		m_ACK=0;
		if(o.GetCount()){
		tftp *p = tftp::Allocate(tftp::tftpOACK::tftpSize(&o));
			ASSERT(p);
			p->SetOpcode(tftp::opOACK);
			p->data.m_OACK.Set(&o);
			PostTFTP(p,TRUE);
		}else
			DoXfer();
	}
	return TRUE;
}

CRRQSocket::CRRQSocket(CPumpKINDlg *daddy,LPCTSTR fileName,LPCTSTR type,SOCKADDR_IN *sin)
	: CXferSocket(daddy,fileName,type,sin)
{
	m_ACK=0;
	m_LastSlack=0;
}

UINT tftp::tftpERROR::tftpSize(LPCTSTR msg)
{
	return tftpHdrSize-tftpSlackSize+sizeof(tftp::tftpERROR::tftpErrorCode)+strlen(msg)+1;
}

tftp* tftp::Allocate(UINT tftpSize)
{
	ASSERT(tftpSize);
tftp* rv = (tftp*) new BYTE[tftpSlackSize+tftpSize];
	ASSERT(rv);
	rv->length=tftpSize;
	return rv;
}

void tftp::errSet(UINT code,LPCTSTR msg)
{
	ASSERT(this);
	ASSERT(length>=data.m_ERROR.tftpSize(msg));
	strcpy((char*)data.m_ERROR.data,msg);
	data.m_ERROR.SetCode(code);
}

void CXferSocket::PostTFTP(tftp* p,BOOL retryable)
{
	ASSERT(p);
	m_Queue.AddTail(p);
	DoSelect();
	if(!m_bRetry){
		if(retryable)
			SetTry(p);
		else
			SetTry();
	}
	ResetTimeout();
}

void CXferSocket::Deny(UINT errCode,UINT errID)
{
	PostError(errCode,errID);
	state=stateDeny;
}

void CRRQSocket::DoXfer()
{
tftp *p = tftp::Allocate(tftp::tftpDATA::tftpSize(m_blkSize));
	ASSERT(p);
	p->SetOpcode(tftp::opDATA);
	TRY{
		m_File.Seek(m_ACK*m_blkSize,CFile::begin);
	int bytes = m_File.Read(p->data.m_DATA.data,m_blkSize);
		p->data.m_DATA.SetBlock(m_ACK+1);
		p->length=p->length-m_blkSize+bytes;
		m_LastSlack = m_blkSize-bytes;
		PostTFTP(p);
		if(bytes<m_blkSize){
			state=stateFinish;
			ASSERT(m_Daddy);
		CString tmp;
			tmp.Format(IDS_LOG_XFERRRQFINISHED,(LPCTSTR)m_FileName);
			m_Daddy->LogLine(tmp);
		}
	}CATCH(CFileException,e){
		Deny(e);
	}END_CATCH
}

UINT tftp::tftpDATA::tftpSize(UINT blkSize)
{
	return tftpHdrSize-tftpSlackSize+sizeof(tftp::tftpDATA)
		-sizeof(BYTE)+blkSize;
}

void CXferSocket::Deny(CFileException* e)
{
	PostError(e);
	state=stateDeny;
}

void CXferSocket::PostError(UINT errCode,UINT errID)
{
CString msg;
	msg.LoadString(errID);
	ASSERT(m_Daddy);
/*	// ***
CString tmp;
	tmp.Format(IDS_LOG_SENTTFTPERROR,errCode,(LPCTSTR)msg);
	m_Daddy->LogLine(tmp);
 */
tftp* err = tftp::Allocate(tftp::tftpERROR::tftpSize(msg));
err->SetOpcode(tftp::opERROR);
	err->errSet(errCode,msg);
	PostTFTP(err);
}

void CXferSocket::PostError(CFileException* e)
{
UINT eCode;
UINT eMsgID;
	switch(e->m_cause){
	case CFileException::fileNotFound:
		eCode=tftp::errNotFound;
		eMsgID=IDS_TFTP_ERROR_NOTFOUND;
		break;
	case CFileException::accessDenied:
		eCode=tftp::errAccessViolation;
		eMsgID=IDS_TFTP_ERROR_ACCESS;
		break;
	case CFileException::directoryFull:
		eCode=tftp::errDiskFull;
		eMsgID=IDS_TFTP_ERROR_DIRFULL;
		break;
	case CFileException::sharingViolation:
		eCode=tftp::errAccessViolation;
		eMsgID=IDS_TFTP_ERROR_SHARING;
		break;
	case CFileException::diskFull:
		eCode=tftp::errDiskFull;
		eMsgID=IDS_TFTP_ERROR_DISKFULL;
		break;
	default:
		eCode=tftp::errUndefined;
		eMsgID=IDS_TFTP_ERROR_UNDEFINED;
		break;
	}
	PostError(eCode,eMsgID);
}

ULONG CRRQSocket::GetACK(void)
{
	return (m_ACK*m_blkSize)-m_LastSlack;
}

BOOL CRRQSocket::OnTFTP(tftp* p)
{
BOOL rv = TRUE;
	switch(p->Opcode()){
	case tftp::opOACK:
		m_ACK=0;
		ASSERT(state!=stateFinish);
	{
	tftp::tftpOptions o;
		if(p->GetOptions(&o)){
		CString v;
			if(o.Lookup(tftpoBSize,v)){
				m_blkSize=atoi(v);
				if(!m_blkSize){	// *** More sanity checks
					Deny(tftp::errOption,IDS_TFTP_ERROR_BSIZE);
					rv = TRUE;
					break;
				}
			}
			if(o.Lookup(tftpoTOut,v)){
				m_timeOut=atoi(v);
				if(!m_timeOut){	// *** More sanity checks
					Deny(tftp::errOption,IDS_TFTP_ERROR_TOUT);
					rv = TRUE;
					break;
				}
			}
			if(o.Lookup(tftpoXResume,v)){
				m_ACK=atoi(v);
			}
		}
		UpdateList();
		DoXfer();
	}
		break;
	case tftp::opACK:
		m_ACK=p->data.m_ACK.Block();
		if(state!=stateFinish){
			UpdateList();
			DoXfer();
		}
		break;
	case tftp::opERROR:
		{
			ASSERT(m_Daddy);
		CString tmp;
			tmp.Format(IDS_LOG_GOTTFTPERROR,p->data.m_ERROR.Code(),(LPCTSTR)p->errMessage());
			m_Daddy->LogLine(tmp);
		}
		Destroy(FALSE);
		rv = FALSE;
		break;
	default:
		{
			ASSERT(m_Daddy);
			CString tmp;
			tmp.Format(IDS_LOG_XFEROPCODE,p->Opcode());
			m_Daddy->LogLine(tmp);
			// *** Self destruct maybe??
		}
		break;
	}
	return rv;
}

BOOL CWRQSocket::OnTFTP(tftp* p)
{
	switch(p->Opcode()){
	case tftp::opOACK:
		ASSERT(state!=stateFinish);
	{
		if(m_bResume)
			m_ACK=m_File.GetLength()/m_blkSize;
		else
			m_ACK=0;
	tftp::tftpOptions o;
		if(p->GetOptions(&o)){
		CString v;
			if(o.Lookup(tftpoBSize,v)){
				m_blkSize=atoi(v);
				if(!m_blkSize){	// *** More sanity checks
					Deny(tftp::errOption,IDS_TFTP_ERROR_BSIZE);
					return TRUE;
				}
			}
			if(o.Lookup(tftpoTOut,v)){
				m_timeOut=atoi(v);
				if(!m_timeOut){	// *** More sanity checks
					Deny(tftp::errOption,IDS_TFTP_ERROR_TOUT);
					return TRUE;
				}
			}
			if(o.Lookup(tftpoTSize,v)){
				m_xferSize=atoi(v);
			}
		}
		UpdateList();
		DoXfer();
	}
		break;
	case tftp::opDATA:
		{
		UINT	block = p->data.m_DATA.Block();
			TRY{
				m_File.Seek((block-1)*m_blkSize,CFile::begin);
			int bytes = p->length-sizeof(p->data.m_DATA.block)-(tftpHdrSize-tftpSlackSize);
				if(bytes){
					m_File.Write(p->data.m_DATA.data,bytes);
					// *** Move to the other place where we can do it not that often
					m_File.SetLength(m_File.GetPosition());
				}
				if(bytes<m_blkSize){
					state=stateFinish;
					ASSERT(m_Daddy);
				CString tmp;
					tmp.Format(IDS_LOG_XFERWRQFINISHED,(LPCTSTR)m_FileName);
					m_Daddy->LogLine(tmp);
				}
				m_ACK=block;
				m_LastSlack=m_blkSize-bytes;
				UpdateList();
				DoXfer();
			}CATCH(CFileException,e){
				Deny(e);
			}END_CATCH
		}
		break;
	case tftp::opERROR:
		{
			ASSERT(m_Daddy);
		CString tmp;
			tmp.Format(IDS_LOG_GOTTFTPERROR,p->data.m_ERROR.Code(),(LPCTSTR)p->errMessage());
			m_Daddy->LogLine(tmp);
		}
		Destroy(FALSE);
		return FALSE;
	default:
		{
			ASSERT(m_Daddy);
			CString tmp;
			tmp.Format(IDS_LOG_XFEROPCODE,p->Opcode());
			m_Daddy->LogLine(tmp);
			// *** Self destruct maybe??
		}
		break;
	}
	return TRUE;
}

void tftp::SetOpcode(WORD op)
{
	opcode = REVERSEBYTES(op);
}
void tftp::tftpDATA::SetBlock(WORD b)
{
	block=REVERSEBYTES(b);
}
WORD tftp::tftpDATA::Block()
{
	return REVERSEBYTES(block);
}
WORD tftp::tftpACK::Block()
{
	return REVERSEBYTES(block);
}
void tftp::tftpACK::SetBlock(WORD b)
{
	block = REVERSEBYTES(b);
}
WORD tftp::tftpERROR::Code()
{
	return REVERSEBYTES(code);
}
void tftp::tftpERROR::SetCode(WORD c)
{
	code = REVERSEBYTES(c);
}


CString tftp::errMessage()
{
CString rv;
	if(memchr(data.m_ERROR.data,0,length-(tftpHdrSize-tftpSlackSize)-sizeof(data.m_ERROR.code)))
		rv = (LPCTSTR)data.m_ERROR.data;
	return rv;
}

void CXferSocket::Destroy(BOOL success)
{
	if(m_wndResolver){
		delete m_wndResolver;
		m_wndResolver=NULL;
	}
	SetTry();
	m_Daddy->m_bnw.StartSound(
		success
			? m_Daddy->m_bnwSuccess
			: m_Daddy->m_bnwAbort
	);
	if(m_File.m_hFile!=CFile::hFileNull){
		TRY{
			m_File.Close();
		}CATCH(CFileException,e){
			TRACE0("Error closing file\n");
		}END_CATCH
	}
	ASSERT(m_Daddy);
	m_Daddy->KillTimer(m_hSocket);
	m_Daddy->m_Xfers.RemoveKey(m_hSocket);
LV_FINDINFO lvf;
	memset(&lvf,0,sizeof(lvf));
	lvf.flags=LVFI_PARAM;
	lvf.lParam=(LPARAM)this;
int i = m_Daddy->m_List.FindItem(&lvf);
	if(i>=0)
		m_Daddy->m_List.DeleteItem(i);
	delete this;
}

void CPumpKINDlg::LogLineToScreen(LPCTSTR str)
{
	ASSERT(m_LogLength);
	while(m_Log.GetCount()>m_LogLength && m_Log.GetCount()!=LB_ERR){
	CTime *t = (CTime*)m_Log.GetItemData(0);
		if(((DWORD)t)!=LB_ERR){
			ASSERT(t);
			m_LogTimes.RemoveKey(t);
			delete t;
		}
		m_Log.DeleteString(0);
	}
int i = m_Log.AddString(str);
	ASSERT(i!=LB_ERR);
CTime *t = new CTime(CTime::GetCurrentTime());
	m_Log.SetItemData(i,(DWORD)(m_LogTimes[t]=t));
	m_Log.SetCurSel(i);
}

void CPumpKINDlg::LogLine(UINT msgID)
{
CString tmp;
	tmp.Format(msgID);
	LogLine(tmp);
}

void CXferSocket::TurnSlashes(CString& fn,BOOL bBack)
{
int	s = fn.Find(bBack?'/':'\\');
	while(s>=0){
		fn.SetAt(s,bBack?'\\':'/');
		s = fn.Find(bBack?'/':'\\');
	}
}

CString CXferSocket::ApplyRoot(LPCTSTR fileName)
{
	ASSERT(m_Daddy);
CString fn  = fileName;
CString rv = m_Daddy->m_TFTPRoot;
	if(rv.IsEmpty())
		rv = ".";
	if(rv[rv.GetLength()-1]!='\\')
		rv+="\\";
	while((!fn.IsEmpty()) && fn[0]=='\\')
		fn=fn.Mid(1);
	rv+=fn;
	return rv;
}

void CPumpKINDlg::OnOptions() 
{
CPropertySheet cps(IDS_TITLE_OPTIONS,this);
CPropsServer server;
CPropsNetwork network;
CPropsSounds sounds;
CPropsACL acl;

	server.m_RRQMode=m_RRQMode;
	server.m_TFTPRoot=m_TFTPRoot;
	server.m_TFTPSubdirs=m_bTFTPSubdirs;
	server.m_WRQMode=m_WRQMode;
	server.m_PromptTimeOut=m_PromptTimeOut;
	server.m_LogFile=m_LogFile;

	network.m_ListenPort=m_ListenPort;
	network.m_SpeakPort=m_SpeakPort;
	network.m_TimeOut=m_TFTPTimeOut.GetTotalSeconds();
	network.m_BlockSize=m_BlockSize;

	sounds.m_Request = m_bnwRequest;
	sounds.m_Success = m_bnwSuccess;
	sounds.m_Abort = m_bnwAbort;

	acl.m_rulist = m_aclRules;

	cps.AddPage(&server);
	cps.AddPage(&network);
	cps.AddPage(&sounds);
	cps.AddPage(&acl);
	if(cps.DoModal()==IDOK){
		m_RRQMode=server.m_RRQMode;
		m_TFTPRoot=server.m_TFTPRoot;
		m_bTFTPSubdirs=server.m_TFTPSubdirs;
		m_WRQMode=server.m_WRQMode;
		m_PromptTimeOut=server.m_PromptTimeOut;
		m_LogFile=server.m_LogFile;

		m_ListenPort=network.m_ListenPort;
		m_SpeakPort=network.m_SpeakPort;
		m_TFTPTimeOut=CTimeSpan(network.m_TimeOut);
		m_BlockSize=network.m_BlockSize;

		m_bnwRequest = sounds.m_Request;
		m_bnwSuccess = sounds.m_Success;
		m_bnwAbort = sounds.m_Abort;

		m_aclRules = acl.m_rulist;

		m_lastlogerr.Empty();
	}
}

BOOL CRRQSocket::ConfirmRequest()
{
CConfirmRRQDlg cd(NULL);
	cd.m_Daddy=this;
	cd.m_File=m_FileName;
	cd.m_Host=inet_ntoa(m_Peer.sin_addr);
	if(cd.DoModal()==IDOK)
		return TRUE;
	return FALSE;
}

CWRQSocket::CWRQSocket(CPumpKINDlg* daddy,LPCTSTR fileName,LPCTSTR type,SOCKADDR_IN *sin)
	: CXferSocket(daddy,fileName,type,sin)
{
	state=stateNone;
	m_ACK=0;
	m_LastSlack=0;
	m_bResume=FALSE;
}

BOOL CWRQSocket::Create(LPCTSTR localFile,LPCTSTR hostName)
{
	if(!CAsyncSocket::Create(0,SOCK_DGRAM))
		return FALSE;
	ASSERT(m_Daddy);
	ASSERT(m_Peer.sin_addr.s_addr!=INADDR_NONE || hostName);
	m_Daddy->m_Xfers[m_hSocket]=this;
	UpdateList();
CString lf;
 	if(!localFile) {
		lf = m_FileName;
		TurnSlashes(lf,TRUE);
	}
CString fn = localFile?ApplyRootGently(localFile):ApplyRoot(lf);
	if(!localFile){	// This is an incoming request..
		if(CheckBadRelativeness(m_FileName)){
			Deny(tftp::errAccessViolation,IDS_TFTP_ERROR_ACCESS);
			return TRUE;
		}
	BOOL exists;
		if(!_access((LPCTSTR)fn,0))
			m_Rename=exists=TRUE;
		else
			m_Rename=exists=FALSE;
		int atar=m_Daddy->m_aclRules.FindTarget(acl_rule::opWRQ,m_Peer.sin_addr.s_addr);
		if(atar<0)
			atar=m_Daddy->m_WRQMode;
		switch(atar){
		case CPumpKINDlg::wrqTakeAll:
			if(exists){
				if(!RenameFile(fn)){
					Deny(tftp::errDiskFull,IDS_TFTP_ERROR_FAILEDTORENAME);
					return TRUE;
				}
			}
			break;
		case CPumpKINDlg::wrqConfirmIfExists:
			if(!exists)
				break;
		case CPumpKINDlg::wrqAlwaysConfirm:
			if(exists)
				m_bResume=TRUE;
			if(ConfirmRequest()){
				if(m_Rename){
					RenameFile(fn);
					if(SaveAs(fn))
						break;
				}else
					break;
			}
		default:
			TRACE1("Unexpected access target: %d\n",atar);
		case CPumpKINDlg::wrqDenyAll:
			Deny(tftp::errAccessViolation,IDS_TFTP_ERROR_ACCESS);
			return TRUE;
		}
	}
CFileException e;
	if(!m_File.Open(
			fn,
			m_bResume
			 ?(CFile::modeWrite|CFile::shareDenyWrite)
			 :(CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite),
			&e
	)){
		if(localFile){	// Outgoing request
		CString tmp;
			tmp.Format(IDS_LOG_FAILEDTOOPEN,fn);
			m_Daddy->LogLine(tmp);
			return FALSE;
		}else{
			Deny(&e);
			return TRUE;
		}
	}
	if(hostName){
		m_HostName=hostName;

	CString tmp;
		tmp.Format(IDS_LOG_REQUESTING,m_FileName,m_HostName);
		m_Daddy->LogLine(tmp);

	CString inAddr = hostName;
	int at = inAddr.Find('@');
		if(at>=0)
			inAddr=inAddr.Mid(at+1);
		if((m_Peer.sin_addr.s_addr=inet_addr((LPCTSTR)inAddr))==INADDR_NONE){
			ASSERT(!m_wndResolver);
			m_wndResolver = new CResolver(this);
			ASSERT(m_wndResolver);
			return m_wndResolver->Resolve();
		}else{
			OnHostKnown();
			return TRUE;
		}
	}
CString v;
tftp::tftpOptions oack;
	if(m_Options.Lookup(tftpoTSize,v)){
		m_xferSize=atol(v);
		if(!m_xferSize){
			Deny(tftp::errOption,IDS_TFTP_ERROR_TSIZE);
			return TRUE;
		}
	}
	if(m_Options.Lookup(tftpoBSize,v)){
		m_blkSize=atoi(v);
		if(!m_blkSize){	//  *** Do more about sanity check
			Deny(tftp::errOption,IDS_TFTP_ERROR_BSIZE);
			return TRUE;
		}
		v.Format("%u",m_blkSize);
		oack[tftpoBSize]=v;
	}
	if(m_Options.Lookup(tftpoTOut,v)){
		m_timeOut=atoi(v);
		if(!m_timeOut){	// *** Do more about sanity check
			Deny(tftp::errOption,IDS_TFTP_ERROR_TOUT);
			return TRUE;
		}
		v.Format("%u",m_timeOut);
		oack[tftpoTOut]=v;
	}
	if(m_Options.Lookup(tftpoXResume,v) && m_bResume){
		m_ACK=m_File.GetLength()/m_blkSize;
		v.Format("%u",m_ACK);
		oack[tftpoXResume]=v;
	}else
		m_ACK=0;
	state=stateXfer;
	if(oack.GetCount()){
	tftp *p = tftp::Allocate(tftp::tftpOACK::tftpSize(&oack));
		ASSERT(p);
		p->SetOpcode(tftp::opOACK);
		p->data.m_OACK.Set(&oack);
		PostTFTP(p,TRUE);
	}else
		DoXfer();
	return TRUE;
}

BOOL CWRQSocket::ConfirmRequest()
{
CConfirmWRQDlg cd(NULL);
	cd.m_Daddy=this;
	cd.m_File=m_FileName;
	cd.m_Host=inet_ntoa(m_Peer.sin_addr);
	switch(cd.DoModal()){
	case IDOK:
		m_Rename=FALSE;
		m_bResume=FALSE;
		return TRUE;
	case IDC_RENAME:
		m_bResume=FALSE;
		m_Rename=TRUE;
		return TRUE;
	case IDC_RESUME:
		m_Rename=FALSE;
		m_bResume=TRUE;
		return TRUE;
	case IDCANCEL:
		return FALSE;
	}
	return FALSE;
}

BOOL CWRQSocket::RenameFile(CString& fn)
{
CString renamed = fn;
	if(fn.IsEmpty())
		return FALSE;
	if(fn[fn.GetLength()-1]==')'){
	int op = fn.ReverseFind('(');
		if(op>0 && fn[op-1]==' '){
			if(fn.Mid(op+1,fn.GetLength()-op-2).SpanExcluding("0123456789").IsEmpty())
				renamed = renamed.Left(op-1);
		}
	}
CString testFN;
	for(UINT tmp=0;tmp<32768;tmp++){
		testFN.Format("%s (%u)",(LPCTSTR)renamed,tmp);
		if(!_access((LPCTSTR)testFN,0))
			continue;
		fn=testFN;
		return TRUE;
	}
	return FALSE;
}

BOOL CWRQSocket::SaveAs(CString& fn)
{
CFileDialog cfd(FALSE,NULL,fn,OFN_EXPLORER|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_PATHMUSTEXIST,NULL,m_Daddy);
CString title;
	title.LoadString(IDS_RENAME_TITLE);
	cfd.m_ofn.lpstrTitle=(LPCTSTR)title;
	if(cfd.DoModal()!=IDOK)
		return FALSE;
	fn = cfd.GetPathName();
	return TRUE;
}

void CWRQSocket::DoXfer()
{
tftp *p = tftp::Allocate(tftp::tftpACK::tftpSize());
	ASSERT(p);
	p->SetOpcode(tftp::opACK);
	p->data.m_ACK.SetBlock(m_ACK);
	TRACE1("WRQ-ACK-%u\n",m_ACK);
	PostTFTP(p,TRUE);	// *** ??? Hope this is right
}

UINT tftp::tftpACK::tftpSize()
{
	return tftpHdrSize-tftpSlackSize+sizeof(tftp::tftpACK);
}

ULONG CWRQSocket::GetACK()
{
	return (m_ACK*m_blkSize)-m_LastSlack;
}

void CXferSocket::ResetTimeout()
{
	ASSERT(m_Daddy);
	m_Daddy->m_Retrier->KillTimer(m_hSocket);
	if(m_Retry)
		m_Daddy->m_Retrier->SetTimer(m_hSocket,min(60,m_Daddy->m_RetryTimeOut.GetTotalSeconds())*1000,NULL);
	if(!m_bRetry){
		m_Daddy->KillTimer(m_hSocket);
		m_Daddy->SetTimer(m_hSocket,min(60,m_timeOut)*1000,NULL);
	}
}

void CXferSocket::Abort()
{
	ASSERT(m_Daddy);
CString tmp;
	tmp.Format(IDS_LOG_XFERABORTED,(LPCTSTR)m_FileName);
	m_Daddy->LogLine(tmp);
	Destroy(FALSE);
}

void CPumpKINDlg::OnTimer(UINT nIDEvent) 
{
CXferSocket *socket;
	if(m_Xfers.Lookup(nIDEvent,socket)){
	CString tmp;
		tmp.Format(IDS_LOG_TIMEDOUT,socket->m_FileName);
		LogLine(tmp);
		socket->Abort();
	}else{
		TRACE0("Failed to find timed out socket!\n");
	}
	CDialog::OnTimer(nIDEvent);
}

void CPumpKINDlg::OnExit() 
{
	if(!m_Xfers.IsEmpty()){
	CString title,text;
		title.LoadString(IDS_CONFIRMEXIT_TITLE);
		text.LoadString(IDS_CONFIRMEXIT_TEXT);
		if(MessageBox(text,title,MB_ICONQUESTION|MB_YESNO)!=IDYES)
			return;
	}
	m_bExiting=TRUE;
	EndDialog(IDOK);
}

void CPumpKINDlg::OnPut() 
{
CRequestDlg crd(NULL);
	crd.m_Put=TRUE;
	crd.m_BSize=m_BlockSize;
	if(crd.DoModal()==IDOK){
	CRRQSocket *socket = new CRRQSocket(this,crd.m_RemoteFile,crd.m_Type,NULL);
		if(crd.m_BSize)
			socket->m__blkSize=crd.m_BSize;
		if(!socket->Create(crd.m_LocalFile,crd.m_Host))
			socket->Destroy();
	}
}

void CPumpKINDlg::OnGet() 
{
CRequestDlg crd(NULL);
	crd.m_Put=FALSE;
	crd.m_BSize=m_BlockSize;
	if(crd.DoModal()==IDOK){
	CWRQSocket *socket = new CWRQSocket(this,crd.m_RemoteFile,crd.m_Type,NULL);
		if(crd.m_BSize)
			socket->m__blkSize=crd.m_BSize;
		if(!socket->Create(crd.m_LocalFile,crd.m_Host))
			socket->Destroy();
	}
}

void CPumpKINDlg::SetupButtons()
{
	m_AbortCtl.EnableWindow(m_List.GetSelectedCount()>0);
}

void CPumpKINDlg::OnDeleteallitemsConnections(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	SetupButtons();
	*pResult = 0;
}

void CPumpKINDlg::OnDeleteitemConnections(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	SetupButtons();
	*pResult = 0;
}

void CPumpKINDlg::OnInsertitemConnections(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	SetupButtons();
	*pResult = 0;
}

void CPumpKINDlg::OnItemchangedConnections(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	SetupButtons();
	*pResult = 0;
}

void CPumpKINDlg::OnAbort() 
{
	if(!m_List.GetSelectedCount())
		return;
int	items = m_List.GetItemCount();
	for(int tmp=0;tmp<items;tmp++){
		if(!(m_List.GetItemState(tmp,LVIS_SELECTED)&LVIS_SELECTED))
			continue;
	CXferSocket *xfer = (CXferSocket*) m_List.GetItemData(tmp);
		ASSERT(xfer);
		xfer->Abort();
	}
}

void CXferSocket::OnFailedToResolve()
{
	TRACE0("Resolve failed\n");
	delete m_wndResolver;
	m_wndResolver=NULL;
	ASSERT(m_Daddy);
CString tmp;
	tmp.Format(IDS_LOG_RESOLVEFAILED,m_HostName);
	m_Daddy->LogLine(tmp);
	Abort();
}

void CXferSocket::OnResolved()
{
	delete m_wndResolver;
	m_wndResolver=NULL;
	TRACE0("Resolved\n");
	m_Peer.sin_addr.s_addr = *(DWORD*)(((hostent*)m_ResolveBuff)->h_addr);
}

void CRRQSocket::OnResolved()
{
	CXferSocket::OnResolved();
	OnHostKnown();
}

void CRRQSocket::OnHostKnown()
{
	ASSERT(m_Daddy);
	m_Peer.sin_port=htons(m_Daddy->m_SpeakPort);
tftp::tftpOptions o;
CString v;
	ASSERT(m_xferSize>=0);
	v.Format("%lu",m_xferSize);
	o[tftpoTSize] = v;
	ASSERT(m__blkSize);
	v.Format("%u",m__blkSize);
	o[tftpoBSize] = v;
	ASSERT(m__timeOut);
	v.Format("%u",m__timeOut);
	o[tftpoTOut] = v;
	o[tftpoXResume] = "0";
tftp	*p = tftp::Allocate(tftp::tftpWRQ::tftpSize(m_FileName,m_Type,&o));
	ASSERT(p);
	p->SetOpcode(tftp::opWRQ);
	p->data.m_WRQ.Set(m_FileName,m_Type,&o);
	PostTFTP(p,TRUE);
	state=stateInit;
	UpdateList();
}

UINT tftp::tftpRQ::tftpSize(LPCTSTR file,LPCTSTR type,tftp::tftpOptions* ops)
{
UINT rv = tftpHdrSize-tftpSlackSize+strlen(file)+1+strlen(type)+1;
	if(ops){
	tftpOptions& o = *ops;
	POSITION p = o.GetStartPosition();
		while(p){
		CString n,v;
			o.GetNextAssoc(p,n,v);
			rv+=n.GetLength()+1+v.GetLength()+1;
		}
	}
	return rv;
}
UINT tftp::tftpRRQ::tftpSize(LPCTSTR file,LPCTSTR type,tftp::tftpOptions* ops)
{
	return tftp::tftpRQ::tftpSize(file,type,ops);
}
UINT tftp::tftpWRQ::tftpSize(LPCTSTR file,LPCTSTR type,tftp::tftpOptions* ops)
{
	return tftp::tftpRQ::tftpSize(file,type,ops);
}
UINT tftp::tftpOACK::tftpSize(tftp::tftpOptions* ops)
{
UINT rv = tftpHdrSize-tftpSlackSize;
	if(ops){
	tftpOptions& o = *ops;
	POSITION p = o.GetStartPosition();
		while(p){
		CString n,v;
			o.GetNextAssoc(p,n,v);
			rv+=n.GetLength()+1+v.GetLength()+1;
		}
	}
	return rv;
}
void tftp::tftpRQ::Set(LPCTSTR file,LPCTSTR type,tftp::tftpOptions* ops)
{
	// MAY BE DANGEROUS!
UINT ptr = 0;
	strcpy((LPTSTR)&data[ptr],file); ptr+=strlen(file)+1;
	strcpy((LPTSTR)&data[ptr],type); ptr+=strlen(type)+1;
	if(ops){
	tftpOptions& o = *ops;
	POSITION p = o.GetStartPosition();
		while(p){
		CString n,v;
			o.GetNextAssoc(p,n,v);
			strcpy((LPTSTR)&data[ptr],(LPCTSTR)n); ptr+=n.GetLength()+1;
			strcpy((LPTSTR)&data[ptr],(LPCTSTR)v); ptr+=v.GetLength()+1;
		}
	}
}
void tftp::tftpRRQ::Set(LPCTSTR file,LPCTSTR type,tftp::tftpOptions* ops)
{
	// MAY BE DANGEROUS!
UINT ptr = 0;
	strcpy((LPTSTR)&data[ptr],file); ptr+=strlen(file)+1;
	strcpy((LPTSTR)&data[ptr],type); ptr+=strlen(type)+1;
	if(ops){
	tftpOptions& o = *ops;
	POSITION p = o.GetStartPosition();
		while(p){
		CString n,v;
			o.GetNextAssoc(p,n,v);
			strcpy((LPTSTR)&data[ptr],(LPCTSTR)n); ptr+=n.GetLength()+1;
			strcpy((LPTSTR)&data[ptr],(LPCTSTR)v); ptr+=v.GetLength()+1;
		}
	}
}
void tftp::tftpWRQ::Set(LPCTSTR file,LPCTSTR type,tftp::tftpOptions* ops)
{
	// MAY BE DANGEROUS!
UINT ptr = 0;
	strcpy((LPTSTR)&data[ptr],file); ptr+=strlen(file)+1;
	strcpy((LPTSTR)&data[ptr],type); ptr+=strlen(type)+1;
	if(ops){
	tftpOptions& o = *ops;
	POSITION p = o.GetStartPosition();
		while(p){
		CString n,v;
			o.GetNextAssoc(p,n,v);
			strcpy((LPTSTR)&data[ptr],(LPCTSTR)n); ptr+=n.GetLength()+1;
			strcpy((LPTSTR)&data[ptr],(LPCTSTR)v); ptr+=v.GetLength()+1;
		}
	}
}
void tftp::tftpOACK::Set(tftpOptions* ops)
{
	ASSERT(ops);
UINT ptr = 0;
tftpOptions& o = *ops;
POSITION p = o.GetStartPosition();
	while(p){
	CString n,v;
		o.GetNextAssoc(p,n,v);
		strcpy((LPTSTR)&data[ptr],(LPCTSTR)n); ptr+=n.GetLength()+1;
		strcpy((LPTSTR)&data[ptr],(LPCTSTR)v); ptr+=v.GetLength()+1;
	}
}

void CWRQSocket::OnResolved()
{
	CXferSocket::OnResolved();
	OnHostKnown();
}

void CWRQSocket::OnHostKnown()
{
	ASSERT(m_Daddy);
	m_Peer.sin_port=htons(m_Daddy->m_SpeakPort);
tftp::tftpOptions o;
CString v;
	o[tftpoTSize]="0";
	if(m__blkSize){
		v.Format("%u",m__blkSize);
		o[tftpoBSize]=v;
	}
	if(m__timeOut){
		v.Format("%u",m__timeOut);
		o[tftpoTOut]=v;
	}
tftp	*p = tftp::Allocate(tftp::tftpRRQ::tftpSize(m_FileName,m_Type,&o));
	ASSERT(p);
	p->SetOpcode(tftp::opRRQ);
	p->data.m_RRQ.Set(m_FileName,m_Type,&o);
	PostTFTP(p,TRUE);
	state=stateInit;
	UpdateList();
}

void CPumpKINDlg::OnClose() 
{
	OnTrayShowpumpkinwindow();
}

void CPumpKINDlg::OnTrayShowpumpkinwindow() 
{
	if(IsWindowVisible()){
		m_bShown=FALSE;
		ShowWindow(SW_HIDE);
	}else{
		m_bShown=TRUE;
		ShowWindow(SW_SHOW);
		SetForegroundWindow();
		SetFocus();
	}
}

void CPumpKINDlg::OnTrayExit() 
{
	OnExit();
}

void CPumpKINDlg::OnTrayAboutpumpkin() 
{
CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CPumpKINDlg::OnTrayFetchfile() 
{
	OnGet();
}

void CPumpKINDlg::OnTrayHelp() 
{
	OnHelp();
}

void CPumpKINDlg::OnTrayOptions() 
{
	OnOptions();
}

void CPumpKINDlg::OnTraySendfile() 
{
	OnPut();
}

void CPumpKINDlg::LoadSettings()
{
CWinApp *app = AfxGetApp();
	ASSERT(app);
	m_bListen=app->GetProfileInt("TFTPSettings","Listen",m_bListen);
	m_bnwRequest=app->GetProfileString("BellsNWhistles","Request",m_bnwRequest);
	m_bnwSuccess=app->GetProfileString("BellsNWhistles","Success",m_bnwSuccess);
	m_bnwAbort=app->GetProfileString("BellsNWhistles","Abort",m_bnwAbort);
	m_bTFTPSubdirs=app->GetProfileInt("TFTPSettings","Subdirs",m_bTFTPSubdirs);
	m_ListenPort=app->GetProfileInt("TFTPSettings","ListenPort",m_ListenPort);
	m_LogLength=app->GetProfileInt("UISettings","LogLength",m_LogLength);
	m_PromptTimeOut=app->GetProfileInt("UISettings","PromptTimeout",m_PromptTimeOut);
	m_RRQMode=app->GetProfileInt("TFTPSettings","RRQMode",m_RRQMode);
	m_SpeakPort=app->GetProfileInt("TFTPSettings","SpeakPort",m_SpeakPort);
	m_TFTPRoot=app->GetProfileString("TFTPSettings","TFTPRoot",m_TFTPRoot);
	m_LogFile=app->GetProfileString("General","LogFile",m_LogFile);
	m_TFTPTimeOut=CTimeSpan(app->GetProfileInt("TFTPSettings","TFTPTimeout",m_TFTPTimeOut.GetTotalSeconds()));
	m_BlockSize=app->GetProfileInt("TFTPSettings","TFTPBlockSize",m_BlockSize);
	m_RetryTimeOut=CTimeSpan(app->GetProfileInt("TFTPSettings","RetryTimeout",m_RetryTimeOut.GetTotalSeconds()));
	m_WRQMode=app->GetProfileInt("TFTPSettings","WRQMode",m_WRQMode);
	m_bShown=app->GetProfileInt("UISettings","Visble",m_bShown);
	if(m_TFTPRoot.IsEmpty()){
	DWORD dL = ::GetCurrentDirectory(0,NULL);
		VERIFY(::GetCurrentDirectory(dL,m_TFTPRoot.GetBuffer(dL)));
		m_TFTPRoot.ReleaseBuffer();
	}
	::SetCurrentDirectory(m_TFTPRoot);
	m_aclRules.LoadProfile(app);
}

void CPumpKINDlg::SaveSettings()
{
CWinApp *app = AfxGetApp();
	ASSERT(app);
	app->WriteProfileInt("TFTPSettings","Listen",m_bListen);
	app->WriteProfileString("BellsNWhistles","Request",m_bnwRequest);
	app->WriteProfileString("BellsNWhistles","Success",m_bnwSuccess);
	app->WriteProfileString("BellsNWhistles","Abort",m_bnwAbort);
	app->WriteProfileInt("TFTPSettings","Subdirs",m_bTFTPSubdirs);
	app->WriteProfileInt("TFTPSettings","ListenPort",m_ListenPort);
	app->WriteProfileInt("UISettings","LogLength",m_LogLength);
	app->WriteProfileInt("UISettings","PromptTimeout",m_PromptTimeOut);
	app->WriteProfileInt("TFTPSettings","RRQMode",m_RRQMode);
	app->WriteProfileInt("TFTPSettings","SpeakPort",m_SpeakPort);
	app->WriteProfileString("TFTPSettings","TFTPRoot",m_TFTPRoot);
	app->WriteProfileString("General","LogFile",m_LogFile);
	app->WriteProfileInt("TFTPSettings","TFTPTimeout",m_TFTPTimeOut.GetTotalSeconds());
	app->WriteProfileInt("TFTPSettings","TFTPBlockSize",m_BlockSize);
	app->WriteProfileInt("TFTPSettings","RetryTimeout",m_RetryTimeOut.GetTotalSeconds());
	app->WriteProfileInt("TFTPSettings","WRQMode",m_WRQMode);
	app->WriteProfileInt("UISettings","Visble",m_bShown);
	m_aclRules.SaveProfile(app);
}

void CPumpKINDlg::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CDialog::OnWindowPosChanging(lpwndpos);
	if(!m_bExiting){
		if(m_bShown){
			lpwndpos->flags&=~SWP_HIDEWINDOW;
			lpwndpos->flags|=SWP_SHOWWINDOW;
		}else{
			lpwndpos->flags&=~SWP_SHOWWINDOW;
			lpwndpos->flags|=SWP_HIDEWINDOW;
			if(::GetForegroundWindow()==m_hWnd && (m_Trayer &&  !m_Trayer->m_inMenu))
				GetDesktopWindow()->SetForegroundWindow();
		}
	}
}

CString CXferSocket::ApplyRootGently(LPCTSTR fn)
{
CString f = fn;
CString rv = f;
	if((!f.IsEmpty()) && f[0]!='\\' && f.Find(":")<0 && f.Find("\\")<0)
		rv = ApplyRoot(f);
	return rv;
}

BOOL CXferSocket::CheckBadRelativeness(LPCTSTR file)
{
CString tmp = file;
	if(tmp.IsEmpty())
		return FALSE;
	if(tmp.Find("..")>=0)
		return TRUE;
	if(tmp.Find(":")>=0)
		return TRUE;
	if((!m_Daddy->m_bTFTPSubdirs) && m_FileName.Find('\\')>=0)
		return TRUE;
	return FALSE;
}

void CAboutDlg::OnKlevernet() 
{
CString url;
	url.LoadString(IDS_KLEVERNET_URL);
	ShellExecute(::GetDesktopWindow(),"open",url,NULL,NULL,SW_SHOWMAXIMIZED);
}

BOOL CPumpKINDlg::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following block was added by the ToolTips component.
	{
		// Let the ToolTip process this message.
		m_tooltip.RelayEvent(pMsg);

		return CDialog::PreTranslateMessage(pMsg);
	}
}

void CPumpKINDlg::OnSelchangeLog() 
{
int sel = m_Log.GetCurSel();
	if(sel==LB_ERR){
		TRACE0("Error retrieving selection\n");
	}else{
	CTime *t = (CTime*)m_Log.GetItemData(sel);
		ASSERT(t);
		m_tooltip.UpdateTipText(t->Format(IDS_LOGTIMEFORMAT),&m_Log);
	}
}

void CPumpKINDlg::OnTrayOpenfilesfolder() 
{
	ShellExecute(::GetDesktopWindow(),NULL,m_TFTPRoot,NULL,NULL,SW_SHOWDEFAULT);
}

void CPumpKINDlg::OnDropFiles(HDROP hDropInfo) 
{
UINT files = ::DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0);
	ASSERT(files);
	for(UINT file=0;file<files;file++){
	CString theFile;
	UINT fileNameLength = ::DragQueryFile(hDropInfo,file,NULL,0);
		ASSERT(fileNameLength);
		VERIFY(::DragQueryFile(hDropInfo,file,theFile.GetBuffer(fileNameLength+5),fileNameLength+4)<=fileNameLength);
		theFile.ReleaseBuffer();
		// Send it over!
	CRequestDlg crd(NULL);
		crd.m_Drop=TRUE;
		crd.m_Put=TRUE;
		crd.m_BSize=m_BlockSize;
		crd.m_LocalFile=theFile;
		if(crd.DoModal()==IDOK){
		CRRQSocket *socket = new CRRQSocket(this,crd.m_RemoteFile,crd.m_Type,NULL);
			if(crd.m_BSize)
				socket->m__blkSize=crd.m_BSize;
			if(!socket->Create(crd.m_LocalFile,crd.m_Host))
				socket->Destroy();
		}
	}
	::DragFinish(hDropInfo);
}

void CPumpKINDlg::OnCancel() 
{
	OnClose();
}

CPumpKINDlg::~CPumpKINDlg()
{
	delete m_Trayer;
	delete m_Retrier;
}

void CXferSocket::OnRetry()
{
	if(!m_Retry){
		TRACE("Retrying unretriable..\n");
		return;
	}
	TRACE0("Retrying..\n");
	m_bRetry=TRUE;
	PostTFTP(tftp::Copy(m_Retry));
	m_bRetry=FALSE;
}

tftp* tftp::Copy(tftp *src)
{
	ASSERT(src);
	ASSERT(src->length);
tftp* rv = Allocate(src->length);
	ASSERT(rv);
	memmove(rv,src,tftpSlackSize+src->length);
	return rv;
}

void CXferSocket::SetTry(tftp *p)
{
	if(m_Retry)
		delete m_Retry;
	m_Retry=p?tftp::Copy(p):NULL;
}

void CPumpKINDlg::OnHelp() 
{
	AfxGetApp()->WinHelp(0,HELP_FINDER);	
}

BOOL CListenSocket::SetListen(BOOL b) {
	ASSERT(m_Daddy);
	if(b==m_bListen)
		return TRUE;
	if(b) {
		if(!Create(m_Daddy->m_ListenPort,SOCK_DGRAM))
			return FALSE;
		return m_bListen=TRUE;
	}else{
		Close(); m_bListen=FALSE;
		return TRUE;
	}
}

void CPumpKINDlg::OnListening() 
{
	if(!m_Listener.SetListen(m_ListenCtl.GetCheck()==1)) {
		TRACE0("Failed to create socket\n");
		AfxMessageBox(IDS_BOX_CANTBIND,MB_OK|MB_ICONEXCLAMATION);
	}
	m_ListenCtl.SetCheck(m_Listener.m_bListen?1:0);
	m_bListen=m_Listener.m_bListen;
}

void CPumpKINDlg::OnTrayListen() 
{
	if(!m_Listener.SetListen(!m_Listener.m_bListen)) {
		TRACE0("Failed to create socket\n");
		AfxMessageBox(IDS_BOX_CANTBIND,MB_OK|MB_ICONEXCLAMATION);
	}
	m_ListenCtl.SetCheck(m_Listener.m_bListen?1:0);
	m_bListen=m_Listener.m_bListen;
}

void CPumpKINDlg::LogLine(LPCTSTR str)
{
	LogLineToScreen(str);
	if(!m_LogFile.IsEmpty()) {
		if(!Klever::LogRecord((LPCTSTR)m_LogFile,str)) {
			if(m_lastlogerr!=m_LogFile) {
				CString tmp;
				tmp.Format(IDS_LOG_LOGERROR,m_LogFile);
				LogLineToScreen(tmp);
				m_lastlogerr=m_LogFile;
			}
		}
	}
}

void CPumpKINDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CDialog::OnGetMinMaxInfo(lpMMI);
	if(m_MinSize.cx>0 && m_MinSize.cy>0){
		lpMMI->ptMinTrackSize.x = m_MinSize.cx;
		lpMMI->ptMinTrackSize.y = m_MinSize.cy;
	}
}

void CPumpKINDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	if(nType==SIZE_RESTORED)
		RecalcLayout(cx,cy);
}

void CPumpKINDlg::RecalcLayout(int,int)
{
	CRect wrc;
	GetClientRect(&wrc);
	AdjustButton(m_GetCtl,wrc);
	AdjustButton(m_PutCtl,wrc);
	AdjustButton(m_AbortCtl,wrc);
	AdjustButton(m_HelpCtl,wrc);
	AdjustButton(m_ExitCtl,wrc);
	AdjustButton(m_OptionsCtl,wrc);
	CRect brc;
	m_List.GetWindowRect(&brc); ScreenToClient(&brc);
	m_List.SetWindowPos(
		0,
		brc.left, brc.top,
		wrc.right-m_rightGapList-brc.left, brc.bottom-brc.top,
		SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_NOCOPYBITS );
	m_Log.GetWindowRect(&brc); ScreenToClient(&brc);
	m_Log.SetWindowPos(
		0,
		brc.left, brc.top,
		wrc.right-m_rightGapButtons-brc.left, wrc.bottom-m_bottomGapLog-brc.top,
		SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_NOCOPYBITS );
	m_ListenCtl.GetWindowRect(&brc); ScreenToClient(&brc);
	m_ListenCtl.SetWindowPos(
		0,
		wrc.right-brc.Width()-m_rightGapButtons, wrc.bottom-brc.Height()-m_bottomGapListen,
		0,0,
		SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOSIZE|SWP_NOZORDER|SWP_NOCOPYBITS );
	int i = m_Log.GetCount();
	if(i!=LB_ERR)
		m_Log.SetCurSel(i-1);
}

void CPumpKINDlg::AdjustButton(CWnd& w,CRect& wrc)
{
	CRect brc;
	w.GetWindowRect(&brc); ScreenToClient(&brc);
	w.SetWindowPos(
		0,
		wrc.right-brc.Width()-m_rightGapButtons, brc.top,
		0, 0,
		SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOSIZE|SWP_NOZORDER|SWP_NOCOPYBITS );
}
