
#import <Cocoa/Cocoa.h>
#include <stdint.h>

enum TFTPOp {
	tftpOpRRQ=1, tftpOpWRQ=2,
	tftpOpDATA=3,
	tftpOpACK=4,
	tftpOpERROR=5,
	tftpOpOACK=6
};

enum TFTPError {
	tftpErrUndefined=0,
	tftpErrNotFound=1,
	tftpErrAccessViolation=2,
	tftpErrDiskFull=3,
	tftpErrIllegalOp=4,
	tftpErrUnknownTID=5,
	tftpErrFileExists=6,
	tftpErrNoUser=7,
	tftpErrOption=8
};

#pragma pack(push,1)
struct AnyTFTPPacket {
    uint16_t op;
    union {
	struct {
	    char data[1];
	} any;
	struct {
	    char data[1];
	} rq;
	struct {
	    char data[1];
	} rrq;
	struct {
	    char data[1];
	} wrq;
	struct {
	    uint16_t block;
	    char data[1];
	} data;
	struct {
	    uint16_t block;
	} ack;
	struct {
	    uint16_t code;
	    char data[1];
	} err;
	struct {
	    char data[1];
	} oack;
    };
};
#pragma pack(pop)

@interface TFTPPacket : NSObject {
    NSData *data;
    struct AnyTFTPPacket *packet;
}

@property (readonly) enum TFTPOp op;
@property (readonly) NSString* rqFilename;
@property (readonly) NSString* rqType;
@property (readonly) NSDictionary* rqOptions;
@property (readonly) NSData *data;
@property (readonly) uint16_t block;
@property (readonly) NSData *rqData;
@property (readonly) uint16_t rqCode;
@property (readonly) NSString* rqMessage;

-(TFTPPacket*)initWithData:(NSData*)d;

+(TFTPPacket*)packetWithData:(NSData*)d;
+(TFTPPacket*)packetWithBytesNoCopy:(void*)b andLength:(size_t)l;

+(TFTPPacket*)packetErrorWithCode:(enum TFTPError)c andMessage:(NSString*)m;
+(TFTPPacket*)packetErrorWithErrno:(int)en andFallback:(NSString*)fb;
+(TFTPPacket*)packetOACKWithOptions:(NSDictionary*)o;
+(TFTPPacket*)packetDataWithBlock:(uint16_t)b andData:(NSData*)d;
+(TFTPPacket*)packetACKWithBlock:(uint16_t)b;
+(TFTPPacket*)packetRRQWithFile:(NSString*)f xferType:(NSString*)t andOptions:(NSDictionary*)o;
+(TFTPPacket*)packetWRQWithFile:(NSString*)f xferType:(NSString*)t andOptions:(NSDictionary*)o;

@end
