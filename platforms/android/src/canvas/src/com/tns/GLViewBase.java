package com.tns;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public abstract class GLViewBase extends GLSurfaceView {

	public GLViewBase(Context context, Renderer renderer) {
		super(context);
		setEGLContextClientVersion(2);
		setRenderer(renderer);
	}
}
