#import "CanvasRenderingContext2d.h"

#include <UIKit/UIKit.h>


CGContextRef CreateBitmapContext (int pixelsWide,
                                    int pixelsHigh)
{
    CGContextRef    context = NULL;
    CGColorSpaceRef colorSpace;
    void *          bitmapData;
    int             bitmapByteCount;
    int             bitmapBytesPerRow;
    
    bitmapBytesPerRow   = (pixelsWide * 4);// 1
    bitmapByteCount     = (bitmapBytesPerRow * pixelsHigh);
    
    colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);// 2
    bitmapData = malloc(bitmapByteCount);// 3
    if (bitmapData == NULL)
    {
        fprintf (stderr, "Memory not allocated!");
        return NULL;
    }
    context = CGBitmapContextCreate (bitmapData,// 4
                                     pixelsWide,
                                     pixelsHigh,
                                     8,      // bits per component
                                     bitmapBytesPerRow,
                                     colorSpace,
                                     kCGImageAlphaPremultipliedLast);
    if (context== NULL)
    {
        free (bitmapData);// 5
        fprintf (stderr, "Context not created!");
        return NULL;
    }
    CGColorSpaceRelease( colorSpace );// 6
    
    return context;// 7
}


@implementation __CanvasRenderingContext2D {
    CGContextRef myContext;
}

-(void)setWidth:(int)width height:(int)height
{
    if (myContext)
    {
        free(CGBitmapContextGetData(myContext));
        CGContextRelease(myContext);
    }
    myContext = CreateBitmapContext(width, height);
}

-(void)fillRectX:(float)x Y:(float)y W:(float)width H:(float)height
{
    CGContextFillRect(myContext, CGRectMake(x, y, width, height));
}

@end
