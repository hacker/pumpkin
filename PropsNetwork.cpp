// PropsNetwork.cpp : implementation file
//

#include "stdafx.h"
#include "PumpKIN.h"
#include "PropsNetwork.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropsNetwork property page

IMPLEMENT_DYNCREATE(CPropsNetwork, CPropertyPage)

CPropsNetwork::CPropsNetwork() : CPropertyPage(CPropsNetwork::IDD)
{
	//{{AFX_DATA_INIT(CPropsNetwork)
	m_ListenPort = 0;
	m_SpeakPort = 0;
	m_TimeOut = 0;
	m_BlockSize = 0;
	m_ListenAddress = _T("");
	//}}AFX_DATA_INIT
}

CPropsNetwork::~CPropsNetwork()
{
}

void CPropsNetwork::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropsNetwork)
	DDX_Control(pDX, IDC_BSIZESPIN, m_BSizeSpinCtl);
	DDX_Control(pDX, IDC_TIMESPIN, m_TimeSpinCtl);
	DDX_Control(pDX, IDC_SPEAKSPIN, m_SpeakSpinCtl);
	DDX_Control(pDX, IDC_LISTENSPIN, m_ListenSpinCtl);
	DDX_Text(pDX, IDC_LISTENPORT, m_ListenPort);
	DDX_Text(pDX, IDC_SPEAKPORT, m_SpeakPort);
	DDX_Text(pDX, IDC_TIMEOUT, m_TimeOut);
	DDV_MinMaxUInt(pDX, m_TimeOut, 5, 60);
	DDX_Text(pDX, IDC_BLOCKSIZE, m_BlockSize);
	DDX_Text(pDX, IDC_LISTENADDRESS, m_ListenAddress);
	DDV_MaxChars(pDX, m_ListenAddress, 15);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropsNetwork, CPropertyPage)
	//{{AFX_MSG_MAP(CPropsNetwork)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropsNetwork message handlers

BOOL CPropsNetwork::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_ListenSpinCtl.SetRange(0,32767);
	m_SpeakSpinCtl.SetRange(0,32767);
	m_TimeSpinCtl.SetRange(5,60);
	m_BSizeSpinCtl.SetRange(512,16384);
UDACCEL uda = {0,512};
	m_BSizeSpinCtl.SetAccel(1,&uda);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
