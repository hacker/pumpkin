
#import "IPTransformer.h"

#include <arpa/inet.h>

@implementation IPTransformer

+(Class)transformedValueClass { return [NSString class]; }
+(BOOL)allowsReverseTransformation { return YES; }
-(id)transformedValue:(id)value {
    if(value &&
       [value respondsToSelector:@selector(UTF8String)]
       && inet_addr([value UTF8String])!=INADDR_NONE )
	return value;
    return nil;
}
-(id)reverseTransformedValue:(id)value {
    return [self transformedValue:value];
}

@end