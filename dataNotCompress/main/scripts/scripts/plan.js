$('.plan .dataPicker .weekLine').weekLine();


$(function () {
    const Data = new Date();

    let dataFormat = (Data.getMonth() + 1) + "/" + Data.getDate() + "/" + Data.getFullYear();
    if (Data.getMonth() < 9) dataFormat = "0" + dataFormat
    if (Data.getDate() < 9) dataFormat = dataFormat.substring(0, 3) + "0" + dataFormat.substring(3, dataFormat.length)
    $("#id_4 input").val(dataFormat);
})

$(".plan-menu .date").click(function () {
    $(this).children().addClass('selected');
    $(this).next().children().removeClass('selected');
    $(".dataPicker .date").removeClass('hidden');
    $(".dataPicker .weekLine").addClass('hidden');
})
$(".plan-menu .weekDay").click(function () {
    $(this).children().addClass('selected');
    $(this).prev().children().removeClass('selected');
    $(".dataPicker .date").addClass('hidden');
    $(".dataPicker .weekLine").removeClass('hidden');
})

function getValuePlan() {
    let arrayPins = [];
    let turnPow = false;
    let data;
    let week = [];
    let hours;

    let pow;

    // Zapisywanie wybranych pinow do tablicy "arrayPins"
    $('.plan .pins-container > div div.selector .checkbox input[name="power"]').each(function () {
        if ($(this).is(':checked')) arrayPins.push(parseInt($(this).attr('pin')));
    })

    // Sprawdzanie czy wybrano przynajmniej jeden pin
    if (arrayPins.length == 0) {
        messageSee('Nie wybrano żadnego pinu', 'info');
        return 'failedRead';
    }

    //Czytanie przycisku włącz wyłącz
    if ($('.delay .switchPower input.checkbox').is(':checked')) actionTurn = true;

    // Sprawdzanie co zostało uzupełnione "weekDay" lub "date"
    if ($('.plan .plan-menu .date a').hasClass('selected')) pow = "date";
    if ($('.plan .plan-menu .weekDay a').hasClass('selected')) pow = "week";

    if (pow == "date") {
        date = $('.plan .dataPicker .date input.form-control').val();
        if (date.length == 0) {
            messageSee('Nie podano daty', 'info');
            return 'failedRead';
        }
    } else if (pow == "week") week = $('.plan .dataPicker .weekLine').weekLine('getSelected', 'indexes').split(',');

    hours = $('.plan .timePicker input[name="end_time"]').val();
    if (hours.length == 0) {
        messageSee('Nie podano godziny', 'info');
        return 'failedRead';
    }

    return {
        pow: pow,
        pins: arrayPins,
        action: turnPow,
        data: data,
        week: week,
        hours: hours
    }
}