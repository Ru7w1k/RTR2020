package com.ru7w1k.framebuffer;

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
import java.nio.IntBuffer;  // IntBuffer

import android.opengl.Matrix; // Matrix


public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

    private final Context context;
    private GestureDetector gestureDetector;

    private int winWidth;
    private int winHeight;

    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;

    private int vertexShaderObject_FBO;
    private int fragmentShaderObject_FBO;
    private int shaderProgramObject_FBO;

    private int[] vaoPyramid = new int[1];
    private int[] vaoCube = new int[1];
    private int[] vboPositionPyramid = new int[1];
    private int[] vboColorPyramid = new int[1];
    private int[] vboPositionCube = new int[1];
    private int[] vboTexcoordCube = new int[1];
    private int mvpUniform;

    private float[] perspectiveProjectionMatrix = new float[16];

    private int mvpUniform_FBO;
    private int samplerUniform_FBO;
    private int[] FBO = new int[1];
    private int[] texture_FBO = new int[1];
    private int[] rboDepth = new int[1];

    private float anglePyramid = 0.0f;
    private float angleCube = 0.0f;

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
            "in vec4 vPosition;" +
            "in vec4 vColor;" +
            "out vec4 out_Color;" +
            "uniform mat4 u_mvp_matrix;" +
            "void main(void)" +
            "{" +
            "   gl_Position = u_mvp_matrix * vPosition;" +
            "   out_Color = vColor;" +
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

        //// FRAGMENT SHADER ////////////////////////////////////////////////
        // create shader object
        fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

        // shader source code
        final String fragmentShaderSourceCode = String.format(
            "#version 320 es" +
            "\n" +
            "precision highp float;" +
            "in vec4 out_Color;" +
            "out vec4 FragColor;" +
            "void main(void)" +
            "{" +
            "   FragColor = out_Color;" +
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

        // attach fragment shader to shader program
        GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

        // pre-linking binding to vertex attribute
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.RMC_ATTRIBUTE_POSITION, "vPosition");
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.RMC_ATTRIBUTE_COLOR, "vColor");

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

        //////////// DRAW TO TEXTURE ////////////////////////////////////////////////

        //// VERTEX SHADER ////////////////////////////////////////////////
        // create shader object
        vertexShaderObject_FBO = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);

        // shader source code
        final String vertexShaderSourceCode_FBO = String.format(
            "#version 320 es" +
            "\n" +
            "in vec4 vPosition;" +
            "in vec2 vTexcoord;" +
            "out vec2 out_Texcoord;" +
            "uniform mat4 u_mvp_matrix;" +
            "void main(void)" +
            "{" +
            "   gl_Position = u_mvp_matrix * vPosition;" +
            "   out_Texcoord = vTexcoord;" +
            "}"
        );

        // attach shader source code to shader object
        GLES32.glShaderSource(vertexShaderObject_FBO, vertexShaderSourceCode_FBO);

        // compile shader source code
        GLES32.glCompileShader(vertexShaderObject_FBO);

        // compilation errors
        iShaderCompileStatus = new int[1];
        iInfoLogLength = new int[1];
        szInfo = null;

        GLES32.glGetShaderiv(vertexShaderObject_FBO, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);

        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(vertexShaderObject_FBO, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfo = GLES32.glGetShaderInfoLog(vertexShaderObject_FBO);
                System.out.println("RMC: Vertex Shader FBO: " + szInfo);
                uninitialize();
                System.exit(0);
            }

        }

        //// FRAGMENT SHADER ////////////////////////////////////////////////
        // create shader object
        fragmentShaderObject_FBO = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

        // shader source code
        final String fragmentShaderSourceCode_FBO = String.format(
            "#version 320 es" +
            "\n" +
            "precision highp float;" +
            "in vec2 out_Texcoord;" +
            "uniform sampler2D u_sampler;" +
            "out vec4 FragColor;" +
            "void main(void)" +
            "{" +
            "   FragColor = texture(u_sampler, out_Texcoord);" +
            "}"
        );

        // attach shader source code to shader object
        GLES32.glShaderSource(fragmentShaderObject_FBO, fragmentShaderSourceCode_FBO);

        // compile shader source code
        GLES32.glCompileShader(fragmentShaderObject_FBO);

        // compilation errors
        iShaderCompileStatus[0] = 0;
        iInfoLogLength[0] = 0;
        szInfo = null;

        GLES32.glGetShaderiv(fragmentShaderObject_FBO, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);

        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(fragmentShaderObject_FBO, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfo = GLES32.glGetShaderInfoLog(fragmentShaderObject_FBO);
                System.out.println("RMC: Fragment Shader FBO: " + szInfo);
                uninitialize();
                System.exit(0);
            }
        }

        // create shader program object
        shaderProgramObject_FBO = GLES32.glCreateProgram();

        // attach vertex shader to shader program
        GLES32.glAttachShader(shaderProgramObject_FBO, vertexShaderObject_FBO);

        // attach fragment shader to shader program
        GLES32.glAttachShader(shaderProgramObject_FBO, fragmentShaderObject_FBO);

        // pre-linking binding to vertex attribute
        GLES32.glBindAttribLocation(shaderProgramObject_FBO, GLESMacros.RMC_ATTRIBUTE_POSITION, "vPosition");
        GLES32.glBindAttribLocation(shaderProgramObject_FBO, GLESMacros.RMC_ATTRIBUTE_TEXCOORD, "vTexcoord");

        // link the shader program
        GLES32.glLinkProgram(shaderProgramObject_FBO);

        // linking errors
        iProgramLinkStatus = new int[1];
        iInfoLogLength[0] = 0;
        szInfo = null;

        GLES32.glGetProgramiv(shaderProgramObject_FBO, GLES32.GL_LINK_STATUS, iProgramLinkStatus, 0);
        if (iProgramLinkStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(shaderProgramObject_FBO, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0)
            {
                szInfo = GLES32.glGetShaderInfoLog(shaderProgramObject_FBO);
                System.out.println("RMC: FBO Program Linking: " + szInfo);
                uninitialize();
                System.exit(0);
            }
        }

        // get unifrom locations
        mvpUniform_FBO = GLES32.glGetUniformLocation(shaderProgramObject_FBO, "u_mvp_matrix");
        samplerUniform_FBO = GLES32.glGetUniformLocation(shaderProgramObject_FBO, "u_sampler");

        /////////////////////////////////////////////////////////////////////////////

        // vertex array
        final float[] pyramidVertices = new float[] {
            /* Front */
             0.0f,  1.0f,  0.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,

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

        final float[] cubeVertices = new float[] {                 
            /* Top */
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,

            /* Bottom */
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            /* Front */
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,

            /* Back */
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,

             /* Right */
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,

            /* Left */
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f
        };

        final float[] pyramidColors = new float[] {
            /* Front */
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,

            /* Right */
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f,

            /* Left */
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f,

            /* Back */
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        };

        final float[] cubeTexcoords = new float[] {
            /* Top */
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,

            /* Bottom */
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,

            /* Front */
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,

            /* Back */
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,

            /* Right */
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,

            /* Left */
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        //// PYRAMID ////

        // create vao
        GLES32.glGenVertexArrays(1, vaoPyramid, 0);
        GLES32.glBindVertexArray(vaoPyramid[0]);

        // vertex position
        GLES32.glGenBuffers(1, vboPositionPyramid, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboPositionPyramid[0]);

        // 1. Allocate buffer directly from native memory
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(pyramidVertices.length * 4);

        // 2. Arrange the buffer in native byte order
        byteBuffer.order(ByteOrder.nativeOrder());

        // 3. Create the float type buffer and convert it to float buffer
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();

        // 4. put data in this COOKED buffer
        positionBuffer.put(pyramidVertices);

        // 5. set the array at 0th position of buffer
        positionBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidVertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_POSITION);

        // vertex colors
        GLES32.glGenBuffers(1, vboColorPyramid, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboColorPyramid[0]);

        // 1. Allocate buffer directly from native memory
        byteBuffer = ByteBuffer.allocateDirect(pyramidColors.length * 4);

        // 2. Arrange the buffer in native byte order
        byteBuffer.order(ByteOrder.nativeOrder());

        // 3. Create the float type buffer and convert it to float buffer
        FloatBuffer colorBuffer = byteBuffer.asFloatBuffer();

        // 4. put data in this COOKED buffer
        colorBuffer.put(pyramidColors);

        // 5. set the array at 0th position of buffer
        colorBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramidColors.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_COLOR);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glBindVertexArray(0);

        //// CUBE ////

        // create vao
        GLES32.glGenVertexArrays(1, vaoCube, 0);
        GLES32.glBindVertexArray(vaoCube[0]);

        // vertex position
        GLES32.glGenBuffers(1, vboPositionCube, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboPositionCube[0]);

        // 1. Allocate buffer directly from native memory
        byteBuffer = ByteBuffer.allocateDirect(cubeVertices.length * 4);

        // 2. Arrange the buffer in native byte order
        byteBuffer.order(ByteOrder.nativeOrder());

        // 3. Create the float type buffer and convert it to float buffer
        positionBuffer = byteBuffer.asFloatBuffer();

        // 4. put data in this COOKED buffer
        positionBuffer.put(cubeVertices);

        // 5. set the array at 0th position of buffer
        positionBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeVertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_POSITION);

        // vertex colors
        GLES32.glGenBuffers(1, vboTexcoordCube, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboTexcoordCube[0]);

        // 1. Allocate buffer directly from native memory
        byteBuffer = ByteBuffer.allocateDirect(cubeTexcoords.length * 4);

        // 2. Arrange the buffer in native byte order
        byteBuffer.order(ByteOrder.nativeOrder());

        // 3. Create the float type buffer and convert it to float buffer
        FloatBuffer texcoordBuffer = byteBuffer.asFloatBuffer();

        // 4. put data in this COOKED buffer
        texcoordBuffer.put(cubeTexcoords);

        // 5. set the array at 0th position of buffer
        texcoordBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeTexcoords.length * 4, texcoordBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_TEXCOORD);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glBindVertexArray(0);

        ////// FRAMEBUFFER STUFF /////////////////////////////////////////////

        GLES32.glGenFramebuffers(1, FBO, 0);
        GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, FBO[0]);

        GLES32.glGenTextures(1, texture_FBO, 0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_FBO[0]);
        GLES32.glTexStorage2D(GLES32.GL_TEXTURE_2D, 1, GLES32.GL_RGBA8, 1024, 1024);

        // turn off mipmaps
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);

        GLES32.glFramebufferTexture(GLES32.GL_FRAMEBUFFER, GLES32.GL_COLOR_ATTACHMENT0, texture_FBO[0], 0);

        final int[] draw_buffers = new int[] {
            GLES32.GL_COLOR_ATTACHMENT0
        };

        // 1. Allocate buffer directly from native memory
        byteBuffer = ByteBuffer.allocateDirect(draw_buffers.length * 4);

        // 2. Arrange the buffer in native byte order
        byteBuffer.order(ByteOrder.nativeOrder());

        // 3. Create the int type buffer and convert it to int buffer
        IntBuffer drawBuffer = byteBuffer.asIntBuffer();

        // 4. put data in this COOKED buffer
        drawBuffer.put(draw_buffers);

        // 5. set the array at 0th position of buffer
        drawBuffer.position(0);

        GLES32.glGenRenderbuffers(1, rboDepth, 0);
        GLES32.glBindRenderbuffer(GLES32.GL_RENDERBUFFER, rboDepth[0]);
        GLES32.glRenderbufferStorage(GLES32.GL_RENDERBUFFER, GLES32.GL_DEPTH_COMPONENT16, 1024, 1024);

        GLES32.glFramebufferRenderbuffer(GLES32.GL_FRAMEBUFFER, GLES32.GL_DEPTH_ATTACHMENT, GLES32.GL_RENDERBUFFER, rboDepth[0]);

        GLES32.glDrawBuffers(1, drawBuffer);

        // int check = GLES32.glCheckFramebufferStatus(GLES32.GL_FRAMEBUFFER);
        // System.out.println("RMC: Framebuffer status: ");
        // System.out.println("RMC: Complete" + GLES32.GL_FRAMEBUFFER_COMPLETE);
        // System.out.println("RMC: undefined" + GLES32.GL_FRAMEBUFFER_UNDEFINED);
        // System.out.println("RMC: incomplete attachment" + GLES32.GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
        // System.out.println("RMC: incomplete missing attchment" + GLES32.GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
        // System.out.println("RMC: unsupported" + GLES32.GL_FRAMEBUFFER_UNSUPPORTED);
        // System.out.println("RMC: got" + check);

        GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);
        System.out.println("RMC: Added framebuffer facility...");

        //////////////////////////////////////////////////////////////////////

        // clear the depth buffer
        GLES32.glClearDepthf(1.0f);

        // clear the screen by OpenGL
        GLES32.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

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

        winWidth = width;
        winHeight = height;

        GLES32.glViewport(0, 0, width, height);

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
        float[] rotationMatrix = new float[16];
        float[] scaleMatrix = new float[16];
        float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];
        float[] perspectiveProjectionFBO = new float[16];

        //// Pyramid //////////////////////////////////////

        // intialize above matrices to identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);
        Matrix.setIdentityM(scaleMatrix, 0);
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
        Matrix.setIdentityM(perspectiveProjectionFBO, 0);

        GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, FBO[0]);
        GLES32.glViewport(0, 0, 1024, 1024);

        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);
    
        GLES32.glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        // perform necessary transformations
        Matrix.translateM(translationMatrix, 0,
            0.0f, 0.0f, -5.0f);

        Matrix.setRotateM(rotationMatrix, 0,
            anglePyramid, 0.0f, 1.0f, 0.0f);

        // do necessary matrix multiplication
        Matrix.multiplyMM(modelViewMatrix, 0,
            modelViewMatrix, 0,
            translationMatrix, 0);

        Matrix.multiplyMM(modelViewMatrix, 0,
            modelViewMatrix, 0,
            rotationMatrix, 0);

        Matrix.perspectiveM(perspectiveProjectionFBO, 0,
            45.0f,
            1.0f,
            0.1f, 100.0f);

        Matrix.multiplyMM(modelViewProjectionMatrix, 0,
            perspectiveProjectionFBO, 0,
            modelViewMatrix, 0);

        // send necessary matrices to shader in respective uniforms
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        // bind with vao (this will avoid many binding to vbo)
        GLES32.glBindVertexArray(vaoPyramid[0]);  

        // bind with textures

        // draw necessary scene
        GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 12);

        // unbind vao
        GLES32.glBindVertexArray(0);

        // unuse program
        GLES32.glUseProgram(0);

        // unbind FBO
        GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);

        GLES32.glViewport(0, 0, winWidth, winHeight);
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

        ////////////////////////////////////////////////////

        //// Cube //////////////////////////////////////////

        GLES32.glUseProgram(shaderProgramObject_FBO);

        // intialize above matrices to setIdentityM
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);
        Matrix.setIdentityM(scaleMatrix, 0);
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        // perform necessary transformations
        Matrix.translateM(translationMatrix, 0,
            0.0f, 0.0f, -5.0f);

        Matrix.setRotateM(rotationMatrix, 0,
            angleCube, 1.0f, 1.0f, 1.0f);

        Matrix.scaleM(scaleMatrix, 0,
            0.8f, 0.8f, 0.8f);

        // do necessary matrix multiplication
        Matrix.multiplyMM(modelViewMatrix, 0,
            modelViewMatrix, 0,
            translationMatrix, 0);

        Matrix.multiplyMM(modelViewMatrix, 0,
            modelViewMatrix, 0,
            rotationMatrix, 0);

        Matrix.multiplyMM(modelViewMatrix, 0,
            modelViewMatrix, 0,
            scaleMatrix, 0);

        Matrix.multiplyMM(modelViewProjectionMatrix, 0,
            perspectiveProjectionMatrix, 0,
            modelViewMatrix, 0);

        // send necessary matrices to shader in respective uniforms
        GLES32.glUniformMatrix4fv(mvpUniform_FBO, 1, false, modelViewProjectionMatrix, 0);

        // use fbo as texture
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_FBO[0]);
        GLES32.glUniform1i(samplerUniform_FBO, 0);

        // bind with vao (this will avoid many binding to vbo)
        GLES32.glBindVertexArray(vaoCube[0]);  

        // bind with textures

        // draw necessary scene
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,  0, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,  4, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN,  8, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

        // unbind vao
        GLES32.glBindVertexArray(0);

        ////////////////////////////////////////////////////

        // unuse program
        GLES32.glUseProgram(0);
        update();
        requestRender();  // ~ swapBuffers
    }

    private void update() {
        if (anglePyramid >= 360.0f)
        {
            anglePyramid = 0.0f;
        }
        else
        {
            anglePyramid += 1.0f;
        }

        if (angleCube >= 360.0f)
        {
            angleCube = 0.0f;
        }
        else
        {
            angleCube += 1.0f;
        }
    }

    private void uninitialize() {
        if (vboPositionPyramid[0] != 0) {
            GLES32.glDeleteBuffers(1, vboPositionPyramid, 0);
            vboPositionPyramid[0] = 0;
        }

        if (vboColorPyramid[0] != 0) {
            GLES32.glDeleteBuffers(1, vboColorPyramid, 0);
            vboColorPyramid[0] = 0;
        }

        if (vboPositionCube[0] != 0) {
            GLES32.glDeleteBuffers(1, vboPositionCube, 0);
            vboPositionCube[0] = 0;
        }

        if (vboTexcoordCube[0] != 0) {
            GLES32.glDeleteBuffers(1, vboTexcoordCube, 0);
            vboTexcoordCube[0] = 0;
        }

        if (vaoPyramid[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vaoPyramid, 0);
            vaoPyramid[0] = 0;
        }

        if (vaoCube[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vaoCube, 0);
            vaoCube[0] = 0;
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

        if (shaderProgramObject_FBO != 0) {
            int[] shaderCount = new int[1];
            int shaderNumber;

            GLES32.glUseProgram(shaderProgramObject_FBO);
            GLES32.glGetProgramiv(shaderProgramObject_FBO, GLES32.GL_ATTACHED_SHADERS, shaderCount, 0);

            int[] shaders = new int[shaderCount[0]];

            GLES32.glGetAttachedShaders(shaderProgramObject_FBO, shaderCount[0], shaderCount, 0, shaders, 0);
            
            for (shaderNumber = 0; shaderNumber < shaderCount[0]; shaderNumber++) {
                // detach shader
                GLES32.glDetachShader(shaderProgramObject_FBO, shaders[shaderNumber]);

                // delete shader
                GLES32.glDeleteShader(shaders[shaderNumber]);
                shaders[shaderNumber] = 0;
            }

            GLES32.glUseProgram(0);
            GLES32.glDeleteProgram(shaderProgramObject_FBO);
            shaderProgramObject_FBO = 0;
        }
    }
}



