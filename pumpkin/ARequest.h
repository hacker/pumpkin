#import <Cocoa/Cocoa.h>

@interface ARequest : NSWindowController {
    BOOL requestIsGet;
    
    CFHostRef cfhost;
    
    NSString *localFile;
    NSString *remoteHost;
    NSNumber *remotePort;
    NSString *remoteFile;
    NSString *xferType;
    NSNumber *blockSize;
    NSNumber *timeout;

    NSTextField *remoteHostBox;
    BOOL doTouchMe;
    NSString *statusLabel;
    NSString *errorLabel;
}

@property BOOL requestIsGet;
@property BOOL doTouchMe;
@property (copy) NSString *statusLabel;
@property (copy) NSString *errorLabel;

@property (copy) NSString *localFile;
@property (copy) NSString *remoteHost;
@property (copy) NSNumber *remotePort;
@property (copy) NSString *remoteFile;
@property (copy) NSString *xferType;
@property (copy) NSNumber *blockSize;
@property (copy) NSNumber *timeout;

@property (assign) IBOutlet NSTextField *remoteHostBox;

- (IBAction)startXfer:(id)sender;
- (IBAction)pickFile:(id)sender;

-(void)hostCallbackWithHost:(CFHostRef)h info:(CFHostInfoType)hi andError:(const CFStreamError*)e;

+(void)getFile;
+(void)putFile;

-(ARequest*)initWithGet:(BOOL)gr;

@end
