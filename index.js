var view = require('ui/core/view');
console.log('before CanvasView');
var CanvasView = require('./CanvasView').CanvasView;
console.log('after CanvasView');

function Canvas() {
	view.View.call(this);

	this._drawingContext = null;
}
__extends(Canvas, view.View);

Canvas.prototype.getContext = function getContext(whichContext) {
	return this._android.getContext(whichContext);
}

Canvas.prototype._draw = function _draw(canvas) {
	if (this._drawingContext) {
		canvas.drawBitmap(this._drawingContext._backbuffer, 0.0, 0.0, null);
	}
}

Canvas.prototype._onSizeChanged = function _onSizeChanged(width, height) {
	if (this._drawingContext) {
		this._drawingContext._initializeBackbuffer(width, height);
	}
}

Canvas.prototype.onLayout = function onLayout(left, right, top, bottom) {
	view.View.onLayout.apply(this, arguments);
	this._onSizeChanged(right - left, top - bottom);
}

Canvas.prototype._createUI = function _createUI() {
	var view = new CanvasView(this._context);
	view._scriptCanvas = this;
	this._android = view;
}

Object.defineProperty(Canvas.prototype, "offsetWidth", {
	get: function () { return this.width; },
});

Object.defineProperty(Canvas.prototype, "offsetHeight", {
	get: function () { return this.height; },
});

Object.defineProperty(Canvas.prototype, "android", {
	get: function () { return this._android; },
});

exports.Canvas = Canvas;
