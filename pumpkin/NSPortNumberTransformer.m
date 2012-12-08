#import "NSPortNumberTransformer.h"

@implementation NSPortNumberTransformer

+(Class)transformedValueClass { return [NSNumber class]; }
+(BOOL)allowsReverseTransformation { return YES; }
-(id)transformedValue:(id)value {
    if(value==nil) return nil;
    if(![value respondsToSelector:@selector(integerValue)]) return nil;
    NSInteger rv = [value integerValue];
    if(rv<1 || rv>32767) return nil;
    return [NSString stringWithFormat:@"%u",rv];
}
-(id)reverseTransformedValue:(id)value {
    if(value==nil) return nil;
    if(![value respondsToSelector:@selector(integerValue)]) return nil;
    NSInteger rv = [value integerValue];
    if(rv<1 || rv>32767) return nil;
    return [NSNumber numberWithInteger:rv];
}

@end
