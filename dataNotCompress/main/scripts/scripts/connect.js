if (!Cookies.get("token")) {
    if (Cookies.get("token").length == 0) {
        location.reload()
    }
}
function getCookieToken() {
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

function onMessage(event) {
    let data = JSON.parse(event.data);

    if (data.what == "power-now") {
        powerNow_change(data.action, data.pinID);
    } else if (data.what == "delay" || data.what == "planData" || data.what == "planWeek" || data.what == "timetableRemove") {
        listActionElm = data.arrayListActivity;
        genListElem()
    }
}

function onLoad(event) {
    initWebSocket();
    initButton();
}

function initButton() {
    // websocket.send()
}

function fanClick() { }
function heatClick() { }
function getMobileToken() { return "cn38h4r8m930m8r7dx934hn78m53dh9r" }
function generateNewToken() { }

function powerNowSend(action, pinId) {
    let objJSON = {
        token: getCookieToken(),
        what: "power-now",
        action: action,
        pinID: pinId
    }
    let converObj = JSON.stringify(objJSON)
    $.ajax({
        url: "power",
        method: "POST",
        dataType: "JSON",
        data: converObj,
    }).fail(function (err) {
        messageSee('Nie udało się wysłać żądania', 'failed')
    }).done(function (data) {
        powerNow_change(data.action, data.pinID);
        messageSee('Pomyślnie wykonano żądanie', 'success');
    });
}

function delaySend(action, pinId, delayDate) {
    let objJSON = {
        token: getCookieToken(),
        what: "delay",
        action: action,
        pinID: pinId,
        delay: delayDate
    }
    let converObj = JSON.stringify(objJSON)
    $.ajax({
        url: "delay",
        method: "POST",
        dataType: "JSON",
        data: converObj,
    }).fail(function (err) {
        errorSendButton(_thisSubBtn);
        clearTimeout(buttonSubmitTO);
    }).done(function (data) {
        listActionElm = data.arrayListActivity;
        genListElem()
        successSendButton(this)
        clearTimeout(buttonSubmitTO);
    });
}

function planDataSend(action, pinId, data, hours) {
    let objJSON = {
        token: getCookieToken(),
        what: "planData",
        action: action,
        pinID: pinId,
        data: data,
        hours: hours
    }
    let converObj = JSON.stringify(objJSON)
    $.ajax({
        url: "planData",
        method: "POST",
        dataType: "JSON",
        data: converObj,
    }).fail(function (err) {
        errorSendButton(_thisSubBtn);
        clearTimeout(buttonSubmitTO);
    }).done(function (data) {
        listActionElm = data.arrayListActivity;
        genListElem()
        successSendButton(this)
        clearTimeout(buttonSubmitTO);
    });
}

function planWeekSend(action, pinId, dayWeek, hours) {
    let objJSON = {
        token: getCookieToken(),
        what: "planWeek",
        action: action,
        pinID: pinId,
        dayWeek: dayWeek,
        hours: hours
    }
    let converObj = JSON.stringify(objJSON)
    $.ajax({
        url: "planWeek",
        method: "POST",
        dataType: "JSON",
        data: converObj,
    }).fail(function (err) {
        messageSee('Nie udało się wysłać żądania', 'failed')
    }).done(function (data) {
        listActionElm = data.arrayListActivity;
        genListElem()
        successSendButton(this)
        clearTimeout(buttonSubmitTO);
    });
}

function setTemperature(pinId, indexEle, temperature) {
    let objJSON = {
        token: getCookieToken(),
        what: "setTemperature",
        pinId: pinId,
        indexEle: indexEle,
        temperature: temperature
    }
    let converObj = JSON.stringify(objJSON)
    $.ajax({
        url: "setTemperature",
        method: "POST",
        dataType: "JSON",
        data: converObj,
    }).fail(function (err) {
        messageSee('Nie udało się wysłać żądania', 'failed')
    }).done(function (data) {
        temperatureEle = data.arrayTemperature;
        messageSee('Pomyślnie wykonano żądanie', 'success');
    });
}

function timetableSend(indexElm, action, pinId, data, hours) {
    console.log("IndexElement: " + indexElm);
    console.log("Akcja: " + action);
    console.log("PinID: " + pinId);
    console.log("Data: " + data);
    let objJSON = {
        token: getCookieToken(),
        what: "timetable",
        removeEle: {
            indexElm: indexElm,
            action: action,
            pinId: pinId,
            data: data,
            hours: hours
        }
    }
    let converObj = JSON.stringify(objJSON)
    $.ajax({
        url: "timetableRemove",
        method: "POST",
        dataType: "JSON",
        data: converObj,
    }).fail(function (err) {
        messageSee('Nie udało się wysłać żądania', 'failed')
    }).done(function (data) {
        listActionElm = data.arrayListActivity;
        messageSee('Pomyślnie wykonano żądanie', 'success');
    });
}


//Action to run send success
function powerNow_change(act, pinId) {
    if (act == "on" || act == true) {
        $('#pinId' + pinId).removeClass('off').addClass('on')
        $('#pinId' + pinId + ' .text-info .info-power').html("WŁĄCZONE")
    } else if (act == "off" || act == false) {
        $('#pinId' + pinId).removeClass('on').addClass('off')
        $('#pinId' + pinId + ' .text-info .info-power').html("WYŁĄCZONE")
    }
}