package com.ru7w1k.graphpaperwithshapes;

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

    private int[] vaoAxes    = new int[1];
    private int[] vboAxesPos = new int[1];
    private int[] vboAxesCol = new int[1];

    private int[] vaoMainAxes    = new int[1];
    private int[] vboMainAxesPos = new int[1];
    private int[] vboMainAxesCol = new int[1];

    private int[] vaoShapes    = new int[1];
    private int[] vboShapesPos = new int[1];

    private float[] perspectiveProjectionMatrix = new float[16];

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
            "in vec4 vColor; \n" +
            "uniform mat4 u_mvpMatrix; \n" +
            "out vec4 out_Color; \n" +

            "void main (void) \n" +
            "{ \n" +
            "	gl_Position = u_mvpMatrix * vPosition; \n" +
            "	out_Color = vColor; \n" +
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

            "in  vec4 out_Color; \n" +
            "out vec4 FragColor; \n" +

            "void main (void) \n" +
            "{ \n" +
            "	FragColor = out_Color; \n" +
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
        GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.RMC_ATTRIBUTE_COLOR, "vColor");

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
        float graphCoords[] = generateGraphCoordinates();

        // color array 
        final float axisCoords[] = new float[] {
            -1.0f,  0.0f, 0.0f,
             1.0f,  0.0f, 0.0f,
             0.0f, -1.0f, 0.0f,
             0.0f,  1.0f, 0.0f
        };

        final float axisColors[] = new float[] {
             1.0f,  0.0f, 0.0f,
             1.0f,  0.0f, 0.0f,
             0.0f,  1.0f, 0.0f,
             0.0f,  1.0f, 0.0f
        };

        float smallAxisColors[] = new float[graphCoords.length];
        for (int i = 0; i < smallAxisColors.length; i += 3) {
            smallAxisColors[i+0] = 0.0f;
            smallAxisColors[i+1] = 0.0f;
            smallAxisColors[i+2] = 1.0f;
        }


        float[] shapesCoords = new float[1300 * 3];
        int shapesCoordsCount = 0;
        float fX, fY;

        shapesCoordsCount = generateOuterCircleCoords(shapesCoords, shapesCoordsCount);

        fX = fY = (float)Math.cos(Math.PI / 4.0);
        shapesCoordsCount = generateSquareCoords(fX, fY, shapesCoords, shapesCoordsCount);
        shapesCoordsCount = generateTriangleAndIncircleCoords(fX, fY, shapesCoords, shapesCoordsCount);

        // create vao
        GLES32.glGenVertexArrays(1, vaoAxes, 0);
        GLES32.glBindVertexArray(vaoAxes[0]);

        GLES32.glGenBuffers(1, vboAxesPos, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboAxesPos[0]);

        ByteBuffer byteBuffer = ByteBuffer.allocateDirect(graphCoords.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
        positionBuffer.put(graphCoords);
        positionBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, graphCoords.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_POSITION);

        // small axes colors
        GLES32.glGenBuffers(1, vboAxesCol, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboAxesCol[0]);

        ByteBuffer byteBufferSmallAxesColor = ByteBuffer.allocateDirect(smallAxisColors.length * 4);
        byteBufferSmallAxesColor.order(ByteOrder.nativeOrder());
        FloatBuffer smallAxesColorBuffer = byteBufferSmallAxesColor.asFloatBuffer();
        smallAxesColorBuffer.put(smallAxisColors);
        smallAxesColorBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, smallAxisColors.length * 4, smallAxesColorBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_COLOR); 

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glBindVertexArray(0);

        //// Main axes /////////////////

        // create vao
        GLES32.glGenVertexArrays(1, vaoMainAxes, 0);
        GLES32.glBindVertexArray(vaoMainAxes[0]);

        // axes vertices
        GLES32.glGenBuffers(1, vboMainAxesPos, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboMainAxesPos[0]);

        ByteBuffer byteBufferCoords = ByteBuffer.allocateDirect(axisCoords.length * 4);
        byteBufferCoords.order(ByteOrder.nativeOrder());
        FloatBuffer positionBufferCoords = byteBufferCoords.asFloatBuffer();
        positionBufferCoords.put(axisCoords);
        positionBufferCoords.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, axisCoords.length * 4, positionBufferCoords, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_POSITION); 

        // axes colors
        GLES32.glGenBuffers(1, vboMainAxesCol, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboMainAxesCol[0]);

        ByteBuffer byteBufferCoordsColor = ByteBuffer.allocateDirect(axisColors.length * 4);
        byteBufferCoordsColor.order(ByteOrder.nativeOrder());
        FloatBuffer colorBufferCoords = byteBufferCoordsColor.asFloatBuffer();
        colorBufferCoords.put(axisColors);
        colorBufferCoords.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, axisColors.length * 4, colorBufferCoords, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_COLOR); 

        //// shapes /////////////////////////////////////////////////////////////////////////////////////////////

        GLES32.glGenVertexArrays(1, vaoShapes, 0);
        GLES32.glBindVertexArray(vaoShapes[0]);

        // shapes vertices
        GLES32.glGenBuffers(1, vboShapesPos, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vboShapesPos[0]);

        ByteBuffer byteBufferShapes = ByteBuffer.allocateDirect(shapesCoords.length * 4);
        byteBufferShapes.order(ByteOrder.nativeOrder());
        FloatBuffer positionBufferShape = byteBufferShapes.asFloatBuffer();
        positionBufferShape.put(shapesCoords);
        positionBufferShape.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, shapesCoords.length * 4, positionBufferShape, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.RMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.RMC_ATTRIBUTE_POSITION);

        GLES32.glVertexAttrib3f(GLESMacros.RMC_ATTRIBUTE_COLOR, 1.0f, 1.0f, 0.0f);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
        GLES32.glBindVertexArray(0);

        //////////////////////////////////////////////////////////////////////

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
        float[] translationMatrix = new float[16];
        float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        // intialize above matrices to identity
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        Matrix.translateM(translationMatrix, 0,
            0.0f, 0.0f, -2.5f);

        Matrix.multiplyMM(modelViewMatrix, 0,
            modelViewMatrix, 0,
            translationMatrix, 0);

        Matrix.multiplyMM(modelViewProjectionMatrix, 0,
            perspectiveProjectionMatrix, 0,
            modelViewMatrix, 0);

        // send necessary matrices to shader in respective uniforms
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        // bind with vao (this will avoid many binding to vbo)
        GLES32.glBindVertexArray(vaoAxes[0]);  

        // draw necessary scene
        GLES32.glLineWidth(1.0f);
        GLES32.glDrawArrays(GLES32.GL_LINES, 0, 160);

        // small axis lines
        GLES32.glBindVertexArray(vaoMainAxes[0]);

        // draw necessary scene
        GLES32.glLineWidth(3.0f);
        GLES32.glDrawArrays(GLES32.GL_LINES, 0, 4);

        // draw necessary scene
        GLES32.glLineWidth(1.0f);
        GLES32.glDrawArrays(GLES32.GL_LINES, 0, 160);

        // shapes
        GLES32.glBindVertexArray(vaoShapes[0]);

        // draw necessary scene
        GLES32.glLineWidth(2.0f);
        GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 629);
        GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 629, 4);
        GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 633, 3);
        GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 636, 629);

        // unbind vao
        GLES32.glBindVertexArray(0);

        // stop using OpenGL program object
        GLES32.glUseProgram(0);

        // ~SwapBuffers()
        requestRender();
    }

    private void update() {
        // code
    }

    private void uninitialize() {
        if (vaoMainAxes[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vaoMainAxes, 0);
            vaoMainAxes[0] = 0;
        }

        if (vboMainAxesPos[0] != 0) {
            GLES32.glDeleteBuffers(1, vboMainAxesPos, 0);
            vboMainAxesPos[0] = 0;
        }

        if (vboMainAxesCol[0] != 0) {
            GLES32.glDeleteBuffers(1, vboMainAxesCol, 0);
            vboMainAxesCol[0] = 0;
        }

        if (vaoAxes[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vaoAxes, 0);
            vaoAxes[0] = 0;
        }

        if (vboAxesPos[0] != 0) {
            GLES32.glDeleteBuffers(1, vboAxesPos, 0);
            vboAxesPos[0] = 0;
        }

        if (vboAxesCol[0] != 0) {
            GLES32.glDeleteBuffers(1, vboAxesCol, 0);
            vboAxesCol[0] = 0;
        }

        if (vaoShapes[0] != 0) {
            GLES32.glDeleteVertexArrays(1, vaoShapes, 0);
            vaoShapes[0] = 0;
        }

        if (vboShapesPos[0] != 0) {
            GLES32.glDeleteBuffers(1, vboShapesPos, 0);
            vboShapesPos[0] = 0;
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

    private float[] generateGraphCoordinates() {
        int iNoOfCoords = 0;

        float[] pos = new float[3 * 160];

        for (float fOffset = -1.0f; fOffset <= 0; fOffset += (1.0f / 20.0f))
        {
            pos[(iNoOfCoords * 3) + 0] = -1.0f;
            pos[(iNoOfCoords * 3) + 1] = fOffset;
            pos[(iNoOfCoords * 3) + 2] = 0.0f;
            iNoOfCoords++;

            pos[(iNoOfCoords * 3) + 0] = 1.0f;
            pos[(iNoOfCoords * 3) + 1] = fOffset;
            pos[(iNoOfCoords * 3) + 2] = 0.0f;
            iNoOfCoords++;

            pos[(iNoOfCoords * 3) + 0] = -1.0f;
            pos[(iNoOfCoords * 3) + 1] = fOffset + 1.0f + (1.0f / 20.0f);
            pos[(iNoOfCoords * 3) + 2] = 0.0f;
            iNoOfCoords++;

            pos[(iNoOfCoords * 3) + 0] = 1.0f;
            pos[(iNoOfCoords * 3) + 1] = fOffset + 1.0f + (1.0f / 20.0f);
            pos[(iNoOfCoords * 3) + 2] = 0.0f;
            iNoOfCoords++;
        }

        for (float fOffset = -1.0f; fOffset <= 0; fOffset += (1.0f / 20.0f))
        {
            pos[(iNoOfCoords * 3) + 0] = fOffset;
            pos[(iNoOfCoords * 3) + 1] = -1.0f;
            pos[(iNoOfCoords * 3) + 2] = 0.0f;
            iNoOfCoords++;

            pos[(iNoOfCoords * 3) + 0] = fOffset;
            pos[(iNoOfCoords * 3) + 1] = 1.0f;
            pos[(iNoOfCoords * 3) + 2] = 0.0f;
            iNoOfCoords++;

            pos[(iNoOfCoords * 3) + 0] = fOffset + 1.0f + (1.0f / 20.0f);
            pos[(iNoOfCoords * 3) + 1] = -1.0f;
            pos[(iNoOfCoords * 3) + 2] = 0.0f;
            iNoOfCoords++;

            pos[(iNoOfCoords * 3) + 0] = fOffset + 1.0f + (1.0f / 20.0f);
            pos[(iNoOfCoords * 3) + 1] = 1.0f;
            pos[(iNoOfCoords * 3) + 2] = 0.0f;
            iNoOfCoords++;      
        }

        return pos;
    }

    private int generateTriangleAndIncircleCoords(float fY, float fX, float[] coords, int idx) {
        // variables 
        float s, a, b, c;
        float fRadius = 1.0f;
        float fAngle = 0.0f;

        // float[] coords = new float[628 * 3];

        /* Triangle */
        coords[idx++] = 0.0f;
        coords[idx++] = fY;
        coords[idx++] = 0.0f;

        coords[idx++] = -fX;
        coords[idx++] = -fY;
        coords[idx++] = 0.0f;

        coords[idx++] = fX;
        coords[idx++] = -fY;
        coords[idx++] = 0.0f;

        /* Radius Of Incircle */
        a = (float)Math.sqrt(Math.pow((-fX - 0.0f), 2.0f) + Math.pow(-fY - fY, 2.0f));
        b = (float)Math.sqrt(Math.pow((fX - (-fX)), 2.0f) + Math.pow(-fY - (-fY), 2.0f));
        c = (float)Math.sqrt(Math.pow((fX - 0.0f), 2.0f) + Math.pow(-fY - fY, 2.0f));
        s = (a + b + c) / 2.0f;
        fRadius = (float)Math.sqrt(s * (s - a) * (s - b) * (s - c)) / s;

        /* Incircle */
        for (fAngle = 0.0f; fAngle < 2 * Math.PI; fAngle += 0.01f)
        {
            coords[idx++] = fRadius * (float)Math.cos(fAngle);
            coords[idx++] = (fRadius * (float)Math.sin(fAngle)) - fX + fRadius;
            coords[idx++] = 0.0f;
        }

        return idx;
    }

    private int generateSquareCoords(float fX, float fY, float[] coords, int idx) {
        
        coords[idx++] = fX;
        coords[idx++] = fY;
        coords[idx++] = 0.0f;

        coords[idx++] = -fX;
        coords[idx++] = fY;
        coords[idx++] = 0.0f;

        coords[idx++] = -fX;
        coords[idx++] = -fY;
        coords[idx++] = 0.0f;

        coords[idx++] = fX;
        coords[idx++] = -fY;
        coords[idx++] = 0.0f;

        return idx;
    }

    private int generateOuterCircleCoords(float[] coords, int idx) {
        float fRadius = 1.0f;

        for (float fAngle = 0.0f; fAngle < 2 * Math.PI; fAngle += 0.01f)
        {
            coords[idx++] = fRadius * (float)Math.cos(fAngle);
            coords[idx++] = fRadius * (float)Math.sin(fAngle);
            coords[idx++] = 0.0f;
        }

        return idx;
    }
}
