
#import "IPFormatter.h"
#include <arpa/inet.h>

@implementation IPFormatter

-(NSString *)stringForObjectValue:(id)obj {
    if(![obj isKindOfClass:[NSString class]]) return nil;
    return obj;
}

-(BOOL)getObjectValue:(id*)anObject forString:(NSString*)string errorDescription:(NSString**)error {
    if(inet_addr(string.UTF8String)==INADDR_NONE) {
	if(error) *error=@"Doesn't look like an IP address to me";
	return NO;
    }
    *anObject = [NSString stringWithString:string];
    return YES;
}

@end
