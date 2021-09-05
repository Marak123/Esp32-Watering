var themeMode = 'dark';
if (Cookies.get('theme')) {
    $('body').attr('data-mode', Cookies.get('theme'));
    themeMode = Cookies.get('theme');
}

$(function () {

    $('body').delegate('.setting-window .setting-window-top-menu li', 'click', function () {
        $('.setting-window .setting-window-top-menu li').removeClass('select')
        $(this).addClass('select')
        $('.setting-window .content div').removeClass('d-flex');
        $('.' + $(this).attr('id')).addClass('d-flex');
    })

    $('.topBar .settings-btn').click(function () {
        $('body').prepend(`<div class="setting-window">
            <div div class= "close-window" > <i class="fas fa-times"></i></div >
        <ul class="setting-window-top-menu">
            <li class="select" id="user-sett">Użytkownik</li>
            <li id="access-sett">Zabezpieczenia</li>
        </ul>
        <div class="content">
            <div></div>
            <div class="user-sett d-flex">
                <div class="free"></div>
                <div class="user-name">
                    <h2>Nazwa użytkownika</h2>
                    <input type="text" name="newUsername" placeholder="Nowa nazwa użytkownika">
                    <input type="password" name="password" placeholder="Hasło">
                    <div class="submit-button col-4">
                        <button class="button-sbm" id="delaySubmit">Zatwierdź</button>
                    </div>
                </div>
                <div class="free"></div>
                <div class="password-user">
                    <h2>Hasło</h2>
                    <input type="password" name="oldPassword" placeholder="Podaj aktualne hasło">
                    <input type="password" name="newPassword" placeholder="Podaj nowe hasło">
                    <input type="password" name="repeat-newPassword" placeholder="Powtórz nowe hasło">
                    <div class="submit-button col-4">
                        <button class="button-sbm" id="delaySubmit">Zatwierdź</button>
                    </div>
                </div>
                <div class="page-theme">
                    <div class="mode-page col-4 h-50">
                        <div class="wrappery">
                            <input type="checkbox" name="checkbox" class="switch-theme" mode="${themeMode}">
                        </div>
                    </div>
                </div>
            </div>
            <div class="access-sett">
                <div class="free"></div>
                <div class="exp-date">
                    <p>Ważność sesji</p>
                    <p>Ilość dni:</p>
                    <input type="number" value="30" placeholder="Ilość dni">
                    <p>Ilość minut:</p>
                    <input type="number" id="" placeholder="Godzin:Minut lub Minut">
                    <div class="submit-button col-4">
                        <button class="button-sbm" id="delaySubmit">Zatwierdź</button>
                    </div>
                </div>
                <div class="free"></div>
                <div class="mobile-token ">
                    <p>Token dostępu z aplikacji mobilnej</p>
                    <div id="mobileToken">
                        <p contenteditable id="token">${getMobileToken()}</p>
                        <i class="fas fa-copy copyToClipboard"></i>
                        <i class="fas fa-redo generateNew"></i>
                    </div>
                    <div class="day-number">
                        <p>Ilość dni: </p>
                        <input type="number" placeholder="Ilosc dni ważności" value="365">
                    </div>
                    <div class="submit-button col-4">
                        <button class="button-sbm" id="delaySubmit">Zatwierdź</button>
                    </div>
                </div>
            </div>
        </div>
    </div>
    <div class="blure">&nbsp;</div>`)
    })

    $('body').delegate('.setting-window .close-window', 'click', function () {
        $('.setting-window').remove()
        $('.blure').remove()
    })

    $('body').delegate('.setting-window .content .user-name .button-sbm', 'click', function () {
        console.log("work 1")
    })

    $('body').delegate('.setting-window .content .password-user .button-sbm', 'click', function () {
        console.log("work 2")
    })

    $('body').delegate('.setting-window .content .page-theme .wrappery .switch-theme', 'click', function () {

        if ($(this).attr('mode') == "dark") {
            Cookies.set('theme', 'light');
            $('body').attr('data-mode', 'light');
            $(this).attr('mode', 'light')
            themeMode = 'light';
        } else if ($(this).attr('mode') == "light") {
            Cookies.set('theme', 'dark');
            $('body').attr('data-mode', 'dark');
            $(this).attr('mode', 'dark')
            themeMode = 'dark';
        }
    })

    $('body').delegate('.setting-window .mobile-token .copyToClipboard', 'click', function (event) {
        document.querySelector('#token').focus()
        document.execCommand('selectAll', false, null)

        try {
            var successful = document.execCommand('copy');
            if (successful) messageSee('Pomyślnie skopiowano', 'success');
            else messageSee('Kopiowanie nie powiodło się. Spróbuj jeszcze raz.', 'failed');
        } catch (err) {
            messageSee('Coś poszło nie tak z kopiowaniem. Spróbuj jeszcze raz.', 'failed');
        }
    })
    $('body').delegate('.setting-window .mobile-token p', 'keydown', function (event) {
        event.preventDefault()
    })

    $('body').delegate('.setting-window #mobileToken .generateNew', 'click', function () {
        let newToken = generateNewToken() || false;
        if (newToken) {
            messageSee('Pomyślnie wygenerowano', 'success');
            $('.setting-window #mobileToken #token').text(newToken);
        } else messageSee('Wygenerowanie nie powiodło się. Spróbuj jeszcze raz.', 'failed');
    })

})