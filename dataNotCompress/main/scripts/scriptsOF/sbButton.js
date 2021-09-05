var _thisSubBtn;
var buttonSubmitTO;
function animateButton(e) {
    e.preventDefault;

    $(this).addClass("animate-spin");

    _thisSubBtn = this
    let whichButton = $(this).attr('id');

    buttonSubmitTO = setTimeout(function () {
        errorSendButton(_thisSubBtn);
    }, 4000)

    if (whichButton == 'delaySubmit') {
        let delay = getValueDelay();
        if (delay != 'failedRead') {
            delaySend(delay.action, delay.pins, delay.delay);
        }
    }
    if (whichButton == 'planSubmit') {
        let plan = getValuePlan();
        if (plan != 'failedRead') {
            if (delay.pow == "week") planWeekSend(plan.action, plan.pins, plan.week, plan.hours);
            else if (delay.pow == "plan") planDataSend(plan.action, plan.pins, plan.data, plan.hours);
        }
    }
};

$(".submit-button").delegate(".button-sbm", 'click', animateButton);

function successSendButton(classNm) {
    messageSee('Pomyślnie wysłano żądanie', 'success')
    $(classNm).removeClass("animate-spin")
    $(classNm).addClass("animate-withoutSpin")
    $(classNm).addClass("success")
    setTimeout(function () {
        $(classNm).removeClass("animate-withoutSpin")
        $(classNm).removeClass("success")
    }, 2000)
}

function errorSendButton(classNm) {
    messageSee('Nie udało się wysłać żądania', 'failed')
    $(classNm).removeClass("animate-spin")
    $(classNm).addClass("animate-withoutSpin")
    $(classNm).addClass("error")
    setTimeout(function () {
        $(classNm).removeClass("animate-withoutSpin")
        $(classNm).removeClass("error")
    }, 2000)
}