/*------------------------------------------------------------------
* Filename: MS_ICMP.H
*
* Description: Prototypes of Microsoft's ICMP.DLL functions for
* access to Internet Control Message Protocol (their stacks do
* not support the standard Berkeley Sockets raw socket API).
* Use this to do "ping" or "traceroute," although beware that
* Microsoft discourages its use.
*/


/* Note 2: For the most part, you can refer to RFC 791 for detials on
* how to fill in values for the IP option information structure. */
typedef struct ip_option_information {
	u_char Ttl; /* Time To Live (used for traceroute) */
	u_char Tos; /* Type Of Service (usually 0) */
	u_char Flags; /* IP header flags (usually 0) */
	u_char OptionsSize; /* Size of options data (usually 0, max 40) */
	u_char FAR *OptionsData;/* Options data buffer */
}	IPINFO, *PIPINFO, FAR *LPIPINFO;

/* Note 1: The Reply Buffer will have an array of ICMP_ECHO_REPLY
* structures, followed by options and the data in ICMP echo reply
* datagram received. You must have room for at least one ICMP
* echo reply structure, plus 8 bytes for an ICMP header. */
typedef struct icmp_echo_reply {
	u_long Address; /* source address */
	u_long Status; /* IP status value (see below) */
	u_long RTTime; /* Round Trip Time in milliseconds */
	u_short DataSize; /* reply data size */
	u_short Reserved; /* */
	void FAR *Data; /* reply data buffer */
	typedef struct ip_option_information Options; /* reply options */
}	ICMPECHO, *PICMPECHO, FAR *LPICMPECHO;

#define IP_STATUS_BASE 11000
#define IP_SUCCESS 0
#define IP_BUF_TOO_SMALL (IP_STATUS_BASE + 1)
#define IP_DEST_NET_UNREACHABLE (IP_STATUS_BASE + 2)
#define IP_DEST_HOST_UNREACHABLE (IP_STATUS_BASE + 3)
#define IP_DEST_PROT_UNREACHABLE (IP_STATUS_BASE + 4)
#define IP_DEST_PORT_UNREACHABLE (IP_STATUS_BASE + 5)
#define IP_NO_RESOURCES (IP_STATUS_BASE + 6)
#define IP_BAD_OPTION (IP_STATUS_BASE + 7)
#define IP_HW_ERROR (IP_STATUS_BASE + 8)
#define IP_PACKET_TOO_BIG (IP_STATUS_BASE + 9)
#define IP_REQ_TIMED_OUT (IP_STATUS_BASE + 10)
#define IP_BAD_REQ (IP_STATUS_BASE + 11)
#define IP_BAD_ROUTE (IP_STATUS_BASE + 12)
#define IP_TTL_EXPIRED_TRANSIT (IP_STATUS_BASE + 13)
#define IP_TTL_EXPIRED_REASSEM (IP_STATUS_BASE + 14)
#define IP_PARAM_PROBLEM (IP_STATUS_BASE + 15)
#define IP_SOURCE_QUENCH (IP_STATUS_BASE + 16)
#define IP_OPTION_TOO_BIG (IP_STATUS_BASE + 17)
#define IP_BAD_DESTINATION (IP_STATUS_BASE + 18)
#define IP_ADDR_DELETED (IP_STATUS_BASE + 19)
#define IP_SPEC_MTU_CHANGE (IP_STATUS_BASE + 20)
#define IP_MTU_CHANGE (IP_STATUS_BASE + 21)
#define IP_UNLOAD (IP_STATUS_BASE + 22)
#define IP_GENERAL_FAILURE (IP_STATUS_BASE + 50)
#define MAX_IP_STATUS IP_GENERAL_FAILURE
#define IP_PENDING (IP_STATUS_BASE + 255)


HANDLE WINAPI PASCAL IcmpCreateFile(VOID); /* INVALID_HANDLE_VALUE on error */
BOOL WINAPI PASCAL IcmpCloseHandle(HANDLE IcmpHandle); /* FALSE on error */
DWORD WINAPI PASCAL IcmpSendEcho(
	HANDLE IcmpHandle, /* handle returned from IcmpCreateFile() */
	u_long DestAddress, /* destination IP address (in network order) */
	LPVOID RequestData, /* pointer to buffer to send */
	WORD RequestSize, /* length of data in buffer */
	LPIPINFO RequestOptns, /* see Note 2 below */
	LPVOID ReplyBuffer, /* see Note 1 below */
	DWORD ReplySize, /* length of reply (must allow at least 1 reply) */
	DWORD Timeout /* time in milliseconds to wait for reply */
);

