package com.ru7w1k.bnwpyramid;

import android.content.Context;

import android.opengl.GLES32;         // OpenGLES 3.2
import android.opengl.GLSurfaceView;
import android.opengl.Matrix;

import javax.microedition.khronos.opengles.GL10; // for OpenGLES 1.0
import javax.microedition.khronos.egl.EGLConfig;

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;

import java.nio.ByteOrder;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnDoubleTapListener, OnGestureListener {

    private final Context context;  // final ~ const
    private GestureDetector gestureDetector;

    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;
    private int mvpUniform;

    private int[] vao = new int[1];
    private int[] vbo = new int[1];

    private float[] perspectiveProjectionMatrix = new float[16];

    private float anglePyramid = 0.0f;

    public GLESView(Context drawingContext) {
        super(drawingContext);
        context = drawingContext;

        // set major OpenGLES version via EGLContext
        setEGLContextClientVersion(3);

        // set Renderer for drawing on the GLESView
        setRenderer(this);

        // set when to render the view
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY); // ~InvalidateRect()

        // parameter 2 and 3 decide: listener or handler 
        gestureDetector = new GestureDetector(context, this, null, false);
        gestureDetector.setOnDoubleTapListener(this);
    }

    // ~ WndProc()
    @Override 
    public boolean onTouchEvent(MotionEvent event) {
        // not used now, but require in event driven apps
        int eventaction = event.getAction();

        // if the event is not related to me, pass it to super class
        if (!gestureDetector.onTouchEvent(event)) {
            super.onTouchEvent(event);
        }

        // else that event will be handled using following 9 methods from this class
        return(true);
    }

    // methods from GLSurfaceView.Renderer
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        String glesVersion = gl.glGetString(GL10.GL_VERSION);
        String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);

        System.out.println("RMC: OpenGLES Version: " + glesVersion);
        System.out.println("RMC: GLSL Version: " + glslVersion);

        initialize(gl);
    }

    @Override
    public void onSurfaceChanged(GL10 unused, int width, int height) {
        resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 unused) {
        update();
        draw();
    }

    // methods from onDoubleTapListener
    @Override
    public boolean onDoubleTap(MotionEvent e) {
        return(true);
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent e) {
        return(true);
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent e) {
        return(true);
    }

    // methods from onGestureListener
    @Override
    public boolean onDown(MotionEvent e) {
        return(true);
    }

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
        return(true);
    }

    @Override
    public void onLongPress(MotionEvent e) {
    }

    @Override
    public void onShowPress(MotionEvent e) {
    }

    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
        uninitialize();
        System.exit(0);
        return(true);
    }

    @Override
    public boolean onSingleTapUp(MotionEvent e) {
        return(true);
    }

    // private methods for OpenGLES drawing
    private void initialize(GL10 gl) {
        // vertex shader        
        final String vertexShaderSourceCode = String.format(
            "#version 320 es \n" +

            "in vec4 vPosition; \n" +
            "uniform mat4 u_mvpMatrix; \n" +

            "void main (void) \n" +
            "{ \n" +
            "	gl_Position = u_mvpMatrix * vPosition; \n" +
            "} \n"
        );

        // create shader and provide source code
        vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);

        // compile shader and check errors
        int[] iShaderCompileStatus = new int[1];
        int[] iInfoLogLength       = new int[1];
        String szInfoLog = null;

        GLES32.glCompileShader(vertexShaderObject);
        GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
                System.out.println("RMC: Vertex Shader Compile Log: \n" + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // fragment shader
        final String fragmentShaderSourceCode = String.format(
            "#version 320 es \n" +
            "precision highp float; \n" +

            "out vec4 FragColor; \n" +

            "void main (void) \n" +
            "{ \n" +
            "	FragColor = vec4(1.0, 1.0, 1.0, 1.0); \n" +
            "} \n"
        );

        // create shader and provide source code
        fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
        GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);

        // compile shader and check errors
        iShaderCompileStatus[0] = 0;
        iInfoLogLength[0]       = 0;
        szInfoLog = null;

        GLES32.glCompileShader(fragmentShaderObject);
        GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
                System.out.println("RMC: Fragment Shader Compile Log: \n" + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // shader program
        shaderProgramObject = GLES32.glCreateProgram();

        // attach shaders 
        GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
        GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

        // pre-linking binding to vertex attribute
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.RMC_ATTRIBUTE_POSITION, "vPosition");

        // link shader program and check errors
        int[] iShaderProgramLinkStatus = new int[1];
        iInfoLogLength[0] = 0;
        szInfoLog = null;

        GLES32.glLinkProgram(shaderProgramObject);
        GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
        if (iShaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
                System.out.println("RMC: Program Compile Log: \n" + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // get uniforms
        mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvpMatrix");

        // vertex array
        final float triangleVertices[] = new float[] {
            /* Front */
             0.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f,
             1.0f, -1.0f, 1.0f,

            /* Right */
            0.0f,  1.0f,  0.0f,
            1.0f, -1.0f,  1.0f,
            1.0f, -1.0f, -1.0f,

            /* Left */
             0.0f,  1.0f,  0.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,

            /* Back */
             0.0f,  1.0f,  0.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
        };

        // create vao
        GLES32.glGenVertexArrays(1, vao, 0);
        GLES32.glBindVertexArray(vao[0]);

        // create vbo
        GLES32.glGenBuffers(1, vbo, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo[0]);

        // create native buffer to pass to OpenGLES
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(triangleVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
        positionBuffer.put(triangleVertices);
        positionBuffer.position(0);

        // pass to OpenGL buffer
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleVertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_POSITION);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glBindVertexArray(0);

        // clear color
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // enable depth
        GLES32.glClearDepthf(1.0f);

        // clear the depth buffer
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
    }

    private void resize(int width, int height) {
        if (height <= 0) {
            height = 1;
        }

        GLES32.glViewport(0, 0, width, height);

        Matrix.perspectiveM(perspectiveProjectionMatrix, 0,
                45.0f,
                (float)width/(float)height,
                0.1f, 100.0f
            );
    }

    private void draw() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        // start using OpenGL program object
        GLES32.glUseProgram(shaderProgramObject);

        //declaration of matrices
        float[] translateMatrix = new float[16];
        float[] rotateMatrix = new float[16];
        float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        // intialize above matrices to identity
        Matrix.setIdentityM(translateMatrix, 0);
        Matrix.setIdentityM(rotateMatrix, 0);
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        // translation transformation
        Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -5.0f);
        Matrix.rotateM(rotateMatrix, 0, rotateMatrix, 0, anglePyramid, 0.0f, 1.0f, 0.0f);

        // do necessary matrix multiplication
        Matrix.multiplyMM(modelViewMatrix, 0,
            translateMatrix, 0,
            rotateMatrix, 0);

        Matrix.multiplyMM(modelViewProjectionMatrix, 0,
            perspectiveProjectionMatrix, 0,
            modelViewMatrix, 0);

        // send necessary to shader in respective uniforms
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        // bind with vao (this will avoid many binding to vbo_vertex)
        GLES32.glBindVertexArray(vao[0]);  

        // draw necessary scene
        GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);

        // unbind vao
        GLES32.glBindVertexArray(0);

        // stop using OpenGL program object
        GLES32.glUseProgram(0);

        // ~SwapBuffers()
        requestRender();
    }

    private void update() {
        anglePyramid += 1.0f;
        if (anglePyramid >= 360.0f)
            anglePyramid = 0.0f;
    }

    private void uninitialize() {
        if (vao[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vao, 0);
            vao[0] = 0;
        }

        if (vbo[0] != 0) {
            GLES32.glDeleteBuffers(1, vbo, 0);
            vbo[0] = 0;
        }

        if (shaderProgramObject != 0) {
            if (vertexShaderObject != 0) {
                GLES32.glDetachShader(shaderProgramObject, vertexShaderObject);
                GLES32.glDeleteShader(vertexShaderObject);
                vertexShaderObject = 0;
            }

            if (fragmentShaderObject != 0) {
                GLES32.glDetachShader(shaderProgramObject, fragmentShaderObject);
                GLES32.glDeleteShader(fragmentShaderObject);
                fragmentShaderObject = 0;
            }

            GLES32.glDeleteProgram(shaderProgramObject);
            shaderProgramObject = 0;
        }
    }
}
