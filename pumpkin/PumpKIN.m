#include <stdarg.h>
#include <sys/stat.h>

#import "PumpKIN.h"
#import "NumberTransformer.h"
#import "IPTransformer.h"
#import "XFer.h"
#import "XFersViewDatasource.h"
#import "ARequest.h"


@implementation PumpKIN
@synthesize toolbar;
@synthesize preferencesWindow;
@synthesize theDefaults;

@synthesize window;
@synthesize logger;
@synthesize xfersView;

-(void) updateListener {
    if(listener) {
	[listener release]; listener = nil;
    }
    if(![[theDefaults.values valueForKey:@"listen"] boolValue]) return;
    @try {
	listener = [[DaemonListener listenerWithDefaults] retain];
    }
    @catch (NSException *e) {
	[self log:@"Error starting the server. %@: %@",e.name,e.reason];
	NSAlert *a = [NSAlert alertWithMessageText:@"Failed to enable tftp server" defaultButton:@"OK" alternateButton:nil otherButton:nil informativeTextWithFormat:@"Failed to enable tftp server.\n%@\n\n%@",e.name,e.reason];
	a.alertStyle = NSWarningAlertStyle;
	enum act_type {
	    actDont = 0,
	    actBindToAny = NSAlertThirdButtonReturn+1,
	    actRemoveTFTPD, actChangePort
	};
	id en;
	if (e.userInfo && (en=(e.userInfo)[@"errno"])) {
	    switch([en intValue]) {
		case EADDRINUSE:
		    {
			int p = [[theDefaults.values valueForKey:@"bindPort"] intValue];
			if(p==69) {
			    a.informativeText = @"The OS reports that the address is already in use.\n\n"
				"It probably means, that some other programm is listening on the TFTP port."
				    " since Mac OS X comes with its own tftpd, it is the likeliest suspect."
				    " If you're going to use tftp server a lot, you may prefer to use that one."
				    " If you want to use PumpKIN, you can either use unprivileged port (but make sure"
				    " the client is aware of that and supports it) or unload Apple tftpd using"
				    " command 'launchctl remove com.apple.tftpd' (as root). I can try doing either for you.";
			    [a addButtonWithTitle:@"Change port to 6969"].tag = actChangePort;
			    [a addButtonWithTitle:@"Try to stop Apple's tftpd"].tag = actRemoveTFTPD;
			}else if(p!=6969) {
			    a.informativeText = @"The OS reports that the address is already in use.\n\n"
				"It probably means, that some other program is listening on the port."
				    " you can either try to find out who's using the port and shut it down or"
				    " change the port. Make sure to notify your peers of the change."
				    " I can help you with changing the port.";
			    [a addButtonWithTitle:@"Change port to 6969"].tag = actChangePort;
			}else {
			    a.informativeText = @"The OS reports that the address is already in use.\n\n"
				"It probably means that some other program is listening on the port."
				"You should either change port to the one that is not used or find the"
				    " offending program and shut it down. Or go on without server.";
			}
		    }
		    break;
		case EADDRNOTAVAIL:
		    a.informativeText = @"The OS reports that the address is not available.\n\n"
			"It probably means, that the IP address you specified is not configured on this machine.\n\n"
			"You can either ignore the error and go on without TFTP server, fix the ip address, by entering the one"
			    " that is configured, or bind to the special '0.0.0.0' ip address which means listen to any"
			    " address configured. The latter you can do automatically with a single click below.";
		    [a addButtonWithTitle:@"Listen to any address"].tag = actBindToAny;
		    [a addButtonWithTitle:@"Try removing Apple's daemon"].tag = actRemoveTFTPD;
		    break;
	    }
	};
	[theDefaults.values setValue:@NO forKey:@"listen"];
	switch([a runModal]) {
	    case actBindToAny:
		[NSTimer scheduledTimerWithTimeInterval:0 target:self selector:@selector(bindToAny) userInfo:nil repeats:NO];
		break;
	    case actChangePort:
		[NSTimer scheduledTimerWithTimeInterval:0 target:self selector:@selector(bindTo6969) userInfo:nil repeats:NO];
		break;
	    case actRemoveTFTPD:
		{
		    @try {
			char const *args[] = { 0,"-k",NULL };
			[self runBiportal:args];
		    }@catch(NSException *e) {
			[self log:@"Error trying to unload com.apple.tftpd. %@: %@",e.name,e.reason];
		    }
		}
		[NSTimer scheduledTimerWithTimeInterval:0 target:self selector:@selector(bindAgain) userInfo:nil repeats:NO];
		break;
	}
    }
}

-(void)bindAgain {
    [theDefaults.values setValue:@YES forKey:@"listen"];
}
-(void)bindTo6969 {
    [theDefaults.values setValue:@6969 forKey:@"bindPort"];
    [self bindAgain];
}
-(void)bindToAny {
    [theDefaults.values setValue:@"0.0.0.0" forKey:@"bindAddress"];
    [self bindAgain];
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context {
    if( object==theDefaults && (
	[keyPath isEqualToString:@"values.listen"]
	|| [keyPath isEqualToString:@"values.bindPort"]
	|| [keyPath isEqualToString:@"values.bindAddress"]) ) {
	[self updateListener];
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [theDefaults addObserver:self forKeyPath:@"values.listen" options:0 context:0];
    [theDefaults addObserver:self forKeyPath:@"values.bindAddress" options:0 context:0];
    [theDefaults addObserver:self forKeyPath:@"values.bindPort" options:0 context:0];

    listener = nil;
    [window.contentView setWantsLayer:true];
    window.backgroundColor = [NSColor colorWithPatternImage:[NSImage imageNamed:@"klever-background.png"]];
    xfersView.dataSource = (xvDatasource = [[XFersViewDatasource alloc] initWithXfers:xfers=[NSMutableArray arrayWithCapacity:4]]);
    [self updateListener];
    if(![[theDefaults values] valueForKey:@"tftpRoot"])
	[self showPreferences:nil];
}

- (IBAction)showPreferences:(id)sender {
    [preferencesWindow makeKeyAndOrderFront:preferencesWindow];
}

- (void)log:(NSString*)fmt,... {
    va_list vl; va_start(vl, fmt);
    NSString *s = [[[[NSString alloc] initWithFormat:fmt arguments:vl] autorelease] stringByAppendingString:@"\n"];
    va_end(vl);
    NSString *lf = [theDefaults.values valueForKey:@"logFile"];
    if(lf && ![lf isEqualTo:@""]) {
	NSFileHandle *l = [NSFileHandle fileHandleForWritingAtPath:lf];
	if(!l) {
	    [[NSFileManager defaultManager] createFileAtPath:lf contents:nil attributes:nil];
	    l = [NSFileHandle fileHandleForWritingAtPath:lf];
	}
	if(!l) {
	    static NSString *bl = nil;
	    if(!(bl && [bl isEqualToString:lf])) {
		[[logger textStorage] appendAttributedString:[[[NSAttributedString alloc] initWithString:
							   [NSString stringWithFormat:@"Failed to open/create '%@' log file\n",lf] ] autorelease]];
		if(bl) [bl release];
		bl = [NSString stringWithString:lf];
	    }
	}else{
	    [l seekToEndOfFile];
	    [l writeData:[[NSString stringWithFormat:@"[%@] %@",[[NSDate date] description],s] dataUsingEncoding:NSUTF8StringEncoding]];
	    [l closeFile];
	}
    }
    [[logger textStorage] appendAttributedString:[[[NSAttributedString alloc] initWithString:
						  s ] autorelease]];
    [logger scrollToEndOfDocument:nil];
}

-(void)registerXfer:(id)xfer {
    [xfers insertObject:xfer atIndex:0];
    [self updateXfers];
}
-(void)unregisterXfer:(id)xfer {
    [xfers removeObject:xfer];
    [self updateXfers];
}
-(void)updateXfers {
    [xfersView reloadData];
}
-(BOOL)hasPeer:(struct sockaddr_in*)sin {
    return NSNotFound!=[xfers indexOfObjectPassingTest:^BOOL(XFer *x,NSUInteger i,BOOL *s) {
	struct sockaddr_in *p = x.peer;
	return p->sin_len==sin->sin_len && !memcmp(p, sin, p->sin_len);
    }];
}

-(BOOL)hasSelectedXfer {
    return [xfersView selectedRow]>=0;
}

-(void)tableViewSelectionDidChange:(NSNotification *)an {
    [toolbar validateVisibleItems];
}
-(BOOL)validateToolbarItem:(NSToolbarItem *)theItem {
    if([theItem.itemIdentifier isEqualToString:@"abort_xfer"])
	return self.hasSelectedXfer;
    return YES;
}
-(IBAction)abortXfer:(id)sender {
    NSInteger r = [xfersView selectedRow];
    NSAssert(r>=0,@"no selected row");
    if(r<0) return;
    XFer *x = xfers[r];
    [self log:@"Aborting transfer of '%@' %@",x.xferFilename,x.xferPrefix];
    [x abort];
}

- (IBAction)getFile:(id)sender {
    [ARequest getFile];
}
- (IBAction)putFile:(id)sender {
    [ARequest putFile];
}

- (IBAction)pickTFTPFolder:(id)sender {
    NSOpenPanel *op = [NSOpenPanel openPanel];
    op.canChooseDirectories = YES; op.canChooseFiles = NO;
    op.canCreateDirectories = YES;
    op.allowsMultipleSelection = NO;
    op.prompt = @"Set TFTP root";
    op.title = @"TFTP root";
    op.nameFieldLabel = @"TFTP root:";
    if([op runModal]!=NSFileHandlingPanelOKButton) return;
    [[theDefaults values] setValue:op.URL.path forKey:@"tftpRoot"];
}

- (IBAction)pickLogFile:(id)sender {
    NSSavePanel *op = [NSSavePanel savePanel];
    op.canCreateDirectories = YES;
    op.prompt = @"Set log file";
    op.title = @"Log to";
    op.nameFieldLabel = @"Log to";
    if([op runModal]!=NSFileHandlingPanelOKButton) return;
    [[theDefaults values] setValue:op.URL.path forKey:@"logFile"];

}

- (void)runBiportal:(char const**)args {
    FILE *f=NULL;
    AuthorizationRef a=nil;
    @try {
	NSString *bip=[[NSBundle mainBundle] pathForAuxiliaryExecutable:@"biportal"];
	struct stat st;
	if(stat(bip.UTF8String, &st)) [NSException raise:@"ToolFailure" format:@"Can't see my tool"];
	if(st.st_uid || !(st.st_mode&S_ISUID)) {
	    OSStatus r = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &a);
	    if(r!=errAuthorizationSuccess)
		[NSException raise:@"AuthFailure" format:@"failed to AuthorizationCreate(): %d",r];
	    AuthorizationItem ai = {kAuthorizationRightExecute,0,NULL,0};
	    AuthorizationRights ar = {1,&ai};
	    r = AuthorizationCopyRights(a, &ar, NULL, kAuthorizationFlagDefaults|kAuthorizationFlagInteractionAllowed|kAuthorizationFlagPreAuthorize|kAuthorizationFlagExtendRights, NULL);
	    if(r!=errAuthorizationSuccess)
		[NSException raise:@"AuthFailure" format:@"failed to AuthorizationCopyRights(): %d",r];
	    const char *args[] = { NULL };
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
	    r = AuthorizationExecuteWithPrivileges(a,bip.UTF8String,
						   kAuthorizationFlagDefaults, (char*const*)args, &f);
#pragma GCC diagnostic pop
	    if(r!=errAuthorizationSuccess)
		[NSException raise:@"AuthFailure" format:@"failed to AuthorizationExecuteWithPrivileges(): %d",r];
	    int e;
	    int sr = fscanf(f, "%d", &e);
	    fclose(f),f=NULL;
	    if(sr!=1)
		[NSException raise:@"ToolFailure" format:@"failed to setup tool"];
	    if(e)
		[NSException raise:@"ToolFailure" format:@"failed to setup tool, error code: %d",e];
	}
	*args = bip.UTF8String;
	pid_t p = fork();
	if(p<0) [NSException raise:@"ToolFailure" format:@"failed to fork"];
	if(!p) execv(*args,(char**)args), exit(errno);
	int r, wp;
	while((wp=waitpid(p,&r,0))<0 && errno==EINTR);
	if(wp!=p) [NSException raise:@"ToolFailure" format:@"failed to wait for tool"];
	if(!WIFEXITED(r)) [NSException raise:@"ToolFailure" format:@"tool failed"];
	if(WEXITSTATUS(r)) {
	    [[NSException exceptionWithName:@"ToolFailure" reason:[NSString stringWithFormat:@"tool failed, error code: %d", WEXITSTATUS(r)] userInfo:@{@"errno": @WEXITSTATUS(r)}] raise];
	}
    }@finally {
	if(f) fclose(f);
	if(a) AuthorizationFree(a,kAuthorizationFlagDefaults);
    }
}

+(void)initialize {
    [[NSUserDefaultsController sharedUserDefaultsController] setInitialValues:
     [NSDictionary dictionaryWithContentsOfFile:
      [[NSBundle mainBundle] pathForResource:@"pumpkin-defaults" ofType:@"plist"]
     ]
    ];
    [NSValueTransformer setValueTransformer:[[[NumberTransformer alloc] init] autorelease] forName:@"PortNumberTransformer"];
    [NSValueTransformer setValueTransformer:[[[IPTransformer alloc] init] autorelease] forName:@"IPAddressTransformer"];
}

@end
