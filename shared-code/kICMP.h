#ifndef	__KICMP_H
#define __KICMP_H

class CICMP {
	enum _mechanismus {
		_icmpUndetermined = -1,
		_icmpWinsock = 0, _icmpDLL
	};
static _mechanismus m_mechanismus;
static void GuessMechanismus();
public:
static
	CICMP* CreateICMP();

	enum {
		ipSuccess = 0,
		ipBuffTooSmall, ipDestNetUnreachable, ipDestHostUnreachable,
		ipDestProtUnreachable, ipDestPortUnreachable, ipNoResources,
		ipBadOption, ipHWError, ipPacketTooBig, ipTimeOut, ipBadRequest,
		ipBadRoute, ipTTLExpiredInTransit, ipTTLExpiredInReasm,
		ipParamProblem, ipSourceQuench, ipOptionTooBig, ipBadDest,
		ipUnknown = -1,
		icmpNotInitialized = -2,
		icmpSocketError = -3
	};
	
	virtual BOOL Initialize() = 0;
	virtual void Deinitialize() = 0;
	
	virtual LONG Ping(const in_addr host,const UINT packetSize=0,
		const UINT timeOut=10000,LPINT pStatus=NULL) = 0;
};

class CICMPDll : public CICMP {
	HANDLE	(WINAPI *m_icmpCF)(VOID);
	BOOL	(WINAPI *m_icmpSE)(HANDLE,ULONG,LPVOID,WORD,
		PIPINFO,LPVOID,DWORD,DWORD);
	BOOL	(WINAPI *m_icmpCH)(HANDLE);
public:
	HINSTANCE m_hICMPDLL;
	HANDLE m_hICMP;
	LPBYTE m_bsIn, m_bsOut;
	UINT m_sizeIn, m_sizeOut;

	CICMPDll() : m_hICMP(INVALID_HANDLE_VALUE), m_hICMPDLL(NULL),
		m_bsIn(NULL), m_bsOut(NULL), m_sizeIn(0), m_sizeOut(0) {}
	virtual ~CICMPDll() { Deinitialize(); }

	virtual BOOL Initialize();
	virtual void Deinitialize();

	virtual LONG Ping(const in_addr host,const UINT packetSize=0,
		const UINT timeOut=10000,LPINT pStatus=NULL);

	BOOL AdjustBuffers(UINT packetSize=0);
};

class CICMPWS : public CICMP {
static
	WORD m_icmpSeq;
public:
	SOCKET m_socket;
	LPBYTE m_bsIn, m_bsOut;
	UINT m_sizeIn, m_sizeOut;

	CICMPWS() : m_socket(INVALID_SOCKET), m_bsIn(NULL), m_bsOut(NULL),
		m_sizeIn(0), m_sizeOut(0) {}
	virtual ~CICMPWS() { Deinitialize(); }
	
	virtual BOOL Initialize();
	virtual void Deinitialize();

	virtual LONG Ping(const in_addr host,const UINT packetSize=0,
		const UINT timeOut=10000,LPINT pStatus=NULL);

	BOOL AdjustBuffers(UINT packetSize=0);
	WORD cksum(LPVOID data,int count);
};

#endif // __KICMP_H