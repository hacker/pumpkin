#import "DaemonListener.h"
#import "TFTPPacket.h"
#import "SendXFer.h"
#import "ReceiveXFer.h"
#import "StringsAttached.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>

static void cbListener(CFSocketRef sockie,CFSocketCallBackType cbt,CFDataRef cba,
					   const void *cbd,void *i) {
    [(DaemonListener*)i callbackWithType:cbt addr:cba data:cbd];
}

@implementation DaemonListener

-(void)callbackWithType:(CFSocketCallBackType)t addr:(CFDataRef)a data:(const void *)d {
    switch(t) {
	case kCFSocketDataCallBack:
	{
	    struct sockaddr_in *sin = (struct sockaddr_in*)CFDataGetBytePtr(a);
	    if([pumpkin hasPeer:sin]) {
		[pumpkin log:@"I'm already processing the request from %@",[NSString stringWithSocketAddress:sin]];
		return;
	    }
	    TFTPPacket *p = [TFTPPacket packetWithData:(NSData*)d];
	    switch([p op]) {
		case tftpOpRRQ: [[[SendXFer alloc] initWithPeer:sin andPacket:p] autorelease]; break;
		case tftpOpWRQ: [[[ReceiveXFer alloc] initWithPeer:sin andPacket:p] autorelease]; break;
		default:
		    [pumpkin log:@"Invalid OP %d received from %@",p.op,[NSString stringWithSocketAddress:sin]];
		    break;
	    }
	}
	    break;
	default:
	    NSLog(@"unhandled callback: %lu",t);
	    break;
    }
}


-(DaemonListener*)initWithAddress:(struct sockaddr_in*)sin {
    if(!(self=[super init])) return self;
    
    pumpkin = NSApplication.sharedApplication.delegate;

    @try {
	CFSocketContext ctx;
	ctx.version = 0;
	ctx.info = self;
	ctx.retain = 0; ctx.release = 0;
	ctx.copyDescription = 0;
	sockie = CFSocketCreate(kCFAllocatorDefault,PF_INET,SOCK_DGRAM,IPPROTO_UDP,
				kCFSocketReadCallBack|kCFSocketDataCallBack,
				cbListener,&ctx);
	if(ntohs(sin->sin_port)>1024) {
	    NSData *nsd = [NSData dataWithBytes:sin length:sizeof(*sin)];
	    if(CFSocketSetAddress(sockie, (CFDataRef)nsd))
		[[NSException exceptionWithName:@"BindFailure"
					 reason:[NSString stringWithFormat:@"Binding failed, error code: %d", errno]
				       userInfo:@{@"errno": @errno}
		  ] raise];
	}else{
	    const char *args[] = {
		0,
		[[NSString stringWithFormat:@"%d", CFSocketGetNative(sockie)] UTF8String],
		[[NSString stringWithHostAddress:sin] UTF8String],
		[[NSString stringWithPortNumber:sin] UTF8String],
		NULL
	    };
	    [pumpkin runBiportal:args];
	}
    }@catch(NSException *e) {
	if(sockie) {
	    CFSocketInvalidate(sockie);
	    CFRelease(sockie);
	}
	@throw;
    }

    runloopSource = CFSocketCreateRunLoopSource(kCFAllocatorDefault, sockie, 0);
    CFRunLoopAddSource(CFRunLoopGetCurrent(),runloopSource, kCFRunLoopDefaultMode);
    return self;
}

-(void)dealloc {
    if(runloopSource) {
	CFRunLoopRemoveSource(CFRunLoopGetCurrent(), runloopSource, kCFRunLoopDefaultMode);
	CFRelease(runloopSource);
    }
    if(sockie) {
	CFSocketInvalidate(sockie);
	CFRelease(sockie);
    }
    [super dealloc];
}

+(DaemonListener*) listenerWithDefaults {
    struct sockaddr_in sin;
    memset(&sin,0,sizeof(sin));
    sin.sin_len=sizeof(sin);
    sin.sin_family=AF_INET;
    id d = [[NSUserDefaultsController sharedUserDefaultsController] values];
    sin.sin_port=htons([[d valueForKey:@"bindPort"] intValue]);
    sin.sin_addr.s_addr=inet_addr([[d valueForKey:@"bindAddress"] UTF8String]);
    return [[[DaemonListener alloc] initWithAddress:&sin] autorelease];
}

@end
