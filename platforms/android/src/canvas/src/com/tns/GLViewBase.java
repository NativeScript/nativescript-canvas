package com.tns;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;

public class GLViewBase extends GLSurfaceView {
    // number of coordinates per vertex in this array
    static final int COORDS_PER_VERTEX = 3;
    static float triangleCoords[] = {   // in counterclockwise order:
             0.0f,  0.622008459f, 0.0f, // top
            -0.5f, -0.311004243f, 0.0f, // bottom left
             0.5f, -0.311004243f, 0.0f  // bottom right
    };
    
	public GLViewBase(Context context, Renderer renderer) {
		super(context);
		setEGLContextClientVersion(2);
		//setRenderer(new MyGLRenderer());
		setRenderer(renderer);
	}
	
//	public class Triangle {
//
//	    private FloatBuffer vertexBuffer;
//	    private final int mProgram;
//
//	    private final String vertexShaderCode =
//	            "attribute vec4 vPosition;" +
//	            "void main() {" +
//	            "  gl_Position = vPosition;" +
//	            "}";
//
//	        private final String fragmentShaderCode =
//	            "precision mediump float;" +
//	            "uniform vec4 vColor;" +
//	            "void main() {" +
//	            "  gl_FragColor = vec4(0.63671875f, 0.76953125f, 0.22265625f, 1.0f);" +
//	            "}";
//
//
//	    public Triangle() {
//	        // initialize vertex byte buffer for shape coordinates
//	        ByteBuffer bb = ByteBuffer.allocateDirect(
//	                // (number of coordinate values * 4 bytes per float)
//	                triangleCoords.length * 4);
//	        // use the device hardware's native byte order
//	        bb.order(ByteOrder.nativeOrder());
//
//	        // create a floating point buffer from the ByteBuffer
//	        vertexBuffer = bb.asFloatBuffer();
//	        // add the coordinates to the FloatBuffer
//	        vertexBuffer.put(triangleCoords);
//	        // set the buffer to read the first coordinate
//	        vertexBuffer.position(0);
//	        
//	        int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER,
//                    vertexShaderCode);
//			int fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER,
//			                    fragmentShaderCode);
//			
//			// create empty OpenGL ES Program
//			mProgram = GLES20.glCreateProgram();
//			
//			// add the vertex shader to program
//			GLES20.glAttachShader(mProgram, vertexShader);
//			
//			// add the fragment shader to program
//			GLES20.glAttachShader(mProgram, fragmentShader);
//			
//			// creates OpenGL ES program executables
//			GLES20.glLinkProgram(mProgram);
//
//	    }
//	    
//	    private int mPositionHandle;
//
//	    private final int vertexCount = triangleCoords.length / COORDS_PER_VERTEX;
//	    private final int vertexStride = COORDS_PER_VERTEX * 4; // 4 bytes per vertex
//
//	    public void draw() {
//	        // Add program to OpenGL ES environment
//	        GLES20.glUseProgram(mProgram);
//
//	        // get handle to vertex shader's vPosition member
//	        mPositionHandle = GLES20.glGetAttribLocation(mProgram, "vPosition");
//
//	        // Enable a handle to the triangle vertices
//	        GLES20.glEnableVertexAttribArray(mPositionHandle);
//
//	        // Prepare the triangle coordinate data
//	        GLES20.glVertexAttribPointer(mPositionHandle, COORDS_PER_VERTEX,
//	                                     GLES20.GL_FLOAT, false,
//	                                     vertexStride, vertexBuffer);
//
//
//	        // Draw the triangle
//	        GLES20.glDrawArrays(GLES20.GL_TRIANGLES, 0, vertexCount);
//
//	        // Disable vertex array
//	        GLES20.glDisableVertexAttribArray(mPositionHandle);
//	    }
//	}
//	
//	public static int loadShader(int type, String shaderCode){
//
//	    // create a vertex shader type (GLES20.GL_VERTEX_SHADER)
//	    // or a fragment shader type (GLES20.GL_FRAGMENT_SHADER)
//	    int shader = GLES20.glCreateShader(type);
//
//	    // add the source code to the shader and compile it
//	    GLES20.glShaderSource(shader, shaderCode);
//	    GLES20.glCompileShader(shader);
//
//	    return shader;
//	}
	
	public class MyGLRenderer implements GLSurfaceView.Renderer {
//		private Triangle mTriangle;


	    public void onSurfaceCreated(GL10 unused, EGLConfig config) {
	        // Set the background frame color
	        GLES20.glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
//	        mTriangle = new Triangle();

	    }

	    public void onDrawFrame(GL10 unused) {
	        // Redraw background color
	        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
//	        mTriangle.draw();

	    }

	    public void onSurfaceChanged(GL10 unused, int width, int height) {
	        GLES20.glViewport(0, 0, width, height);
	    }
	}
}
