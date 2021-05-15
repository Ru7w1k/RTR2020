// global variables
var canvas = null;
var context = null;

// onload function
function main() {
    /* document is inbuilt variable */
    // get canvas element from DOM    
    canvas = document.getElementById("RMC");
    if (!canvas)
        console.log("obtaining canvas failed..");
    else
        console.log("obtaining canvas succeeded..");

    // print canvas dimensions on console
    console.log("canvas width: " + canvas.width + " and canvas height: " + canvas.height);

    // get drawing context from the canvas
    context = canvas.getContext("2d");
    if (!context)
        console.log("obtaining context failed..");
    else
        console.log("obtaining context succeeded..");

    // paint the background by black color
    context.fillStyle = "black";
    context.fillRect(0, 0, canvas.width, canvas.height);

    drawText("Hello World!");

    /* window is inbuilt variable 
       it is one of the DOM element */
    // register event handlers
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
}

function drawText(text) {
    // center the text
    context.textAlign = "center";    // horizontal center
    context.textBaseline = "middle"; // vertical center

    // set font
    context.font = "48px sans-serif";

    // font color
    context.fillStyle = "white";

    // display string
    context.fillText(text, canvas.width / 2, canvas.height / 2);
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
    }
    // else canvas is in fullscreen state
    // call browser specific exit fullscreen function
    else {
        if (document.exitFullscreen) document.exitFullscreen();
        else if (docuement.webkitExitFullscreen) docuement.webkitExitFullscreen();
        else if (docuement.mozCancelFullScreen) docuement.mozCancelFullScreen();
        else if (docuement.msExitFullscreen) docuement.msExitFullscreen();
    }
}

function keyDown(event) {
    // code
    switch (event.keyCode) {
        case 70: // f key
            toggleFullscreen();
            // ~ repaint 
            drawText("Hello World!");
            break;
    }
}

function mouseDown() {
    // code
}
