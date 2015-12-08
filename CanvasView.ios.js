exports.platform = 'ios';
var CanvasView = UIView.extend({
	drawRect: function drawRect(dirtyRect) {
		//var ctx = UIGraphicsGetCurrentContext();
		// var rafs = this._rafs;
		// this._rafs = [];
		//CGContextClearRect(ctx, this.bounds);
	    //CGContextSetRGBFillColor(ctx, 0.5, 0.2, 0.8, 1);
		//CGContextFillRect(ctx, this.bounds);
		// this._context = ctx;
		// rafs.forEach(function(callback) {
		// 	callback();
		// })
		// this._context = null;
		
		//self.setNeedsDisplay();
		if (this._context) {
			this._context.__draw();
		}
		// var image = CGBitmapContextCreateImage(this._context);
		// CGContextDrawLayerAtPoint(ctx, CGPointMake(0, 0), this._layer);
		// CGContextDrawImage(ctx, this.bounds, image);
		// CGImageRelease(image);
	},
	
	getContext: function(kind) {
		this.contentScaleFactor = 1;	
		this.layer.contentsScale = 1;
		this._rafs = [];
		if (!this._context) {
			if (kind === '2d') {
				this._context = __CanvasRenderingContext2D.alloc().init();
				this._context.setWidthHeight(this.bounds.size.width, this.bounds.size.height);
			}
		}
		this.setNeedsDisplay();
		return this._context;
		
		// console.log('Creating context with size ' + this.bounds.size.width + ' x ' + this.bounds.size.height);
		// var ctx = UIGraphicsGetCurrentContext();
		// var colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
		// this._layer = CGLayerCreateWithContext(ctx, this.bounds.size, null);
		// var size = this.bounds.size;
		
		// this._context = CGBitmapContextCreate(null, size.width, size.height, 8, 0, colorSpace, kCGImageAlphaPremultipliedLast);
		// CGColorSpaceRelease(colorSpace);
	},
	
	didMoveToWindow: function() {
		this.contentScaleFactor = 1;	
		
		this.layer.contentsScale = 1;
	},
	
	setSize: function(width, height) {
		if (width !== this.bounds.size.width || height !== this.bounds.size.height) {
			if (this._context) {
				this._context.setWidthHeight(width, height);
			}
			this.bounds = CGRectMake(0, 0, width, height);
			console.log('resizing now!' + width + " " + height);
		}
	}
})

exports.createView = function createView(width, height) {
	return CanvasView.alloc().initWithFrame(CGRectMake(0, 0, width, height));	
}
