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

var mvpMatrixUniform;

var vao = 0;
var vboVertex = 0;
var vboColorSmallAxes = 0;

var vaoAxes = 0;
var vboVertexAxes = 0;
var vboColorAxes = 0;

var vaoShapes = 0;
var vboVertexShapes = 0;

var perspectiveProjectionMatrix;

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
    var graphCoords = generateGraphCoordinates();

    // color array 
    var axisCoords = new Float32Array([
        -1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, 1.0, 0.0
    ]);

    var axisColors = new Float32Array([
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0
    ]);

    var smallAxisColors = new Float32Array(graphCoords.length);
    for (var i = 0; i < smallAxisColors.length; i += 3) {
        smallAxisColors[i + 0] = 0.0;
        smallAxisColors[i + 1] = 0.0;
        smallAxisColors[i + 2] = 1.0;
    }

    var shapesCoords = new Float32Array(1300 * 3);
    var shapesCoordsCount = 0;
    var fX, fY;

    shapesCoordsCount = generateOuterCircleCoords(shapesCoords, shapesCoordsCount);

    fX = fY = Math.cos(Math.PI / 4.0);
    shapesCoordsCount = generateSquareCoords(fX, fY, shapesCoords, shapesCoordsCount);
    shapesCoordsCount = generateTriangleAndIncircleCoords(fX, fY, shapesCoords, shapesCoordsCount);

    /////// graph
    vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

    vboVertex = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboVertex);
    gl.bufferData(gl.ARRAY_BUFFER, graphCoords, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_VERTEX);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vboColorSmallAxes = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboColorSmallAxes);
    gl.bufferData(gl.ARRAY_BUFFER, smallAxisColors, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_COLOR);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    /////// axes
    vaoAxes = gl.createVertexArray();
    gl.bindVertexArray(vaoAxes);

    vboVertexAxes = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboVertexAxes);
    gl.bufferData(gl.ARRAY_BUFFER, axisCoords, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_VERTEX);

    vboColorAxes = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboColorAxes);
    gl.bufferData(gl.ARRAY_BUFFER, axisColors, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_COLOR);

    /////// shapes
    vaoShapes = gl.createVertexArray();
    gl.bindVertexArray(vaoShapes);

    vboVertexShapes = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboVertexShapes);
    gl.bufferData(gl.ARRAY_BUFFER, shapesCoords, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_VERTEX, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_VERTEX);

    gl.vertexAttrib3f(WebGLMacros.RMC_ATTRIBUTE_COLOR, 1.0, 1.0, 0.0);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);
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
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -2.0]);

    mat4.multiply(modelViewProjectionMatrix,
        perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

    // graph
    gl.bindVertexArray(vao);
    gl.lineWidth(1.0);
    gl.drawArrays(gl.LINES, 0, 160);
    gl.bindVertexArray(null);

    // axes
    gl.bindVertexArray(vaoAxes);
    gl.lineWidth(3.0);
    gl.drawArrays(gl.LINES, 0, 4);
    gl.bindVertexArray(null);

    // shapes
    gl.bindVertexArray(vaoShapes);
    gl.lineWidth(2.0);
    gl.drawArrays(gl.LINE_LOOP, 0, 629);
    gl.drawArrays(gl.LINE_LOOP, 629, 4);
    gl.drawArrays(gl.LINE_LOOP, 633, 3);
    gl.drawArrays(gl.LINE_LOOP, 636, 629);
    gl.bindVertexArray(null);

    gl.useProgram(null);

    // animation loop
    requestAnimationFrame(draw, canvas);
}

function uninit() {
    // code
    if (vao) {
        gl.deleteVertexArray(vao);
        vao = null;
    }

    if (vboVertex) {
        gl.deleteBuffer(vboVertex);
        vboVertex = null;
    }

    if (vaoAxes) {
        gl.deleteVertexArray(vaoAxes);
        vaoAxes = null;
    }

    if (vboVertexAxes) {
        gl.deleteBuffer(vboVertexAxes);
        vboVertexAxes = null;
    }

    if (vboColorAxes) {
        gl.deleteBuffer(vboColorAxes);
        vboColorAxes = null;
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

function generateGraphCoordinates() {
    var iNoOfCoords = 0;

    var pos = new Float32Array(3 * 160);

    for (var fOffset = -1.0; fOffset <= 0; fOffset += (1.0 / 20.0)) {
        pos[(iNoOfCoords * 3) + 0] = -1.0;
        pos[(iNoOfCoords * 3) + 1] = fOffset;
        pos[(iNoOfCoords * 3) + 2] = 0.0;
        iNoOfCoords++;

        pos[(iNoOfCoords * 3) + 0] = 1.0;
        pos[(iNoOfCoords * 3) + 1] = fOffset;
        pos[(iNoOfCoords * 3) + 2] = 0.0;
        iNoOfCoords++;

        pos[(iNoOfCoords * 3) + 0] = -1.0;
        pos[(iNoOfCoords * 3) + 1] = fOffset + 1.0 + (1.0 / 20.0);
        pos[(iNoOfCoords * 3) + 2] = 0.0;
        iNoOfCoords++;

        pos[(iNoOfCoords * 3) + 0] = 1.0;
        pos[(iNoOfCoords * 3) + 1] = fOffset + 1.0 + (1.0 / 20.0);
        pos[(iNoOfCoords * 3) + 2] = 0.0;
        iNoOfCoords++;
    }

    for (var fOffset = -1.0; fOffset <= 0; fOffset += (1.0 / 20.0)) {
        pos[(iNoOfCoords * 3) + 0] = fOffset;
        pos[(iNoOfCoords * 3) + 1] = -1.0;
        pos[(iNoOfCoords * 3) + 2] = 0.0;
        iNoOfCoords++;

        pos[(iNoOfCoords * 3) + 0] = fOffset;
        pos[(iNoOfCoords * 3) + 1] = 1.0;
        pos[(iNoOfCoords * 3) + 2] = 0.0;
        iNoOfCoords++;

        pos[(iNoOfCoords * 3) + 0] = fOffset + 1.0 + (1.0 / 20.0);
        pos[(iNoOfCoords * 3) + 1] = -1.0;
        pos[(iNoOfCoords * 3) + 2] = 0.0;
        iNoOfCoords++;

        pos[(iNoOfCoords * 3) + 0] = fOffset + 1.0 + (1.0 / 20.0);
        pos[(iNoOfCoords * 3) + 1] = 1.0;
        pos[(iNoOfCoords * 3) + 2] = 0.0;
        iNoOfCoords++;
    }

    return pos;
}

function generateTriangleAndIncircleCoords(fY, fX, coords, idx) {
    // variables 
    var s, a, b, c;
    var fRadius = 1.0;
    var fAngle = 0.0;

    /* Triangle */
    coords[idx++] = 0.0;
    coords[idx++] = fY;
    coords[idx++] = 0.0;

    coords[idx++] = -fX;
    coords[idx++] = -fY;
    coords[idx++] = 0.0;

    coords[idx++] = fX;
    coords[idx++] = -fY;
    coords[idx++] = 0.0;

    /* Radius Of Incircle */
    a = Math.sqrt(Math.pow((-fX - 0.0), 2.0) + Math.pow(-fY - fY, 2.0));
    b = Math.sqrt(Math.pow((fX - (-fX)), 2.0) + Math.pow(-fY - (-fY), 2.0));
    c = Math.sqrt(Math.pow((fX - 0.0), 2.0) + Math.pow(-fY - fY, 2.0));
    s = (a + b + c) / 2.0;
    fRadius = Math.sqrt(s * (s - a) * (s - b) * (s - c)) / s;

    /* Incircle */
    for (fAngle = 0.0; fAngle < 2 * Math.PI; fAngle += 0.01) {
        coords[idx++] = fRadius * Math.cos(fAngle);
        coords[idx++] = (fRadius * Math.sin(fAngle)) - fX + fRadius;
        coords[idx++] = 0.0;
    }

    return idx;
}

function generateSquareCoords(fX, fY, coords, idx) {

    coords[idx++] = fX;
    coords[idx++] = fY;
    coords[idx++] = 0.0;

    coords[idx++] = -fX;
    coords[idx++] = fY;
    coords[idx++] = 0.0;

    coords[idx++] = -fX;
    coords[idx++] = -fY;
    coords[idx++] = 0.0;

    coords[idx++] = fX;
    coords[idx++] = -fY;
    coords[idx++] = 0.0;

    return idx;
}

function generateOuterCircleCoords(coords, idx) {
    var fRadius = 1.0;

    for (var fAngle = 0.0; fAngle < 2 * Math.PI; fAngle += 0.01) {
        coords[idx++] = fRadius * Math.cos(fAngle);
        coords[idx++] = fRadius * Math.sin(fAngle);
        coords[idx++] = 0.0;
    }

    return idx;
}
