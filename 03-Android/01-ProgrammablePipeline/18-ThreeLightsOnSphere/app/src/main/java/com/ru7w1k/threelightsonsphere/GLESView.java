package com.ru7w1k.threelightsonsphere;

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
import java.nio.ShortBuffer;

public class GLESView extends GLSurfaceView implements GLSurfaceView.Renderer, OnDoubleTapListener, OnGestureListener {

    private final Context context;  // final ~ const
    private GestureDetector gestureDetector;

    // P E R  V E R T E X //////////////////////////////////////////////////////
    private int vertexShaderObjectPV;
    private int fragmentShaderObjectPV;
    private int shaderProgramObjectPV;
    private int mUniformPV;
    private int vUniformPV;
    private int pUniformPV;
    private int[] laUniformPV = new int[3];
    private int[] ldUniformPV = new int[3];
    private int[] lsUniformPV = new int[3];
    private int[] lightPositionUniformPV = new int[3];
    private int kaUniformPV;
    private int kdUniformPV;
    private int ksUniformPV;
    private int shininessUniformPV;
    private int enableLightUniformPV;
    ////////////////////////////////////////////////////////////////////////////

    // P E R  F R A G M E N T //////////////////////////////////////////////////
    private int vertexShaderObjectPF;
    private int fragmentShaderObjectPF;
    private int shaderProgramObjectPF;
    private int mUniformPF;
    private int vUniformPF;
    private int pUniformPF;
    private int[] laUniformPF = new int[3];
    private int[] ldUniformPF = new int[3];
    private int[] lsUniformPF = new int[3];
    private int[] lightPositionUniformPF = new int[3];
    private int kaUniformPF;
    private int kdUniformPF;
    private int ksUniformPF;
    private int shininessUniformPF;
    private int enableLightUniformPF;
    ////////////////////////////////////////////////////////////////////////////

    private int[] vaoSphere = new int[1];
    private int[] vboSpherePos = new int[1];
    private int[] vboSphereNor = new int[1];
    private int[] vboSphereElm = new int[1];
    private int numVertices;
    private int numElements;

    private Light[] lights = new Light[3];

    private float[] perspectiveProjectionMatrix = new float[16];
    private boolean bLight = false;
    private boolean bFragment = false;

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
        bFragment = !bFragment;
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
        //// P E R  V E R T E X  S H A D E R ///////////////////////////////////
        // vertex shader
        final String vertexShaderSourceCodePV = String.format(
            "#version 320 es \n" +
            "precision lowp int; \n" +

            "in vec4 vPosition; \n" +
            "in vec3 vNormal; \n" +

            "uniform mat4 u_mMatrix; \n" +
            "uniform mat4 u_vMatrix; \n" +
            "uniform mat4 u_pMatrix; \n" +

            "uniform vec3 u_La[3]; \n" +
            "uniform vec3 u_Ld[3]; \n" +
            "uniform vec3 u_Ls[3]; \n" +
            "uniform vec3 u_Ka; \n" +
            "uniform vec3 u_Kd; \n" +
            "uniform vec3 u_Ks; \n" +

            "uniform float u_Shininess; \n" +
            "uniform vec4 u_LightPos[3]; \n" +
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

            "		for (int i = 0; i < 3; i++) \n" +
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
        vertexShaderObjectPV = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        GLES32.glShaderSource(vertexShaderObjectPV, vertexShaderSourceCodePV);

        // compile shader and check errors
        int[] iShaderCompileStatus = new int[1];
        int[] iInfoLogLength       = new int[1];
        String szInfoLog = null;

        GLES32.glCompileShader(vertexShaderObjectPV);
        GLES32.glGetShaderiv(vertexShaderObjectPV, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(vertexShaderObjectPV, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObjectPV);
                System.out.println("RMC: Per Vertex Vertex Shader Compile Log: \n" + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // fragment shader
        final String fragmentShaderSourceCodePV = String.format(
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
        fragmentShaderObjectPV = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
        GLES32.glShaderSource(fragmentShaderObjectPV, fragmentShaderSourceCodePV);

        // compile shader and check errors
        iShaderCompileStatus[0] = 0;
        iInfoLogLength[0]       = 0;
        szInfoLog = null;

        GLES32.glCompileShader(fragmentShaderObjectPV);
        GLES32.glGetShaderiv(fragmentShaderObjectPV, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(fragmentShaderObjectPV, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObjectPV);
                System.out.println("RMC: Per Vertex Fragment Shader Compile Log: \n" + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // shader program
        shaderProgramObjectPV = GLES32.glCreateProgram();

        // attach shaders 
        GLES32.glAttachShader(shaderProgramObjectPV, vertexShaderObjectPV);
        GLES32.glAttachShader(shaderProgramObjectPV, fragmentShaderObjectPV);

        // pre-linking binding to vertex attribute
        GLES32.glBindAttribLocation(shaderProgramObjectPV, GLESMacros.RMC_ATTRIBUTE_POSITION, "vPosition");
        GLES32.glBindAttribLocation(shaderProgramObjectPV, GLESMacros.RMC_ATTRIBUTE_NORMAL, "vNormal");

        // link shader program and check errors
        int[] iShaderProgramLinkStatus = new int[1];
        iInfoLogLength[0] = 0;
        szInfoLog = null;

        GLES32.glLinkProgram(shaderProgramObjectPV);
        GLES32.glGetProgramiv(shaderProgramObjectPV, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
        if (iShaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetProgramiv(shaderProgramObjectPV, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObjectPV);
                System.out.println("RMC: Per Vertex Program Compile Log: \n" + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // get uniforms
        mUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_mMatrix");
        vUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_vMatrix");
        pUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_pMatrix");

        for (int i = 0; i < 3; i++) {
            laUniformPV[i] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_La["+i+"]");
            ldUniformPV[i] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_Ld["+i+"]");
            lsUniformPV[i] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_Ls["+i+"]");
            lightPositionUniformPV[i] = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_LightPos["+i+"]");
        }

        kaUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_Ka");
        kdUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_Kd");
        ksUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_Ks");

        shininessUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_Shininess");
        enableLightUniformPV = GLES32.glGetUniformLocation(shaderProgramObjectPV, "u_bLight");

        ////////////////////////////////////////////////////////////////////////

        //// P E R  F R A G M E N T  S H A D E R ///////////////////////////////
        // vertex shader
        final String vertexShaderSourceCodePF = String.format(
            "#version 320 es \n" +
            "precision lowp int; \n" +

            "in vec4 vPosition; \n" +
            "in vec3 vNormal; \n" +

            "uniform mat4 u_mMatrix; \n" +
            "uniform mat4 u_vMatrix; \n" +
            "uniform mat4 u_pMatrix; \n" +

            "uniform vec4 u_LightPos[3]; \n" +
            "uniform int u_bLight; \n" +

            "out vec3 tNorm; \n" +
            "out vec3 lightDir[3]; \n" +
            "out vec3 viewerVector; \n" +

            "void main (void) \n" +
            "{ \n" +
            "	if (u_bLight == 1)" +
            "	{ \n" +
            "		vec4 eyeCoordinates = u_vMatrix * u_mMatrix * vPosition; \n" +
            "		tNorm = mat3(u_vMatrix * u_mMatrix) * vNormal; \n" +
            "		viewerVector = normalize(vec3(-eyeCoordinates.xyz)); \n" +
            "		for (int i = 0; i < 3; i++) \n" +
            "		{ \n" +
            "			lightDir[i] = vec3(u_LightPos[i] - eyeCoordinates); \n" +
            "		} \n" +
            "	} \n" +
            "	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition; \n" +
            "} \n"
        );

        // create shader and provide source code
        vertexShaderObjectPF = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        GLES32.glShaderSource(vertexShaderObjectPF, vertexShaderSourceCodePF);

        // compile shader and check errors
        iShaderCompileStatus = new int[1];
        iInfoLogLength       = new int[1];
        szInfoLog = null;

        GLES32.glCompileShader(vertexShaderObjectPF);
        GLES32.glGetShaderiv(vertexShaderObjectPF, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(vertexShaderObjectPF, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObjectPF);
                System.out.println("RMC: Per Fragment Vertex Shader Compile Log: \n" + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // fragment shader
        final String fragmentShaderSourceCodePF = String.format(
            "#version 320 es \n" +
            "precision highp float; \n" +
            "precision lowp int; \n" +

            "in vec3 tNorm; \n" +
            "in vec3 lightDir[3]; \n" +
            "in vec3 viewerVector; \n" +

            "uniform vec3 u_La[3]; \n" +
            "uniform vec3 u_Ld[3]; \n" +
            "uniform vec3 u_Ls[3]; \n" +
            "uniform vec3 u_Ka; \n" +
            "uniform vec3 u_Kd; \n" +
            "uniform vec3 u_Ks; \n" +

            "uniform float u_Shininess; \n" +
            "uniform int u_bLight; \n" +

            "out vec4 FragColor; \n" +

            "void main (void) \n" +
            "{ \n" +
            "	if (u_bLight == 1)" +
            "	{ \n" +
            "		vec3 phongLight = vec3(0.0); \n" +
            "		vec3 normTNorm = normalize(tNorm); \n" +
            "		vec3 normViewerVector = normalize(viewerVector); \n" +

            "		for (int i = 0; i < 3; i++) \n" +
            "		{ \n" +
            "			vec3 normLightDir = normalize(lightDir[i]); \n" +
            "			vec3 reflectionVector = reflect(-normLightDir, normTNorm); \n" +
            "			float tNormDotLightDir = max(dot(normTNorm, normLightDir), 0.0); \n" +

            "			vec3 ambient = u_La[i] * u_Ka; \n" +
            "			vec3 diffuse = u_Ld[i] * u_Kd * tNormDotLightDir; \n" +
            "			vec3 specular = u_Ls[i] * u_Ks * pow(max(dot(reflectionVector, normViewerVector), 0.0), u_Shininess); \n" +
            "			phongLight += ambient + diffuse + specular; \n" +

            "		} \n" +
            "		FragColor = vec4(phongLight, 1.0); \n" +
            "	} \n" +
            "	else \n" +
            "	{ \n" +
            "		FragColor = vec4(1.0); \n" +
            "	} \n" +
            "} \n"
        );

        // create shader and provide source code
        fragmentShaderObjectPF = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
        GLES32.glShaderSource(fragmentShaderObjectPF, fragmentShaderSourceCodePF);

        // compile shader and check errors
        iShaderCompileStatus[0] = 0;
        iInfoLogLength[0]       = 0;
        szInfoLog = null;

        GLES32.glCompileShader(fragmentShaderObjectPF);
        GLES32.glGetShaderiv(fragmentShaderObjectPF, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
        if (iShaderCompileStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetShaderiv(fragmentShaderObjectPF, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObjectPF);
                System.out.println("RMC: Per Fragment Fragment Shader Compile Log: \n" + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // shader program
        shaderProgramObjectPF = GLES32.glCreateProgram();

        // attach shaders 
        GLES32.glAttachShader(shaderProgramObjectPF, vertexShaderObjectPF);
        GLES32.glAttachShader(shaderProgramObjectPF, fragmentShaderObjectPF);

        // pre-linking binding to vertex attribute
        GLES32.glBindAttribLocation(shaderProgramObjectPF, GLESMacros.RMC_ATTRIBUTE_POSITION, "vPosition");
        GLES32.glBindAttribLocation(shaderProgramObjectPF, GLESMacros.RMC_ATTRIBUTE_NORMAL, "vNormal");

        // link shader program and check errors
        iShaderProgramLinkStatus = new int[1];
        iInfoLogLength[0] = 0;
        szInfoLog = null;

        GLES32.glLinkProgram(shaderProgramObjectPF);
        GLES32.glGetProgramiv(shaderProgramObjectPF, GLES32.GL_LINK_STATUS, iShaderProgramLinkStatus, 0);
        if (iShaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
            GLES32.glGetProgramiv(shaderProgramObjectPF, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
            if (iInfoLogLength[0] > 0) {
                szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObjectPF);
                System.out.println("RMC: Per Fragment Program Compile Log: \n" + szInfoLog);
                uninitialize();
                System.exit(0);
            }
        }

        // get uniforms
        mUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_mMatrix");
        vUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_vMatrix");
        pUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_pMatrix");

        for (int i = 0; i < 3; i++) {
            laUniformPF[i] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_La["+i+"]");
            ldUniformPF[i] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_Ld["+i+"]");
            lsUniformPF[i] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_Ls["+i+"]");
            lightPositionUniformPF[i] = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_LightPos["+i+"]");
        }

        kaUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_Ka");
        kdUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_Kd");
        ksUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_Ks");

        shininessUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_Shininess");
        enableLightUniformPF = GLES32.glGetUniformLocation(shaderProgramObjectPF, "u_bLight");

        ////////////////////////////////////////////////////////////////////////

        // vertex array
        Sphere sphere = new Sphere();
        float[] sphere_vertices = new float[1146];
        float[] sphere_normals = new float[1146];
        float[] sphere_textures = new float[764];
        short[] sphere_elements = new short[2280];
        
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();

        // create vao
        GLES32.glGenVertexArrays(1, vaoSphere, 0);
        GLES32.glBindVertexArray(vaoSphere[0]);

        // create vbo for position
        GLES32.glGenBuffers(1, vboSpherePos, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboSpherePos[0]);

        // create native buffer to pass to OpenGLES
        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer floatBuffer = byteBuffer.asFloatBuffer();
        floatBuffer.put(sphere_vertices);
        floatBuffer.position(0);

        // pass to OpenGL buffer
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_vertices.length * 4, floatBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_POSITION);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        // create vbo for normals
        GLES32.glGenBuffers(1, vboSphereNor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboSphereNor[0]);

        // create native buffer to pass to OpenGLES
        byteBuffer = ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        floatBuffer = byteBuffer.asFloatBuffer();
        floatBuffer.put(sphere_normals);
        floatBuffer.position(0);

        // pass to OpenGL buffer
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, sphere_normals.length * 4, floatBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_NORMAL);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        // create vbo for elements 
        GLES32.glGenBuffers(1, vboSphereElm, 0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vboSphereElm[0]);

        // create native buffer to pass to OpenGLES
        byteBuffer = ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer shortBuffer = byteBuffer.asShortBuffer();
        shortBuffer.put(sphere_elements);
        shortBuffer.position(0);

        // pass to OpenGL buffer
        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, sphere_elements.length * 2, shortBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, 0);

        GLES32.glBindVertexArray(0);

        // light configurations
        lights[0] = new Light();
        lights[1] = new Light();
        lights[2] = new Light();

        // RED light
        lights[0].lightAmbient[0] = 0.0f;
        lights[0].lightAmbient[1] = 0.0f;
        lights[0].lightAmbient[2] = 0.0f;
        lights[0].lightAmbient[3] = 1.0f;

        lights[0].lightDiffuse[0] = 1.0f;
        lights[0].lightDiffuse[1] = 0.0f;
        lights[0].lightDiffuse[2] = 0.0f;
        lights[0].lightDiffuse[3] = 1.0f;

        lights[0].lightSpecular[0] = 1.0f;
        lights[0].lightSpecular[1] = 0.0f;
        lights[0].lightSpecular[2] = 0.0f;
        lights[0].lightSpecular[3] = 1.0f;

        lights[0].lightPosition[0] = 0.0f;
        lights[0].lightPosition[1] = 0.0f;
        lights[0].lightPosition[2] = 0.0f;
        lights[0].lightPosition[3] = 1.0f;

        lights[0].angle = 0.0f;

        // GREEN light
        lights[1].lightAmbient[0] = 0.0f;
        lights[1].lightAmbient[1] = 0.0f;
        lights[1].lightAmbient[2] = 0.0f;
        lights[1].lightAmbient[3] = 1.0f;

        lights[1].lightDiffuse[0] = 0.0f;
        lights[1].lightDiffuse[1] = 1.0f;
        lights[1].lightDiffuse[2] = 0.0f;
        lights[1].lightDiffuse[3] = 1.0f;

        lights[1].lightSpecular[0] = 0.0f;
        lights[1].lightSpecular[1] = 1.0f;
        lights[1].lightSpecular[2] = 0.0f;
        lights[1].lightSpecular[3] = 1.0f;

        lights[1].lightPosition[0] = 0.0f;
        lights[1].lightPosition[1] = 0.0f;
        lights[1].lightPosition[2] = 0.0f;
        lights[1].lightPosition[3] = 1.0f;

        lights[1].angle = 0.0f;

        // BLUE light
        lights[2].lightAmbient[0] = 0.0f;
        lights[2].lightAmbient[1] = 0.0f;
        lights[2].lightAmbient[2] = 0.0f;
        lights[2].lightAmbient[3] = 1.0f;

        lights[2].lightDiffuse[0] = 0.0f;
        lights[2].lightDiffuse[1] = 0.0f;
        lights[2].lightDiffuse[2] = 1.0f;
        lights[2].lightDiffuse[3] = 1.0f;

        lights[2].lightSpecular[0] = 0.0f;
        lights[2].lightSpecular[1] = 0.0f;
        lights[2].lightSpecular[2] = 1.0f;
        lights[2].lightSpecular[3] = 1.0f;

        lights[2].lightPosition[0] = 0.0f;
        lights[2].lightPosition[1] = 0.0f;
        lights[2].lightPosition[2] = 0.0f;
        lights[2].lightPosition[3] = 1.0f;

        lights[2].angle = 0.0f;

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

        if (bFragment) {
            GLES32.glUseProgram(shaderProgramObjectPF);
        
            // send necessary to shader in respective uniforms
            GLES32.glUniformMatrix4fv(mUniformPF, 1, false, modelMatrix, 0);
            GLES32.glUniformMatrix4fv(vUniformPF, 1, false, viewMatrix, 0);
            GLES32.glUniformMatrix4fv(pUniformPF, 1, false, perspectiveProjectionMatrix, 0);

            if (bLight) {
                for (int i = 0; i < 3; i++) {
                    GLES32.glUniform3fv(laUniformPF[i], 1, lights[i].lightAmbient, 0);
                    GLES32.glUniform3fv(ldUniformPF[i], 1, lights[i].lightDiffuse, 0);
                    GLES32.glUniform3fv(lsUniformPF[i], 1, lights[i].lightSpecular, 0);
                    GLES32.glUniform4fv(lightPositionUniformPF[i], 1, lights[i].lightPosition, 0);
                }
                
                GLES32.glUniform3f(kaUniformPF, 0.0f, 0.0f, 0.0f);
                GLES32.glUniform3f(kdUniformPF, 1.0f, 1.0f, 1.0f);
                GLES32.glUniform3f(ksUniformPF, 1.0f, 1.0f, 1.0f);
                GLES32.glUniform1f(shininessUniformPF, 128.0f);

                GLES32.glUniform1i(enableLightUniformPF, 1);
            }
            else {
                GLES32.glUniform1i(enableLightUniformPF, 0);
            }

        } else {
            GLES32.glUseProgram(shaderProgramObjectPV);
        
            // send necessary to shader in respective uniforms
            GLES32.glUniformMatrix4fv(mUniformPV, 1, false, modelMatrix, 0);
            GLES32.glUniformMatrix4fv(vUniformPV, 1, false, viewMatrix, 0);
            GLES32.glUniformMatrix4fv(pUniformPV, 1, false, perspectiveProjectionMatrix, 0);

            if (bLight) {
                for (int i = 0; i < 3; i++) {
                    GLES32.glUniform3fv(laUniformPV[i], 1, lights[i].lightAmbient, 0);
                    GLES32.glUniform3fv(ldUniformPV[i], 1, lights[i].lightDiffuse, 0);
                    GLES32.glUniform3fv(lsUniformPV[i], 1, lights[i].lightSpecular, 0);
                    GLES32.glUniform4fv(lightPositionUniformPV[i], 1, lights[i].lightPosition, 0);
                }

                GLES32.glUniform3f(kaUniformPV, 0.0f, 0.0f, 0.0f);
                GLES32.glUniform3f(kdUniformPV, 1.0f, 1.0f, 1.0f);
                GLES32.glUniform3f(ksUniformPV, 1.0f, 1.0f, 1.0f);
                GLES32.glUniform1f(shininessUniformPV, 128.0f);

                GLES32.glUniform1i(enableLightUniformPV, 1);
            }
            else {
                GLES32.glUniform1i(enableLightUniformPV, 0);
            }
        }

        // bind with vao (this will avoid many binding to vbo_vertex)
        GLES32.glBindVertexArray(vaoSphere[0]);

        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vboSphereElm[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

        // unbind vao
        GLES32.glBindVertexArray(0);

        // stop using OpenGL program object
        GLES32.glUseProgram(0);

        // ~SwapBuffers()
        requestRender();
    }

    private void update() {
        if (lights[0].angle < 360.0f) {
            lights[0].angle += 0.02f;
        } else {
            lights[0].angle = 0.0f;
        }

        if (lights[1].angle < 360.0f) {
            lights[1].angle += 0.02f;
        } else {
            lights[1].angle = 0.0f;
        }

        if (lights[2].angle < 360.0f) {
            lights[2].angle += 0.02f;
        } else {
            lights[2].angle = 0.0f;
        }

        lights[0].lightPosition = new float[] {0.0f, 100.0f*(float)Math.cos(lights[0].angle), 100.0f*(float)Math.sin(lights[0].angle), 1.0f};
        lights[1].lightPosition = new float[] {100.0f*(float)Math.cos(lights[1].angle), 0.0f, 100.0f*(float)Math.sin(lights[1].angle), 1.0f};
        lights[2].lightPosition = new float[] {100.0f*(float)Math.cos(lights[2].angle), 100.0f*(float)Math.sin(lights[2].angle), 0.0f, 1.0f};
    }

    private void uninitialize() {
        if (vaoSphere[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vaoSphere, 0);
            vaoSphere[0] = 0;
        }

        if (vboSpherePos[0] != 0) {
            GLES32.glDeleteBuffers(1, vboSpherePos, 0);
            vboSpherePos[0] = 0;
        }

        if (vboSphereNor[0] != 0) {
            GLES32.glDeleteBuffers(1, vboSphereNor, 0);
            vboSphereNor[0] = 0;
        }

        if (vboSphereElm[0] != 0) {
            GLES32.glDeleteBuffers(1, vboSphereElm, 0);
            vboSphereElm[0] = 0;
        }

        if (shaderProgramObjectPF != 0) {
            if (vertexShaderObjectPF != 0) {
                GLES32.glDetachShader(shaderProgramObjectPF, vertexShaderObjectPF);
                GLES32.glDeleteShader(vertexShaderObjectPF);
                vertexShaderObjectPF = 0;
            }

            if (fragmentShaderObjectPF != 0) {
                GLES32.glDetachShader(shaderProgramObjectPF, fragmentShaderObjectPF);
                GLES32.glDeleteShader(fragmentShaderObjectPF);
                fragmentShaderObjectPF = 0;
            }

            GLES32.glDeleteProgram(shaderProgramObjectPF);
            shaderProgramObjectPF = 0;
        }

        if (shaderProgramObjectPV != 0) {
            if (vertexShaderObjectPV != 0) {
                GLES32.glDetachShader(shaderProgramObjectPV, vertexShaderObjectPV);
                GLES32.glDeleteShader(vertexShaderObjectPV);
                vertexShaderObjectPV = 0;
            }

            if (fragmentShaderObjectPV != 0) {
                GLES32.glDetachShader(shaderProgramObjectPV, fragmentShaderObjectPV);
                GLES32.glDeleteShader(fragmentShaderObjectPV);
                fragmentShaderObjectPV = 0;
            }

            GLES32.glDeleteProgram(shaderProgramObjectPV);
            shaderProgramObjectPV = 0;
        }
    }
}
