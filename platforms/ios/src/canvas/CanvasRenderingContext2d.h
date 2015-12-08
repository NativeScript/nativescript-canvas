#import <Foundation/Foundation.h>

@interface __CanvasRenderingContext2D : NSObject
@property NSString* fillStyle;

-(void)setWidth:(int)width height:(int)height;
-(void)__draw;

-(void)fillRect:(float)x :(float)y :(float)width :(float)height;

@end
