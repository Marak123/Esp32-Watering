var text_desc = [];
// const info_desc = {
//     "url_pattern_start": "https://raw.githubusercontent.com/Marak123/test/main/",
//     "url_pattern_end": "/Readme.md",
//     "version": {
//         "current": "0.0.5",
//         "last_new": "0.1.1",
//         "older": [
//             "0.1.0",
//             "0.0.5",
//             "0.0.4",
//             "0.0.3",
//             "0.0.2",
//             "0.0.1"
//         ]
//     }
// };

$(document).ready(function() {
    let select_option = `<option value="last_new" selected>najnowsza(${info_desc.version.last_new})</option>`;
    for(let ver of info_desc.version.older) select_option += `<option value="${ver}">${ver}</option>`;
    $("#version_select").html(select_option);
    delete select_option;

    $('#desc_text').html(urlDesc(info_desc.version.last_new).responseText);
    $('#current_version').html(info_desc.version.current);

    var oldVersionInstall = false;
    $("#update_button").on("click", function(){
        let select_version = $("#version_select").val();
        if(select_version == info_desc.version.current){
            if(select_version != info_desc.version.last_new) seeAlert("Wersja którą wybrałeś/aś jest aktualnie zainstalowana.");
            else seeAlert("Wersja którą wybrałeś/aś jest aktualnie zainstalowana. Jest to najnowsza wydana wersja oprogramowania.");
            return;
        }
        if(!compareVersion(select_version, info_desc.version.current) && !oldVersionInstall){
            seeAlert("Wybrana wersja jest starsza niż aktualnie zainstalowana wersja.<par style='color: rgb(230, 0, 0); font-weight: bold;'>Zainstalowanie starszej wersji może skutkować utratą danych.</par><br><br>Czy na pewno chcesz zainstalować starszą wersje oprogramonia?", true);
            return 0;
        }

        oldVersionInstall = false;
        $.ajax({
            url: "/update/make_update",
            type: "POST",
            data: {
                version: select_version,
                token: getCookie("ESPSESSIONUPDATEID"),
                what: "updateSoftware_adm"
            }
        })
    })

    $('#version_select').on('change', async function() {
        let sel_ver = $("#version_select").val() == "last_new" ? info_desc.version.last_new : $("#version_select").val(),
            find = text_desc.find(x => x.version == sel_ver),
            desc = find == undefined ? urlDesc(sel_ver).responseText : find.description;

        $('#ver_desc_nr').html(sel_ver);
        $('#desc_text').html(desc == "404: Not Found" ? "<p style=\"color: red;\">Nie znaleziono pliku z opisem, błędne dane do źródła.<br> Error 404: Not Found. <br><br><br>Aktualizacja może pomóc.<br> Jeżeli nie można wykonać aktualizacji postępuj zgodnie z instrukcją pod tym adresem url<br><br>|<br>\\/<br><br> <a href=\"https://github.com/Marak123/Esp32-Watering\">https://github.com/Marak123/Esp32-Watering</a></p>" : desc);
    });

    $('alert #close-alert').on('click', function() {
        $('alert#info').hide();
    });
    $('alert #yes-no-choice #yes-alert').on('click', function() {
        $('alert#info').hide();
        oldVersionInstall = true;
        $('#update_button').trigger('click');
    });
    $('alert #yes-no-choice #no-alert').on('click', function() {
        $('alert#info').hide();
        oldVersionInstall = false;
    });
});

function seeAlert(message, yesno){
    if(yesno) {
        $('alert #close-button').hide()
        $("alert #yes-no-choice").show()
    }else{
        $("alert #yes-no-choice").hide()
        $('alert #close-button').show()
    }
    $('#info-message').html(message);
    $('#info').show();
}

function compareVersion(bigger, smaller){
    let first = bigger.split("."),
        second = smaller.split(".");

    if(first[0] > second[0]) return true;
    else if(first[0] < second[0]) return false;

    if(first[1] > second[1]) return true;
    else if(first[1] < second[1]) return false;

    if(first[2] > second[2]) return true;
    else if(first[2] < second[2]) return false;

    return undefined;
}

function urlDesc(ver){
    return $.ajax({
        url: info_desc.url_pattern_start + ver + info_desc.url_pattern_end,
        type: "GET",
        async: false,
        success: function(data){
            text_desc.push({"version": ver, "description": data});
        },
        error: function(xhr, status, error){
            console.log(error, status, xhr);
        }
    });
}

function getCookie(cname) {
    let name = cname + "=";
    let ca = document.cookie.split(';');
    for (let i = 0; i < ca.length; i++) {
        let c = ca[i];
        while (c.charAt(0) == ' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
            return c.substring(name.length, c.length);
        }
    }
    return "";
}