// global variables
var canvas = null;
var gl = null;
var canvas_original_width;
var canvas_original_height;
var bFullscreen = false;

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

var sphere = null;

var mvpUniform;
var pUniform;
var ldUniform;
var kdUniform;
var enableLightUniform;
var lightPositionUniform;
var mvpMatrixUniform;

var perspectiveProjectionMatrix;
var angleCube = 0.0;
var bLight = false;

var stack = new Stack();
var day = 0.0;
var year = 0.0;


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

        "precision lowp int; \n" +

        "in vec4 vPosition; \n" +
        "in vec4 vColor; \n" +

        "uniform mat4 u_mvpMatrix; \n" +

        "out vec4 out_Color; \n" +

        "void main (void) \n" +
        "{ \n" +
        "	gl_Position = u_mvpMatrix * vPosition; \n" +
        "   out_Color = vColor; \n" +
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

        "in vec4 out_Color; \n" +
        "out vec4 FragColor;" +

        "void main(void) \n" +
        "{ \n" +
        "	FragColor = out_Color; \n" +
        "} \n";

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
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.RMC_ATTRIBUTE_COLOR, "vColor");

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
    mvpUniform = gl.getUniformLocation(shaderProgramObject, "u_mvpMatrix");

    gl.vertexAttrib3f(WebGLMacros.RMC_ATTRIBUTE_COLOR, 0.5, 0.35, 0.05);

    sphere = new Mesh();
    makeSphere(sphere, 0.5, 30, 30);

    // set clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    // set depth testing
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

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

    //declaration of matrices
    var modelViewMatrix = mat4.create()
    var modelViewProjectionMatrix = mat4.create();

    mat4.lookAt(modelViewMatrix, [0.0, 0.0, 5.0], [0.0, 0.0, 0.0], [0.0, 1.0, 0.0]);
    stack.push(modelViewMatrix);

    // sun
    mat4.multiply(modelViewProjectionMatrix,
        perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    sphere.draw(1.0, 1.0, 0.0);

    mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(year));
    mat4.translate(modelViewMatrix, modelViewMatrix, [1.5, 0.0, 0.0]);
    mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(day));

    stack.push(modelViewMatrix);

    mat4.scale(modelViewMatrix, modelViewMatrix, [0.4, 0.4, 0.4]);
    mat4.multiply(modelViewProjectionMatrix,
        perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    sphere.draw(0.4, 0.9, 1.0);

    modelViewMatrix = mat4.create();
    modelViewMatrix = stack.pop();

    mat4.translate(modelViewMatrix, modelViewMatrix, [0.7, 0.0, 0.0]);
    mat4.scale(modelViewMatrix, modelViewMatrix, [0.175, 0.175, 0.175]);

    // send necessary matrices to shader in respective uniforms
    modelViewProjectionMatrix = mat4.create();
    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);

    sphere.draw(0.9, 0.9, 0.9);

    stack.reset();

    // animation loop
    requestAnimationFrame(draw, canvas);
}

function update() {

}

function uninit() {
    // code
    sphere.deallocate();

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

        case 83: // for 'S' or 's'
            day = (day - 3) % 360;
            break;

        case 68: // for 'D' or 'd'
            day = (day + 3) % 360;
            break;

        case 84: // for 'T' or 't'
            year = (year - 3) % 360;
            break;

        case 89: // fpr 'Y' or 'y'
            year = (year + 3) % 360;
            break;
    }
}

function mouseDown() {
    // code
}

function degToRad(deg) {
    return deg * Math.PI / 180.0;
}
