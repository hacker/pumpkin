
#import "StringsAttached.h"
#include <arpa/inet.h>

@implementation NSString (StringsAttached)

+ stringWithSocketAddress:(const struct sockaddr_in*)sin {
    return [NSString stringWithFormat:@"%@:%u",[NSString stringWithHostAddress:sin],ntohs(sin->sin_port)];
}
+ stringWithHostAddress:(const struct sockaddr_in*)sin {
    char tmp[32];
    addr2ascii(sin->sin_family,&sin->sin_addr,sizeof(sin->sin_addr),tmp);
    return @(tmp);
}
+ (id)stringWithPortNumber:(const struct sockaddr_in *)sin {
    return [NSString stringWithFormat:@"%u",ntohs(sin->sin_port)];
}

@end
