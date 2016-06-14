function onopen(evt) {
    evt;
}
function onerror(evt) {
    evt;
}
function onclose(evt) {
    evt.code;
    evt.reason;
    evt.wasClean;
}
function onmessage(evt) {
    evt.data;
    evt.origin;
    evt.ports;
    evt.source;
}

var sock;
var secure;
try sock = WebSocket("ws"+(secure?"s":"")+"://www.example.com/socketserver");
catch(SECURITY_ERR) {}
sock.onerror = onerror;
sock.onmessage = onmessage;
sock.onopen = onopen;
sock.onclose = onclose;
switch(sock.readyState) {
case WebSocket.CONNECTING: "not yet opened" break;
case WebSocket.OPEN: "opened" break;
case WebSocket.CLOSING: break;
case WebSocket.CLOSED: "not opened" break;
}
sock.binaryType = "arraybuffer";
sock.send(arraybuf);
var bytes_not_yet_sent = sock.bufferedAmount;
while(sock.bufferedAmount > 0);
sock.close();
