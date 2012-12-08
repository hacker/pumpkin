
#import "XFersViewDatasource.h"
#import "XFer.h"

@implementation XFersViewDatasource

- (id)initWithXfers:(NSMutableArray*)x {
    if(!(self = [super init])) return self;
    xfers = [x retain];
    return self;
}
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex {
    return [xfers[rowIndex] cellValueForColumn:aTableColumn.identifier];
}
- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView {
    return xfers.count;
}

- (void) dealloc {
    [xfers release];
    [super dealloc];
}

@end
