function animateButton(e) {
    e.preventDefault;
    $(this).addClass("animate-spin");
    let _this = this
    const buttonSubmitTO = setTimeout(function () {
        errorSendButton(_this);
    }, 4000)

    let op = submitDate(buttonSubmitTO, $(this).attr('id'));
    if (op) successSendButton(this)
    else errorSendButton(this)
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