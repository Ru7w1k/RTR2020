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
var shaderProgramObject;

var vaoPyramid;
var vboPyramidPos;
var vboPyramidNor;

var mUniform;
var vUniform
var pUniform;

var laUniform;
var ldUniform;
var lsUniform;
var lightPositionUniform;

var kaUniform;
var kdUniform;
var ksUniform;
var shininessUniform;

var enableLightUniform;

var perspectiveProjectionMatrix;
var anglePyramid = 0.0;
var bLight = false;

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

        "in vec3 out_PhongLight; \n" +
        "out vec4 FragColor; \n" +

        "void main (void) \n" +
        "{ \n" +
        "	FragColor = vec4(out_PhongLight, 1.0); \n" +
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
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.RMC_ATTRIBUTE_NORMAL, "vNormal");

    // linking
    gl.linkProgram(shaderProgramObject);
    if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS)) {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    // get uniforms
    mUniform = gl.getUniformLocation(shaderProgramObject, "u_mMatrix");
    vUniform = gl.getUniformLocation(shaderProgramObject, "u_vMatrix");
    pUniform = gl.getUniformLocation(shaderProgramObject, "u_pMatrix");

    laUniform = gl.getUniformLocation(shaderProgramObject, "u_La");
    ldUniform = gl.getUniformLocation(shaderProgramObject, "u_Ld");
    lsUniform = gl.getUniformLocation(shaderProgramObject, "u_Ls");

    kaUniform = gl.getUniformLocation(shaderProgramObject, "u_Ka");
    kdUniform = gl.getUniformLocation(shaderProgramObject, "u_Kd");
    ksUniform = gl.getUniformLocation(shaderProgramObject, "u_Ks");

    shininessUniform = gl.getUniformLocation(shaderProgramObject, "u_Shininess");
    enableLightUniform = gl.getUniformLocation(shaderProgramObject, "u_bLight");
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_LightPos");

    // vertex, colors, shader attribs, vbo, vao initalizations
    var pyramidVertices = new Float32Array([
        /* Front */
        0.0, 1.0, 0.0,
        -1.0, -1.0, 1.0,
        1.0, -1.0, 1.0,

        /* Right */
        0.0, 1.0, 0.0,
        1.0, -1.0, 1.0,
        1.0, -1.0, -1.0,

        /* Left */
        0.0, 1.0, 0.0,
        -1.0, -1.0, -1.0,
        -1.0, -1.0, 1.0,

        /* Back */
        0.0, 1.0, 0.0,
        1.0, -1.0, -1.0,
        -1.0, -1.0, -1.0,
    ]);

    var pyramidNormals = new Float32Array([
        /* Front */
        0.0, 0.447214, 0.894427,
        0.0, 0.447214, 0.894427,
        0.0, 0.447214, 0.894427,

        /* Right */
        0.894427, 0.447214, 0.0,
        0.894427, 0.447214, 0.0,
        0.894427, 0.447214, 0.0,

        /* Left */
        -0.894427, 0.447214, 0.0,
        -0.894427, 0.447214, 0.0,
        -0.894427, 0.447214, 0.0,

        /* Back */
        0.0, 0.447214, -0.894427,
        0.0, 0.447214, -0.894427,
        0.0, 0.447214, -0.894427
    ]);

    vaoPyramid = gl.createVertexArray();
    gl.bindVertexArray(vaoPyramid);

    vboPyramidPos = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboPyramidPos);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vboPyramidNor = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vboPyramidNor);
    gl.bufferData(gl.ARRAY_BUFFER, pyramidNormals, gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_NORMAL, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_NORMAL);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    gl.bindVertexArray(null);

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

    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();

    // triangle
    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -4.0]);
    mat4.rotateY(modelMatrix, modelMatrix, degToRad(anglePyramid));

    // send necessary to shader in respective uniforms
    gl.uniformMatrix4fv(mUniform, false, modelMatrix);
    gl.uniformMatrix4fv(vUniform, false, viewMatrix);
    gl.uniformMatrix4fv(pUniform, false, perspectiveProjectionMatrix);


    if (bLight) {
        const la = new Float32Array([
            0.0, 0.0, 0.0,
            0.0, 0.0, 0.0,
        ]);

        const ld = new Float32Array([
            1.0, 0.0, 0.0,
            0.0, 0.0, 1.0,
        ]);

        const ls = new Float32Array([
            1.0, 0.0, 0.0,
            0.0, 0.0, 1.0,
        ]);

        const lPositions = new Float32Array([
            2.0, 0.0, 0.0, 1.0,
            -2.0, 0.0, 0.0, 1.0,
        ]);

        gl.uniform3fv(laUniform, la);
        gl.uniform3fv(ldUniform, ld);
        gl.uniform3fv(lsUniform, ls);
        gl.uniform4fv(lightPositionUniform, lPositions);

        gl.uniform3f(kaUniform, 0.0, 0.0, 0.0);
        gl.uniform3f(kdUniform, 0.5, 0.2, 0.7);
        gl.uniform3f(ksUniform, 0.7, 0.7, 0.7);
        gl.uniform1f(shininessUniform, 128.0);

        gl.uniform1i(enableLightUniform, 1);
    }
    else {
        gl.uniform1i(enableLightUniform, 0);
    }

    gl.bindVertexArray(vaoPyramid);
    gl.drawArrays(gl.TRIANGLES, 0, 12);
    gl.bindVertexArray(null);

    gl.useProgram(null);

    // animation loop
    requestAnimationFrame(draw, canvas);
    update();
}

function update() {
    anglePyramid += 1.0;
    if (anglePyramid >= 360.0)
        anglePyramid = 0.0;
}

function uninit() {
    // code
    if (vaoPyramid) {
        gl.deleteVertexArray(vaoPyramid);
        vaoPyramid = null;
    }

    if (vboPyramidCol) {
        gl.deleteBuffer(vboPyramidCol);
        vboPyramidCol = null;
    }

    if (vboPyramidPos) {
        gl.deleteBuffer(vboPyramidPos);
        vboPyramidPos = null;
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

        case 76: // for 'L' or 'l'
            bLight = !bLight;
            break;
    }
}

function mouseDown() {
    // code
}

function degToRad(deg) {
    return deg * Math.PI / 180.0;
}
