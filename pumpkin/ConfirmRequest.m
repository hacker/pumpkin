
#import "ConfirmRequest.h"
#import "StringsAttached.h"

@implementation ConfirmRequest
@synthesize remoteHost;
@synthesize remoteAction;
@synthesize fileName;
@synthesize fileExists;
@synthesize isWriteRequest;

-(void)sentence:(int)v {
    [timeout invalidate], [timeout release], timeout=nil;
    [xfer goOnWithVerdict:v];
    [self.window performClose:nil];
    [[[NSUserDefaultsController sharedUserDefaultsController] values]
     setValue:@(v) forKey:isWriteRequest?@"WRQ.lastSentence":@"RRQ.lastSentence"];
    [self release];
}

- (IBAction)letItBe:(id)sender { [self sentence:verdictAllow]; }
- (IBAction)deny:(id)sender { [self sentence:verdictDeny]; }
- (IBAction)rename:(id)sender { [self sentence:verdictRename]; }
- (void)timeout { [self sentence:verdictDefault]; }

- (ConfirmRequest*) initWithXfer:(XFer *)x {
    enum TFTPOp op = x.initialPacket.op;
    NSAssert(op==tftpOpRRQ || op==tftpOpWRQ,@"Invalid request to confirm");
    if(!(self=[super initWithWindowNibName:@"ConfirmRequest"])) return self;
    isWriteRequest = op==tftpOpWRQ;
    remoteHost = [[NSString stringWithHostAddress:x.peer] retain];
    remoteAction = isWriteRequest?@"tries to send you":@"requests the file";
    fileName = x.xferFilename;
    fileExists = [[NSFileManager defaultManager] fileExistsAtPath:x.localFile];
    xfer = [x retain];
    switch([[[[NSUserDefaultsController sharedUserDefaultsController] values] valueForKey:isWriteRequest?@"WRQ.lastSentence":@"RRQ.lastSentence"] intValue]) {
	case verdictAllow: self.window.initialFirstResponder = self.allowButton; break;
	case verdictDeny: self.window.initialFirstResponder = self.denyButton; break;
	case verdictRename: self.window.initialFirstResponder = self.renameButton; break;
    }
    [self.window makeKeyAndOrderFront:nil];
    timeout = [[NSTimer scheduledTimerWithTimeInterval:[[[[NSUserDefaultsController sharedUserDefaultsController] values] valueForKey:@"confirmationTimeout"] intValue] target:self selector:@selector(timeout) userInfo:nil repeats:NO] retain];
    return self;
}

- (void) dealloc {
    if(timeout) [timeout invalidate], [timeout release];
    if(remoteHost) [remoteHost release];
    if(xfer) [xfer release];
    [super dealloc];
}

+ (void) confirmationWithXfer:(XFer *)x {
    [[self alloc] initWithXfer:x];
}

@end
