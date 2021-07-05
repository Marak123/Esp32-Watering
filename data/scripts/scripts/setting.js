if (Cookies.get('theme')){
    $('body').attr('data-mode', Cookies.get('theme'));
    $('.setting-window .content .page-theme .wrappery .switch-theme').attr('mode', Cookies.get('theme'))
}

$(function () {

    $('.setting-window .setting-window-top-menu li').click(function () {
        $('.setting-window .setting-window-top-menu li').removeClass('select')
        $(this).addClass('select')
        $('.setting-window .content div').removeClass('d-flex');
        $('.' + $(this).attr('id')).addClass('d-flex');
    })

    $('.setting-window .content .user-name .button-sbm').click(function () {
        console.log("work 1")
    })

    $('.setting-window .content .password-user .button-sbm').click(function () {
        console.log("work 2")
    })

    $('.setting-window .content .page-theme .wrappery .switch-theme').click(function () {

        if($(this).attr('mode') == "dark"){
            Cookies.set('theme', 'light');
            $('body').attr('data-mode', 'light');
            $(this).attr('mode', 'light')
        }else if($(this).attr('mode') == "light"){
            Cookies.set('theme', 'dark');
            $('body').attr('data-mode', 'dark');
            $(this).attr('mode', 'dark')
        }
    })

})