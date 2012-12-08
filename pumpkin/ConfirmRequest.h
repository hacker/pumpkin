
#import <Cocoa/Cocoa.h>
#import "TFTPPacket.h"
#import "XFer.h"

enum RequestVerdict {
    verdictDeny = 0,
    verdictAllow, verdictRename,
    verdictDefault = verdictDeny
};

@interface ConfirmRequest : NSWindowController {
    XFer *xfer;
    NSString *remoteHost;
    NSString *remoteAction;
    NSString *fileName;
    BOOL fileExists;
    BOOL isWriteRequest;
    NSTimer *timeout;
}

@property (copy) NSString *remoteHost;
@property (copy) NSString *remoteAction;
@property (copy) NSString *fileName;
@property BOOL fileExists;
@property BOOL isWriteRequest;

@property (assign) IBOutlet NSButton *allowButton;
@property (assign) IBOutlet NSButton *denyButton;
@property (assign) IBOutlet NSButton *renameButton;

- (IBAction)letItBe:(id)sender;
- (IBAction)deny:(id)sender;
- (IBAction)rename:(id)sender;

+ (void) confirmationWithXfer:(XFer*)x;

@end
