
#import <Foundation/Foundation.h>
#include <netinet/in.h>

@interface NSString (StringsAttached)

+ stringWithSocketAddress:(const struct sockaddr_in*)sin;
+ stringWithHostAddress:(const struct sockaddr_in*)sin;
+ stringWithPortNumber:(const struct sockaddr_in*)sin;

@end
