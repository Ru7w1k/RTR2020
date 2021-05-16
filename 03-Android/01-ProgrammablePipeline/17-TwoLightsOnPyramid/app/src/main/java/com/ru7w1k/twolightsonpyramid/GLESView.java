package com.ru7w1k.twolightsonpyramid;

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
    private int mUniform;
    private int vUniform;
    private int pUniform;

    private int laUniform;
    private int ldUniform;
    private int lsUniform;
    private int lightPositionUniform;

    private int kaUniform;
    private int kdUniform;
    private int ksUniform;
    private int shininessUniform;

    private int enableLightUniform;

    private int[] vao = new int[1];
    private int[] vboPos = new int[1];
    private int[] vboNor = new int[1];

    private float[] perspectiveProjectionMatrix = new float[16];

    private float anglePyramid = 0.0f;
    private boolean bLight = false;

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
        bLight = !bLight;
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
            "precision lowp int; \n" +

            "in vec4 vPosition; \n" +
            "in vec3 vNormal; \n" +

            "uniform mat4 u_mMatrix; \n" +
            "uniform mat4 u_vMatrix; \n" +
            "uniform mat4 u_pMatrix; \n" +

            "uniform vec3 u_La[2]; \n" +
            "uniform vec3 u_Ld[2]; \n" +
            "uniform vec3 u_Ls[2]; \n" +
            "uniform vec3 u_Ka; \n" +
            "uniform vec3 u_Kd; \n" +
            "uniform vec3 u_Ks; \n" +
            "uniform vec3 u_Ks; \n" +

            "uniform float u_Shininess; \n" +
            "uniform vec4 u_LightPos[2]; \n" +
            "uniform int u_bLight; \n" +

            "out vec3 out_PhongLight; \n" +

            "void main (void) \n" +
            "{ \n" +
            "	out_PhongLight = vec3(0.0); \n" +
            "	if (u_bLight == 1)" +
            "	{ \n" +
            "		vec4 eyeCoordinates = u_vMatrix * u_mMatrix * vPosition; \n" +
            "		vec3 tNorm = normalize(mat3(u_vMatrix * u_mMatrix) * vNormal); \n" +
            "		vec3 viewerVector = normalize(vec3(-eyeCoordinates.xyz)); \n" +

            "		for(int i = 0; i < 2; i++) \n" +
            "		{ \n" +
            "			vec3 lightDir = normalize(vec3(u_LightPos[i] - eyeCoordinates)); \n" +
            "			float tNormDotLightDir = max(dot(tNorm, lightDir), 0.0); \n" +
            "			vec3 reflectionVector = reflect(-lightDir, tNorm); \n" +

            "			vec3 ambient = u_La[i] * u_Ka; \n" +
            "			vec3 diffuse = u_Ld[i] * u_Kd * tNormDotLightDir; \n" +
            "			vec3 specular = u_Ls[i] * u_Ks * pow(max(dot(reflectionVector, viewerVector), 0.0), u_Shininess); \n" +

            "			out_PhongLight += ambient + diffuse + specular; \n" +
            "		} \n" +
            "	} \n" +
            "	else \n" +
            "	{ \n" +
            "		out_PhongLight = vec3(1.0, 1.0, 1.0); \n" +
            "	} \n" +
            "	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition; \n" +
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

            "in vec3 out_PhongLight; \n" +
            "out vec4 FragColor; \n" +

            "void main (void) \n" +
            "{ \n" +
            "	FragColor = vec4(out_PhongLight, 1.0); \n" +
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
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.RMC_ATTRIBUTE_NORMAL, "vNormal");

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
        mUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mMatrix");
        vUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_vMatrix");
        pUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_pMatrix");

        laUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_La");
        ldUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Ld");
        lsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Ls");

        kaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Ka");
        kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Kd");
        ksUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Ks");

        shininessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_Shininess");
        enableLightUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_bLight");
        lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_LightPos");

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

        // vertex array
        final float triangleNormals[] = new float[] {
            /* Front */
            0.0f, 0.447214f, 0.894427f,
            0.0f, 0.447214f, 0.894427f,
            0.0f, 0.447214f, 0.894427f,

            /* Right */
            0.894427f, 0.447214f, 0.0f,
            0.894427f, 0.447214f, 0.0f,
            0.894427f, 0.447214f, 0.0f,

            /* Left */
            -0.894427f, 0.447214f, 0.0f,
            -0.894427f, 0.447214f, 0.0f,
            -0.894427f, 0.447214f, 0.0f,

            /* Back */
            0.0f, 0.447214f, -0.894427f,
            0.0f, 0.447214f, -0.894427f,
            0.0f, 0.447214f, -0.894427f
        };

        // create vao
        GLES32.glGenVertexArrays(1, vao, 0);
        GLES32.glBindVertexArray(vao[0]);

        // create vbo
        GLES32.glGenBuffers(1, vboPos, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboPos[0]);

        // create native buffer to pass to OpenGLES
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(triangleVertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer floatBuffer = byteBuffer.asFloatBuffer();
        floatBuffer.put(triangleVertices);
        floatBuffer.position(0);

        // pass to OpenGL buffer
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleVertices.length * 4, floatBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        // create vbo for normals
        GLES32.glGenBuffers(1, vboNor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboNor[0]);

        // create native buffer to pass to OpenGLES
        byteBuffer = ByteBuffer.allocateDirect(triangleNormals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        floatBuffer = byteBuffer.asFloatBuffer();
        floatBuffer.put(triangleNormals);
        floatBuffer.position(0);

        // pass to OpenGL buffer
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleNormals.length * 4, floatBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_NORMAL);
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
        float[] modelMatrix = new float[16];
        float[] viewMatrix = new float[16];

        // intialize above matrices to identity
        Matrix.setIdentityM(translateMatrix, 0);
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);

        // translation transformation
        Matrix.translateM(translateMatrix, 0, 0.0f, 0.0f, -3.0f);
        modelMatrix = translateMatrix;

        // send necessary to shader in respective uniforms
        GLES32.glUniformMatrix4fv(mUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(vUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(pUniform, 1, false, perspectiveProjectionMatrix, 0);

        if (bLight) {
            final float[] la = new float[] {
                0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f,
            };
    
            final float[] ld = new float[] {
                1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f,
            };
    
            final float[] ls = new float[] {
                1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f,
            };
    
            final float[] lPositions = new float[] {
                 2.0f, 0.0f, 0.0f, 1.0f,
                -2.0f, 0.0f, 0.0f, 1.0f,
            };
    
            GLES32.glUniform3fv(laUniform, 2, la, 0);
            GLES32.glUniform3fv(ldUniform, 2, ld, 0);
            GLES32.glUniform3fv(lsUniform, 2, ls, 0);
            GLES32.glUniform4fv(lightPositionUniform, 2, lPositions, 0);
            
            GLES32.glUniform3f(kaUniform, 0.0f, 0.0f, 0.0f);
            GLES32.glUniform3f(kdUniform, 0.5f, 0.2f, 0.7f);
            GLES32.glUniform3f(ksUniform, 0.7f, 0.7f, 0.7f);
            GLES32.glUniform1f(shininessUniform, 128.0f);

            GLES32.glUniform1i(enableLightUniform, 1);
        }
        else {
            GLES32.glUniform1i(enableLightUniform, 0);
        }

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

        if (vboPos[0] != 0) {
            GLES32.glDeleteBuffers(1, vboPos, 0);
            vboPos[0] = 0;
        }

        if (vboNor[0] != 0) {
            GLES32.glDeleteBuffers(1, vboNor, 0);
            vboNor[0] = 0;
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
