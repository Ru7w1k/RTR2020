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

var vaoCube;
var vboCube;

var mUniform;
var vUniform;
var pUniform;

var texture_marble;
var samplerUniform;

var laUniform;
var kaUniform;
var ldUniform;
var kdUniform;
var lsUniform;
var ksUniform;
var shininessUniform;

var enableLightUniform;
var lightPositionUniform;

var lightAmbient = new Float32Array([0.5, 0.5, 0.5]);
var lightDiffuse = new Float32Array([1.0, 1.0, 1.0]);
var lightSpecular = new Float32Array([1.0, 1.0, 1.0]);
var lightPosition = new Float32Array([10.0, 10.0, 10.0, 1.0]);

var materialAmbient = new Float32Array([0.5, 0.5, 0.5]);
var materialDiffuse = new Float32Array([1.0, 1.0, 1.0]);
var materialSpecular = new Float32Array([1.0, 1.0, 1.0]);
var materialShininess = 128.0;

var perspectiveProjectionMatrix;

var bLight = false;
var angleCube = 0;

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

    // vertex shader
    var vertexShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "precision highp int;" +
        "in vec4 vPosition;" +
        "in vec4 vColor;" +
        "in vec3 vNormal;" +
        "in vec2 vTexcoord;" +
        "uniform mat4 u_m_matrix;" +
        "uniform mat4 u_v_matrix;" +
        "uniform mat4 u_p_matrix;" +
        "uniform vec4 u_light_position;" +
        "uniform int u_enable_light;" +
        "out vec3 tnorm;" +
        "out vec3 light_direction;" +
        "out vec3 viewer_vector;" +
        "out vec2 out_Texcoord;" +
        "out vec4 out_Color;" +
        "void main (void)" +
        "{" +
        "   if (u_enable_light == 1) " +
        "   { " +
        "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" +
        "       tnorm = mat3(u_v_matrix * u_m_matrix) * vNormal;" +
        "       light_direction = vec3(u_light_position - eye_coordinates);" +
        "       float tn_dot_ldir = max(dot(tnorm, light_direction), 0.0);" +
        "       viewer_vector = vec3(-eye_coordinates.xyz);" +
        "   }" +
        "   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vPosition;" +
        "   out_Texcoord = vTexcoord;" +
        "   out_Color = vColor;" +
        "}";

    vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);

    if (!gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS)) {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    // fragment shader
    var fragmentShaderSourceCode =
        "#version 300 es" +
        "\n" +
        "precision highp float;" +
        "precision highp int;" +
        "in vec2 out_Texcoord;" +
        "in vec4 out_Color;" +
        "in vec3 tnorm;" +
        "in vec3 light_direction;" +
        "in vec3 viewer_vector;" +
        "uniform vec3 u_la;" +
        "uniform vec3 u_ld;" +
        "uniform vec3 u_ls;" +
        "uniform vec3 u_ka;" +
        "uniform vec3 u_kd;" +
        "uniform vec3 u_ks;" +
        "uniform float u_shininess;" +
        "uniform int u_enable_light;" +
        "uniform sampler2D u_sampler;" +
        "out vec4 FragColor;" +
        "void main (void)" +
        "{" +
        "   vec3 phong_ads_light = vec3(1.0);" +
        "   if (u_enable_light == 1) " +
        "   { " +
        "       vec3 ntnorm = normalize(tnorm);" +
        "       vec3 nlight_direction = normalize(light_direction);" +
        "       vec3 nviewer_vector = normalize(viewer_vector);" +
        "       vec3 reflection_vector = reflect(-nlight_direction, ntnorm);" +
        "       float tn_dot_ldir = max(dot(ntnorm, nlight_direction), 0.0);" +
        "       vec3 ambient  = u_la * u_ka;" +
        "       vec3 diffuse  = u_ld * u_kd * tn_dot_ldir;" +
        "       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, nviewer_vector), 0.0), u_shininess);" +
        "       phong_ads_light = ambient + diffuse + specular;" +
        "   }" +
        "   vec4 tex = texture(u_sampler, out_Texcoord);" +
        "   FragColor = vec4((vec3(tex) * vec3(out_Color) * phong_ads_light), 1.0);" +
        "}";

    fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);

    if (!gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS)) {
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
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.RMC_ATTRIBUTE_NORMAL, "vNormal");
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

    // get unifrom locations
    mUniform = gl.getUniformLocation(shaderProgramObject, "u_m_matrix");
    vUniform = gl.getUniformLocation(shaderProgramObject, "u_v_matrix");
    pUniform = gl.getUniformLocation(shaderProgramObject, "u_p_matrix");

    samplerUniform = gl.getUniformLocation(shaderProgramObject, "u_sampler");

    laUniform = gl.getUniformLocation(shaderProgramObject, "u_la");
    kaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
    ldUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
    kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
    lsUniform = gl.getUniformLocation(shaderProgramObject, "u_ls");
    ksUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");
    shininessUniform = gl.getUniformLocation(shaderProgramObject, "u_shininess");

    enableLightUniform = gl.getUniformLocation(shaderProgramObject, "u_enable_light");
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");

    // pyramid Position
    var cubeVertices = new Float32Array([
        /* Top */
        1.0, 1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
        -1.0, 1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
        -1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0,
        1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0,

        /* Bottom */
        1.0, -1.0, 1.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 1.0,
        -1.0, -1.0, 1.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 0.0, 1.0,
        -1.0, -1.0, -1.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0,
        1.0, -1.0, -1.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0,

        /* Front */
        1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0,
        -1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0,
        -1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0,
        1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0,

        /* Back */
        1.0, -1.0, -1.0, 0.0, 1.0, 1.0, 0.0, 0.0, -1.0, 1.0, 0.0,
        -1.0, -1.0, -1.0, 0.0, 1.0, 1.0, 0.0, 0.0, -1.0, 1.0, 1.0,
        -1.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0,
        1.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0, 0.0, -1.0, 0.0, 0.0,

        /* Right */
        1.0, 1.0, -1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0,
        1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0,
        1.0, -1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0,
        1.0, -1.0, -1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0,

        /* Left */
        -1.0, 1.0, 1.0, 1.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0,
        -1.0, 1.0, -1.0, 1.0, 1.0, 0.0, -1.0, 0.0, 0.0, 1.0, 0.0,
        -1.0, -1.0, -1.0, 1.0, 1.0, 0.0, -1.0, 0.0, 0.0, 1.0, 1.0,
        -1.0, -1.0, 1.0, 1.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0
    ]);


    vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

    vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
    gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);

    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_POSITION,
        3, // 3 for x,y,z axes in vertex array
        gl.FLOAT,
        false, // is normalized?
        11 * 4, 0 * 4); // stride and offset
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_POSITION);

    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_COLOR,
        3, // 3 for r,g,b axes in vertex array
        gl.FLOAT,
        false, // is normalized?
        11 * 4, 3 * 4); // stride and offset
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_COLOR);

    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_NORMAL,
        3, // 3 for x,y,z axes in vertex array
        gl.FLOAT,
        false, // is normalized?
        11 * 4, 6 * 4); // stride and offset
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_NORMAL);

    gl.vertexAttribPointer(WebGLMacros.RMC_ATTRIBUTE_TEXCOORD,
        2, // 2 for s,t axes in vertex array
        gl.FLOAT,
        false, // is normalized?
        11 * 4, 9 * 4); // stride and offset
    gl.enableVertexAttribArray(WebGLMacros.RMC_ATTRIBUTE_TEXCOORD);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    // load texture
    texture_marble = loadGLTexture("marble.png");

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

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -5.0]);
    mat4.rotateX(modelMatrix, modelMatrix, degToRad(angleCube));
    mat4.rotateY(modelMatrix, modelMatrix, degToRad(angleCube));
    mat4.rotateZ(modelMatrix, modelMatrix, degToRad(angleCube));

    gl.uniformMatrix4fv(mUniform, false, modelMatrix);
    gl.uniformMatrix4fv(vUniform, false, viewMatrix);
    gl.uniformMatrix4fv(pUniform, false, perspectiveProjectionMatrix);

    gl.uniform3fv(laUniform, lightAmbient);
    gl.uniform3fv(ldUniform, lightDiffuse);
    gl.uniform3fv(lsUniform, lightSpecular);
    gl.uniform4fv(lightPositionUniform, lightPosition);

    gl.uniform3fv(kaUniform, materialAmbient);
    gl.uniform3fv(kdUniform, materialDiffuse);
    gl.uniform3fv(ksUniform, materialSpecular);
    gl.uniform1f(shininessUniform, materialShininess);

    if (bLight == true)
        gl.uniform1i(enableLightUniform, 1);
    else
        gl.uniform1i(enableLightUniform, 0);

    // bind with textures
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, texture_marble);
    gl.uniform1i(samplerUniform, 0);

    gl.bindVertexArray(vao);
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
    gl.bindVertexArray(null);
    gl.useProgram(null);

    // animation loop
    requestAnimationFrame(draw, canvas);
    update();
}

function update() {
    angleCube += 1.0;
    if (angleCube >= 360.0)
        angleCube = 0.0;
}

function uninit() {
    // code
    if (vaoCube) {
        gl.deleteVertexArray(vaoCube);
        vaoCube = null;
    }

    if (vboCube) {
        gl.deleteBuffer(vboCube);
        vboCube = null;
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

        case 76:
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

function loadGLTexture(img) {
    var tex = gl.createTexture();
    tex.image = new Image();
    tex.image.src = img;
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
