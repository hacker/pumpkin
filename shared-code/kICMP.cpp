#include "../stdafx.h"
#include "kICMP.h"

CICMP::_mechanismus CICMP::m_mechanismus = CICMP::_icmpUndetermined;

BOOL CICMPDll::Initialize()
{
	if(m_hICMP!=INVALID_HANDLE_VALUE || m_hICMPDLL)
		Deinitialize();
	m_hICMPDLL = ::LoadLibraryEx("ICMP",NULL,0);
	if(!m_hICMPDLL)
		return FALSE;
	*(FARPROC*)&m_icmpCF = ::GetProcAddress(m_hICMPDLL,"IcmpCreateFile");
	*(FARPROC*)&m_icmpSE = ::GetProcAddress(m_hICMPDLL,"IcmpSendEcho");
	*(FARPROC*)&m_icmpCH = ::GetProcAddress(m_hICMPDLL,"IcmpCloseHandle");
	if(!(m_icmpCF && m_icmpSE && m_icmpCH)){
		Deinitialize(); return FALSE;
	}
	m_hICMP = (*m_icmpCF)();
	if(m_hICMP==INVALID_HANDLE_VALUE){
		Deinitialize(); return FALSE;
	}
	TRACE0("ICMP-DLL Initialized\n");
	return TRUE;
}
void CICMPDll::Deinitialize()
{
	if(m_hICMPDLL){
		if(m_hICMP!=INVALID_HANDLE_VALUE && m_icmpCH)
			(*m_icmpCH)(m_hICMP);
		::FreeLibrary(m_hICMPDLL); m_hICMPDLL = NULL;
		m_icmpCF = NULL;
		m_icmpSE = NULL; 
		m_icmpCH = NULL;
	}
	m_hICMP=INVALID_HANDLE_VALUE;
	if(m_sizeOut && m_bsOut){
		delete m_bsOut;
		m_bsOut = NULL; m_sizeOut = 0;
	}
	if(m_sizeIn && m_bsIn){
		delete m_bsIn;
		m_bsIn = NULL; m_sizeIn = 0;
	}
}

LONG CICMPDll::Ping(const in_addr host,const UINT packetSize,
		const UINT timeOut,LPINT pStatus)
{
	if(!(m_hICMP && m_hICMPDLL && m_icmpSE)){
		if(pStatus)
			(*pStatus) = icmpNotInitialized;
		return -1;
	}
	VERIFY(AdjustBuffers(packetSize));
IPINFO ipi;
	memset(&ipi,0,sizeof(ipi));
	ipi.Ttl = 30;
	for(UINT tmp=0;tmp<packetSize;tmp++)
		m_bsOut[tmp]=tmp&0xFF;
LPICMPECHO pRep = (LPICMPECHO)m_bsIn;
	pRep->Status = 0xFFFFFFFFl;
	if((*m_icmpSE)(m_hICMP,host.s_addr,m_bsOut,packetSize,
					  &ipi,pRep,m_sizeIn,timeOut))
		TRACE0("ICMP-SendEcho succeeded\n");
	else
		TRACE0("ICMP-SendEcho failed\n");
LONG lrv = -1;
INT rv = ipUnknown;
	switch(pRep->Status){
	case IP_SUCCESS:
		lrv = pRep->RTTime; rv = ipSuccess;
		break;
	case IP_BUF_TOO_SMALL: rv = ipBuffTooSmall; break;
	case IP_DEST_NET_UNREACHABLE: rv = ipDestNetUnreachable; break;
	case IP_DEST_HOST_UNREACHABLE: rv = ipDestHostUnreachable; break;
	case IP_DEST_PROT_UNREACHABLE: rv = ipDestProtUnreachable; break;
	case IP_DEST_PORT_UNREACHABLE: rv = ipDestPortUnreachable; break;
	case IP_NO_RESOURCES: rv = ipNoResources; break;
	case IP_BAD_OPTION: rv = ipBadOption; break;
	case IP_HW_ERROR: rv = ipHWError; break;
	case IP_PACKET_TOO_BIG: rv = ipPacketTooBig; break;
	case IP_REQ_TIMED_OUT: rv = ipTimeOut; break;
	case IP_BAD_REQ: rv = ipBadRequest; break;
	case IP_BAD_ROUTE: rv = ipBadRoute; break;
	case IP_TTL_EXPIRED_TRANSIT: rv = ipTTLExpiredInTransit; break;
	case IP_TTL_EXPIRED_REASSEM: rv = ipTTLExpiredInReasm; break;
	case IP_PARAM_PROBLEM: rv = ipParamProblem; break;
	case IP_SOURCE_QUENCH: rv = ipSourceQuench; break;
	case IP_OPTION_TOO_BIG: rv = ipOptionTooBig; break;
	case IP_BAD_DESTINATION: rv = ipBadDest; break;
	}
	if(pStatus)
		(*pStatus)=rv;
	return lrv;
}

BOOL CICMPDll::AdjustBuffers(UINT packetSize)
{
	if(!packetSize)
		packetSize=1;
	if(packetSize>m_sizeOut){
		if(m_sizeOut && m_bsOut)
			delete m_bsOut;
		m_bsOut = new BYTE[m_sizeOut=packetSize];
		if(!m_bsOut)
			return FALSE;
	}
UINT sin = sizeof(ICMPECHO)+SIZE_ICMP_HDR+packetSize;
	if(sin>m_sizeIn){
		if(m_sizeIn && m_bsIn)
			delete m_bsIn;
		m_bsIn = new BYTE[m_sizeIn=sin];
		if(!m_bsIn)
			return FALSE;
	}
	return TRUE;
}


WORD CICMPWS::m_icmpSeq = 0;

BOOL CICMPWS::Initialize()
{
	if(m_socket!=INVALID_SOCKET)
		Deinitialize();
	m_socket = socket(AF_INET,SOCK_RAW,1/*ICMP*/);
	if(m_socket==INVALID_SOCKET)
		return FALSE;
	TRACE0("ICMP-WS Initialized\n");
	return TRUE;
}
void CICMPWS::Deinitialize()
{
	if(m_socket!=INVALID_SOCKET){
		closesocket(m_socket);
		m_socket=INVALID_SOCKET;
	}
	if(m_sizeOut && m_bsOut){
		delete m_bsOut;
		m_bsOut = NULL; m_sizeOut = 0;
	}
	if(m_sizeIn && m_bsIn){
		delete m_bsIn;
		m_bsIn = NULL; m_sizeIn = 0;
	}
}
LONG CICMPWS::Ping(const in_addr host,const UINT packetSize,
		const UINT timeOut,LPINT pStatus)
{
	if(m_socket==INVALID_SOCKET){
		if(pStatus)
			(*pStatus)=icmpNotInitialized;
	}
	VERIFY(AdjustBuffers(packetSize));
icmp* pPacket = (icmp*)m_bsOut;
	memset(pPacket,0,m_sizeOut);
	pPacket->icmp_type = ICMP_ECHO;
	pPacket->icmp_seq = m_icmpSeq++;
	pPacket->icmp_id = (WORD)(::GetCurrentThreadId()&0xFFFF);
	for(UINT tmp=0;tmp<packetSize;tmp++)
		pPacket->icmp_data[tmp]=tmp&0xFF;
	pPacket->icmp_cksum = cksum(pPacket,SIZE_ICMP_HDR+packetSize);
sockaddr_in to;
	memset(&to,0,sizeof(to));
	to.sin_addr.s_addr = host.s_addr;
	to.sin_family = AF_INET;
	if(sendto(m_socket,(char*)pPacket,SIZE_ICMP_HDR+packetSize,0,
		(SOCKADDR*)&to,sizeof(to)) != (int)(SIZE_ICMP_HDR+packetSize)){
		TRACE1("sendto: %lu\n",WSAGetLastError());
		if(pStatus)
			(*pStatus)=icmpSocketError;
		return -1;
	}
DWORD sentTime = ::GetTickCount();
sockaddr_in from;
	memset(&from,0,sizeof(from));
	from.sin_family=AF_INET;
	from.sin_addr.s_addr=INADDR_ANY;
fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_socket,&fds);
long lrv = -1;
INT rv = ipTimeOut;
	for(;;){
	DWORD ct = ::GetTickCount();
		if((ct-sentTime)>=timeOut){
			TRACE0("Timeout\n");
			break;
		}
	timeval tv = {
			(timeOut-ct+sentTime)/1000,
			(timeOut-ct+sentTime)%1000
	};	// tv_sec, tv_usec (secs,microsecs)
		if(!select(m_socket,&fds,NULL,NULL,&tv)){
			TRACE1("select: %d\n",WSAGetLastError());
			break;
		}
	DWORD rtime = ::GetTickCount();
		ASSERT(FD_ISSET(m_socket,&fds));
	int fl = sizeof(from);
	int rb = recvfrom(m_socket,(char*)m_bsIn,m_sizeIn,0,(SOCKADDR*)&from,&fl);
	ip* pIP = (ip*)m_bsIn;
	icmp* pICMP = (icmp*)&m_bsIn[sizeof(ip)];
		if(pICMP->icmp_id!=pPacket->icmp_id)
			continue;
		if(pICMP->icmp_seq!=pPacket->icmp_seq)
			continue;
		if(from.sin_addr.s_addr!=host.s_addr)
			continue;
		if(pICMP->icmp_type==ICMP_ECHOREPLY){
			lrv=rtime-sentTime;
			rv=ipSuccess;
			break;
		}
		rv = ipUnknown;	// *** 
		break;
	}
	if(pStatus)
		(*pStatus)=rv;
	return lrv;
}

BOOL CICMPWS::AdjustBuffers(UINT packetSize)
{
	if(!packetSize)
		packetSize=0;
UINT osize = packetSize+SIZE_ICMP_HDR;
	if(m_sizeOut<osize){
		if(m_sizeOut && m_bsOut)
			delete m_bsOut;
		m_bsOut = new BYTE[m_sizeOut=osize];
		if(!m_bsOut)
			return FALSE;
	}
UINT isize = osize+sizeof(ip);
	if(m_sizeIn<isize){
		if(m_sizeIn && m_bsIn)
			delete m_bsIn;
		m_bsIn = new BYTE[m_sizeIn=isize];
		if(!m_bsIn)
			return FALSE;
	}
	return TRUE;
}

WORD CICMPWS::cksum(LPVOID data,int count)
{
long lSum = 0;
WORD *pData = (WORD*)data;
	while(count>0){
		if(count>1){
			lSum+=*(pData++);
			count-=2;
		}else{
			lSum+=((WORD)*(BYTE*)pData)&0xFF;
			count--;
		}
	}
	lSum = (lSum&0xFFFF)+(lSum>>16);
	lSum += (lSum>>16);
	return (~lSum)&0xFFFF;
}

CICMP* CICMP::CreateICMP()
{
	if(m_mechanismus==_icmpUndetermined)
		GuessMechanismus();
	switch(m_mechanismus){
	case _icmpWinsock:
		return new CICMPWS;
		break;
	case _icmpDLL:
		return new CICMPDll;
		break;
	}
	return NULL;
}

void CICMP::GuessMechanismus()
{
	m_mechanismus=_icmpUndetermined;
SOCKET testSocket = socket(AF_INET,SOCK_RAW,1);
	if(testSocket!=INVALID_SOCKET){
		closesocket(testSocket);
		m_mechanismus=_icmpWinsock;
	}else{
	HINSTANCE hICMP = ::LoadLibraryEx("ICMP",NULL,0);
		if(!hICMP)
			return;
	BOOL isThere = (
		::GetProcAddress(hICMP,"IcmpCreateFile")
		&& ::GetProcAddress(hICMP,"IcmpSendEcho")
		&& ::GetProcAddress(hICMP,"IcmpCloseHandle")
	);
		::FreeLibrary(hICMP);
		if(isThere)
			m_mechanismus=_icmpDLL;
	}
}