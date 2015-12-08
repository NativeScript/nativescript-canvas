#import "CanvasRenderingContext2d.h"

#include <UIKit/UIKit.h>

#include <algorithm>

CGContextRef CreateBitmapContext (int pixelsWide,
                                    int pixelsHigh)
{
    CGContextRef    context = NULL;
    CGColorSpaceRef colorSpace;
    int             bitmapBytesPerRow;
    
    bitmapBytesPerRow   = (pixelsWide * 4);// 1
    
    colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);// 2
    context = CGBitmapContextCreate (NULL,// 4
                                     pixelsWide,
                                     pixelsHigh,
                                     8,      // bits per component
                                     bitmapBytesPerRow,
                                     colorSpace,
                                     kCGImageAlphaPremultipliedLast);
    if (context== NULL)
    {
        fprintf (stderr, "Context not created!");
        return NULL;
    }
    CGColorSpaceRelease( colorSpace );// 6
    
    return context;// 7
}

static bool TryParseHex(char hexChar, int* value)
{
    if (hexChar >= '0' && hexChar <= '9')
    {
        *value = hexChar - '0';
        return true;
    }
    if (hexChar >= 'a' && hexChar <= 'f')
    {
        *value = hexChar - 'a' + 10;
        return true;
    }
    if (hexChar >= 'A' && hexChar <= 'F')
    {
        *value = hexChar - 'A' + 10;
        return true;
    }
    return false;
}

static bool TryParseColor(char* cssColorStr, CGFloat* result)
{
    unsigned color = 0xFF000000;
    size_t len = strlen(cssColorStr);
    std::remove_if(cssColorStr, cssColorStr + len, isspace);
    if (cssColorStr[0] == '#')
    {
        if (len == 4)
        {
            int C[3]; // blue, green, red
            for (int i = 0; i < 3; ++i)
            {
                if (!TryParseHex(cssColorStr[i + 1], &C[2 - i]))
                    return false;
            }
            for (int i = 0; i < 3; ++i)
            {
                color |= C[i] << (8 * i);
                color |= C[i] << (8 * i + 4);
            }
        }
        else if (len == 7)
        {
            int C[6]; // blue x2, green x2, red x2
            for (int i = 0; i < 6; ++i)
            {
                if (!TryParseHex(cssColorStr[i + 1], &C[5 - i]))
                    return false;
            }
            for (int i = 0; i < 6; ++i)
                color |= C[i] << (4 * i);
        }
    }
    else
    {
        int C[3];
        float alpha = 1;
        if (sscanf(cssColorStr, "rgb(%d,%d,%d)", &C[2], &C[1], &C[0]) == 3
            || sscanf(cssColorStr, "rgba(%d,%d,%d,%f)", &C[2], &C[1], &C[0], &alpha) == 4)
        {
            if (alpha != 1)
            {
                if (alpha < 0)
                    alpha = 0;
                if (alpha > 1)
                    alpha = 1;
                color = ((int) round(alpha * 255)) << 24;
            }
            for (int i = 0; i < 3; ++i)
                color |= C[i] << (8 * i);
        }
        else
        {
            return false;
        }
    }
    
    unsigned char* colorBits =(unsigned char*) &color;
    for (int i = 0; i < 4; ++i)
    {
        result[i] = colorBits[i] / 255.f;
    }
    return true;
}



@implementation __CanvasRenderingContext2D {
    CGContextRef myContext;
    int myWidth;
    int myHeight;
}

@synthesize fillStyle = myFillStyle;

-(void)__draw
{
    CGRect viewport = CGRectMake(0, 0, myWidth, myHeight);
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    CGContextClearRect(ctx, viewport);
    CGImageRef image = CGBitmapContextCreateImage(myContext);
    CGContextDrawImage(ctx, viewport, image);
    CGImageRelease(image);
}

-(void)setWidth:(int)width height:(int)height
{
    if (myContext)
    {
        CGContextRelease(myContext);
    }
    myContext = CreateBitmapContext(width, height);
    myWidth = width;
    myHeight = height;
}

-(NSString*)fillStyle
{
    return myFillStyle;
}

-(void)setFillStyle:(NSString *)fillStyle
{
    char str[200];
    [fillStyle getCString:str maxLength:200 encoding:NSUTF8StringEncoding];
    CGFloat color[4];
    if (TryParseColor(str, color))
    {
        myFillStyle = fillStyle;
        CGContextSetRGBFillColor(myContext, color[2], color[1], color[0], color[3]);
    }
}

-(void)fillRect:(float)x :(float)y :(float)width :(float)height
{
    CGContextFillRect(myContext, CGRectMake(x, y, width, height));
}


@end
