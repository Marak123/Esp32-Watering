$(function () {
    var htmlListAction = "";
    for (var el of listActionElm) {
        htmlListAction += ` <div class="pin_${allPins[el[3]][0]}_ac listElm">
                                            <div class="dt"><p>${(el[0][0] >= 0 && el[0][0] <= 9) ? "0" + el[0][0] : el[0][0]}.${(el[0][1] >= 0 && el[0][1] <= 9) ? "0" + el[0][1] : el[0][1]}.${el[0][2]}</p></div>
                                            <div class="hr"><p>${(el[1][0] >= 0 && el[1][0] <= 9) ? "0" + el[1][0] : el[1][0]}:${(el[1][1] >= 0 && el[1][1] <= 9) ? "0" + el[1][1] : el[1][1]}</p></div>
                                            <div class="ac"><p>${el[2] ? "Włączenie" : "Wyłączenie"}</p></div>
                                            <div class="nm"><p>${allPins[el[3]][1]}(${allPins[el[3]][0]})</p></div>
                                        </div>`;
    }
    // $('.timetable .main .list .elemList').html(htmlListAction);

    $('.timetable .cld-menu > div').click(function () {
        $('.timetable .cld-menu > div').removeClass('select');
        $(this).addClass('select');

        $('.timetable .main > div').removeClass('select');
        $('.timetable .main .' + $(this).attr('em')).addClass('select');
    })

    $('.timetable .elemList .listElm').hover(function(){
        $(this).append('<div class="list-elem-tooltip" tooltip><i class="fas fa-trash"></i></div>');
    }, function(){
        $('.timetable .elemList .listElm .list-elem-tooltip').remove();
    })
})