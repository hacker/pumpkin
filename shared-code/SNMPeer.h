#ifndef	__SNMPEER_H
#define __SNMPEER_H

namespace Klever {

class	CASNAny	{
public:
	enum	{
		asnCls = 0xC0,
			asnClsUniversal = 0x00,
			asnClsApplication = 0x40,
			asnClsContextSpecific = 0x80,
			asnClsPrivate = 0xC0,
		asnConstructed = 0x20,
		asnPrimitive = 0x00,
		asnTag = 0x1F,
			// ASN.1 Primitive Tags
			asnTagInteger = 0x02,
			asnTagOctetString = 0x04,
			asnTagNull = 0x05,
			asnTagOID = 0x06,
			// ASN.1 Constructed Tags
			asnTagSequence = 0x10,
			// RFC1155 Primitive Tags
			asnTagIP = 0x00,		
			asnTagCounter = 0x01,
			asnTagGauge = 0x02,
			asnTagTicks = 0x03,
			asnTagOpaque = 0x04,
			// RFC1213 alias
			asnTagDispString = 0x04,	// (ASN.1 Octet string)
			// RFC1157 Constructed Tags
			asnTagGetRequest = 0x00,
			asnTagGetNextRequest = 0x01,
			asnTagGetResponse = 0x02,
			asnTagSetRequest = 0x03,
			asnTagTrap = 0x04
	};
	enum	{
		typeASNInteger = (asnClsUniversal|asnPrimitive|asnTagInteger),
		typeASNOctetString = (asnClsUniversal|asnPrimitive|asnTagOctetString),
		typeASNNull = (asnClsUniversal|asnPrimitive|asnTagNull),
		typeASNOID = (asnClsUniversal|asnPrimitive|asnTagOID),

		typeASNSequence = (asnClsUniversal|asnConstructed|asnTagSequence),
		typeASNSequenceOf = (asnClsUniversal|asnConstructed|asnTagSequence),
		
		typeASNIP = (asnClsApplication|asnPrimitive|asnTagIP),
		typeASNCounter = (asnClsApplication|asnPrimitive|asnTagCounter),
		typeASNGauge = (asnClsApplication|asnPrimitive|asnTagGauge),
		typeASNTicks = (asnClsApplication|asnPrimitive|asnTagTicks),
		typeASNOpaque = (asnClsApplication|asnPrimitive|asnTagOpaque),
		typeASNDispString = (asnClsUniversal|asnPrimitive|asnTagOctetString),

		typeASNGetRequest = (asnClsContextSpecific|asnConstructed|asnTagGetRequest),
		typeASNGetNextRequest = (asnClsContextSpecific|asnConstructed|asnTagGetNextRequest),
		typeASNGetResponse = (asnClsContextSpecific|asnConstructed|asnTagGetResponse),
		typeASNSetRequest = (asnClsContextSpecific|asnConstructed|asnTagSetRequest),
		typeASNTrap = (asnClsContextSpecific|asnConstructed|asnTagTrap)
	};

	typedef	LONG asnInteger;
	typedef LARGE_INTEGER asnInteger64;
	typedef	DWORD asnCounter;
	typedef ULARGE_INTEGER asnCounter64;
	typedef DWORD asnGauge;
	typedef ULARGE_INTEGER asnGauge64;
	typedef	DWORD asnTicks;
	typedef ULARGE_INTEGER asnTicks64;
	struct	asnDynamic	{
		UINT size;
		LPBYTE data;
		BOOL Allocate(UINT size) {
		BOOL rv = Free();
			if(size)
				rv=rv&&(data=new BYTE[size]);
			if(rv)
				asnDynamic::size=size;
			return rv;
		}
		BOOL Set(LPBYTE data,UINT size) {
		BOOL rv = Allocate(size);
			if(rv && size)
				memmove(asnDynamic::data,data,size);
			return rv;
		}
		BOOL Free() {
			if(!size)
				return TRUE;
			delete data;
			size=0;
			data=0;
			return TRUE;
		}
		void Clean() {
			size=0;
			data=0;
		}
		BOOL Copy(asnDynamic& src) {
		BOOL rv = Free();
			if(rv){
				if(src.size)
					rv=rv&&(data = new BYTE[src.size]);
				if(rv){
					if(size=src.size)
						memmove(data,src.data,size);
				}
			}
			return rv;
		}
	};
	typedef	asnDynamic asnOctetString;
	typedef asnDynamic asnOID;
	typedef in_addr asnIP;
	typedef asnDynamic asnSequence;

	BYTE type;
	enum _storeType	{
		storeDynamic,
		storeStatic
	}	storeType;
	union	{
		asnInteger number;
		asnInteger64 number64;
		asnOctetString string;
		asnOID oid;
		asnSequence sequence;
		asnIP ip;
		asnCounter counter;
		asnCounter64 counter64;
		asnGauge gauge;
		asnGauge64 gauge64;
		asnTicks ticks;
		asnTicks64 ticks64;
		asnDynamic data;
	} value;

	CASNAny() : type(typeASNNull), storeType(storeStatic) { value.data.Clean(); }
	CASNAny(CASNAny& src) : type(typeASNNull), storeType(storeStatic) { value.data.Clean();Copy(src); }
	CASNAny(BYTE type) : type(type), storeType(storeStatic) { value.data.Clean(); }
	CASNAny(BYTE type,LONG number) : type(typeASNNull), storeType(storeStatic) { value.data.Clean();Set(type,number); }
	CASNAny(BYTE type,LONGLONG number) : type(typeASNNull), storeType(storeStatic) { value.data.Clean();Set(type,number); }
	CASNAny(BYTE type,LPCTSTR string) : type(typeASNNull), storeType(storeStatic) { value.data.Clean();Set(type,string); }
	CASNAny(BYTE type,LPBYTE data,UINT length) : type(typeASNNull), storeType(storeStatic) { value.data.Clean();Set(type,data,length); }
	CASNAny(BYTE type,UINT* data,UINT size) : type(typeASNNull), storeType(storeStatic) { value.data.Clean();Set(type,(LPBYTE)data,size); }
	CASNAny(in_addr& ip) : type(typeASNNull), storeType(storeStatic) { value.data.Clean();Set(ip); }
	~CASNAny() { Free(); }

	BOOL Set(BYTE type) {
	BOOL rv = Free();
		CASNAny::type=type;
		return rv;
	}
	BOOL Set(BYTE type,LONG number) {
	BOOL rv = Free();
		CASNAny::type=type;
		value.number=number;
		storeType=storeStatic;
		return rv;
	}
	BOOL Set(BYTE type,LONGLONG number) {
	BOOL rv = Free();
		CASNAny::type=type;
		value.number64.QuadPart = number;
		storeType=storeStatic;
		return rv;
	}
	BOOL Set(BYTE type,LPCTSTR string) {
	BOOL rv = Free();
		CASNAny::type=type;
		rv=rv&&value.string.Set((LPBYTE)string,strlen(string)+1);
		if(rv){
			value.string.size--;
			storeType=storeDynamic;
		}
		return rv;
	}
	BOOL Set(BYTE type,LPBYTE data,UINT length) {
	BOOL rv = Free();
		CASNAny::type=type;
		rv=rv&&value.data.Set(data,length);
		if(rv)
			storeType=storeDynamic;
		return rv;
	}
	BOOL Set(in_addr& ip) {
	BOOL rv = Free();
		memmove(&value.ip,&ip,sizeof(value.ip));
		type=typeASNIP;
		storeType=storeStatic;
		return rv;
	}
	BOOL Free() {
		if(storeType==storeDynamic)
			value.data.Free();
		else{
			memset(&value,0,sizeof(value));
			value.data.Clean();
		}
		storeType=storeStatic;
		type=typeASNNull;
		return TRUE;
	}
	BOOL Copy(CASNAny& src) {
	BOOL rv = Free();
		if(src.storeType==storeDynamic){
			rv=rv&&value.data.Copy(src.value.data);
			if(rv){
				type=src.type;
				storeType=src.storeType;
			}
		}else{
			memmove(this,&src,sizeof(*this));
		}
		return rv;
	}
	CASNAny& operator=(CASNAny& src) {
		VERIFY(Copy(src));
		return *this;
	}

	// High Level
	CString GetString() {
		ASSERT(storeType==storeDynamic);
	CString rv;
	LPTSTR b = rv.GetBuffer(value.data.size+1);
		ASSERT(b);
		b[value.data.size]=0;
		memmove(b,value.data.data,value.data.size);
		rv.ReleaseBuffer();
		return rv;
	}
};


class	CSNMPVarBind	{
public:
	CASNAny name;
	CASNAny value;

	CSNMPVarBind() {}
	CSNMPVarBind(CASNAny& name,CASNAny& value) : name(name), value(value) {}
	CSNMPVarBind(CASNAny& name) : name(name) {}
	CSNMPVarBind(CSNMPVarBind& src) { Copy(src); }
	BOOL Copy(CSNMPVarBind& src) { 
		name.Copy(src.name);
		value.Copy(src.value);
		return TRUE;
	}
	CSNMPVarBind& operator=(CSNMPVarBind& src) {
		Copy(src);
		return *this;
	}
	// High level
	BOOL IsName(UINT* prefix,UINT prefixSize,BOOL bExact=FALSE) {
		if(name.type!=CASNAny::typeASNOID)
			return FALSE;
		if(name.value.oid.size<prefixSize)
			return FALSE;
		if(bExact && (name.value.oid.size!=prefixSize))
			return FALSE;
		return !memcmp(prefix,name.value.oid.data,prefixSize);
	}
};

class CSNMPVarBindList	: public CList<CSNMPVarBind,CSNMPVarBind&> {
public:
	CSNMPVarBind* GetVarBind(UINT* prefix,UINT prefixSize,BOOL bExact=FALSE) {
	POSITION p = GetHeadPosition();
		while(p){
		CSNMPVarBind& vb = GetNext(p);
			if(vb.IsName(prefix,prefixSize,bExact))
				return &vb;
		}
		return NULL;
	}
};

class	CSNMPeer	{
public:
	virtual BOOL Request(BYTE type,CSNMPVarBindList& in,CSNMPVarBindList& ou) = 0;
};

};

#endif	// __SNMPEER_H
