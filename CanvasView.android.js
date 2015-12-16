var _ = require('lodash');

exports.platform = 'android';


var EventableImpl = {
    __dispatchMouseEvent: function (name, x, y) {
        var evt = this.__createEvent(name);
        evt.screenX = evt.clientX = x;
        evt.screenY = evt.clientY = y;
        return this.__dispatchEvent(evt);
    },

    __beginTouchEvent: function (name) {
        var evt = this.__touchEvt = this.__createEvent(name);
        evt.touches = evt.targetTouches = evt.changedTouches = [];
    },
    __addTouch: function (id, x, y, force, radiusX, radiusY, rotation) {
        this.__touchEvt.touches.push({
            identifier: id,
            target: this,
            radiusX: radiusX,
            radiusY: radiusY,
            rotationAngle: rotation,
            force: force,
            screenX: x,
            screenY: y,
            clientX: x,
            clientY: y,
            pageX: x,
            pageY: y,
        });
    },
    __completeTouchEvent: function () {
        var evt = this.__touchEvt;
        delete this.__touchEvt;
        return this.__dispatchEvent(evt);
    },

    __createEvent: function createEvent(name) {
        return {
            bubbles: false,
            cancelable: false,
            currentTarget: this,
            defaultPrevented: false,
            eventPhase: 2,
            target: this,
            timeStamp: new Date(),
            type: name,
            preventDefault: function () {
                this.defaultPrevented = true;
            },
            stopImmediatePropagation: function () {
                this.immediatePropagationStopped = true;
            },
            stopPropagation: function () {
                this.propagationStopped = true;
            },
        };
    },

    __dispatchEvent: function (evt) {
        if (this.__events) {
            var listeners = this.__events[evt.type];
            if (listeners) {
                for (var i = 0; i < listeners.length; ++i) {
                    listeners[i](evt);
                    if (evt.immediatePropagationStopped) {
                        break;
                    }
                }
            }
        }
        return evt.defaultPrevented;
    },

    addEventListener: function (evt, listener) {
        var events = this.__events || (this.__events = {});
        var listeners = events[evt] || (events[evt] = []);
        listeners.push(listener);
    },

    removeEventListener: function (evt, listener) {
        var events = this.__events || (this.__events = {});
        var listeners = events[evt] || (events[evt] = []);
        var index = listeners.indexOf(listener);
        if (index > -1) {
            listeners.splice(index, 1);
        }
    },

    requestAnimationFrame: function (callback) {
        var rafs = (this._rafs || (this._rafs = []));
        rafs.push(callback);
    },
};

var CanvasViewImpl = {
    __drawImpl: function (pixelsPtrLow, pixelsPtrHigh, w, h) {
        if (this.onFrame) {
            this.onFrame();
        }
        if (this.__context) {
            this.__context.__draw(pixelsPtrLow, pixelsPtrHigh, w, h);
        }
    },

    __sizeChangedImpl: function (l, t, w, h) {
        this.offsetWidth = this.width = w;
        this.offsetHeight = this.height = h;
        this.__boundingRect = {
            left: l,
            right: l + w,
            top: t,
            bottom: t + h,
            width: w,
            height: h,
        };
        if (this.onLoaded) {
            this.onLoaded();
        }
        if (this.__context) {
            this.__context.__sizeChanged(w, h);
        }
    },

    getContext: function (kind) {
        if (this.__context && this.__context.__kind !== kind) {
            return null;
        }
        if (!this.__context) {
            var native = require(__dirname + "/../../../../lib/libcanvas.so");
            if (kind === '2d') {
                this.__context = native.create2d(this);
            }
        }
        return this.__context;
    },

    getBoundingClientRect: function () {
        return this.__boundingRect;
    }
};
_.extend(CanvasViewImpl, EventableImpl);
var CanvasView = com.tns.CanvasViewBase.extend(CanvasViewImpl);

var GLSurfaceRenderer = android.opengl.GLSurfaceView.Renderer.extend({
    onDrawFrame: function (gl10) {
        this._view.executeRafs();
    },

    onSurfaceChanged: function (gl10, width, height) {
       // this._context.__surfaceChanged(width, height);
    },

    onSurfaceCreated: function (gl10, config) {
       // this._context.__surfaceCreated();
    }
});

var GLViewImpl = {
    getContext: function (kind) {
        if (!this.__context) {
            var native = require(__dirname + "/../../../../lib/libcanvas.so");
            if (kind === 'webgl') {
                this.__context = native.createGL(this);
            }
        }
        return this.__context;
    },
    executeRafs: function () {
        var rafs = this._rafs;
        if (rafs) {
            this._rafs = [];
            rafs.forEach(function (callback) { callback(); });
        }
    }
};
_.extend(GLViewImpl, EventableImpl);
var GLView = com.tns.GLViewBase.extend(GLViewImpl);

exports.createView = function createView(width, height, context, kind) {
    if (kind === 'webgl') {
        var renderer = new GLSurfaceRenderer();
        var glview = new GLView(context, renderer);
        renderer._view = glview;
        return glview;
    } else {
        return new CanvasView(context);
    }
}
