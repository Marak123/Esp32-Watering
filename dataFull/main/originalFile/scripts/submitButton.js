function animateButton(e) {
    e.preventDefault;
    $(this).addClass("animate-spin");

    setTimeout(function(){
        errorSendButton("delaySubmit");
    }, 4000)
};

$(".button-sbm").click(animateButton);

function successSendButton(classNm){
    $("." + classNm).removeClass("animate-spin")
    $("." + classNm).addClass("animate-withoutSpin")
    $("." + classNm).addClass("success")
    setTimeout(function(){
        $("." + classNm).removeClass("animate-withoutSpin")
        $("." + classNm).removeClass("success")
    }, 2000)
}

function errorSendButton(classNm){
    $("." + classNm).removeClass("animate-spin")
    $("." + classNm).addClass("animate-withoutSpin")
    $("." + classNm).addClass("error")
    setTimeout(function () {
        $("." + classNm).removeClass("animate-withoutSpin")
        $("." + classNm).removeClass("error")
    }, 2000)
}