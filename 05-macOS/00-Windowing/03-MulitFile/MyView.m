#import "MyView.h"

@implementation MyView {
 @private
  NSString *centralText;
}

- (id)initWithFrame:(NSRect)frame {
  // code
  self = [super initWithFrame:frame];

  if (self) {
    centralText = @"Hello World!";
  }
  return (self);
}

- (void)drawRect:(NSRect)dirtyRect {
  // code
  // black color for background
  NSColor *backgroundColor = [NSColor blackColor];
  [backgroundColor set];
  NSRectFill(dirtyRect);

  NSDictionary *dictionaryForTextAttributes = [NSDictionary
      dictionaryWithObjectsAndKeys:[NSFont fontWithName:@"Helvetica" size:32], NSFontAttributeName,
                                   [NSColor greenColor], NSForegroundColorAttributeName, nil];

  NSSize textSize = [centralText sizeWithAttributes:dictionaryForTextAttributes];

  NSPoint point;
  point.x = (dirtyRect.size.width / 2) - (textSize.width / 2);
  // +12 to adjust the title bar of window
  point.y = (dirtyRect.size.height / 2) - (textSize.height / 2) + 12;

  [centralText drawAtPoint:point withAttributes:dictionaryForTextAttributes];
}

- (BOOL)acceptsFirstResponder {
  // code
  [[self window] makeFirstResponder:self];
  return (YES);
}

- (void)keyDown:(NSEvent *)theEvent {
  // code
  int key = (int)[[theEvent characters] characterAtIndex:0];

  switch (key) {
    case 27:  // esc key
      [self release];
      [NSApp terminate:self];
      break;

    case 'F':
    case 'f':
      [[self window] toggleFullScreen:self];
      break;
  }
}

- (void)mouseDown:(NSEvent *)theEvent {
  // code
  centralText = @"left mouse button is clicked!";
  // ~ invalidate display, so it will repaint
  [self setNeedsDisplay:YES];
}

- (void)rightMouseDown:(NSEvent *)theEvent {
  // code
  centralText = @"right mouse button is clicked!";
  // ~ invalidate display, so it will repaint
  [self setNeedsDisplay:YES];
}

- (void)otherMouseDown:(NSEvent *)theEvent {
  // code
  centralText = @"other mouse button is clicked!";
  // ~ invalidate display, so it will repaint
  [self setNeedsDisplay:YES];
}

- (void)dealloc {
  // code
  [super dealloc];
}

@end
