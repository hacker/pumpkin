
#import <Cocoa/Cocoa.h>
#include <netinet/in.h>

@class PumpKIN;
@interface DaemonListener : NSObject {
    CFSocketRef sockie;
    PumpKIN *pumpkin;
    CFRunLoopSourceRef runloopSource;
}

+(DaemonListener*)listenerWithDefaults;
-(void)callbackWithType:(CFSocketCallBackType)t addr:(CFDataRef)a data:(const void *)d;

@end
