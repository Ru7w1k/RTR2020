// onload function
function main() {
    // get canvas element from DOM
    var canvas = document.getElementById("RMC");
    if (!canvas)
        console.log("obtaining canvas failed..");
    else
        console.log("obtaining canvas succeeded..");

    // print canvas dimensions on console
    console.log("canvas width: " + canvas.width + " and canvas height: " + canvas.height);

    // get drawing context from the canvas
    var context = canvas.getContext("2d");
    if (!context)
        console.log("obtaining context failed..");
    else
        console.log("obtaining context succeeded..");

    // paint the background by black color
    context.fillStyle = "black";
    context.fillRect(0, 0, canvas.width, canvas.height);

    // center the text
    context.textAlign = "center";    // horizontal center
    context.textBaseline = "middle"; // vertical center

    // set font
    context.font = "48px sans-serif";

    // declare the string to display
    var str = "Hello World!";

    // font color
    context.fillStyle = "white";

    // display string
    context.fillText(str, canvas.width/2, canvas.height/2);
}
