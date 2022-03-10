package com.ru7w1k.interleaved;

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

import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.opengl.GLUtils;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

    private final Context context;
    private GestureDetector gestureDetector;

    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;

    private int[] vaoCube = new int[1];
    private int[] vboCube = new int[1];

    private int mUniform;
    private int vUniform;
    private int pUniform;

    private int[] texture_marble = new int[1];
    private int samplerUniform;

    private int laUniform;
    private int kaUniform;
    private int ldUniform;
    private int kdUniform;
    private int lsUniform;
    private int ksUniform;
    private int shininessUniform;

    private int enableLightUniform;
    private int lightPositionUniform;

    private float[] lightAmbient  = new float[] { 0.5f, 0.5f, 0.5f, 1.0f };
    private float[] lightDiffuse  = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
    private float[] lightSpecular = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
    private float[] lightPosition = new float[] { 10.0f, 10.0f, 10.0f, 1.0f };

    private float[] materialAmbient  = new float[] { 0.5f, 0.5f, 0.5f, 1.0f };
    private float[] materialDiffuse  = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
    private float[] materialSpecular = new float[] { 1.0f, 1.0f, 1.0f, 1.0f };
    private float materialShininess = 128.0f;

    private float[] perspectiveProjectionMatrix = new float[16];

    private boolean bLight = false;
    private float angleCube;

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
        bLight = !bLight;
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
            "precision highp int;" +
            "in vec4 vPosition;" +
            "in vec4 vColor;" +
            "in vec3 vNormal;" +
            "in vec2 vTexcoord;" +
            "uniform mat4 u_m_matrix;" +
            "uniform mat4 u_v_matrix;" +
            "uniform mat4 u_p_matrix;" +
            "uniform vec4 u_light_position;" +
            "uniform int u_enable_light;" +
            "out vec3 tnorm;" +
            "out vec3 light_direction;" +
            "out vec3 viewer_vector;" +
            "out vec2 out_Texcoord;" +
            "out vec4 out_Color;" +
            "void main (void)" +
            "{" +
            "   if (u_enable_light == 1) " +
            "   { " +
            "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" +
            "       tnorm = mat3(u_v_matrix * u_m_matrix) * vNormal;" +
            "       light_direction = vec3(u_light_position - eye_coordinates);" +
            "       float tn_dot_ldir = max(dot(tnorm, light_direction), 0.0);" +
            "       viewer_vector = vec3(-eye_coordinates.xyz);" +
            "   }" +
            "   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPosition;" +
            "   out_Texcoord = vTexcoord;" +
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
            "precision highp int;" +
            "in vec2 out_Texcoord;" +
            "in vec4 out_Color;" +
            "in vec3 tnorm;" +
            "in vec3 light_direction;" +
            "in vec3 viewer_vector;" +
            "uniform vec3 u_la;" +
            "uniform vec3 u_ld;" +
            "uniform vec3 u_ls;" +
            "uniform vec3 u_ka;" +
            "uniform vec3 u_kd;" +
            "uniform vec3 u_ks;" +
            "uniform float u_shininess;" +
            "uniform int u_enable_light;" +
            "uniform sampler2D u_sampler;" +
            "out vec4 FragColor;" +
            "void main (void)" +
            "{" +
            "   vec3 phong_ads_light = vec3(1.0);" +
            "   if (u_enable_light == 1) " +
            "   { " +
            "       vec3 ntnorm = normalize(tnorm);" +
            "       vec3 nlight_direction = normalize(light_direction);" +
            "       vec3 nviewer_vector = normalize(viewer_vector);" +
            "       vec3 reflection_vector = reflect(-nlight_direction, ntnorm);" +
            "       float tn_dot_ldir = max(dot(ntnorm, nlight_direction), 0.0);" +
            "       vec3 ambient  = u_la * u_ka;" +
            "       vec3 diffuse  = u_ld * u_kd * tn_dot_ldir;" +
            "       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, nviewer_vector), 0.0), u_shininess);" +
            "       phong_ads_light = ambient + diffuse + specular;" +
            "   }" +
            "   vec4 tex = texture(u_sampler, out_Texcoord);" +
            "   FragColor = vec4((vec3(tex) * vec3(out_Color) * phong_ads_light), 1.0);" +
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
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.RMC_ATTRIBUTE_NORMAL, "vNormal");
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.RMC_ATTRIBUTE_TEXCOORD, "vTexcoord");

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
                szInfo = GLES32.glGetProgramInfoLog(shaderProgramObject);
                System.out.println("RMC: Program Linking: " + szInfo);
                uninitialize();
                System.exit(0);
            }
        }

        // get unifrom locations
        mUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_m_matrix");
        vUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_v_matrix");
        pUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_p_matrix");

        samplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_sampler");

        laUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_la");
        kaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ka");
        ldUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ld");
        kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kd");
        lsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ls");
        ksUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ks");
        shininessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_shininess");

        enableLightUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_enable_light");
        lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position");

        // pyramid Position
        final float[] cubeVertices = new float[] {
            /* Top */
             1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
            -1.0f,  1.0f, -1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
            -1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
             1.0f,  1.0f,  1.0f,    1.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,

            /* Bottom */
             1.0f, -1.0f,  1.0f,    0.0f, 1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,    0.0f, 1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
             1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 0.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,

            /* Front */
             1.0f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
            -1.0f,  1.0f,  1.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
             1.0f, -1.0f,  1.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,

            /* Back */
             1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,    0.0f, 1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
             1.0f,  1.0f, -1.0f,    0.0f, 1.0f, 1.0f,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,

            /* Right */
            1.0f,  1.0f, -1.0f,     1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
            1.0f,  1.0f,  1.0f,     1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
            1.0f, -1.0f,  1.0f,     1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
            1.0f, -1.0f, -1.0f,     1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,

            /* Left */
            -1.0f,  1.0f,  1.0f,    1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
            -1.0f,  1.0f, -1.0f,    1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,    1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
            -1.0f, -1.0f,  1.0f,    1.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 1.0f
        };

        //// cube /////////////////

        // create vao
        GLES32.glGenVertexArrays(1, vaoCube, 0);
        GLES32.glBindVertexArray(vaoCube[0]);

        // Cube vertices
        GLES32.glGenBuffers(1, vboCube, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboCube[0]);

        // 1. Allocate buffer directly from native memory
        ByteBuffer byteBufferCube = ByteBuffer.allocateDirect(cubeVertices.length * 4);

        // 2. Arrange the buffer in native byte order
        byteBufferCube.order(ByteOrder.nativeOrder());

        // 3. Create the float type buffer and convert it to float buffer
        FloatBuffer positionBufferCube = byteBufferCube.asFloatBuffer();

        // 4. put data in this COOKED buffer
        positionBufferCube.put(cubeVertices);

        // 5. set the array at 0th position of buffer
        positionBufferCube.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeVertices.length * 4, positionBufferCube, GLES32.GL_STATIC_DRAW);

        // cube position
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 11*4, 0*4);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_POSITION); 

        // Cube colors
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 11*4, 3*4);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_COLOR); 

        // Cube normal
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 11*4, 6*4);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_NORMAL); 

        // Cube texcoord
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 11*4, 9*4);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_TEXCOORD); 


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

        // enable texture
        GLES32.glEnable(GLES32.GL_TEXTURE_2D);
        texture_marble[0] = loadGLTexture(R.raw.marble);

        Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
    }

    private void resize(int width, int height) {
        System.out.println("RMC: resize called with: " + width + " " + height);
        if (height == 0)
        {
            height = 1;
        }

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
        float[] modelMatrix = new float[16];
        float[] viewMatrix = new float[16];

        //// Cube //////////////////////////////////////////

        // intialize above matrices to setIdentityM
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);

        // perform necessary transformations
        Matrix.translateM(translationMatrix, 0,
            0.0f, 0.0f, -6.0f);

        Matrix.setRotateM(rotationMatrix, 0,
            angleCube, 1.0f, 1.0f, 1.0f);

        // do necessary matrix multiplication
        Matrix.multiplyMM(modelMatrix, 0,
            modelMatrix, 0,
            translationMatrix, 0);

        Matrix.multiplyMM(modelMatrix, 0,
            modelMatrix, 0,
            rotationMatrix, 0);

        // send necessary matrices to shader in respective uniforms
        GLES32.glUniformMatrix4fv(mUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(vUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(pUniform, 1, false, perspectiveProjectionMatrix, 0);

        GLES32.glUniform3fv(laUniform, 1, lightAmbient, 0);
        GLES32.glUniform3fv(ldUniform, 1, lightDiffuse, 0);
        GLES32.glUniform3fv(lsUniform, 1, lightSpecular, 0);
        GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);

        GLES32.glUniform3fv(kaUniform, 1, materialAmbient, 0);
        GLES32.glUniform3fv(kdUniform, 1, materialDiffuse, 0);
        GLES32.glUniform3fv(ksUniform, 1, materialSpecular, 0);
        GLES32.glUniform1f(shininessUniform, materialShininess);

        if (bLight == true)
            GLES32.glUniform1i(enableLightUniform, 1);
        else
            GLES32.glUniform1i(enableLightUniform, 0);

        // bind with textures
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_marble[0]);
        GLES32.glUniform1i(samplerUniform, 0);

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

    private void uninitialize() {
        
        if (vboCube[0] != 0) {
            GLES32.glDeleteBuffers(1, vboCube, 0);
            vboCube[0] = 0;
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
    }

    private void update() {

        if (angleCube >= 360.0f)
        {
            angleCube = 0.0f;
        }
        else
        {
            angleCube += 2.0f;
        }
    }

    private int loadGLTexture(int imageFileResourceID) {
        int[] texture = new int[1];
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;
        Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), imageFileResourceID, options);

        GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 4);
        GLES32.glGenTextures(1, texture, 0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);

        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR);

        GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
        GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);

        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);
        return texture[0];
    }
}



