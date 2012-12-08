
#import "TFTPPacket.h"

@interface NSDictionary (TFTPOptions)

- (size_t)tftpBytesLength;
- (size_t)tftpGetBytes:(char*)p maxLength:(size_t)ml;

@end
@implementation NSDictionary (TFTPOptions)

- (size_t)tftpBytesLength {
    __block size_t rv = 0;
    [self enumerateKeysAndObjectsUsingBlock:^(id k,id v,BOOL *s) {
	rv += [k lengthOfBytesUsingEncoding:NSUTF8StringEncoding]+[v lengthOfBytesUsingEncoding:NSUTF8StringEncoding]+2;
    }];
    return rv;
}

- (size_t)tftpGetBytes:(char*)p maxLength:(size_t)ml {
    __block char *_p = p;
    __block size_t rl = ml;
    __block size_t rv = 0;
    [self enumerateKeysAndObjectsUsingBlock:^(NSString *k,NSString *v,BOOL *s) {
	NSUInteger l;
	[k getBytes:_p maxLength:rl usedLength:&l encoding:NSUTF8StringEncoding options:0 range:NSMakeRange(0,k.length) remainingRange:NULL];
	_p+=l; *_p++=0; rl-=l+1; rv+=l+1;
	[v getBytes:_p maxLength:rl usedLength:&l encoding:NSUTF8StringEncoding options:0 range:NSMakeRange(0,v.length) remainingRange:NULL];
	_p+=l; *_p++=0; rl-=l+1; rv+=l+1;
    }];
    return rv;
}

@end

@implementation TFTPPacket
@synthesize data;

-(BOOL) isRQOp {
    return self.op==tftpOpRRQ || self.op==tftpOpWRQ;
}
-(BOOL) isOptionsOp {
    return self.isRQOp || self.op==tftpOpOACK;
}
-(BOOL) isBlockOp {
    return self.op==tftpOpDATA || self.op==tftpOpACK;
}

-(enum TFTPOp)op {
    NSAssert(data.length,@"no data");
    return (enum TFTPOp)ntohs(packet->op);
}
-(NSString*)rqFilename {
    NSAssert( self.isRQOp, @"Wrong TFTP opcode for rq filename retrieval");
    if(!memchr(packet->rq.data, 0, [data length]-sizeof(packet->op))) return nil;
    return @(packet->rq.data);
}
-(NSString*)rqType {
    NSAssert( self.isRQOp, @"Wrong TFTP opcode for rq type retrieval");
    const char *z = (const char*)memchr(packet->rq.data,0, data.length-sizeof(packet->op));
    if(!z) return nil;
    if(!memchr(z+1,0,data.length-sizeof(packet->op)-(z-packet->rq.data))) return nil;
    return @(z+1);
}
-(NSDictionary*)rqOptions {
    enum TFTPOp op = self.op;
    NSAssert( self.isOptionsOp, @"Wrong TFTP opcode for options retrieval");
    const char *p = packet->any.data, *p1 = (const char*)packet + data.length;
    if(op==tftpOpRRQ || op==tftpOpWRQ) {
	p = (const char *)memchr(p,0,p1-p);
	if(!p) return nil;
	p = (const char *)memchr(p+1,0,p1-p);
	if(!p) return nil;
	++p;
    }
    NSMutableDictionary *rv = [NSMutableDictionary dictionaryWithCapacity:8];
    while(p<p1) {
	const char *on = p;
	p = (const char *)memchr(p,0,p1-p);
	if(!p) break;
	const char *ov = ++p;
	p = (const char *)memchr(p,0,p1-p);
	if(!p) break;
	++p;
	rv[[@(on) lowercaseString]] = @(ov);
    }
    return rv;
}
-(uint16_t)block {
    NSAssert( self.isBlockOp, @"Wrong TFTP opcode for block number retrieval");
    return ntohs(*(uint16_t*)&packet->data);
}
-(NSData*)rqData {
    NSAssert( self.op==tftpOpDATA, @"Can't get data from the request that doesn't have it");
    return [NSData dataWithBytes:packet->data.data length:data.length-sizeof(packet->op)-sizeof(packet->data.block)];
}
-(uint16_t)rqCode {
    NSAssert(self.op==tftpOpERROR,@"Wrong TFTP opcode for error code retrieval");
    return ntohs(packet->err.code);
}
-(NSString*)rqMessage {
    NSAssert(self.op==tftpOpERROR,@"Wrong TFTP opcode for error message retrieval");
    return @(packet->err.data);
}

-(TFTPPacket*)initWithData:(NSData *)d {
    if(!(self = [super init])) return self;
    packet = (struct AnyTFTPPacket*)(data = [d retain]).bytes;
    return self;
}


+(TFTPPacket*)packetWithData:(NSData*)d {
    return [[[self alloc] initWithData:d] autorelease];
}
+(TFTPPacket*)packetWithBytesNoCopy:(void*)b andLength:(size_t)l {
    return [[[self alloc] initWithData:[NSData dataWithBytesNoCopy:b length:l]] autorelease];
}

+(TFTPPacket*)packetErrorWithCode:(enum TFTPError)c andMessage:(NSString*)m {
    NSUInteger ml = [m lengthOfBytesUsingEncoding:NSUTF8StringEncoding], bb;
    struct AnyTFTPPacket *b = (struct AnyTFTPPacket*)malloc(bb = sizeof(b->op)+sizeof(b->err.code)+ml+1);
    if(!b) return nil;
    b->op = htons(tftpOpERROR);
    b->err.code = ntohs(c);
    [m getBytes:b->err.data maxLength:ml usedLength:NULL encoding:NSUTF8StringEncoding options:0 range:NSMakeRange(0,m.length) remainingRange:NULL];
    b->err.data[ml]=0;
    return [self packetWithBytesNoCopy:b andLength:bb];
}
+(TFTPPacket*)packetErrorWithErrno:(int)en andFallback:(NSString *)fb{
    switch(en) {
	case EACCES:
	    return [self packetErrorWithCode:tftpErrAccessViolation andMessage:@"acess violation"];
	case ENOENT:
	    return [self packetErrorWithCode:tftpErrNotFound andMessage:@"not found"];
    }
    return [self packetErrorWithCode:tftpErrUndefined andMessage:fb];
}

+(TFTPPacket*)packetXRQWithOp:(enum TFTPOp)op file:(NSString*)f xferType:(NSString*)t andOptions:(NSDictionary*)o {
    NSAssert(f && t && o,@"Something is amiss in packetXRQWithOp");
    __block size_t dl = o.tftpBytesLength
	+[f lengthOfBytesUsingEncoding:NSUTF8StringEncoding]
	+[t lengthOfBytesUsingEncoding:NSUTF8StringEncoding]
	+2;
    size_t pl = dl;
    struct AnyTFTPPacket *b = (struct AnyTFTPPacket*)malloc(pl+=sizeof(b->op));
    if(!b) return nil;
    b->op = htons(op);
    __block char *p = b->rrq.data;
    NSUInteger l;
    [f getBytes:p maxLength:dl usedLength:&l encoding:NSUTF8StringEncoding options:0 range:NSMakeRange(0,f.length) remainingRange:NULL];
    p+=l; *p++=0; dl-=l+1;
    [t getBytes:p maxLength:dl usedLength:&l encoding:NSUTF8StringEncoding options:0 range:NSMakeRange(0,t.length) remainingRange:NULL];
    p+=l; *p++=0; dl-=l+1;
    l = [o tftpGetBytes:p maxLength:dl];
    p+=l; dl-=l;
    NSAssert1(dl==0,@"packet of the wrong size, remaining count: %lu",dl);
    return [self packetWithBytesNoCopy:b andLength:pl];    
}

+(TFTPPacket*)packetRRQWithFile:(NSString *)f xferType:(NSString *)t andOptions:(NSDictionary *)o {
    return [self packetXRQWithOp:tftpOpRRQ file:f xferType:t andOptions:o];
}
+(TFTPPacket*)packetWRQWithFile:(NSString *)f xferType:(NSString *)t andOptions:(NSDictionary *)o {
    return [self packetXRQWithOp:tftpOpWRQ file:f xferType:t andOptions:o];
}

+(TFTPPacket*)packetOACKWithOptions:(NSDictionary*)o {
    __block NSUInteger pl = [o tftpBytesLength];
    __block NSUInteger rc = pl;
    __block struct AnyTFTPPacket *b = (struct AnyTFTPPacket*)malloc(pl+=sizeof(b->op));
    if(!b) return nil;
    b->op = htons(tftpOpOACK);
    __block char *p = b->oack.data;
    rc -= [o tftpGetBytes:p maxLength:pl];
    NSAssert1(rc==0,@"packet of the wrong size, remaining count: %lu",rc);
    return [self packetWithBytesNoCopy:b andLength:pl];
}
+(TFTPPacket*)packetDataWithBlock:(uint16_t)b andData:(NSData*)d {
    NSUInteger pl;
    struct AnyTFTPPacket *p = (struct AnyTFTPPacket*)malloc(pl=sizeof(p->op)+sizeof(p->data.block)+d.length);
    if(!p) return nil;
    p->op = htons(tftpOpDATA);
    p->data.block = htons(b);
    [d getBytes:p->data.data length:d.length];
    return [self packetWithBytesNoCopy:p andLength:pl];
}
+(TFTPPacket*)packetACKWithBlock:(uint16_t)b {
    NSUInteger pl;
    struct AnyTFTPPacket *p = (struct AnyTFTPPacket*)malloc(pl=sizeof(p->op)+sizeof(p->ack.block));
    if(!p) return nil;
    p->op = htons(tftpOpACK);
    p->ack.block = htons(b);
    return [self packetWithBytesNoCopy:p andLength:pl];
}

-(void)dealloc {
    [data release];
    [super dealloc];
}

@end
