
#import "pumpkin.h"
#import "ARequest.h"
#import "ReceiveXFer.h"
#import "SendXFer.h"

static void cbHost(CFHostRef h,CFHostInfoType hi,const CFStreamError *e,void *i) {
    [(ARequest*)i hostCallbackWithHost:h info:hi andError:e];
}


@implementation ARequest
@synthesize requestIsGet;
@synthesize doTouchMe;
@synthesize statusLabel;
@synthesize errorLabel;

@synthesize localFile;
@synthesize remoteHost;
@synthesize remotePort;
@synthesize remoteFile;
@synthesize xferType;
@synthesize blockSize;
@synthesize timeout;

@synthesize remoteHostBox;

-(void)unhost {
    if(!cfhost) return;
    CFHostCancelInfoResolution(cfhost, kCFHostAddresses);
    CFHostUnscheduleFromRunLoop(cfhost, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    CFRelease(cfhost);
    cfhost = nil;
}
-(void)loadDefaults {
    id d = [NSUserDefaultsController.sharedUserDefaultsController values];
    self.remotePort = [d valueForKey:@"remotePort"];
    self.blockSize = [d valueForKey:@"blockSize"];
    self.xferType = [d valueForKey:@"xferType"];
    self.remoteHost = [d valueForKey:@"remoteHost"];
    self.timeout = [d valueForKey:@"timeout"];
    
    self.localFile = [[d valueForKey:@"tftpRoot"] stringByAppendingString:@"/"];
}
-(void)saveDefaults {
    NSUserDefaultsController *dc = [NSUserDefaultsController sharedUserDefaultsController];
    id d = dc.values;
    [d setValue:self.remotePort forKey:@"remotePort"];
    [d setValue:self.remoteHost forKey:@"remoteHost"];
    [d setValue:self.blockSize forKey:@"blockSize"];
    [d setValue:self.xferType forKey:@"xferType"];
    [d setValue:self.timeout forKey:@"timeout"];
    [dc save:self];
}


- (IBAction)startXfer:(id)sender {
    if(!(cfhost = CFHostCreateWithName(kCFAllocatorDefault, (CFStringRef)remoteHost))) {
	self.errorLabel = @"failed to even try to resolve.";
	return;
    }
    struct CFHostClientContext hc;
    hc.version=0; hc.info=self; hc.retain=0;hc.release=0;
    hc.copyDescription=0;
    CFHostSetClient(cfhost, cbHost, &hc);
    CFHostScheduleWithRunLoop(cfhost, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    CFStreamError e;
    if(!CFHostStartInfoResolution(cfhost, kCFHostAddresses, &e)) {
	self.errorLabel = @"failed to start host resolution.";
	[self unhost];
	return;
    }
    self.statusLabel = @"resolving remote host…";
    self.doTouchMe = NO;
}

-(void)hostCallbackWithHost:(CFHostRef)h info:(CFHostInfoType)hi andError:(const CFStreamError *)e {
    NSString *el = nil;
    CFArrayRef aa = nil;
    __block struct sockaddr_in peer;
    do {
	if(e && (e->domain || e->error)) {
	    el=@"failed to resolve remote address"; break;
	}
	Boolean hbr;
	aa = CFHostGetAddressing(cfhost, &hbr);
	if(!(hbr && aa && CFArrayGetCount(aa))) {
	    el=@"failed to find remote address"; break;
	}
	peer.sin_addr.s_addr=INADDR_NONE; [(NSArray*)aa enumerateObjectsUsingBlock:^(NSData *o,NSUInteger i,BOOL *s) {
	    const struct sockaddr_in *sin = o.bytes;
	    if(sin->sin_family!=AF_INET) return;
	    memmove(&peer,sin,sizeof(peer));
	    *s = YES;
	}];
	if(peer.sin_addr.s_addr==INADDR_NONE) {
	    el=@"found no ipv4 address"; break;
	}
	peer.sin_port = htons([remotePort unsignedIntValue]);
    }while(false);
    [self unhost];
    if(el) {
	self.errorLabel = el; self.doTouchMe = YES; return;
    }
    [self saveDefaults];
    [[[requestIsGet?ReceiveXFer.class:SendXFer.class alloc]
      initWithLocalFile:localFile peerAddress:&peer remoteFile:remoteFile xferType:xferType blockSize:blockSize.unsignedIntValue andTimeout:timeout.intValue]
     autorelease];
    [self.window performClose:nil];
}

- (IBAction)pickFile:(id)sender {
    NSSavePanel *p = nil;
    if(requestIsGet) {
	p = [NSSavePanel savePanel];
	p.canCreateDirectories = YES;
    }else{
	NSOpenPanel *pp = [NSOpenPanel openPanel];
	pp.canChooseDirectories = NO;
	pp.canChooseFiles = YES;
	pp.allowsMultipleSelection = NO;
	p = pp;
    }
    p.prompt = @"Pick the local file";
    if([p runModal]!=NSFileHandlingPanelOKButton) return;
    self.localFile = p.URL.path;
}

- (ARequest*) initWithGet:(BOOL)gr {
    if(!(self = [super initWithWindowNibName:@"ARequest"])) return self;
    self.doTouchMe = YES;
    cfhost = nil;
    requestIsGet = gr;
    if(requestIsGet) {
	self.window.title = @"Get file from remote TFTP server";
	self.window.initialFirstResponder = remoteHostBox;
    }else{
	self.window.title = @"Put file to remote TFTP server";
    }
    [self loadDefaults];
    [self addObserver:self forKeyPath:@"localFile" options:0 context:0];
    [self addObserver:self forKeyPath:@"remoteFile" options:0 context:0];
    return [self retain];
}
-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
    if(requestIsGet) {
	if([keyPath isEqualToString:@"remoteFile"]) {
	    if(self.remoteFile.length) {
		self.localFile= [([self.localFile hasSuffix:@"/"]
				   ?self.localFile
				   :[self.localFile stringByDeletingLastPathComponent])
				  stringByAppendingPathComponent:self.remoteFile.lastPathComponent];
	    }else
		self.localFile=[[self.localFile stringByDeletingLastPathComponent] stringByAppendingString:@"/"];
	}
    }else{
	if([keyPath isEqualToString:@"localFile"]) {
	    self.remoteFile=[self.localFile hasSuffix:@"/"]
		?@"":self.localFile.lastPathComponent;
	}
    }
}

+ (ARequest*) aRequestWithGet:(BOOL)gr {
    return [[[ARequest alloc] initWithGet:gr] autorelease];
}

static void popMeUp(BOOL g) {
    [[ARequest aRequestWithGet:g].window makeKeyAndOrderFront:nil];
}
+ (void)getFile { popMeUp(YES); }
+ (void)putFile { popMeUp(NO); }

- (void)windowDidLoad {
}

- (void)windowWillClose:(NSNotification*)n {
    [self unhost];
    [self removeObserver:self forKeyPath:@"localFile" context:0];
    [self removeObserver:self forKeyPath:@"remoteFile" context:0];
    [self release];
}

@end
