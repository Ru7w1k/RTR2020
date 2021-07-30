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

var vaoQuad;
var vboQuadPos;
var vboQuadTex;

var mvpMatrixUniform;
var samplerUniform;
var useTexureUniform;

var smileyTexture;

var perspectiveProjectionMatrix;
var tapCount = 0;

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
        "in vec2 vTexcoord;" +

        "uniform mat4 u_mvpMatrix; \n" +

        "out vec2 out_Texcoord;" +

        "void main (void) \n" +
        "{ \n" +
        "	gl_Position = u_mvpMatrix * vPosition; \n" +
        "   out_Texcoord = vTexcoord;" +
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
        "precision lowp int; \n" +

        "in vec2 out_Texcoord;" +

        "uniform sampler2D u_sampler;" +
        "uniform int u_useTexture;" +

        "out vec4 FragColor;\n" +

        "void main (void) \n" +
        "{ \n" +
        "   if (u_useTexture == 1)\n" +
        "	    FragColor = texture(u_sampler, out_Texcoord); \n" +
        "   else \n" +
        "	    FragColor = vec4(1.0); \n" +
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
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.RMC_ATTRIBUTE_TEXCOORD, "vTexcoord");

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
    samplerUniform = gl.getUniformLocation(shaderProgramObject, "u_sampler");
    useTexureUniform = gl.getUniformLocation(shaderProgramObject, "u_useTexture");

    // vertex, colors, shader attribs, vbo, vao initalizations
    var quadVertices = new Float32Array([
        1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0,
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0
    ]);

    var quadTexcoords = new Float32Array([
        1.0, 1.0,
        0.0, 1.0,
        0.0, 0.0,
        1.0, 0.0
    ]);

    vaoQuad = gl.createVertexArray();
    gl.bindVertexArray(vaoQuad);

    vboQuadPos = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboQuadPos);
    gl.bufferData(gl.ARRAY_BUFFER, quadVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vboQuadTex = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboQuadTex);
    gl.bufferData(gl.ARRAY_BUFFER, quadTexcoords, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_TEXCOORD);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);

    smileyTexture = loadTexture("smiley.png");

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

    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
    mat4.multiply(modelViewProjectionMatrix,
        perspectiveProjectionMatrix, modelViewMatrix);

    if (tapCount == 0) {
        gl.uniform1i(useTexureUniform, 0);
    } else {
        gl.uniform1i(useTexureUniform, 1);
    }

    gl.bindTexture(gl.TEXTURE_2D, smileyTexture);
    gl.uniform1i(samplerUniform, 0);
    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

    gl.bindVertexArray(vaoQuad);

    gl.bindBuffer(gl.ARRAY_BUFFER, vboQuadTex);
    gl.bufferData(gl.ARRAY_BUFFER, getTexCoords(), gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.bindVertexArray(null);
    gl.useProgram(null);

    // animation loop
    requestAnimationFrame(draw, canvas);
    update();
}

function update() {
    //
}

function uninit() {
    // code
    if (vaoQuad) {
        gl.deleteVertexArray(vaoQuad);
        vaoQuad = null;
    }

    if (vboPyramidCol) {
        gl.deleteBuffer(vboPyramidCol);
        vboPyramidCol = null;
    }

    if (vboQuadPos) {
        gl.deleteBuffer(vboQuadPos);
        vboQuadPos = null;
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

        case 48: // 0
        case 96: // numpad0
            tapCount = 0;
            break;


        case 49: // 1
        case 97: // numpad1
            tapCount = 1;
            break;

        case 50: // 2
        case 98: // numpad2
            tapCount = 2;
            break;

        case 51: // 3
        case 99: // numpad3
            tapCount = 3;
            break;

        case 52: // 4
        case 100: // numpad4
            tapCount = 4;
            break;
    }
}

function mouseDown() {
    // code
}

function degToRad(deg) {
    return deg * Math.PI / 180.0;
}

function loadTexture(src) {
    var tex = gl.createTexture();
    tex.image = new Image();
    tex.image.src = src;

    tex.image.onload = function () {
        gl.bindTexture(gl.TEXTURE_2D, tex);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, tex.image);
        gl.bindTexture(gl.TEXTURE_2D, null);
    };

    return tex;
}

function getTexCoords() {
    switch (tapCount) {
        case 1:
            return new Float32Array([
                0.5, 0.5,
                0.0, 0.5,
                0.0, 0.0,
                0.5, 0.0
            ]);

        case 2:
            return new Float32Array([
                1.0, 1.0,
                0.0, 1.0,
                0.0, 0.0,
                1.0, 0.0
            ]);

        case 3:
            return new Float32Array([
                2.0, 2.0,
                0.0, 2.0,
                0.0, 0.0,
                2.0, 0.0
            ]);

        case 4:
            return new Float32Array([
                0.5, 0.5,
                0.5, 0.5,
                0.5, 0.5,
                0.5, 0.5
            ]);

        default:
            tapCount = 0;
            return new Float32Array([
                1.0, 1.0,
                0.0, 1.0,
                0.0, 0.0,
                1.0, 0.0
            ]);
    }
}
