var entry_point;
var canvas;
var gl;

function fate_log_info() {
    var gl_info_elem = document.getElementById("gl_info");
    gl_info_elem.innerHTML = 
          "WebGL version  : " + gl.getParameter(gl.VERSION) + "<br>"
        + "WebGL renderer : " + gl.getParameter(gl.RENDERER) + "<br>"
        + "WebGL vendor   : " + gl.getParameter(gl.VENDOR) + "<br>"
        + "Extensions : ";
    var exts = gl.getSupportedExtensions();
    for(var i=0 ; i<exts.length ; ++i)
        gl_info.innerHTML += exts[i] + "<br>";
}
function fate_lose_restore() {
    canvas.setRestoreTimeout(-1);
    window.addEventListener("mousedown", function() {
        canvas.loseContext();
    });
    window.addEventListener("mousedown", function() {
        canvas.restoreContext();
    });
}

function fate_onkeydown(e) { console.log(e); }
function fate_onkeyup(e) { console.log(e); }
function fate_onwheel(e) { console.log(e); }
function fate_onresize(e) { console.log(e); }
function fate_onmouseup(e) { console.log(e); }
function fate_onmousemove(e) { console.log(e); }
function fate_onmousedown(e) { console.log(e); }
function fate_onmouseout(e) { console.log(e); }
function fate_onmouseenter(e) { console.log(e); }

function fate_resize() {
    if(gl.canvas.width  != gl.canvas.clientWidth 
    || gl.canvas.height != gl.canvas.clientHeight) 
    {
        gl.canvas.width  = gl.canvas.clientWidth;
        gl.canvas.height = gl.canvas.clientHeight;
        gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
    }
}
var timeSpent = 0;
var old_abs_time_ms = 0;
function fate_render(abs_time_ms) {
    //console.log(abs_time_ms);
    var delta_ms = abs_time_ms - old_abs_time_ms;
    old_abs_time_ms = abs_time_ms;
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    timeSpent += delta_ms*0.001;
    var factor = (Math.sin(timeSpent) + 1) * 0.5;
    gl.clearColor(factor * 0.7 + 0.3, 0.0, 0.0, 1.0);
    window.requestAnimationFrame(fate_render);
}
function fate_init() {
    entry_point = document.getElementById("FATE_entry_point");
    entry_point.removeChild(document.getElementById("FATE_no_js"));
    canvas = document.createElement("canvas");
    canvas.id = "FATE_canvas";
    canvas.width = 512;
    canvas.height = 512;
    canvas.style.backgroundColor = "blue";
    entry_point.appendChild(canvas);
    
    gl = WebGLUtils.setupWebGL(canvas);
    if(!gl) 
        return; //A fallback text is displayed by webgl-utils.
    canvas.addEventListener("webglcontextlost", function(event) {
        event.preventDefault();
        alert("The WebGL context has been lost; "
            + "the game has to restart.");
    }, false);
    canvas.addEventListener("webglcontextrestored", fate_init, false);

    document.onkeydown = fate_onkeydown;
    document.onkeyup = fate_onkeydown;
    document.onwheel = fate_onwheel;
    window.onresize = fate_onresize;
    document.onmouseup = fate_onmouseup;
    document.onmousemove = fate_onmousemove;
    document.onmousedown = fate_onmousedown;
    canvas.onmouseout = fate_onmouseout;
    canvas.onmouseenter = fate_onmouseenter;

    gl.viewport(0, 0, canvas.clientWidth, canvas.clientHeight);
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    window.requestAnimationFrame(fate_render);
}

if(document.onload)
    document.onload = function() { document.onload(); fate_init(); }
else 
    document.onload = fate_init();

