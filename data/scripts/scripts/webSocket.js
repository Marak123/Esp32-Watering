function fanClick() { }
function heatClick() { }
function setTemerature() { }
function removeElementAction(indxElm) { }
function submitDate(cTO, elemID) { }

function getCookie() {
    return Cookies.get("token")
}

const gateway = `ws://${window.location.hostname}/ws`;
var websocket;
// window.addEventListener('load', onLoad);
function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen = console.log('Połączenie z ESP zostało otwarte.');
    websocket.onclose = () => { console.log('Połączenie z ESP zostało zamknięte ale nastąpi próba ponownego otwarcia połączenia.'); setTimeout(initWebSocket, 2000); };
    websocket.onmessage = onMessage; // <-- add this line
}

function onMessage(event) { }

function onLoad(event) {
    initWebSocket();
    initButton();
}

function initButton() {
    // websocket.send()
}



function powerNow(action, pinId) {
    let objJSON = {
        token: getCookie(),
        what: "power-now",
        action: action,
        pinID: pinId
    }
    let converObj = JSON.stringify(objJSON)
    $.ajax({
        url: "power",
        method: "GET",
        dataType: "JSON",
        data: { converObj },
    });
}

function delay(action, pinId, delay) {
    let objJSON = {
        token: getCookie(),
        what: "delay",
        action: action,
        pinID: pinId,
        delay: delay
    }
    let converObj = JSON.stringify(objJSON)
    $.ajax({
        url: "delay",
        method: "GET",
        dataType: "JSON",
        data: { converObj },
    });
}

function planData(action, pinId, data) {
    let objJSON = {
        token: getCookie(),
        what: "planData",
        action: action,
        pinID: pinId,
        data: data
    }
    let converObj = JSON.stringify(objJSON)
    $.ajax({
        url: "planData",
        method: "GET",
        dataType: "JSON",
        data: { converObj },
    });
}

function planWeek(action, pinId, dayWeek) {
    let objJSON = {
        token: getCookie(),
        what: "planWeek",
        action: action,
        pinID: pinId,
        dayWeek: dayWeek
    }
    let converObj = JSON.stringify(objJSON)
    $.ajax({
        url: "planWeek",
        method: "GET",
        dataType: "JSON",
        data: { converObj },
    });
}

function temperature(what, action, pinId) {
    let url = ""
    let objJSON = {}
    if (what == "air") {
        objJSON = {
            token: getCookie(),
            what: "air",
            action: action,
            pinID: pinId,
        }
        url = "temperatureAir"
    } else if (what == "heat") {
        objJSON = {
            token: getCookie(),
            what: "planWeek",
            action: action,
            pinID: pinId,
        }
        url = "temperatureHeat"
    }
    let converObj = JSON.stringify(objJSON)
    $.ajax({
        url: url,
        method: "GET",
        dataType: "JSON",
        data: { converObj },
    });
}

function powerNow_change(act, pinId) {
    if (act == "on" || act == true) {
        $('#pinId' + pinId).removeClass('off').addClass('on')
        $('#pinId' + pinId + ' .text-info .info-power').html("WŁĄCZONE")
    } else if (act == "off" || act == false) {
        $('#pinId' + pinId).removeClass('on').addClass('off')
        $('#pinId' + pinId + ' .text-info .info-power').html("WYŁĄCZONE")
    }
}

function addToList() {

}