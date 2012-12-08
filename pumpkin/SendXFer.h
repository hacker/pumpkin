#import <Cocoa/Cocoa.h>
#import "TFTPPacket.h"
#import "PumpKIN.h"
#import "XFer.h"

#include <netinet/in.h>

@interface SendXFer : XFer {
}

-(SendXFer*)initWithPeer:(struct sockaddr_in *)sin andPacket:(TFTPPacket*)p;
-(SendXFer*)initWithLocalFile:(NSString *)lf peerAddress:(const struct sockaddr_in *)pa remoteFile:(NSString *)rf xferType:(NSString *)xt blockSize:(uint16_t)bs andTimeout:(int)to;

-(void)xfer;

@end
