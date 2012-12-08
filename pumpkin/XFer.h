
#import <Foundation/Foundation.h>
#import "PumpKIN.h"
#include <netinet/in.h>
#import "TFTPPacket.h"

enum XFerState {
    xferStateNone = 0,
    xferStateConnecting,
    xferStateXfer,
    xferStateShutdown
};

@interface XFer : NSObject {
    struct sockaddr_in peer;
    PumpKIN *pumpkin;
    CFSocketRef sockie;
    CFRunLoopSourceRef runloopSource;
    NSFileHandle *theFile;
    uint16_t blockSize;
    uint16_t acked;
    unsigned long long xferSize;
    uint16_t xferBlocks;
    enum XFerState state;
    NSString *xferType;
    NSString *xferFilename;
    NSTimeInterval retryTimeout;
    NSTimeInterval giveupTimeout;
    TFTPPacket *lastPacket;
    NSTimer *retryTimer;
    NSTimer *giveupTimer;
    TFTPPacket *initialPacket;
    NSString *xferPrefix;

    NSString *localFile;

    NSMutableArray *queue;
}
@property (readonly) struct sockaddr_in *peer;
@property (readonly) TFTPPacket *initialPacket;
@property (readonly) NSString *xferFilename;
@property (readonly) NSString *localFile;
@property (readonly) NSString *xferPrefix;

- (id) init;
- (id) initWithPeer:(struct sockaddr_in *)sin andPacket:(TFTPPacket*)p;

- (BOOL) createSocket;
- (void) callbackWithType:(CFSocketCallBackType)t addr:(CFDataRef)a data:(const void *)d;
- (void) queuePacket:(TFTPPacket*)p;

- (void) eatTFTPPacket:(TFTPPacket*)p from:(struct sockaddr_in*)sin;

- (id) cellValueForColumn:(NSString*)ci;

- (void) updateView;
- (void) appear;
- (void) disappear;

- (BOOL) isPeer:(struct sockaddr_in*)sin;

- (void) abort;

- (void) goOnWithVerdict:(int)verdict;

- (BOOL) makeLocalFileName:(NSString*)xf;

@end
