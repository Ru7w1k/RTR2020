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

var checkerTex;

var perspectiveProjectionMatrix;
var tapCount = 0;

var checkImageWidth = 64;
var checkImageHeight = 64;

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

        "out vec4 FragColor;\n" +

        "void main (void) \n" +
        "{ \n" +
        "	FragColor = texture(u_sampler, out_Texcoord); \n" +
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

    // vertex, colors, shader attribs, vbo, vao initalizations
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
    gl.bufferData(gl.ARRAY_BUFFER, 12 * 4, gl.DYNAMIC_DRAW);
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

    checkerTex = makeCheckImage();

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

    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -4.6]);
    mat4.multiply(modelViewProjectionMatrix,
        perspectiveProjectionMatrix, modelViewMatrix);

    gl.bindTexture(gl.TEXTURE_2D, checkerTex);
    gl.uniform1i(samplerUniform, 0);
    gl.uniformMatrix4fv(mvpMatrixUniform, false, modelViewProjectionMatrix);

    gl.bindVertexArray(vaoQuad);

    var quadsPosition = new Float32Array([
		-2.0, -1.0, 0.0,
		-2.0,  1.0, 0.0,
		 0.0,  1.0, 0.0,
		 0.0, -1.0, 0.0,
    ]);

    gl.bindBuffer(gl.ARRAY_BUFFER, vboQuadPos);
    gl.bufferData(gl.ARRAY_BUFFER, quadsPosition, gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

    quadsPosition[0] = 1.0;
	quadsPosition[1] = -1.0;
	quadsPosition[2] = 0.0;

	quadsPosition[3] = 1.0;
	quadsPosition[4] = 1.0;
	quadsPosition[5] = 0.0;

	quadsPosition[6] = 2.41421;
	quadsPosition[7] = 1.0;
	quadsPosition[8] = -1.41421;

	quadsPosition[9]  = 2.41421;
	quadsPosition[10] = -1.0;
	quadsPosition[11] = -1.41421;

    gl.bindBuffer(gl.ARRAY_BUFFER, vboQuadPos);
    gl.bufferData(gl.ARRAY_BUFFER, quadsPosition, gl.DYNAMIC_DRAW);
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
    }
}

function mouseDown() {
    // code
}

function degToRad(deg) {
    return deg * Math.PI / 180.0;
}

function makeCheckImage() {
    var checkImage = new Uint8Array(checkImageWidth * checkImageHeight * 4);

    for (var i = 0; i < checkImageHeight; i++) {
        for (var j = 0; j < checkImageWidth; j++) {
            var c = ((i & 8) ^ (j & 8)) * 255;

            checkImage[((i * 64) + j) * 4 + 0] = c;
            checkImage[((i * 64) + j) * 4 + 1] = c;
            checkImage[((i * 64) + j) * 4 + 2] = c;
            checkImage[((i * 64) + j) * 4 + 3] = 255;
        }
    }

    var checkTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, checkTexture);
    gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, checkImageWidth, checkImageHeight, 0, gl.RGBA, gl.UNSIGNED_BYTE, checkImage);
    gl.bindTexture(gl.TEXTURE_2D, null);

    return checkTexture;
}
