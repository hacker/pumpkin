; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CPropsSounds
LastTemplate=CPropertyPage
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "PumpKIN.h"

ClassCount=12
Class1=CPumpKINApp
Class2=CPumpKINDlg
Class3=CAboutDlg

ResourceCount=9
Resource1=IDD_REQUEST
Resource2=IDD_PROPS_SERVER
Resource3=IDD_CONFIRM_RRQ
Resource4=IDD_ABOUTBOX
Class4=CPropsServer
Class5=CPropsNetwork
Resource5=IDD_CONFIRM_WRQ
Resource6=IDD_PROPS_NETWORK
Class6=CConfirmRRQDlg
Class7=CConfirmWRQDlg
Resource7=IDD_PUMPKIN_DIALOG
Class8=CRequestDlg
Class9=CResolver
Class10=CRetrier
Class11=CTrayer
Resource8=IDD_PROPS_SOUNDS
Class12=CPropsSounds
Resource9=IDM_POPUPS

[CLS:CPumpKINApp]
Type=0
HeaderFile=PumpKIN.h
ImplementationFile=PumpKIN.cpp
Filter=N

[CLS:CPumpKINDlg]
Type=0
HeaderFile=PumpKINDlg.h
ImplementationFile=PumpKINDlg.cpp
Filter=W
BaseClass=CDialog
VirtualFilter=dWC
LastObject=ID_HELP

[CLS:CAboutDlg]
Type=0
HeaderFile=PumpKINDlg.h
ImplementationFile=PumpKINDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=5
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_KLEVERNET,button,1342242816

[DLG:IDD_PUMPKIN_DIALOG]
Type=1
Class=CPumpKINDlg
ControlCount=9
Control1=IDC_CONNECTIONS,SysListView32,1350631681
Control2=IDC_GET,button,1342259200
Control3=IDC_PUT,button,1342259200
Control4=IDC_ABORT,button,1342259200
Control5=IDC_OPTIONS,button,1342259200
Control6=IDC_EXIT,button,1342259200
Control7=ID_HELP,button,1342259200
Control8=IDC_LOG,listbox,1353728129
Control9=IDCANCEL,button,1073741824

[DLG:IDD_PROPS_SERVER]
Type=1
Class=CPropsServer
ControlCount=15
Control1=IDC_STATIC,button,1342177287
Control2=IDC_TFTPROOT,edit,1350631552
Control3=IDC_BROWSE,button,1342242880
Control4=IDC_TFTPSUBDIRS,button,1342242819
Control5=IDC_STATIC,button,1342177287
Control6=IDC_RRQ_GIVEALL,button,1342324745
Control7=IDC_RRQ_ALWAYSCONFIRM,button,1342193673
Control8=IDC_RRQ_DENYALL,button,1342193673
Control9=IDC_STATIC,button,1342308359
Control10=IDC_WRQ_TAKEALL,button,1342308361
Control11=IDC_WRQ_PROMPTEXISTING,button,1342177289
Control12=IDC_WRQ_ALWAYSCONFIRM,button,1342177289
Control13=IDC_WRQ_DENYALL,button,1342177289
Control14=IDC_STATIC,static,1342308609
Control15=IDC_PROMPTTIMEOUT,msctls_trackbar32,1342242823

[DLG:IDD_PROPS_NETWORK]
Type=1
Class=CPropsNetwork
ControlCount=15
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342308354
Control3=IDC_LISTENPORT,edit,1350631552
Control4=IDC_LISTENSPIN,msctls_updown32,1342177463
Control5=IDC_STATIC,static,1342308354
Control6=IDC_SPEAKPORT,edit,1350631552
Control7=IDC_SPEAKSPIN,msctls_updown32,1342177463
Control8=IDC_STATIC,static,1342308352
Control9=IDC_TIMEOUT,edit,1350639744
Control10=IDC_TIMESPIN,msctls_updown32,1342177463
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_BLOCKSIZE,edit,1350639744
Control15=IDC_BSIZESPIN,msctls_updown32,1342177463

[CLS:CPropsServer]
Type=0
HeaderFile=PropsServer.h
ImplementationFile=PropsServer.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC
LastObject=CPropsServer

[CLS:CPropsNetwork]
Type=0
HeaderFile=PropsNetwork.h
ImplementationFile=PropsNetwork.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC
LastObject=CPropsNetwork

[DLG:IDD_CONFIRM_RRQ]
Type=1
Class=CConfirmRRQDlg
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_HOST,static,1350701313
Control5=IDC_STATIC,static,1342308353
Control6=IDC_FILE,static,1350701313
Control7=IDC_STATIC,static,1342177296
Control8=IDC_STATIC,static,1342177283
Control9=IDC_STATIC,static,1342177283

[DLG:IDD_CONFIRM_WRQ]
Type=1
Class=CConfirmWRQDlg
ControlCount=11
Control1=IDOK,button,1342242817
Control2=IDC_RENAME,button,1342242816
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC,static,1342308352
Control5=IDC_HOST,static,1350701313
Control6=IDC_STATIC,static,1342308353
Control7=IDC_FILE,static,1350701313
Control8=IDC_STATIC,static,1342177296
Control9=IDC_STATIC,static,1342177283
Control10=IDC_STATIC,static,1342177283
Control11=IDC_RESUME,button,1342242816

[CLS:CConfirmRRQDlg]
Type=0
HeaderFile=ConfirmRRQDlg.h
ImplementationFile=ConfirmRRQDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CConfirmRRQDlg

[CLS:CConfirmWRQDlg]
Type=0
HeaderFile=ConfirmWRQDlg.h
ImplementationFile=ConfirmWRQDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_RESUME

[DLG:IDD_REQUEST]
Type=1
Class=CRequestDlg
ControlCount=15
Control1=IDC_STATIC,static,1342308608
Control2=IDC_LOCALFILE,edit,1350631552
Control3=IDC_BROWSE,button,1342242880
Control4=IDC_STATIC,static,1342308608
Control5=IDC_REMOTEFILE,edit,1350631552
Control6=IDC_STATIC,static,1342308608
Control7=IDC_TALKS,combobox,1344341313
Control8=IDC_REFRESH,button,1342271232
Control9=IDC_STATIC,static,1342308352
Control10=IDC_TYPE,combobox,1344356418
Control11=IDC_STATIC,static,1342308352
Control12=IDC_BSIZE,combobox,1344339970
Control13=IDOK,button,1342242817
Control14=IDCANCEL,button,1342242816
Control15=IDC_STATIC,static,1342177297

[CLS:CRequestDlg]
Type=0
HeaderFile=RequestDlg.h
ImplementationFile=RequestDlg.cpp
BaseClass=CDialog
Filter=W
VirtualFilter=dWC
LastObject=CRequestDlg

[CLS:CResolver]
Type=0
HeaderFile=Resolver.h
ImplementationFile=Resolver.cpp
BaseClass=CWnd
Filter=W
LastObject=CResolver
VirtualFilter=WC

[MNU:IDM_POPUPS]
Type=1
Class=CPumpKINDlg
Command1=ID_TRAY_SENDFILE
Command2=ID_TRAY_FETCHFILE
Command3=ID_TRAY_OPTIONS
Command4=ID_TRAY_SHOWPUMPKINWINDOW
Command5=ID_TRAY_OPENFILESFOLDER
Command6=ID_TRAY_HELP
Command7=ID_TRAY_ABOUTPUMPKIN
Command8=ID_TRAY_EXIT
CommandCount=8

[CLS:CRetrier]
Type=0
HeaderFile=Retrier.h
ImplementationFile=Retrier.cpp
BaseClass=CWnd
Filter=W
LastObject=CRetrier
VirtualFilter=WC

[CLS:CTrayer]
Type=0
HeaderFile=Trayer.h
ImplementationFile=Trayer.cpp
BaseClass=CWnd
Filter=W
LastObject=CTrayer
VirtualFilter=WC

[DLG:IDD_PROPS_SOUNDS]
Type=1
Class=CPropsSounds
ControlCount=12
Control1=IDC_STATIC,static,1342308352
Control2=IDC_RING,combobox,1344340290
Control3=IDC_RING_BROWSE,button,1342242880
Control4=IDC_RING_PLAY,button,1342242880
Control5=IDC_STATIC,static,1342308352
Control6=IDC_FINISHED,combobox,1344340290
Control7=IDC_FINISHED_BROWSE,button,1342242880
Control8=IDC_FINISHED_PLAY,button,1342242880
Control9=IDC_STATIC,static,1342308352
Control10=IDC_ABORTED,combobox,1344340290
Control11=IDC_ABORTED_BROWSE,button,1342242880
Control12=IDC_ABORTED_PLAY,button,1342242880

[CLS:CPropsSounds]
Type=0
HeaderFile=PropsSounds.h
ImplementationFile=PropsSounds.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CPropsSounds
VirtualFilter=idWC

