// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxtempl.h>

#include <afxsock.h>		// MFC socket extensions
#include <MMSystem.h>
// CG: The following line was added by the Windows Multimedia component.
#pragma comment(lib, "winmm.lib")

enum	{
	WM_RESOLVED = WM_USER,
	WM_TRAYICON
};

#include "shared-code/kHelpers.h"
#include "shared-code/BellsNWhistles.h"