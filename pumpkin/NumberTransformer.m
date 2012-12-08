
#import "NumberTransformer.h"

@implementation NumberTransformer

+(Class)transformedValueClass { return [NSNumber class]; }
+(BOOL)allowsReverseTransformation { return YES; }
-(id)transformedValue:(id)value {
    if(value==nil) return nil;
    if(![value respondsToSelector:@selector(integerValue)]) return nil;
    return [NSString stringWithFormat:@"%lu",[value integerValue]];
}
-(id)reverseTransformedValue:(id)value {
    if(value==nil) return nil;
    if(![value respondsToSelector:@selector(integerValue)]) return nil;
    return @([value integerValue]);
}

@end
