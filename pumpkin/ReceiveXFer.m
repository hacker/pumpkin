#import "ReceiveXFer.h"
#import "StringsAttached.h"
#import "ConfirmRequest.h"

@implementation ReceiveXFer

- (ReceiveXFer*)initWithLocalFile:(NSString *)lf peerAddress:(const struct sockaddr_in *)pa remoteFile:(NSString *)rf xferType :(NSString *)xt blockSize:(uint16_t)bs andTimeout:(int)to{
    if(!(self = [super init])) return self;
    xferPrefix = @"⬇";
    retryTimeout = to;
    localFile = lf;
    memmove(&peer,pa,sizeof(peer));
    [[NSFileManager defaultManager] createFileAtPath:localFile contents:nil attributes:nil];
    if(!(theFile = [[NSFileHandle fileHandleForWritingAtPath:localFile] retain])) {
	[pumpkin log:@"Failed to create '%@', transfer aborted.", localFile];
	return self;
    }
    [self createSocket];
    NSMutableDictionary *o = [NSMutableDictionary dictionaryWithCapacity:4];
    [o setValue:[NSString stringWithFormat:@"%u",bs] forKey:@"blksize"];
    [o setValue:@"" forKey:@"tsize"];
    [o setValue:[NSString stringWithFormat:@"%d",(int)retryTimeout] forKey:@"timeout"];
    state = xferStateConnecting;
    [self queuePacket:[TFTPPacket packetRRQWithFile:xferFilename=rf xferType:xferType=xt andOptions:o]];
    [self appear];
    return self;
}

- (ReceiveXFer*)initWithPeer:(struct sockaddr_in *)sin andPacket:(TFTPPacket *)p {
    if(!(self = [super initWithPeer:sin andPacket:p])) return self;
    xferPrefix = @"⬇";
    xferFilename=[p.rqFilename retain]; xferType=[p.rqType retain];
    [pumpkin log:@"'%@' of type '%@' is coming from %@", xferFilename, xferType, [NSString stringWithSocketAddress:&peer]];
    
    [self createSocket];
    [self appear];
    
    if(![self makeLocalFileName:xferFilename])
	return self;

    switch([[pumpkin.theDefaults.values valueForKey:@"wrqBehavior"] intValue]) {
	case onWRQDeny: [self goOnWithVerdict:verdictDeny]; break;
	case onWRQTake: [self goOnWithVerdict:verdictAllow]; break;
	case onWRQPromptIfExists:
	    if(![[NSFileManager defaultManager] fileExistsAtPath:localFile]) {
		[self goOnWithVerdict:verdictAllow];
		break;
	    }
	case onWRQPrompt:
	    [ConfirmRequest confirmationWithXfer:self];
	    break;
    }
    return self;
}
-(void)goOnWithVerdict:(int)verdict {
    if(!(verdict==verdictAllow || verdict==verdictRename)) {
	[self queuePacket:[TFTPPacket packetErrorWithCode:tftpErrAccessViolation andMessage:@"Access denied"]];
	return;
    }
    NSFileManager *fm = [NSFileManager defaultManager];
    if(verdict==verdictRename) {
	int i;
	for(i=1;i>0;++i) {
	    if(![self makeLocalFileName:[NSString stringWithFormat:@"%@ (%d)",xferFilename,i]])
		return;
	    if(![fm fileExistsAtPath:localFile]) break;
	    [localFile release],localFile=nil;
	}
	if(!localFile) {
	    [self queuePacket:[TFTPPacket packetErrorWithCode:tftpErrFileExists andMessage:@"Couldn't find a name for a file"]];
	    return;
	}
    }
    [pumpkin log:@"Receiving '%@'",localFile];
    [fm createFileAtPath:localFile contents:nil attributes:nil];
    if(!(theFile = [[NSFileHandle fileHandleForWritingAtPath:localFile] retain])) {
	[self queuePacket:[TFTPPacket packetErrorWithErrno:errno andFallback:@"couldn't write to file"]];
	return;
    }
    xferSize=0;
    NSMutableDictionary *o = [NSMutableDictionary dictionaryWithCapacity:4];
    [initialPacket.rqOptions enumerateKeysAndObjectsUsingBlock:^(NSString* k,NSString *v,BOOL *s) {
	if([k isEqualToString:@"blksize"]) {
	    [o setValue:[NSString stringWithFormat:@"%u",blockSize=v.intValue] forKey:@"blksize"];
	}else if([k isEqualToString:@"tsize"]) {
	    [o setValue:[NSString stringWithFormat:@"%lld",xferSize=v.longLongValue] forKey:@"tsize"];
	}else if([k isEqualToString:@"timeout"]) {
	    [o setValue:[NSString stringWithFormat:@"%d",v.intValue] forKey:@"timeout"];
	    retryTimeout = v.intValue;
	}else
	    [pumpkin log:@"Unknown option '%@' with value '%@'. Ignoring.",k,v];
    }];
    if(xferSize) {
	long xb = (xferSize/blockSize)+1;
	if(xb>UINT16_MAX) {
	    [self queuePacket:[TFTPPacket packetErrorWithCode:tftpErrUndefined andMessage:[NSString stringWithFormat:@"file seems to be too big (%lld bytes) and would take %ld blocks to be transferred with the block size of %d bytes", xferSize, xb,blockSize]] ];
	    return;
	}
	xferBlocks = xb;
    }
    state = xferStateXfer;
    if([o count]) {
	[self queuePacket:[TFTPPacket packetOACKWithOptions:o]];
    }else{
	[self queuePacket:[TFTPPacket packetACKWithBlock:acked=0]];
    }
    [self updateView];
}

-(void)eatTFTPPacket:(TFTPPacket *)p from:(struct sockaddr_in *)sin {
    if(state==xferStateConnecting) {
	peer.sin_port = sin->sin_port;
	[self updateView];
    }else if(![self isPeer:sin]) {
	[pumpkin log:@"Packet from unexpected source (%@) received",[NSString stringWithSocketAddress:sin]];
	return;
    }
    switch(p.op) {
	case tftpOpDATA:
	{
	    NSData *d=p.rqData;;
	    @try {
		if(p.block > (acked+1))
		    [pumpkin log:@"While transferring %@ block %d seems to immediately follow block %d",xferFilename,p.block,acked];
		[theFile seekToFileOffset:(p.block-1)*blockSize];
		[theFile writeData:d];
		[theFile truncateFileAtOffset:(p.block-1)*blockSize+d.length];
	    }@catch (NSException *e) {
		[self queuePacket:[TFTPPacket packetErrorWithCode:tftpErrUndefined andMessage:e.reason]];
		break;
	    }
	    [self queuePacket:[TFTPPacket packetACKWithBlock: acked=p.block]];
	    [self updateView];
	    if(d.length<blockSize)
		state = xferStateShutdown;
	}
	    break;
	case tftpOpOACK:
	{
	    __block BOOL a=NO;
	    [p.rqOptions enumerateKeysAndObjectsUsingBlock:^(NSString *k,NSString *v,BOOL *s) {
		if([k isEqualToString:@"blksize"])
		    blockSize = v.intValue;
		else if([k isEqualToString:@"tsize"])
		    xferSize = v.longLongValue;
		else if([k isEqualToString:@"timeout"])
		    retryTimeout = v.intValue;
		else{
		    [pumpkin log:@"Totally unknown option %@ acknowledged by remote.",k];
		    a=YES;
		}
	    }];
	    if(a) {
		[self abort];
		break;
	    }
	    [self queuePacket:[TFTPPacket packetACKWithBlock:0]];
	    state = xferStateXfer;
	    [self updateView];
	}
	    break;
	default:
	    [pumpkin log:@"Totaly unexpected opcode %d received",p.op];
	    break;
    }
}

@end
