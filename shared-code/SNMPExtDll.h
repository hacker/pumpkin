#ifndef	__SNMPEXTDLL_H
#define __SNMPEXTDLL_H

#include "snmpeer.h"

#include <snmp.h>

namespace Klever {

class CSNMPExtDLL	: public CSNMPeer	{
public:
	HINSTANCE m_hInstance;
	HANDLE m_hEvent;
	AsnObjectIdentifier m_OID;
	BOOL (SNMP_FUNC_TYPE *m_extInit)(DWORD dw,HANDLE h,AsnObjectIdentifier* aoid);
	BOOL (SNMP_FUNC_TYPE *m_extQuery)(BYTE b,RFC1157VarBindList* rvbl,AsnInteger* ai1,AsnInteger* ai2);
	BOOL (SNMP_FUNC_TYPE *m_extTrap)(AsnObjectIdentifier*,AsnNetworkAddress*,AsnInteger*,AsnInteger*,AsnTimeticks*,RFC1157VarBindList*);

	HINSTANCE m_hSNMPAPI;
	void (SNMP_FUNC_TYPE *m_snmpOIDFree)(AsnObjectIdentifier*);
	LPVOID (SNMP_FUNC_TYPE *m_snmpAlloc)(UINT);
	void (SNMP_FUNC_TYPE *m_snmpFree)(LPVOID);
	void (SNMP_FUNC_TYPE *m_snmpVBLFree)(RFC1157VarBindList* vbl);
	void InitSNMP() {
		m_hSNMPAPI = ::LoadLibraryEx("SNMPAPI",NULL,0);
		if(!m_hSNMPAPI)
			return;
		*(FARPROC*)&m_snmpOIDFree = ::GetProcAddress(m_hSNMPAPI,"SnmpUtilOidFree");
		*(FARPROC*)&m_snmpAlloc = ::GetProcAddress(m_hSNMPAPI,"SnmpUtilMemAlloc");
		*(FARPROC*)&m_snmpFree = ::GetProcAddress(m_hSNMPAPI,"SnmpUtilMemFree");
		*(FARPROC*)&m_snmpVBLFree = ::GetProcAddress(m_hSNMPAPI,"SnmpUtilVarBindListFree");
		if(
			(m_snmpFree && !m_snmpAlloc) ||
			(m_snmpAlloc && !m_snmpFree)
		)
			DeinitSNMP();
	}
	void DeinitSNMP() {
		if(!m_hSNMPAPI)
			return;
		::FreeLibrary(m_hSNMPAPI);
		m_hSNMPAPI=NULL;
	}
	void SNMPFreeOID(AsnObjectIdentifier* oid) {
		if(m_hSNMPAPI && m_snmpOIDFree)
			(*m_snmpOIDFree)(oid);
		else
			::GlobalFree((HGLOBAL)oid->ids);
	}
	LPVOID SNMPAlloc(UINT size) {
		if(m_hSNMPAPI && m_snmpAlloc)
			return (*m_snmpAlloc)(size);
		else
			return ::GlobalAlloc(GMEM_FIXED,size);
	}
	void SNMPFree(LPVOID addr) {
		if(m_hSNMPAPI && m_snmpFree)
			(*m_snmpFree)(addr);
		else
			:: GlobalFree((HGLOBAL)addr);
	}
	void SNMPFreeVBL(RFC1157VarBindList& vbl) {
		if(m_hSNMPAPI && m_snmpVBLFree)
			(*m_snmpVBLFree)(&vbl);
		else{
			for(UINT tmp=0;tmp<vbl.len;tmp++) {
				SNMPFree(vbl.list[tmp].name.ids);
				switch(vbl.list[tmp].value.asnType){
				case ASN_OCTETSTRING:
				case ASN_SEQUENCE:
				case ASN_RFC1155_IPADDRESS:
				case ASN_RFC1155_OPAQUE:
//				case ASN_RFC1213_DISPSTRING:
					if(vbl.list[tmp].value.asnValue.arbitrary.dynamic)
						SNMPFree(vbl.list[tmp].value.asnValue.arbitrary.stream);
					break;
				case ASN_OBJECTIDENTIFIER:
					SNMPFree(vbl.list[tmp].value.asnValue.object.ids);
					break;
				default:
					break;
				}
			}
			SNMPFree(vbl.list);
		}
	}

	BOOL SNMPBuildVBL(RFC1157VarBindList& vbl,CSNMPVarBindList& in) {
		vbl.len = in.GetCount();
		vbl.list = (RFC1157VarBind*)SNMPAlloc(sizeof(RFC1157VarBind)*vbl.len);
	POSITION p = in.GetHeadPosition();
	UINT ptr = 0;
		while(p){
		CSNMPVarBind& vb = in.GetNext(p);
			ASSERT(ptr<vbl.len);
			SNMPBuildVB(vbl.list[ptr++],vb);
		}
		return TRUE;
	}
	BOOL SNMPBuildVB(RFC1157VarBind& vb,CSNMPVarBind& in) {
		ASSERT(in.name.type==CASNAny::typeASNOID);
		return SNMPBuildOID(vb.name,in.name.value.oid) && SNMPBuildAA(vb.value,in.value);
	}
	BOOL SNMPBuildAA(AsnAny& aa,CASNAny& in) {
		aa.asnType=in.type;
		switch(in.type) {
		case CASNAny::typeASNInteger:
			aa.asnValue.number=in.value.number; break;
		case CASNAny::typeASNOctetString:
//		case CASNAny::typeASNDispString:
			SNMPBuildOS(aa.asnValue.string,in.value.string); break;
		case CASNAny::typeASNNull:
			break;
		case CASNAny::typeASNOID:
			SNMPBuildOID(aa.asnValue.object,in.value.oid); break;
		case CASNAny::typeASNSequence:
//		case CASNAny::typeASNSequenceOf:
			SNMPBuildOS(aa.asnValue.sequence,in.value.sequence); break;
		case CASNAny::typeASNIP:
			SNMPBuildFLOS(aa.asnValue.address,(LPBYTE)&in.value.ip,sizeof(in.value.ip)); break;
		case CASNAny::typeASNCounter:
			aa.asnValue.counter = in.value.counter; break;
		case CASNAny::typeASNGauge:
			aa.asnValue.gauge = in.value.gauge; break;
		case CASNAny::typeASNTicks:
			aa.asnValue.ticks = in.value.ticks; break;
		case CASNAny::typeASNOpaque:
			ASSERT(in.storeType==CASNAny::storeDynamic);
			SNMPBuildOS(aa.asnValue.arbitrary,in.value.data); break;
			break;
		default:
			ASSERT(FALSE);
			return FALSE;
		}
		return TRUE;
	}
	BOOL SNMPBuildOS(AsnOctetString& os,CASNAny::asnOctetString& in) {
		return SNMPBuildFLOS(os,in.data,in.size);
	}
	BOOL SNMPBuildOID(AsnObjectIdentifier& oid,CASNAny::asnOID& in) {
		oid.idLength = in.size/sizeof(UINT);
		ASSERT(!(in.size%sizeof(UINT)));
		VERIFY(oid.ids = (UINT*)SNMPAlloc(in.size));
		memmove(oid.ids,in.data,in.size);
		return TRUE;
	}
	BOOL SNMPBuildFLOS(AsnOctetString& os,LPBYTE data,UINT size) {
		os.length = size;
		VERIFY(os.stream = (BYTE*)SNMPAlloc(os.length));
		os.dynamic=TRUE;
		memmove(os.stream,data,size);
		return TRUE;
	}
	BOOL SNMPParseVBL(RFC1157VarBindList& vbl,CSNMPVarBindList& ou) {
		for(UINT tmp=0;tmp<vbl.len;tmp++){
		CSNMPVarBind vb;
			SNMPParseVB(vbl.list[tmp],vb);
			ou.AddTail(vb);
		}
		return TRUE;
	}
	BOOL SNMPParseVB(RFC1157VarBind& vb,CSNMPVarBind& ou) {
		ou.name.Set(CASNAny::typeASNOID,(LPBYTE)vb.name.ids,vb.name.idLength*sizeof(UINT));
		return SNMPParseAA(vb.value,ou.value);
	}
	BOOL SNMPParseAA(AsnAny& aa,CASNAny& ou) {
		switch(aa.asnType){
		case ASN_INTEGER:
			ou.Set(CASNAny::typeASNInteger,aa.asnValue.number); break;
		case ASN_OCTETSTRING:
//		case ASN_RFC1213_DISPSTRING:
			ou.Set(CASNAny::typeASNOctetString,aa.asnValue.string.stream,aa.asnValue.string.length); break;
		case ASN_OBJECTIDENTIFIER:
			ou.Set(CASNAny::typeASNOID,(LPBYTE)aa.asnValue.object.ids,aa.asnValue.object.idLength);
			SNMPParseOID(aa.asnValue.object,ou.value.oid); break;
		case ASN_SEQUENCE:
			ou.Set(CASNAny::typeASNSequence,aa.asnValue.sequence.stream,aa.asnValue.sequence.length); break;
		case ASN_RFC1155_IPADDRESS:
			SNMPParseIP(aa.asnValue.address,ou); break;
		case ASN_RFC1155_COUNTER:
			ou.Set(CASNAny::typeASNCounter,(LONG)aa.asnValue.counter); break;
		case ASN_RFC1155_GAUGE:
			ou.Set(CASNAny::typeASNGauge,(LONG)aa.asnValue.gauge); break;
		case ASN_RFC1155_TIMETICKS:
			ou.Set(CASNAny::typeASNTicks,(LONG)aa.asnValue.ticks); break;
		case ASN_RFC1155_OPAQUE:
			ou.Set(CASNAny::typeASNOpaque,aa.asnValue.arbitrary.stream,aa.asnValue.arbitrary.length); break;
		case ASN_NULL:
			ou.Free(); break;
		default:
			ASSERT(FALSE);
			return FALSE;
		}
		return TRUE;
	}
	BOOL SNMPParseOID(AsnObjectIdentifier& oid,CASNAny::asnOID& ou) {
		ASSERT(ou.size==(oid.idLength*sizeof(UINT)));
		memmove(ou.data,oid.ids,ou.size);
		return TRUE;
	}
	BOOL SNMPParseIP(AsnIPAddress& ip,CASNAny& ou) {
	in_addr i;
		if(ip.length>sizeof(i))
			return FALSE;
		i.s_addr=0;
		memmove(&i,ip.stream,ip.length);
		ou.Set(i);
		return TRUE;
	}


	CSNMPExtDLL(LPCTSTR dllName) : m_hInstance(NULL) { InitSNMP(); Init(dllName); }
	~CSNMPExtDLL() { Deinit(); DeinitSNMP(); }

	BOOL Init(LPCTSTR dllName) {
		Deinit();
		m_hInstance = ::LoadLibraryEx(dllName,NULL,0);
		if(!m_hInstance)
			return FALSE;
		*(FARPROC*)&m_extInit = ::GetProcAddress(m_hInstance,"SnmpExtensionInit");
		*(FARPROC*)&m_extQuery = ::GetProcAddress(m_hInstance,"SnmpExtensionQuery");
		*(FARPROC*)&m_extTrap = ::GetProcAddress(m_hInstance,"SnmpExtensionTrap");
		if(!(m_extInit && m_extQuery && m_extTrap)){
			Deinit();
			return FALSE;
		}
		if(!((*m_extInit)(GetCurrentTime(),&m_hEvent,&m_OID))){
			Deinit();
			return FALSE;
		}
		return TRUE;
	}
	void Deinit() {
		if(!m_hInstance)
			return;
		::FreeLibrary(m_hInstance);
	}
	virtual BOOL Request(BYTE type,CSNMPVarBindList& in,CSNMPVarBindList& ou) {
	RFC1157VarBindList vbl;
		SNMPBuildVBL(vbl,in);
	AsnInteger errorStatus, errorIndex;
		(*m_extQuery)(type,&vbl,&errorStatus,&errorIndex);
		ou.RemoveAll();
		SNMPParseVBL(vbl,ou);
		SNMPFreeVBL(vbl);
		return TRUE;
	}
};

};

#endif	// __SNMPEXTDLL_H
