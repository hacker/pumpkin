#ifndef	__FINDIFACE_H
#define	__FINDIFACE_H

#include "SNMPeer.h"
#include "SNMPExtDLL.h"
#include "SNMPOIDs.h"

namespace Klever {

inline BOOL FindIFace(in_addr& target,in_addr& source)
{
DEFINE_OID(ipRouteDest,		OIDipRouteDest);
DEFINE_OID(ipRouteMask,		OIDipRouteMask);
DEFINE_OID(ipRouteIfIndex,	OIDipRouteIfIndex);
DEFINE_OID(ipRouteMetric1,	OIDipRouteMetric1);
DEFINE_OID(ipAdEntIfIndex,	OIDipAdEntIfIndex);
DEFINE_OID(ipAdEntAddr,		OIDipAdEntAddr);
struct	_route	{
	int iface;
	int metric;
	DWORD nm;
}	routes[16];
int nRoutes = 0;
CSNMPVarBindList vbl;
	vbl.AddTail(CSNMPVarBind(CASNAny(CASNAny::typeASNOID,ipRouteDest,sizeof(ipRouteDest))));
	vbl.AddTail(CSNMPVarBind(CASNAny(CASNAny::typeASNOID,ipRouteMask,sizeof(ipRouteMask))));
	vbl.AddTail(CSNMPVarBind(CASNAny(CASNAny::typeASNOID,ipRouteIfIndex,sizeof(ipRouteIfIndex))));
	vbl.AddTail(CSNMPVarBind(CASNAny(CASNAny::typeASNOID,ipRouteMetric1,sizeof(ipRouteMetric1))));
CSNMPExtDLL snmp("INETMIB1");
	while(nRoutes<(sizeof(routes)/sizeof(routes[0]))){
		if(
			snmp.Request(CASNAny::typeASNGetNextRequest,vbl,vbl)
			&& vbl.GetCount() == 4
		){
		POSITION p = vbl.GetHeadPosition();
		_route r = {-1,-1};
		in_addr d, m;
		BOOL bD = FALSE, bM = FALSE;
			while(p){
			CSNMPVarBind& vb = vbl.GetNext(p);
				if(
					vb.IsName(ipRouteDest,sizeof(ipRouteDest))
					&& vb.value.type==CASNAny::typeASNIP
				){
					d.s_addr=vb.value.value.ip.s_addr; bD = TRUE;
				}else if(
					vb.IsName(ipRouteMask,sizeof(ipRouteMask))
					&& vb.value.type==CASNAny::typeASNIP
				){
					m.s_addr=vb.value.value.ip.s_addr; bM = TRUE;
				}else if(
					vb.IsName(ipRouteIfIndex,sizeof(ipRouteIfIndex))
					&& vb.value.type==CASNAny::typeASNInteger
				){
					r.iface=vb.value.value.number;
				}else if(
					vb.IsName(ipRouteMetric1,sizeof(ipRouteMetric1))
					&& vb.value.type==CASNAny::typeASNInteger
				){
					r.metric=vb.value.value.number;
				}else
					break;
			}
			if(r.iface<0 || r.metric<0 || (!bD) || (!bM))
				break;
			if((target.s_addr&m.s_addr)==(d.s_addr&m.s_addr)){
				r.nm=htonl(m.s_addr);
				memmove(&routes[nRoutes++],&r,sizeof(routes[0]));
			}
		}else
			break;
	}
	if(!nRoutes)
		return FALSE;
int rn = 0;
	if(nRoutes>1){
		for(int tmp=1;tmp<nRoutes;tmp++)
			if(
				routes[tmp].metric<routes[rn].metric
				|| routes[tmp].nm>routes[rn].nm
			)
				rn = tmp;
	}
int iface = routes[rn].iface;
	vbl.RemoveAll();
	vbl.AddTail(CSNMPVarBind(CASNAny(CASNAny::typeASNOID,ipAdEntAddr,sizeof(ipAdEntAddr))));
	vbl.AddTail(CSNMPVarBind(CASNAny(CASNAny::typeASNOID,ipAdEntIfIndex,sizeof(ipAdEntIfIndex))));
	for(;;){
		if(
			snmp.Request(CASNAny::typeASNGetNextRequest,vbl,vbl)
			&& vbl.GetCount()==2
		){
		in_addr a; a.s_addr = INADDR_NONE;
		int ifn = -1;
		POSITION p = vbl.GetHeadPosition();
			while(p){
			CSNMPVarBind& vb = vbl.GetNext(p);
				if(
					vb.IsName(ipAdEntAddr,sizeof(ipAdEntAddr))
					&& vb.value.type==CASNAny::typeASNIP
				){
					a.s_addr=vb.value.value.ip.s_addr;
				}else if(
					vb.IsName(ipAdEntIfIndex,sizeof(ipAdEntIfIndex))
					&& vb.value.type==CASNAny::typeASNInteger
				){
					ifn = vb.value.value.number;
				}else
					break;
			}
			if(ifn<0)
				break;
			if(ifn==iface){
				source.s_addr=a.s_addr;
				return TRUE;
			}
		}else
			break;
	}
	return FALSE;
}

};

#endif	// __FINDIFACE_H
