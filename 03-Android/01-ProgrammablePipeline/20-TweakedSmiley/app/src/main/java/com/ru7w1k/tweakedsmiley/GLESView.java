package com.ru7w1k.tweakedsmiley;

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

import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.opengl.GLUtils;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnDoubleTapListener, OnGestureListener {

    private final Context context;  // final ~ const
    private GestureDetector gestureDetector;

    private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;
    private int mvpUniform;
    private int samplerUniform;
    private int textureUniform;

    private int[] vao = new int[1];
    private int[] vboPos = new int[1];
    private int[] vboTex = new int[1];

    private int[] texture_smiley = new int[1];
    private float[] perspectiveProjectionMatrix = new float[16];
    private int tapCount = 0;

    private FloatBuffer floatBuffer;

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
        tapCount++;
        if (tapCount >= 5) {
            tapCount = 0;
        }
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
            "in vec2 vTexcoord; \n" +

            "uniform mat4 u_mvpMatrix; \n" +

            "out vec2 out_Texcoord; \n" +

            "void main (void) \n" +
            "{ \n" +
            "	gl_Position = u_mvpMatrix * vPosition; \n" +
            "	out_Texcoord = vTexcoord; \n" +
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
            "precision lowp int; \n" +

            "in vec2 out_Texcoord; \n" +

            "uniform int u_textureEnable; \n" +
            "uniform sampler2D u_textureSampler; \n" +

            "out vec4 FragColor; \n" +

            "void main (void) \n" +
            "{ \n" +
            "	if (u_textureEnable == 1) " +
            "		FragColor = texture(u_textureSampler, out_Texcoord); \n" +
            "	else " +
            "		FragColor = vec4(1.0); \n" +
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
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.RMC_ATTRIBUTE_TEXCOORD, "vTexcoord");

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
        samplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_textureSampler");
        textureUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_textureEnable");

        // vertex position array
        final float quadVertices[] = new float[] {
             1.0f,  1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f
        };

        // create vao
        GLES32.glGenVertexArrays(1, vao, 0);
        GLES32.glBindVertexArray(vao[0]);

        // create vbo for position
        GLES32.glGenBuffers(1, vboPos, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboPos[0]);

        // create native buffer to pass to OpenGLES
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(quadVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
        positionBuffer.put(quadVertices);
        positionBuffer.position(0);

        // pass to OpenGL buffer
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, quadVertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        // create vbo for texcoords
        GLES32.glGenBuffers(1, vboTex, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboTex[0]);

        // create native buffer to pass to OpenGLES
        byteBuffer = ByteBuffer.allocateDirect(8 * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        floatBuffer = byteBuffer.asFloatBuffer();

        // pass to OpenGL buffer
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 8 * 4, null, GLES32.GL_DYNAMIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_TEXCOORD);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glBindVertexArray(0);

        // clear color
        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // enable depth
        GLES32.glClearDepthf(1.0f);

        // clear the depth buffer
        GLES32.glEnable(GLES32.GL_DEPTH_TEST);
        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        // texture
        GLES32.glEnable(GLES32.GL_TEXTURE_2D);
        texture_smiley[0] = loadGLTexture(R.raw.smiley);

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
        float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        // intialize above matrices to identity
        Matrix.setIdentityM(translateMatrix, 0);
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        // translation transformation
        Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);

        // do necessary matrix multiplication
        Matrix.multiplyMM(modelViewMatrix, 0,
            modelViewMatrix, 0,
            translateMatrix, 0);

        Matrix.multiplyMM(modelViewProjectionMatrix, 0,
            perspectiveProjectionMatrix, 0,
            modelViewMatrix, 0);

        // send necessary to shader in respective uniforms
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        // bind with vao (this will avoid many binding to vbo_vertex)
        GLES32.glBindVertexArray(vao[0]);

        float[] rectTexcoords = new float[] {
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f
        };

        GLES32.glUniform1i(textureUniform, 1);

        if (tapCount == 1)
        {
            rectTexcoords[0] = 0.5f;
            rectTexcoords[1] = 0.5f;
            rectTexcoords[2] = 0.0f;
            rectTexcoords[3] = 0.5f;
            rectTexcoords[4] = 0.0f;
            rectTexcoords[5] = 0.0f;
            rectTexcoords[6] = 0.5f;
            rectTexcoords[7] = 0.0f;
        }
        else if (tapCount == 2)
        {
            rectTexcoords[0] = 1.0f;
            rectTexcoords[1] = 1.0f;
            rectTexcoords[2] = 0.0f;
            rectTexcoords[3] = 1.0f;
            rectTexcoords[4] = 0.0f;
            rectTexcoords[5] = 0.0f;
            rectTexcoords[6] = 1.0f;
            rectTexcoords[7] = 0.0f;
        }
        else if (tapCount == 3)
        {
            rectTexcoords[0] = 2.0f;
            rectTexcoords[1] = 2.0f;
            rectTexcoords[2] = 0.0f;
            rectTexcoords[3] = 2.0f;
            rectTexcoords[4] = 0.0f;
            rectTexcoords[5] = 0.0f;
            rectTexcoords[6] = 2.0f;
            rectTexcoords[7] = 0.0f;
        }
        else if (tapCount == 4)
        {
            rectTexcoords[0] = 0.5f;
            rectTexcoords[1] = 0.5f;
            rectTexcoords[2] = 0.5f;
            rectTexcoords[3] = 0.5f;
            rectTexcoords[4] = 0.5f;
            rectTexcoords[5] = 0.5f;
            rectTexcoords[6] = 0.5f;
            rectTexcoords[7] = 0.5f;
        }
        else
        {
            GLES32.glUniform1i(textureUniform, 0);
        }

        floatBuffer.put(rectTexcoords);
        floatBuffer.position(0);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboTex[0]);
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, rectTexcoords.length * 4, floatBuffer, GLES32.GL_DYNAMIC_DRAW);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        // bind texture
        GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_smiley[0]);
        GLES32.glUniform1i(samplerUniform, 0);

        // draw necessary scene
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

        // unbind vao
        GLES32.glBindVertexArray(0);

        // stop using OpenGL program object
        GLES32.glUseProgram(0);

        // ~SwapBuffers()
        requestRender();
    }

    private void uninitialize() {
        if (vao[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vao, 0);
            vao[0] = 0;
        }

        if (vboPos[0] != 0) {
            GLES32.glDeleteBuffers(1, vboPos, 0);
            vboPos[0] = 0;
        }

        if (vboTex[0] != 0) {
            GLES32.glDeleteBuffers(1, vboTex, 0);
            vboTex[0] = 0;
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

    private int loadGLTexture(int imageFileResourceID) {
        int[] texture = new int[1];

        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;

        Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), imageFileResourceID, options);

        GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 4);
        GLES32.glGenTextures(1, texture, 0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);

        // setting texture parameters
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR);

        GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);
        GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);

        return texture[0];
    }
}
