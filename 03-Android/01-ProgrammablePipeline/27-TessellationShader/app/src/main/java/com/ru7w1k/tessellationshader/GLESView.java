package com.ru7w1k.tessellationshader;

// added by me
import android.opengl.GLSurfaceView;              // SurfaceView with support of OpenGL
import android.opengl.GLES32;                     // OpenGL ES 3.2
import javax.microedition.khronos.opengles.GL10;  // OpenGL Extension for basic features of OpenGL ES
import javax.microedition.khronos.egl.EGLConfig;  // Embedded Graphics Library

import android.content.Context;                           // Context
import android.view.MotionEvent;                          // MotionEvent
import android.view.GestureDetector;                      // GestureDetector
import android.view.GestureDetector.OnGestureListener;    // OnGestureListener
import android.view.GestureDetector.OnDoubleTapListener;  // OnDoubleTapListener

import java.nio.ByteBuffer;   // ByteBuffer
import java.nio.ByteOrder;    // ByteOrder
import java.nio.FloatBuffer;  // FloatBuffer

import android.opengl.Matrix; // Matrix


public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

    private final Context context;
    private GestureDetector gestureDetector;

    private float gWidth = 0.0f;
    private float gHeight = 0.0f;

    private int vertexShaderObject;
    private int tessControlShaderObject;
    private int tessEvaluationShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;

    private int[] vao = new int[1];
    private int[] vbo = new int[1];
    private int mvpUniform;
    private int gNumberOfSegmentsUniform;
    private int gNumberOfStripsUniform;
    private int gLineColorUniform;

    private int gNumberOfLineSegments = 1;

    private float[] perspectiveProjectionMatrix = new float[16];

	public GLESView(Context drawingContext) {
		super(drawingContext);
		context = drawingContext;

        setEGLContextClientVersion(3);   // highest supported 3.x
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY); 

        gestureDetector = new GestureDetector(context, this, null, false);
        gestureDetector.setOnDoubleTapListener(this);
	}

    // handling 'onTouchEvent' is the most important
    // because it triggers all gesture and tap events
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        // code
        int eventaction = event.getAction();  // not required for now
        if (!gestureDetector.onTouchEvent(event))
            super.onTouchEvent(event);
        return(true);
    }

    // abstract method from OnDoubleTapEventListener so must be implemented
    @Override
    public boolean onDoubleTap(MotionEvent e) {
        
        return(true);
    }

    // abstract method from OnDoubleTapListener so must be implemented
    @Override
    public boolean onDoubleTapEvent(MotionEvent e) {
        // do not write anything here, because already written 'onDoubleTap'
        return(true);
    }

    // abstract method from OnDoubleTapListener so must be implemented
    @Override
    public boolean onSingleTapConfirmed(MotionEvent e) {
        return(true);
    }

    // abstract method from OnGestureListener so must implement
    @Override
    public boolean onDown(MotionEvent e) {
        // do not write anything here, because already written in 'onSingleTapConfirmed'
        return(true);
    }

    // abstract method from OnGestureListener so must be implemented
    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
        return(true);
    }

    // abstract method from OnGestureListener so must be implemented
    @Override
    public void onLongPress(MotionEvent e) {
    }

    // abstract method from OnGestureListener so must be implemented
    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
        uninitialize();
        System.exit(0);
        return(true);
    }

    // abstract method from OnGestureListener so must be implemented
    @Override
    public void onShowPress(MotionEvent e) {

    }

    // abstract method from OnGestureListener so must be implemented
    @Override
    public boolean onSingleTapUp(MotionEvent e) {
        if (e.getX() > gWidth / 2.0f) {
            // right half
            gNumberOfLineSegments++;
            if (gNumberOfLineSegments >= 50)
                gNumberOfLineSegments = 50;

        } else {
            // left half
            gNumberOfLineSegments--;
            if (gNumberOfLineSegments <= 0)
                gNumberOfLineSegments = 1;
        }

        return(true);
    }

    ///// implementation of GLSurfaceView.Renderer methods
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        String version = gl.glGetString(GL10.GL_VERSION);
        System.out.println("RMC: OpenGL Version: " + version);

        // version = gl.glGetString(GL10.GL_SHADING_LANGUAGE_VERSION);
        // System.out.println("RMC: Shading Language Version: " + version);

        String vendor = gl.glGetString(GL10.GL_VENDOR);
        System.out.println("RMC: Vendor: " + vendor);

        String renderer = gl.glGetString(GL10.GL_RENDERER);
        System.out.println("RMC: Renderer: " + renderer);

        initialize();
    }

    @Override
    public void onSurfaceChanged(GL10 unused, int width, int height) {
        resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 unused) {
        display();
    }

    /////////// Rendering Functions //////////////////////////////////////////////////// 

    private void initialize() {

        //// VERTEX SHADER ////////////////////////////////////////////////
        // create shader object
        vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

        // shader source code
        final String vertexShaderSourceCode = String.format(
            "#version 320 es" +
            "\n" +
            "precision highp float;" +
            "precision highp int;" +
            "in vec2 vPosition;" +
            "void main(void)" +
            "{" +
            "   gl_Position = vec4(vPosition, 0.0, 1.0);" +
            "}"
        );

        // attach shader source code to shader object
        GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);

        // compile shader source code
        GLES32.glCompileShader(vertexShaderObject);

        // compilation errors
        int[] iShaderCompileStatus = new int[1];
        int[] iInfoLogLength = new int[1];
        String szInfo = null;

        GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);

        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfo = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("RMC: Vertex Shader: " + szInfo);
                uninitialize();
                System.exit(0);
            }

        }

        //// TESSELLATION CONTROL SHADER ////////////////////////////////////////////////
        // create shader object
        tessControlShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_CONTROL_SHADER);

        // shader source code
        final String tessControlShaderSourceCode = String.format(
            "#version 320 es" +
            "\n" +
            "precision highp float;" +
            "precision highp int;" +
            "layout(vertices=4)out;" +
            "uniform int numberOfSegments;" +
            "uniform int numberOfStripes;" +
            "void main (void)" +
            "{" +
            "   gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" +
            "   gl_TessLevelOuter[0] = float(numberOfStripes);" +
            "   gl_TessLevelOuter[1] = float(numberOfSegments);" +
            "}"
        );

        // attach shader source code to shader object
        GLES32.glShaderSource(tessControlShaderObject, tessControlShaderSourceCode);

        // compile shader source code
        GLES32.glCompileShader(tessControlShaderObject);

        // compilation errors
        iShaderCompileStatus[0] = 0;
        iInfoLogLength[0] = 0;
        szInfo = null;

        GLES32.glGetShaderiv(tessControlShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);

        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(tessControlShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfo = GLES32.glGetShaderInfoLog(tessControlShaderObject);
                System.out.println("RMC: Tessellation Control Shader: " + szInfo);
                uninitialize();
                System.exit(0);
            }
        }

        //// TESSELLATION EVALUATION SHADER ////////////////////////////////////////////////
        // create shader object
        tessEvaluationShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_EVALUATION_SHADER);

        // shader source code
        final String tessEvaluationShaderSourceCode = String.format(
            "#version 320 es" +
            "\n" +
            "precision highp float;" +
            "precision highp int;" +
            "layout(isolines)in;" +
            "uniform mat4 u_mvp_matrix;" +
            "void main (void)" +
            "{" +
            "   float u = gl_TessCoord.x;" +
            "   vec3 p0 = gl_in[0].gl_Position.xyz;" +
            "   vec3 p1 = gl_in[1].gl_Position.xyz;" +
            "   vec3 p2 = gl_in[2].gl_Position.xyz;" +
            "   vec3 p3 = gl_in[3].gl_Position.xyz;" +
            "   float u1 = (1.0 - u);" +
            "   float u2 = u * u;" +
            "   float b3 = u2 * u;" +
            "   float b2 = 3.0 * u2 * u1;" +
            "   float b1 = 3.0 * u * u1 * u1;" +
            "   float b0 = u1 * u1 * u1;" +
            "   vec3 p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;" +
            "   gl_Position = u_mvp_matrix * vec4(p, 1.0);" +
            "}"
        );

        // attach shader source code to shader object
        GLES32.glShaderSource(tessEvaluationShaderObject, tessEvaluationShaderSourceCode);

        // compile shader source code
        GLES32.glCompileShader(tessEvaluationShaderObject);

        // compilation errors
        iShaderCompileStatus[0] = 0;
        iInfoLogLength[0] = 0;
        szInfo = null;

        GLES32.glGetShaderiv(tessEvaluationShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);

        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(tessEvaluationShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfo = GLES32.glGetShaderInfoLog(tessEvaluationShaderObject);
                System.out.println("RMC: Tessellation Evaluation Shader: " + szInfo);
                uninitialize();
                System.exit(0);
            }
        }

        //// FRAGMENT SHADER ////////////////////////////////////////////////
        // create shader object
        fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

        // shader source code
        final String fragmentShaderSourceCode = String.format(
            "#version 320 es" +
            "\n" +
            "precision highp float;" +
            "uniform vec4 lineColor;" +
            "out vec4 FragColor;" +
            "void main(void)" +
            "{" +
            "   FragColor = lineColor;" +
            "}"
        );

        // attach shader source code to shader object
        GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);

        // compile shader source code
        GLES32.glCompileShader(fragmentShaderObject);

        // compilation errors
        iShaderCompileStatus[0] = 0;
        iInfoLogLength[0] = 0;
        szInfo = null;

        GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);

        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfo = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("RMC: Fragment Shader: " + szInfo);
                uninitialize();
                System.exit(0);
            }
        }

        // create shader program object
        shaderProgramObject = GLES32.glCreateProgram();

        // attach vertex shader to shader program
        GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);

        // attach tessellation control shader to shader program
        GLES32.glAttachShader(shaderProgramObject, tessControlShaderObject);

        // attach tessellation evaluation shader to shader program
        GLES32.glAttachShader(shaderProgramObject, tessEvaluationShaderObject);

        // attach fragment shader to shader program
        GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

        // pre-linking binding to vertex attribute
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.RMC_ATTRIBUTE_POSITION, "vPosition");

        // link the shader program
        GLES32.glLinkProgram(shaderProgramObject);

        // linking errors
        int[] iProgramLinkStatus = new int[1];
        iInfoLogLength[0] = 0;
        szInfo = null;

        GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iProgramLinkStatus, 0);
        if (iProgramLinkStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0)
            {
                szInfo = GLES32.glGetShaderInfoLog(shaderProgramObject);
                System.out.println("RMC: Program Linking: " + szInfo);
                uninitialize();
                System.exit(0);
            }
        }

        // get unifrom locations
        mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
        gNumberOfSegmentsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "numberOfSegments");
        gNumberOfStripsUniform= GLES32.glGetUniformLocation(shaderProgramObject, "numberOfStripes");
        gLineColorUniform = GLES32.glGetUniformLocation(shaderProgramObject, "lineColor");


        System.out.println("RMC: Uniform mvp: " + mvpUniform);
        System.out.println("RMC: Uniform gNumberOfSegmentsUniform: " + gNumberOfSegmentsUniform);
        System.out.println("RMC: Uniform gNumberOfStripsUniform: " + gNumberOfStripsUniform);
        System.out.println("RMC: Uniform gLineColorUniform: " + gLineColorUniform);


        // vertex array
        final float[] vertices = new float[] {
            -1.0f, -1.0f,
            -0.5f, 1.0f,
            0.5f, -1.0f,
            1.0f, 1.0f
        };

        // create vao
        GLES32.glGenVertexArrays(1, vao, 0);
        GLES32.glBindVertexArray(vao[0]);

        GLES32.glGenBuffers(1, vbo, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo[0]);

        // 1. Allocate buffer directly from native memory
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(vertices.length * 4);

        // 2. Arrange the buffer in native byte order
        byteBuffer.order(ByteOrder.nativeOrder());

        // 3. Create the float type buffer and convert it to float buffer
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();

        // 4. put data in this COOKED buffer
        positionBuffer.put(vertices);

        // 5. set the array at 0th position of buffer
        positionBuffer.position(0);

        ////
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, vertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_POSITION, 2, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_POSITION);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glBindVertexArray(0);

        //////////////////////////////////////////////////////////////////////

        // clear the depth buffer
        GLES32.glClearDepthf(1.0f);

        // clear the screen by OpenGL
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // enable depth
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
    }

    private void resize(int width, int height) {
        System.out.println("RMC: resize called with: " + width + " " + height);
        if (height == 0)
        {
            height = 1;
        }

        GLES32.glViewport(0, 0, width, height);
        gWidth = width;
        gHeight = height;

        Matrix.perspectiveM(perspectiveProjectionMatrix, 0,
            45.0f,
            (float)width / (float)height,
            0.1f, 100.0f);
    }

    private void display() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        // use shader program
        GLES32.glUseProgram(shaderProgramObject);

        //declaration of matrices
        float[] translationMatrix = new float[16];
        float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        // intialize above matrices to identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        // perform necessary transformations
        Matrix.translateM(translationMatrix, 0,
            0.0f, 0.0f, -4.0f);

        // do necessary matrix multiplication
        Matrix.multiplyMM(modelViewMatrix, 0,
            modelViewMatrix, 0,
            translationMatrix, 0);

        Matrix.multiplyMM(modelViewProjectionMatrix, 0,
            perspectiveProjectionMatrix, 0,
            modelViewMatrix, 0);

        // send necessary matrices to shader in respective uniforms
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        GLES32.glUniform1i(gNumberOfSegmentsUniform, gNumberOfLineSegments);
        GLES32.glUniform1i(gNumberOfStripsUniform, 1);

        if (gNumberOfLineSegments == 50)
            GLES32.glUniform4f(gLineColorUniform, 0.0f, 1.0f, 0.0f, 1.0f);
        else if (gNumberOfLineSegments == 1)
            GLES32.glUniform4f(gLineColorUniform, 1.0f, 0.0f, 0.0f, 1.0f);
        else
            GLES32.glUniform4f(gLineColorUniform, 1.0f, 1.0f, 0.0f, 1.0f);

        // bind with vao (this will avoid many binding to vbo)
        GLES32.glBindVertexArray(vao[0]);  

        // bind with textures

        GLES32.glPatchParameteri(GLES32.GL_PATCH_VERTICES, 4);

        GLES32.glDrawArrays(GLES32.GL_PATCHES, 0, 4);
        //GLES32.glDrawArrays(GLES32.GL_LINES, 0, 4);


        // unbind vao
        GLES32.glBindVertexArray(0);

        // unuse program
        GLES32.glUseProgram(0);
        requestRender();  // ~ swapBuffers
    }

    private void uninitialize() {
        if (vbo[0] != 0) {
            GLES32.glDeleteBuffers(1, vbo, 0);
            vbo[0] = 0;
        }

        if (vao[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vao, 0);
            vao[0] = 0;
        }

        if (shaderProgramObject != 0) {
            int[] shaderCount = new int[1];
            int shaderNumber;

            GLES32.glUseProgram(shaderProgramObject);
            GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_ATTACHED_SHADERS, shaderCount, 0);

            int[] shaders = new int[shaderCount[0]];

            GLES32.glGetAttachedShaders(shaderProgramObject, shaderCount[0], shaderCount, 0, shaders, 0);
            
            for (shaderNumber = 0; shaderNumber < shaderCount[0]; shaderNumber++) {
                // detach shader
                GLES32.glDetachShader(shaderProgramObject, shaders[shaderNumber]);

                // delete shader
                GLES32.glDeleteShader(shaders[shaderNumber]);
                shaders[shaderNumber] = 0;
            }

            GLES32.glUseProgram(0);
            GLES32.glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }
    }
}



