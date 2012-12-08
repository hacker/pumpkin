#import <Cocoa/Cocoa.h>
#import "DaemonListener.h"
#include <netinet/in.h>
#import "XFersViewDatasource.h"

enum {
    onRRQGive=0, onRRQPrompt, onRRQDeny,
    onWRQTake=0, onWRQPromptIfExists, onWRQPrompt, onWRQDeny
};

@interface PumpKIN : NSObject <NSApplicationDelegate> {
    NSWindow *window;
    NSTextView *logger;
    DaemonListener *listener;
    NSWindow *preferencesWindow;
    NSUserDefaultsController *theDefaults;
    NSMutableArray *xfers;
    NSTableView *xfersView;
    XFersViewDatasource *xvDatasource;
    NSToolbar *toolbar;
}

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet NSTextView *logger;
- (IBAction)showPreferences:(id)sender;
@property (assign) IBOutlet NSWindow *preferencesWindow;
@property (assign) IBOutlet NSUserDefaultsController *theDefaults;
- (IBAction)pickTFTPFolder:(id)sender;
- (IBAction)pickLogFile:(id)sender;
@property (assign) IBOutlet NSTableView *xfersView;
@property (readonly) BOOL hasSelectedXfer;
@property (assign) IBOutlet NSToolbar *toolbar;
- (IBAction)abortXfer:(id)sender;
- (IBAction)getFile:(id)sender;
- (IBAction)putFile:(id)sender;

+(void)initialize;

-(void)log:(NSString*)fmt,...;
-(void)registerXfer:(id)xfer;
-(void)unregisterXfer:(id)xfer;
-(void)updateXfers;
-(BOOL)hasPeer:(struct sockaddr_in*)sin;

-(void)tableViewSelectionDidChange:(NSNotification*)an;

- (void)runBiportal:(char const**)arg;

@end
