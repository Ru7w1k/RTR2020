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
var sphere = null;
var lights = null;

// P E R  V E R T E X //////////////////////////////////////////////////////
var vertexShaderObjectPV;
var fragmentShaderObjectPV;
var shaderProgramObjectPV;
var mUniformPV;
var vUniformPV;
var pUniformPV;
var laUniformPV;
var ldUniformPV;
var lsUniformPV;
var lightPositionUniformPV;
var kaUniformPV;
var kdUniformPV;
var ksUniformPV;
var shininessUniformPV;
var enableLightUniformPV;
////////////////////////////////////////////////////////////////////////////

// P E R  F R A G M E N T //////////////////////////////////////////////////
var vertexShaderObjectPF;
var fragmentShaderObjectPF;
var shaderProgramObjectPF;
var mUniformPF;
var vUniformPF;
var pUniformPF;
var laUniformPF;
var ldUniformPF;
var lsUniformPF;
var lightPositionUniformPF;
var kaUniformPF;
var kdUniformPF;
var ksUniformPF;
var shininessUniformPF;
var enableLightUniformPF;
////////////////////////////////////////////////////////////////////////////

var perspectiveProjectionMatrix;
var bLight = false;
var bFragment = false;
var axis = 0;

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

    //// per vertex shader /////////////////////////////////////////////////////////////////////////

    var vertexShaderSourceCodePV =
        "#version 300 es \n" +
        "precision lowp int; \n" +

        "in vec4 vPosition; \n" +
        "in vec3 vNormal; \n" +

        "uniform mat4 u_mMatrix; \n" +
        "uniform mat4 u_vMatrix; \n" +
        "uniform mat4 u_pMatrix; \n" +

        "uniform vec3 u_La; \n" +
        "uniform vec3 u_Ld; \n" +
        "uniform vec3 u_Ls; \n" +
        "uniform vec3 u_Ka; \n" +
        "uniform vec3 u_Kd; \n" +
        "uniform vec3 u_Ks; \n" +

        "uniform float u_Shininess; \n" +
        "uniform vec4 u_LightPos; \n" +
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

        "		vec3 lightDir = normalize(vec3(u_LightPos - eyeCoordinates)); \n" +
        "		float tNormDotLightDir = max(dot(tNorm, lightDir), 0.0); \n" +
        "		vec3 reflectionVector = reflect(-lightDir, tNorm); \n" +

        "		vec3 ambient = u_La * u_Ka; \n" +
        "		vec3 diffuse = u_Ld * u_Kd * tNormDotLightDir; \n" +
        "		vec3 specular = u_Ls * u_Ks * pow(max(dot(reflectionVector, viewerVector), 0.0), u_Shininess); \n" +

        "		out_PhongLight = ambient + diffuse + specular; \n" +
        "	} \n" +
        "	else \n" +
        "	{ \n" +
        "		out_PhongLight = vec3(1.0, 1.0, 1.0); \n" +
        "	} \n" +
        "	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition; \n" +
        "} \n";

    vertexShaderObjectPV = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObjectPV, vertexShaderSourceCodePV);
    gl.compileShader(vertexShaderObjectPV);

    if (gl.getShaderParameter(vertexShaderObjectPV, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(vertexShaderObjectPV);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    var fragmentShaderSourceCodePV =
        "#version 300 es \n" +
        "precision highp float; \n" +

        "in vec3 out_PhongLight; \n" +
        "out vec4 FragColor; \n" +

        "void main (void) \n" +
        "{ \n" +
        "	FragColor = vec4(out_PhongLight, 1.0); \n" +
        "} \n";

    fragmentShaderObjectPV = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObjectPV, fragmentShaderSourceCodePV);
    gl.compileShader(fragmentShaderObjectPV);

    if (gl.getShaderParameter(fragmentShaderObjectPV, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(fragmentShaderObjectPV);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    // shader program 
    shaderProgramObjectPV = gl.createProgram();
    gl.attachShader(shaderProgramObjectPV, vertexShaderObjectPV);
    gl.attachShader(shaderProgramObjectPV, fragmentShaderObjectPV);

    // pre-linking binding of shader program object with vertex shader attributes
    gl.bindAttribLocation(shaderProgramObjectPV, WebGLMacros.RMC_ATTRIBUTE_POSITION, "vPosition");
    gl.bindAttribLocation(shaderProgramObjectPV, WebGLMacros.RMC_ATTRIBUTE_NORMAL, "vNormal");

    // linking
    gl.linkProgram(shaderProgramObjectPV);
    if (!gl.getProgramParameter(shaderProgramObjectPV, gl.LINK_STATUS)) {
        var error = gl.getProgramInfoLog(shaderProgramObjectPV);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    // get uniform locations
    mUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_mMatrix");
    vUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_vMatrix");
    pUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_pMatrix");

    laUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_La");
    ldUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_Ld");
    lsUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_Ls");
    lightPositionUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_LightPos");

    kaUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_Ka");
    kdUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_Kd");
    ksUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_Ks");

    shininessUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_Shininess");
    enableLightUniformPV = gl.getUniformLocation(shaderProgramObjectPV, "u_bLight");

    ////////////////////////////////////////////////////////////////////////////////////////////////

    //// per fragment shader ///////////////////////////////////////////////////////////////////////

    var vertexShaderSourceCodePF =
        "#version 300 es \n" +
        "precision lowp int; \n" +

        "in vec4 vPosition; \n" +
        "in vec3 vNormal; \n" +

        "uniform mat4 u_mMatrix; \n" +
        "uniform mat4 u_vMatrix; \n" +
        "uniform mat4 u_pMatrix; \n" +

        "uniform vec4 u_LightPos; \n" +
        "uniform int u_bLight; \n" +

        "out vec3 tNorm; \n" +
        "out vec3 lightDir; \n" +
        "out vec3 viewerVector; \n" +

        "void main (void) \n" +
        "{ \n" +
        "	if (u_bLight == 1)" +
        "	{ \n" +
        "		vec4 eyeCoordinates = u_vMatrix * u_mMatrix * vPosition; \n" +
        "		tNorm = mat3(u_vMatrix * u_mMatrix) * vNormal; \n" +
        "		viewerVector = normalize(vec3(-eyeCoordinates.xyz)); \n" +
        "		lightDir = vec3(u_LightPos - eyeCoordinates); \n" +
        "	} \n" +
        "	gl_Position = u_pMatrix * u_vMatrix * u_mMatrix * vPosition; \n" +
        "} \n";

    vertexShaderObjectPF = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vertexShaderObjectPF, vertexShaderSourceCodePF);
    gl.compileShader(vertexShaderObjectPF);

    if (gl.getShaderParameter(vertexShaderObjectPF, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(vertexShaderObjectPF);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    var fragmentShaderSourceCodePF =
        "#version 300 es \n" +
        "precision highp float; \n" +
        "precision lowp int; \n" +

        "in vec3 tNorm; \n" +
        "in vec3 lightDir; \n" +
        "in vec3 viewerVector; \n" +

        "uniform vec3 u_La; \n" +
        "uniform vec3 u_Ld; \n" +
        "uniform vec3 u_Ls; \n" +
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

        "		vec3 normLightDir = normalize(lightDir); \n" +
        "		vec3 reflectionVector = reflect(-normLightDir, normTNorm); \n" +
        "		float tNormDotLightDir = max(dot(normTNorm, normLightDir), 0.0); \n" +

        "		vec3 ambient = u_La * u_Ka; \n" +
        "		vec3 diffuse = u_Ld * u_Kd * tNormDotLightDir; \n" +
        "		vec3 specular = u_Ls * u_Ks * pow(max(dot(reflectionVector, normViewerVector), 0.0), u_Shininess); \n" +
        "		phongLight += ambient + diffuse + specular; \n" +

        "		FragColor = vec4(phongLight, 1.0); \n" +
        "	} \n" +
        "	else \n" +
        "	{ \n" +
        "		FragColor = vec4(1.0); \n" +
        "	} \n" +
        "} \n";

    fragmentShaderObjectPF = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragmentShaderObjectPF, fragmentShaderSourceCodePF);
    gl.compileShader(fragmentShaderObjectPF);

    if (gl.getShaderParameter(fragmentShaderObjectPF, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(fragmentShaderObjectPF);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    // shader program 
    shaderProgramObjectPF = gl.createProgram();
    gl.attachShader(shaderProgramObjectPF, vertexShaderObjectPF);
    gl.attachShader(shaderProgramObjectPF, fragmentShaderObjectPF);

    // pre-linking binding of shader program object with vertex shader attributes
    gl.bindAttribLocation(shaderProgramObjectPF, WebGLMacros.RMC_ATTRIBUTE_POSITION, "vPosition");
    gl.bindAttribLocation(shaderProgramObjectPF, WebGLMacros.RMC_ATTRIBUTE_NORMAL, "vNormal");

    // linking
    gl.linkProgram(shaderProgramObjectPF);
    if (!gl.getProgramParameter(shaderProgramObjectPF, gl.LINK_STATUS)) {
        var error = gl.getProgramInfoLog(shaderProgramObjectPF);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }

    // get uniform locations
    mUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_mMatrix");
    vUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_vMatrix");
    pUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_pMatrix");

    laUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_La");
    ldUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_Ld");
    lsUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_Ls");
    lightPositionUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_LightPos");

    kaUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_Ka");
    kdUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_Kd");
    ksUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_Ks");

    shininessUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_Shininess");
    enableLightUniformPF = gl.getUniformLocation(shaderProgramObjectPF, "u_bLight");

    ////////////////////////////////////////////////////////////////////////////////////////////////

    // light configuration
    lights = new Array(1);

    // WHITE light
    lights[0] = {
        lightAmbient: new Float32Array([0.0, 0.0, 0.0]),
        lightDiffuse: new Float32Array([1.0, 1.0, 1.0]),
        lightSpecular: new Float32Array([1.0, 1.0, 1.0]),
        lightPosition: new Float32Array([0.0, 0.0, 0.0, 1.0]),
        angle: 0.0
    }

    sphere = new Mesh();
    makeSphere(sphere, 1.0, 30, 30);

    // set clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    // set depth testing
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    initMaterials();

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


    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -3.0]);

    if (bFragment) {
        gl.useProgram(shaderProgramObjectPF);
        gl.uniformMatrix4fv(mUniformPF, false, modelMatrix);
        gl.uniformMatrix4fv(vUniformPF, false, viewMatrix);
        gl.uniformMatrix4fv(pUniformPF, false, perspectiveProjectionMatrix);

        if (bLight) {
            gl.uniform3fv(laUniformPF, lights[0].lightAmbient);
            gl.uniform3fv(ldUniformPF, lights[0].lightDiffuse);
            gl.uniform3fv(lsUniformPF, lights[0].lightSpecular);

            switch (axis) {
                case 0:
                    gl.uniform4fv(lightPositionUniformPF,
                        [0.0, 100.0 * Math.cos(lights[0].angle), 100.0 * Math.sin(lights[0].angle), 1.0]);
                    break;

                case 1:
                    gl.uniform4fv(lightPositionUniformPF,
                        [100.0 * Math.cos(lights[0].angle), 0.0, 100.0 * Math.sin(lights[0].angle), 1.0]);
                    break;

                case 2:
                    gl.uniform4fv(lightPositionUniformPF,
                        [100.0 * Math.cos(lights[0].angle), 100.0 * Math.sin(lights[0].angle), 0.0, 1.0]);
                    break;
            }

            gl.uniform1i(enableLightUniformPF, 1);
        }
        else {
            gl.uniform1i(enableLightUniformPF, 0);
        }
    }
    else {
        gl.useProgram(shaderProgramObjectPV);
        gl.uniformMatrix4fv(mUniformPV, false, modelMatrix);
        gl.uniformMatrix4fv(vUniformPV, false, viewMatrix);
        gl.uniformMatrix4fv(pUniformPV, false, perspectiveProjectionMatrix);

        if (bLight) {
            gl.uniform3fv(laUniformPV, lights[0].lightAmbient);
            gl.uniform3fv(ldUniformPV, lights[0].lightDiffuse);
            gl.uniform3fv(lsUniformPV, lights[0].lightSpecular);

            switch (axis) {
                case 0:
                    gl.uniform4fv(lightPositionUniformPV,
                        [0.0, 100.0 * Math.cos(lights[0].angle), 100.0 * Math.sin(lights[0].angle), 1.0]);
                    break;

                case 1:
                    gl.uniform4fv(lightPositionUniformPV,
                        [100.0 * Math.cos(lights[0].angle), 0.0, 100.0 * Math.sin(lights[0].angle), 1.0]);
                    break;

                case 2:
                    gl.uniform4fv(lightPositionUniformPV,
                        [100.0 * Math.cos(lights[0].angle), 100.0 * Math.sin(lights[0].angle), 0.0, 1.0]);
                    break;
            }

            gl.uniform1i(enableLightUniformPV, 1);
        }
        else {
            gl.uniform1i(enableLightUniformPV, 0);
        }
    }

    for (var i = 0; i < 6; i++) {
        for (var j = 0; j < 4; j++) {

            gl.viewport(
                i * canvas.width / 6.0,
                j * canvas.height / 4.0,
                canvas.width / 6.0,
                canvas.height / 4.0);

            mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width / 6.0) / parseFloat(canvas.height / 4.0), 0.1, 100);

            if (bFragment) {
                gl.uniformMatrix4fv(pUniformPF, false, perspectiveProjectionMatrix);

                // setting material properties
                gl.uniform3fv(kaUniformPF, materials[(4 * i) + j].MaterialAmbient);
                gl.uniform3fv(kdUniformPF, materials[(4 * i) + j].MaterialDiffuse);
                gl.uniform3fv(ksUniformPF, materials[(4 * i) + j].MaterialSpecular);
                gl.uniform1f(shininessUniformPF, materials[(4 * i) + j].MaterialShininess);
            } else {
                gl.uniformMatrix4fv(pUniformPV, false, perspectiveProjectionMatrix);

                // setting material properties
                gl.uniform3fv(kaUniformPV, materials[(4 * i) + j].MaterialAmbient);
                gl.uniform3fv(kdUniformPV, materials[(4 * i) + j].MaterialDiffuse);
                gl.uniform3fv(ksUniformPV, materials[(4 * i) + j].MaterialSpecular);
                gl.uniform1f(shininessUniformPV, materials[(4 * i) + j].MaterialShininess);
            }

            sphere.draw();
        }
    }

    gl.useProgram(null);

    // animation loop
    requestAnimationFrame(draw, canvas);
    update();
}

function update() {
    // code
    if (lights[0].angle < 360.0) {
        lights[0].angle += 0.01;
    } else {
        lights[0].angle = 0.0;
    }
}

function uninit() {
    // code
    if (vaoCube) {
        gl.deleteVertexArray(vaoCube);
        vaoCube = null;
    }

    if (vboCubeNor) {
        gl.deleteBuffer(vboCubeNor);
        vboCubeNor = null;
    }

    if (vboCubePos) {
        gl.deleteBuffer(vboCubePos);
        vboCubePos = null;
    }

    if (shaderProgramObjectPV) {
        if (fragmentShaderObjectPV) {
            gl.detachShader(shaderProgramObjectPV, fragmentShaderObjectPV);
            gl.deleteShader(fragmentShaderObjectPV);
            fragmentShaderObjectPV = null;
        }

        if (vertexShaderObjectPV) {
            gl.detachShader(shaderProgramObjectPV, vertexShaderObjectPV);
            gl.deleteShader(vertexShaderObjectPV);
            vertexShaderObjectPV = null;
        }

        gl.deleteProgram(shaderProgramObjectPV);
        shaderProgramObjectPV = null;
    }

    if (shaderProgramObjectPF) {
        if (fragmentShaderObjectPF) {
            gl.detachShader(shaderProgramObjectPF, fragmentShaderObjectPF);
            gl.deleteShader(fragmentShaderObjectPF);
            fragmentShaderObjectPF = null;
        }

        if (vertexShaderObjectPF) {
            gl.detachShader(shaderProgramObjectPF, vertexShaderObjectPF);
            gl.deleteShader(vertexShaderObjectPF);
            vertexShaderObjectPF = null;
        }

        gl.deleteProgram(shaderProgramObjectPF);
        shaderProgramObjectPF = null;
    }
}

function keyDown(event) {
    // code
    switch (event.keyCode) {
        case 27: // Esc
            uninit();
            window.close();
            break;

        case 76: // for 'L' or 'l'
            bLight = !bLight;
            break;

        case 69: // for 'E' or 'e'
            toggleFullscreen();
            break;

        case 70: // for 'F' and 'f'
            bFragment = true;
            break;

        case 86: // for 'v' or 'V'
            bFragment = false;
            break;

        case 88: // 'x'
            axis = 0;
            break;

        case 89: // 'y'
            axis = 1;
            break;

        case 90: // 'z'
            axis = 2;
            break;
    }
}

function mouseDown() {
    // code
}

function degToRad(deg) {
    return deg * Math.PI / 180.0;
}
