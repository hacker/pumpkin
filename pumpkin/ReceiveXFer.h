
#import "XFer.h"

@interface ReceiveXFer : XFer

-(ReceiveXFer*)initWithPeer:(struct sockaddr_in *)sin andPacket:(TFTPPacket*)p;
-(ReceiveXFer*)initWithLocalFile:(NSString *)lf peerAddress:(const struct sockaddr_in *)pa remoteFile:(NSString *)rf xferType:(NSString *)xt blockSize:(uint16_t)bs andTimeout:(int)to;
@end
