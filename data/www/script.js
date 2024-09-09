
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
// Init web socket when the page loads
window.addEventListener('load', onload);

function onload(event) {
    initWebSocket();
    initButtons();
}

function getReadings(){
    websocket.send("getReadings");
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

// When websocket is established, call the getReadings() function
function onOpen(event) {
    console.log('Connection opened');
    getReadings();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function initButtons() {
    eleButtonAudioOff = document.getElementById('button_audio_off');
    if(eleButtonAudioOff != null) eleButtonAudioOff.addEventListener('click', buttonOffPressed);

    eleButtonAudioWebradio = document.getElementById('button_audio_webradio');
    if(eleButtonAudioWebradio != null) eleButtonAudioWebradio.addEventListener('click', buttonWebradioPressed);    
    
    eleButtonAudioBluetooth = document.getElementById('button_audio_bluetooth');
    if(eleButtonAudioBluetooth != null) eleButtonAudioBluetooth.addEventListener('click', buttonBluetoothPressed);
}

function buttonOffPressed(){
    console.log('off pressed');
    websocket.send('buttonOffPressed');
}

function buttonWebradioPressed(){
    websocket.send('buttonWebradioPressed');
}

function buttoBluetoothPressed(){
    websocket.send('buttonBluetoothPressed');
}

// Function that receives the message from the ESP32 with the readings
function onMessage(event) {
    console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);

    for (var i = 0; i < keys.length; i++){
        var key = keys[i];
        var ele = document.getElementById(key);
        if(ele != null){
            ele.innerHTML = myObj[key];
        }
    }
}