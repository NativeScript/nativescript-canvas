var view = require('ui/core/view');
var canvasViewLib = require('./CanvasView');

function Canvas() {
	view.View.call(this);

	this._drawingContext = null;
}
__extends(Canvas, view.View);

Canvas.prototype.getContext = function getContext(kind) {
	return this._native.getContext(kind);
}

Canvas.prototype._draw = function _draw(canvas) {
	if (this._drawingContext) {
		canvas.drawBitmap(this._drawingContext._backbuffer, 0.0, 0.0, null);
	}
}

Canvas.prototype._onSizeChanged = function _onSizeChanged(width, height) {
	if (this._native) {
		this._native.setSize(width, height);
	}
}

Canvas.prototype.onLayout = function onLayout(left, top, right, bottom) {
	view.View.prototype.onLayout.apply(this, arguments);
	this._onSizeChanged(right - left, bottom - top);
}

Canvas.prototype._createUI = function _createUI() {
	console.log('Creating CanvasView');
	var view = canvasViewLib.createView(1, 1, this._context);
	view._scriptCanvas = this;
	this._native = view;
}

Object.defineProperty(Canvas.prototype, "offsetWidth", {
	get: function () { return this.width; },
});

Object.defineProperty(Canvas.prototype, "offsetHeight", {
	get: function () { return this.height; },
});

Object.defineProperty(Canvas.prototype, canvasViewLib.platform, {
	get: function () {
		if (!this._native && canvasViewLib.platform === 'ios') {
			this._createUI();
		}
		return this._native;
	}
});

exports.Canvas = Canvas;
