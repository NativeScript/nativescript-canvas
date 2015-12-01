exports.CanvasView = com.tns.CanvasViewBase.extend({
	__drawImpl: function (pixelsPtrLow, pixelsPtrHigh, w, h) {
		if (this.onFrame) {
			this.onFrame();
		}
		this.__context.__draw(pixelsPtrLow, pixelsPtrHigh, w, h);
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
		this.__context.__sizeChanged(w, h);
    },

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

	getContext: function (kind) {
        if (!this.__context) {
            var arch;
            var sysArch = java.lang.System.getProperty("os.arch");
            var lcArch = sysArch.toLowerCase();
            if (lcArch.indexOf("arm") > -1) {
                arch = "arm";
            } else if (lcArch.indexOf("i686") > -1) {
                arch = "x86";
            }
            if (arch) {
                var native = require("./libcanvas-" + arch + ".so");
                if (kind === '2d') {
                    this.__context = native.create2d(this);
                }
            }
        }
        if (this.__context && this.__context.__kind !== kind) {
            return null;
        }
        return this.__context;
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

    getBoundingClientRect: function () {
        return this.__boundingRect;
    },
});

