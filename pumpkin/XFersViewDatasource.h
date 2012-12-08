
@interface XFersViewDatasource : NSObject <NSTableViewDataSource> {
    NSMutableArray *xfers;
}

- (id)initWithXfers:(NSMutableArray*)x;

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex;
- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView;

@end
