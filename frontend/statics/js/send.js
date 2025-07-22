let ws = new WebSocket("ws://localhost:9002");

ws.onopen = () => appendLog("system: connection success!");
ws.onclose = () => appendLog("system : connection closed!");
ws.onerror = () => appendLog("system : connection error!");

ws.onmessage = (event) => {
    appendLog("Server: " + event.data);
};

function sendMessage(){
    const input =  document.getElementById("msgInput");
    if(input.value.trim() === "") return;
    ws.send(input.value);
    appendLog("me: " + input.value);
    input.value = "";
}

function appendLog(message){
    const log = document.getElementById("chatLog");
    log.innerHTML += message + "<br>";
    log.scrollTop = log.scrollHeight;
}