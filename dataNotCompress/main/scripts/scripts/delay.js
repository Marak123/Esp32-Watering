function generatePins() {
    let delayPins = "";
    let pinName = "";
    for (let ele of allPins) {
        if (ele[2] == "power") {
            pinName = "";
            for (i = 0; i < ele[1].length; i++) {
                pinName += `<p>${ele[1][i]}</p>`;
            }
            delayPins += `<div class="pin_${ele[0]}">
                                <div class="selector">
                                    <label class="checkbox bounce">
                                        <input type="checkbox" name="${ele[2]}" pin="${ele[0]}">
                                        <svg viewBox="0 0 21 21">
                                            <polyline points="5 10.75 8.5 14.25 16 6"></polyline>
                                        </svg>
                                    </label>
                                </div>
                                <div class="pin-name">${pinName}</div>
                            </div>`
        }
    }
    return delayPins;
}

$(".pins-container").html(generatePins());

function getValueDelay() {
    let arrayPins = [];
    let actionTurn = false;
    let timeInp;

    // Zapisywanie wybranych pinow do tablicy "arrayPins"
    $('.delay .pins-container > div div.selector .checkbox input[name="power"]').each(function () {
        if ($(this).is(':checked')) arrayPins.push(parseInt($(this).attr('pin')));
    })

    // Sprawdzanie czy wybrano przynajmniej jeden pin
    if (arrayPins.length == 0) {
        messageSee('Nie wybrano żadnego pinu', 'info');
        return 'failedRead';
    }

    //Czytanie przycisku włącz wyłącz
    if ($('.delay .switchPower input.checkbox').is(':checked')) actionTurn = true;

    // Pobieranie opóźnienia z inputu timepicker
    timeInp = $('.delay .timePicker input.form-control').val()

    // Sprawdzanie czy jest jaka kolwiek wartość w inpucie
    if (timeInp.length == 0) {
        messageSee('Nie podano opóźnienia', 'info');
        return 'failedRead';
    }

    return {
        action: actionTurn,
        pins: arrayPins,
        delay: timeInp
    }
}