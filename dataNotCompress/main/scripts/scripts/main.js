function messageSee(text, action) {
    if (!($('div.messageAlert').length)) {
        $('body').prepend(`<div class="messageAlert ${action}"><p>${text}</p></div>`);
        $('.messageAlert').css({ 'right': '-300%' }).animate({ "right": "10" }, "slow")
        setTimeout(function () {
            $('.messageAlert').css({ 'right': '10px' }).animate({ "right": "-300%" }, "slow")
            setTimeout(function () {
                $('.messageAlert').remove();
            }, 500)
        }, 5000)
    }
}