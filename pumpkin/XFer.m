
#import "XFer.h"
#import "TFTPPacket.h"
#import "StringsAttached.h"

static void cbXfer(CFSocketRef sockie,CFSocketCallBackType cbt,CFDataRef cba,
		       const void *cbd,void *i) {
    [(XFer*)i callbackWithType:cbt addr:cba data:cbd];
}

@implementation XFer
@synthesize initialPacket;
@synthesize xferFilename;
@synthesize localFile;
@synthesize xferPrefix;

- (id) init {
    if(!(self = [super init])) return self;
    blockSize = 512;
    sockie = NULL;
    theFile = nil;
    acked = 0;
    xferSize = 0; xferBlocks = 0;
    xferType = nil; xferFilename = nil;
    state = xferStateNone;
    pumpkin = NSApplication.sharedApplication.delegate;
    queue = [[NSMutableArray alloc]initWithCapacity:4];
    localFile = nil;
    retryTimeout = 3;
    giveupTimeout = [[[[NSUserDefaultsController sharedUserDefaultsController] values] valueForKey:@"giveUpTimeout"] intValue];
    lastPacket = nil; retryTimer = nil;
    giveupTimer = nil;
    initialPacket = nil;
    return self;
    
}

- (id) initWithPeer:(struct sockaddr_in *)sin andPacket:(TFTPPacket*)p {
    if(!(self=[self init])) return self;
    memmove(&peer,sin,sizeof(peer));
    initialPacket = [p retain];
    return self;
}

- (struct sockaddr_in*)peer { return &peer; }

- (BOOL) makeLocalFileName:(NSString *)xf {
    NSString *fn = [xf stringByReplacingOccurrencesOfString:@"\\" withString:@"/"];
    if([fn hasPrefix:@"../"] || [fn hasSuffix:@"/.."] || [fn rangeOfString:@"/../"].location!=NSNotFound) {
	[self queuePacket:[TFTPPacket packetErrorWithCode:tftpErrAccessViolation andMessage:@"bad path"]];
	return NO;
    }
    localFile = [[[pumpkin.theDefaults.values valueForKey:@"tftpRoot"] stringByAppendingPathComponent:fn] retain];
    return YES;
}

- (void) retryTimeout {
    [self queuePacket:lastPacket]; [lastPacket release]; lastPacket = nil;
}
- (void) giveUp {
    [pumpkin log:@"Connection timeout for '%@'",xferFilename];
    [self abort];
}
- (void) renewHope {
    if(giveupTimer) {
	[giveupTimer invalidate]; [giveupTimer release];
    }
    giveupTimer = [[NSTimer scheduledTimerWithTimeInterval:giveupTimeout target:self selector:@selector(giveUp) userInfo:nil repeats:NO] retain];
}

- (void) callbackWithType:(CFSocketCallBackType)t addr:(CFDataRef)a data:(const void *)d {
    if(!giveupTimer) [self renewHope];
    if(retryTimer) {
	[retryTimer release]; [retryTimer invalidate]; retryTimer = nil;
    }
    switch (t) {
	case kCFSocketWriteCallBack:
	    if(queue.count) {
		TFTPPacket *p = queue[0];
		CFSocketError r = CFSocketSendData(sockie, (CFDataRef)[NSData dataWithBytesNoCopy:&peer length:sizeof(peer) freeWhenDone:NO], (CFDataRef)[NSData dataWithData:p.data], 0);
		if(r!=kCFSocketSuccess)
		    [pumpkin log:@"Failed to send data, error %d",errno];
		if(!(p.op==tftpOpDATA || p.op==tftpOpERROR)) {
		    if(lastPacket) [lastPacket release];
		    lastPacket = [p retain];
		    if(retryTimer) {
			[retryTimer invalidate]; [retryTimer release];
		    }
		    retryTimer = [[NSTimer scheduledTimerWithTimeInterval:retryTimeout target:self selector:@selector(retryTimeout) userInfo:nil repeats:NO] retain];
		}else{
		    [lastPacket release]; lastPacket = nil;
		}
		[queue removeObjectAtIndex:0];
		if([queue count] || state==xferStateShutdown)
		    CFSocketEnableCallBacks(sockie, kCFSocketWriteCallBack);
	    }else if(state==xferStateShutdown) {
		[pumpkin log:@"%@ Transfer of '%@' finished.",xferPrefix,xferFilename];
		[self disappear];
	    }
	    break;
	case kCFSocketDataCallBack:
	    [self renewHope];
	    [self eatTFTPPacket:[TFTPPacket packetWithData:(NSData*)d] from:(struct sockaddr_in*)CFDataGetBytePtr(a)];
	    break;
	default:
	    NSLog(@"unhandled %lu callback",t);
	    break;
    }
}

- (BOOL) createSocket {
    CFSocketContext ctx;
    ctx.version=0; ctx.info=self; ctx.retain=0; ctx.release=0; ctx.copyDescription=0;
    sockie = CFSocketCreate(kCFAllocatorDefault, PF_INET, SOCK_DGRAM, IPPROTO_UDP,
			    kCFSocketReadCallBack|kCFSocketWriteCallBack|kCFSocketDataCallBack,
			    cbXfer, &ctx);
    if(!sockie) return NO;
    struct sockaddr_in a; memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    if(CFSocketSetAddress(sockie, (CFDataRef)[NSData dataWithBytesNoCopy:&a length:sizeof(a) freeWhenDone:NO])
       !=kCFSocketSuccess) {
	[pumpkin log:@"failed to set socket address"];
	return NO;
    }
    runloopSource = CFSocketCreateRunLoopSource(kCFAllocatorDefault, sockie, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runloopSource, kCFRunLoopDefaultMode);
    return YES;
}

- (void) queuePacket:(TFTPPacket*)p {
    [queue addObject:p];
    CFSocketEnableCallBacks(sockie, kCFSocketWriteCallBack|kCFSocketReadCallBack);
    if(p.op==tftpOpERROR) state = xferStateShutdown;
}

- (void) goOnWithVerdict:(int)verdict {
    NSAssert(false,@"unimplemented goOnWithVerdict");
}

- (void) eatTFTPPacket:(TFTPPacket*)p from:(struct sockaddr_in*)sin {
    NSAssert(false,@"unimplemented eatTFTPPacket");
}
-(void) abort {
    [self queuePacket:[TFTPPacket packetErrorWithCode:tftpErrUndefined andMessage:@"transfer cancelled"]];
}

- (id) cellValueForColumn:(NSString*)ci {
    if([ci isEqualToString:@"fileName"]) {
	return [NSString stringWithFormat:@"%@ %@",xferPrefix,xferFilename];
    }else if([ci isEqualToString:@"xferType"]) {
	return xferType;
    }else if([ci isEqualToString:@"peerAddress"]) {
	switch (state) {
	    case xferStateConnecting: return [NSString stringWithHostAddress:&peer];
	    default: return [NSString stringWithSocketAddress:&peer];
	}
    }else if([ci isEqualToString:@"ackBytes"]) {
	return [NSString stringWithFormat:@"%u",acked*blockSize];
    }else if([ci isEqualToString:@"xferSize"]) {
	return xferSize?[NSString stringWithFormat:@"%llu",xferSize]:nil;
    }
    return nil;
}

- (void) updateView {
    [pumpkin updateXfers];
}
- (void) appear {
    [pumpkin registerXfer:self];
}
- (void) disappear {
    if(retryTimer) {
	[retryTimer invalidate]; [retryTimer release]; retryTimer = nil;
    }
    if(giveupTimer) {
	[giveupTimer invalidate]; [giveupTimer release]; retryTimer = nil;
    }
    [pumpkin unregisterXfer:self];
}

- (BOOL) isPeer:(struct sockaddr_in*)sin {
    return sin->sin_len==peer.sin_len && !memcmp(sin,&peer,sin->sin_len);
}

-(void)dealloc {
    if(runloopSource) {
 	CFRunLoopSourceInvalidate(runloopSource);
	CFRelease(runloopSource);
    }
    if(sockie) {
	CFSocketInvalidate(sockie);
	CFRelease(sockie);
    }
    [queue release];
    if(theFile) [theFile release];
    if(xferFilename) [xferFilename release];
    if(xferType) [xferType release];
    if(lastPacket) [lastPacket release];
    if(initialPacket) [initialPacket release];
    if(localFile) [localFile release];
    [super dealloc];
}


@end
