#import "SendXFer.h"
#import "StringsAttached.h"
#import "ConfirmRequest.h"

@implementation SendXFer

-(SendXFer*)initWithLocalFile:(NSString *)lf peerAddress:(const struct sockaddr_in *)pa remoteFile:(NSString *)rf xferType:(NSString *)xt blockSize:(uint16_t)bs andTimeout:(int)to {
    if(!(self = [super init])) return self;
    xferPrefix = @"⬆";
    retryTimeout = to;
    localFile = lf;
    memmove(&peer,pa,sizeof(peer));
    if(!(theFile = [[NSFileHandle fileHandleForReadingAtPath:localFile] retain])) {
	[pumpkin log:@"Failed to open '%@', transfer aborted.",localFile];
	return self;
    }
    
    long xb = ((xferSize=[theFile seekToEndOfFile])/blockSize)+1;
    if(xb > UINT16_MAX) {
	[pumpkin log:@"file is too big (%lld bytes) and will take %ld blocks to be sent with block size of %d bytes",xferSize,xb,blockSize];
	return self;
    }
    xferBlocks = xb;
    
    [self createSocket];
    NSMutableDictionary *o = [NSMutableDictionary dictionaryWithCapacity:4];
    [o setValue:[NSString stringWithFormat:@"%u",bs] forKey:@"blksize"];
    [o setValue:[NSString stringWithFormat:@"%llu",xferSize] forKey:@"tsize"];
    [o setValue:[NSString stringWithFormat:@"%d",(int)retryTimeout] forKey:@"timeout"];
    state = xferStateConnecting;
    [self queuePacket:[TFTPPacket packetWRQWithFile:xferFilename=rf xferType:xferType=xt andOptions:o]];
    [self appear];
    return self;
}

-(SendXFer*)initWithPeer:(struct sockaddr_in *)sin andPacket:(TFTPPacket*)p {
    if(!(self = [super initWithPeer:sin andPacket:p])) return self;
    xferPrefix = @"⬆";
    xferFilename = [p.rqFilename retain]; xferType = [p.rqType retain];
    [pumpkin log:@"'%@' of type '%@' is requested from %@",
		xferFilename, xferType, [NSString stringWithSocketAddress:&peer] ];

    [self createSocket];
    [self appear];

    if(![self makeLocalFileName:xferFilename])
	return self;

    switch([[pumpkin.theDefaults.values valueForKey:@"rrqBehavior"] intValue]) {
	case onRRQDeny: [self goOnWithVerdict:verdictDeny]; break;
	case onRRQGive: [self goOnWithVerdict:verdictAllow]; break;
	default: 
	    [ConfirmRequest confirmationWithXfer:self];
	    break;
    }
    return self;
}
-(void)goOnWithVerdict:(int)verdict {
    if(verdict!=verdictAllow) {
	[self queuePacket:[TFTPPacket packetErrorWithCode:tftpErrAccessViolation andMessage:@"Access denied"]];
	return;
    }
    if(!(theFile = [[NSFileHandle fileHandleForReadingAtPath:localFile] retain])) {
	[self queuePacket:[TFTPPacket packetErrorWithErrno:errno andFallback:@"couldn't open file"]];
	return;
    }
    xferSize = [theFile seekToEndOfFile];
    NSMutableDictionary *o = [NSMutableDictionary dictionaryWithCapacity:4];
    [[initialPacket rqOptions] enumerateKeysAndObjectsUsingBlock:^(NSString* k, NSString* v, BOOL *stop) {
	if([k isEqualToString:@"blksize"]) {
	    [o setValue:[NSString stringWithFormat:@"%u",blockSize=v.intValue] forKey:@"blksize"];
	}else if([k isEqualToString:@"tsize"]) {
	    [o setValue:[NSString stringWithFormat:@"%lld",xferSize] forKey:@"tsize"];
	}else if([k isEqualToString:@"timeout"]) {
	    [o setValue:[NSString stringWithFormat:@"%d",v.intValue] forKey:@"timeout"];
	    retryTimeout = v.intValue;
	}else
	    [pumpkin log:@"Unknown option '%@' with value '%@'. Ignoring.",k,v];
    }];
    long xb = (xferSize/blockSize)+1;
    if(xb > UINT16_MAX) {
	[self queuePacket:[TFTPPacket packetErrorWithCode:tftpErrUndefined andMessage:[NSString stringWithFormat:@"file is too big (%lld bytes) and will take %ld blocks to be sent with block size of %d bytes",xferSize,xb,blockSize]]];
	return;
    }
    xferBlocks = xb;
    state = xferStateXfer;
    if(o.count) {
	[self queuePacket:[TFTPPacket packetOACKWithOptions:o]];
    }else{
	[self xfer];
    }
}

- (void) xfer {
    NSAssert(theFile,@"no file!");
    [theFile seekToFileOffset:acked*blockSize];
    [self queuePacket:[TFTPPacket packetDataWithBlock:acked+1 andData:[theFile readDataOfLength:blockSize]]];
}

- (void) eatTFTPPacket:(TFTPPacket*)p from:(struct sockaddr_in*)sin{
    if(state==xferStateConnecting) {
	peer.sin_port = sin->sin_port;
	[self updateView];
    }else if(![self isPeer:sin]) {
	[pumpkin log:@"Packet from unexpected source (%@) recevied",[NSString stringWithSocketAddress:sin]];
	return;
    }
    switch(p.op) {
	case tftpOpACK:
	    if(state==xferStateShutdown || ( (acked=p.block)==xferBlocks && (state=xferStateShutdown) ) ) {
		CFSocketEnableCallBacks(sockie, kCFSocketWriteCallBack);
		return;
	    }
	    [self updateView];
	    [self xfer];
	    break;
	case tftpOpERROR:
	    [pumpkin log:@"Error %u:%@",p.rqCode, p.rqMessage];
	    [self updateView];
	    [self disappear];
	    return;
	case tftpOpOACK:
	    if(acked) {
		[pumpkin log:@"It's a bit too late to acknowledge options, ignoring OACK packet"];
		break;
	    }
	{
	    __block BOOL a=NO;
	    [p.rqOptions enumerateKeysAndObjectsUsingBlock:^(NSString *k,NSString *v,BOOL *s) {
		if([k isEqualToString:@"blksize"])
		    blockSize = v.intValue;
		else if([k isEqualToString:@"tsize"]) {
		}else if([k isEqualToString:@"timeout"])
		    retryTimeout = v.intValue;
		else{
		    [pumpkin log:@"Totally unknown option '%@' with value '%@' acknowledged by peer",k,v];
		    a=YES;
		}
	    }];
	    if(a) {
		[self abort];
		break;
	    }
	    state = xferStateXfer;
	    [self updateView];
	    [self xfer];
	}
	    break;
	default:
	    [pumpkin log:@"Totaly unexpected opcode %d received",p.op];
	    break;
    }
}


@end
