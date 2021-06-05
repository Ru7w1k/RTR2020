// global variables
var canvas = null;
var gl = null;
var canvas_original_width;
var canvas_original_height;
var bFullscreen = false;

// macro for vertex attributes
const WebGLMacros = {
    RMC_ATTRIBUTE_POSITION: 0,
    RMC_ATTRIBUTE_COLOR: 1,
    RMC_ATTRIBUTE_NORMAL: 2,
    RMC_ATTRIBUTE_TEXCOORD: 3,
};

// to start animation (browser)
var requestAnimationFrame =
    window.requestAnimationFrame ||       /* generic */
    window.webkitRequestAnimationFrame || /* apple */
    window.mozRequestAnimationFrame ||    /* mozilla */
    window.oRequestAnimationFrame ||      /* opera */
    window.msRequestAnimationFrame ||     /* microsoft */
    null;

// global varibles
var vertexShaderObject;
var fragmentShaderObject;
var gShaderProgramObject;

var vaoTri;
var vboTriPos;
var vboTriCol;

var vaoQuad;
var vboQuadPos;
var mvpMatrixUniform;

var perspectiveProjectionMatrix;
var angleTri = 0.0;
var angleQuad = 0.0;

// onload function
function main() {
    /* document is inbuilt variable */
    // get canvas element from DOM    
    canvas = document.getElementById("RMC");
    if (!canvas)
        console.log("obtaining canvas failed..");
    else
        console.log("obtaining canvas succeeded..");

    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;

    /* window is inbuilt variable 
       it is one of the DOM element */
    // register event handlers
    window.addEventListener("keydown", keyDown, false); // false - bubbling is expected
    window.addEventListener("click", mouseDown, false);
    window.addEventListener("resize", resize, false);

    // initialize WebGL2
    init();

    // warmup resize and draw call
    resize();
    draw(); /* no default draw call */

}

function toggleFullscreen() {
    // this element fetches the current fullscreen state
    var fullscreen_element =
        document.fullscreenElement ||     /* generic */
        document.webkitFullscreen ||      /* apple */
        document.mozFullScreenElement ||  /* mozilla */
        document.msFullscreen ||          /* microsoft */
        null;

    // if the state is null -> no fullscreen at the moment
    // call browser specific fullscreen function
    if (fullscreen_element == null) {
        if (canvas.requestFullscreen) canvas.requestFullscreen();
        else if (canvas.webkitRequestFullscreen) canvas.webkitRequestFullscreen();
        else if (canvas.mozRequestFullScreen) canvas.mozRequestFullScreen();
        else if (canvas.msRequestFullscreen) canvas.msRequestFullscreen();
        bFullscreen = true;
    }
    // else canvas is in fullscreen state
    // call browser specific exit fullscreen function
    else {
        if (document.exitFullscreen) document.exitFullscreen();
        else if (docuement.webkitExitFullscreen) docuement.webkitExitFullscreen();
        else if (docuement.mozCancelFullScreen) docuement.mozCancelFullScreen();
        else if (docuement.msExitFullscreen) docuement.msExitFullscreen();
        bFullscreen = false;
    }
}

function init() {
    // get webgl2 context from the canvas
    gl = canvas.getContext("webgl2");
    if (!gl)
        console.log("obtaining WebGL2 context failed..");
    else
        console.log("obtaining WebGL2 context succeeded..");

    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    var vertexShaderSourceCode =
        "#version 300 es \n" +

        "in vec4 vPosition; \n" +
        "in vec3 vColor; \n" +
        "out vec4 out_Color; \n" +
        "uniform mat4 u_mvpMatrix; \n" +

        "void main (void) \n" +
        "{ \n" +
        "	out_Color = vec4(vColor, 1.0); \n" +
        "	gl_Position = u_mvpMatrix * vPosition; \n" +
        "} \n";

    vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);

    if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    var fragmentShaderSourceCode =
        "#version 300 es \n" +
        "precision highp float; \n" +

        "in  vec4 out_Color; \n" +
        "out vec4 FragColor; \n" +

        "void main (void) \n" +
        "{ \n" +
        "	FragColor = out_Color; \n" +
        "} \n"

    fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);

    if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    // shader program 
    shaderProgramObject = gl.createProgram();
    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);

    // pre-linking binding of shader program object with vertex shader attributes
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.RMC_ATTRIBUTE_POSITION, "vPosition");

    // linking
    gl.linkProgram(shaderProgramObject);
    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    // get mvp uniform location
    mvpMatrixUniform = gl.getUniformLocation(shaderProgramObject, "u_mvpMatrix");

    // vertex, colors, shader attribs, vbo, vao initalizations
    var triangleVertices = new Float32Array([
        0.0, 1.0, 0.0,
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0
    ]);

    var triangleColors = new Float32Array([
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 0.0, 1.0
    ]);

    var quadVertices = new Float32Array([
        1.0, 1.0, 0.0,
        1.0, -1.0, 0.0,
        - 1.0, -1.0, 0.0,
        -1.0, 1.0, 0.0,
    ]);


    vaoTri = gl.createVertexArray();
    gl.bindVertexArray(vaoTri);

    vboTriPos = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboTriPos);
    gl.bufferData(gl.ARRAY_BUFFER, triangleVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vboTriCol = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboTriCol);
    gl.bufferData(gl.ARRAY_BUFFER, triangleColors, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_COLOR);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);


    vaoQuad = gl.createVertexArray();
    gl.bindVertexArray(vaoQuad);

    vboQuadPos = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboQuadPos);
    gl.bufferData(gl.ARRAY_BUFFER, quadVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.vertexAttrib3f(WebGLMacros.RMC_ATTRIBUTE_COLOR, 0.0, 0.0, 1.0);

    gl.bindVertexArray(null);

    // set clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    // initialize projection matrix
    perspectiveProjectionMatrix = mat4.create();
}

function resize() {
    if (bFullscreen == true) {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    } else {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    // set the viewport
    gl.viewport(0, 0, canvas.width, canvas.height);

    // perspective projection
    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
}

function draw() {
    // clear buffers
    gl.clear(gl.COLOR_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    // triangle
    mat4.translate(modelViewMatrix, modelViewMatrix, [-1.5, 0.0, -5.0]);
    mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(angleTri));

    mat4.multiply(modelViewProjectionMatrix,
        perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);
    gl.bindVertexArray(vaoTri);
    gl.drawArrays(gl.TRIANGLES, 0, 3);
    gl.bindVertexArray(null);

    // quad
    modelViewMatrix = mat4.create();
    mat4.translate(modelViewMatrix, modelViewMatrix, [1.5, 0.0, -5.0]);
    mat4.rotateX(modelViewMatrix, modelViewMatrix, degToRad(angleQuad));

    mat4.multiply(modelViewProjectionMatrix,
        perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);
    gl.bindVertexArray(vaoQuad);
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.bindVertexArray(null);

    gl.useProgram(null);

    // animation loop
    requestAnimationFrame(draw, canvas);
    update();
}

function update() {
    angleTri += 1.0;
    if (angleTri >= 360.0)
        angleTri = 0.0;

    angleQuad += 1.0;
    if (angleQuad >= 360.0)
        angleQuad = 0.0;
}

function uninit() {
    // code
    if (vaoQuad) {
        gl.deleteVertexArray(vaoQuad);
        vaoQuad = null;
    }

    if (vboQuadPos) {
        gl.deleteBuffer(vboQuadPos);
        vboQuadPos = null;
    }

    if (vaoTri) {
        gl.deleteVertexArray(vaoTri);
        vaoTri = null;
    }

    if (vboTriCol) {
        gl.deleteBuffer(vboTriCol);
        vboTriCol = null;
    }

    if (vboTriPos) {
        gl.deleteBuffer(vboTriPos);
        vboTriPos = null;
    }

    if (shaderProgramObject) {
        if (fragmentShaderObject) {
            gl.detachShader(shaderProgramObject, fragmentShaderObject);
            gl.deleteShader(fragmentShaderObject);
            fragmentShaderObject = null;
        }

        if (vertexShaderObject) {
            gl.detachShader(shaderProgramObject, vertexShaderObject);
            gl.deleteShader(vertexShaderObject);
            vertexShaderObject = null;
        }

        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }
}

function keyDown(event) {
    // code
    switch (event.keyCode) {
        case 27: // Esc
            uninit();
            window.close();
            break;

        case 70: // f key
            toggleFullscreen();
            break;
    }
}

function mouseDown() {
    // code
}

function degToRad(degrees) {
    return (degrees * Math.PI / 180.0);
}
