package com.tns;

import java.util.HashMap;
import java.util.Map;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

public abstract class CanvasViewBase extends View
{
	private Bitmap bitmap;
	
	static
	{
		System.loadLibrary("canvas");
	}
	
	public CanvasViewBase(Context context)
	{
		super(context);
	}	
	
	public CanvasViewBase(Context context, AttributeSet attrSet)
	{
		super(context, attrSet);
	}

	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh)
	{
		this.bitmap = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
		this.__sizeChangedImpl(this.getLeft(), this.getTop(), w, h);
	}
	
	@Override
	protected void onDraw(Canvas canvas)
	{
		long pixelsPtr = lockBitmap(this.bitmap);
		this.__drawImpl((int)(pixelsPtr & 0xFFFFFFFF), (int)(pixelsPtr >> 32), this.bitmap.getWidth(), this.bitmap.getHeight());
		unlockBitmap(this.bitmap);
		canvas.drawBitmap(this.bitmap, 0, 0, null);
		this.invalidate();
	}
	
	@Override
	public boolean onGenericMotionEvent(MotionEvent evt)
	{
		int action = evt.getAction();
		if (pointerActions.containsKey(action))
		{
			return this.__dispatchMouseEvent(pointerActions.get(action), evt.getRawX(), evt.getRawY());
		}
		else if (touchActions.containsKey(action))
		{
			this.__beginTouchEvent(touchActions.get(action));
			for (int i = 0; i < evt.getPointerCount(); ++i)
			{
				int id = evt.getPointerId(i);
				float force = evt.getPressure(i);
				float x = evt.getX(i);
				float y = evt.getY(i);
				float radiusX = evt.getTouchMajor(i);
				float radiusY = evt.getTouchMinor(i);
				float rotation = (float)(evt.getOrientation(i) * 180.0 / Math.PI);
				this.__addTouch(id, x, y, force, radiusX, radiusY, rotation);
			}
			return this.__completeTouchEvent();
		}
		return super.onGenericMotionEvent(evt);
	}
	
	public abstract void __drawImpl(int pixelsPtrLow, int pixelsPtrHigh, int w, int h);
	public abstract void __sizeChangedImpl(int l, int t, int w, int h);
	
	public abstract boolean __dispatchMouseEvent(String evt, float x, float y);
	
	public abstract void __beginTouchEvent(String evt);
	public abstract void __addTouch(int id, float x, float y, float force, float radiusX, float radiusY, float rotation);
	public abstract boolean __completeTouchEvent();

	private native long lockBitmap(Bitmap bitmap);
	private native void unlockBitmap(Bitmap bitmap);
	
	private static Map<Integer, String> pointerActions = new HashMap<Integer, String>();
	private static Map<Integer, String> touchActions = new HashMap<Integer, String>();
	
	static
	{
		pointerActions.put(MotionEvent.ACTION_HOVER_ENTER, "mouseenter");
		pointerActions.put(MotionEvent.ACTION_HOVER_MOVE, "mousemove");
		pointerActions.put(MotionEvent.ACTION_HOVER_EXIT, "mouseleave");
		
		touchActions.put(MotionEvent.ACTION_DOWN, "touchstart");
		touchActions.put(MotionEvent.ACTION_UP, "touchend");
		touchActions.put(MotionEvent.ACTION_CANCEL, "touchcancel");
		touchActions.put(MotionEvent.ACTION_MOVE, "touchmove");
	}
}
