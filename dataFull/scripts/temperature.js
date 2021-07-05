$(function () {
    var htmlMenu = "";
    var htmlElem = "";

    let airOn = "";
    let heatOn = "";
    let firstElem = true;

    let tempeF = "";
    let tempeS = "";

    let whatName = 100 / temperatureEle.length < 15 ? true : false;
    for (var elm of temperatureEle) {
        htmlMenu += `<div class="pinsMenu ${!temperatureEle.indexOf(elm) ? 'select' : ''}" id="pin_${allPins[elm[0]][0]}" style="width: ${100 / temperatureEle.length}%; ${!temperatureEle.indexOf(elm) ? 'border-top-left-radius: 20px;' : ''}${temperatureEle.indexOf(elm) == temperatureEle.length - 1 ? 'border-top-right-radius: 20px;' : ''} ">${whatName ? allPins[elm[0]][0]:allPins[elm[0]][1]}</div>`;

        if (elm[3]) airOn = "pw-on";
        if (elm[4]) heatOn = "pw-on";

        let tempeF = elm[4].substring(0, elm[4].indexOf("."));
        let tempeS = elm[4].substring(elm[4].indexOf(".") + 1, elm[4].lenght);

        htmlElem += ` <div id="pin_${allPins[elm[0]][0]}" style="display: ${!temperatureEle.indexOf(elm) ? 'flex' : 'none'}">
                    <h3>${allPins[elm[0]][1]}</h3>
                    <div class="term-bar">
                        <div class="den">
                            <div class="dene">
                                <div class="denem">
                                    <div class="deneme">
                                        ${tempeF}<span>.${tempeS}</span><strong>&deg;</strong>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="icon-sts">
                        <div class="fan ${airOn}" id="pin_${allPins[elm[0]][0]}">
                            <svg version="1.1" id="Capa_1" xmlns="http://www.w3.org/2000/svg"
                                xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 164.97 164.97"
                                style="enable-background:new 0 0 164.97 164.97;" xml:space="preserve">
                                <path d="M153.679,103.607c-7.948-6.581-16.133-7.965-23.354-9.186c-4.795-0.811-8.715-1.483-12.168-3.469 c19.313-4.195,33.809-14.162,41.238-28.565c7.123-13.809,6.648-30.314-1.299-45.279c-4.818-9.086-15.28-15.717-26.651-16.893 c-10.926-1.129-21.093,2.913-27.905,11.106c-6.58,7.948-7.964,16.133-9.185,23.354c-0.805,4.761-1.474,8.658-3.426,12.093 c-4.191-19.323-14.16-33.826-28.567-41.259C48.553-1.614,32.048-1.141,17.08,6.806C7.993,11.626,1.362,22.088,0.186,33.461 C-0.943,44.386,3.1,54.552,11.292,61.363c7.949,6.581,16.134,7.965,23.355,9.186c4.796,0.811,8.716,1.483,12.17,3.47 c-19.314,4.194-33.81,14.161-41.24,28.564c-7.123,13.809-6.649,30.314,1.298,45.28c4.819,9.087,15.282,15.718,26.655,16.893 c1.206,0.125,2.401,0.187,3.585,0.187c9.542,0,18.257-4.005,24.316-11.292c6.581-7.948,7.965-16.133,9.186-23.354 c0.805-4.761,1.474-8.659,3.426-12.094c4.191,19.323,14.16,33.826,28.567,41.258c6.499,3.353,13.594,5.022,20.846,5.022 c8.157,0,16.512-2.113,24.435-6.32c9.087-4.819,15.717-15.282,16.893-26.655C165.913,120.584,161.87,110.418,153.679,103.607z  M82.485,101.144c-10.289,0-18.66-8.371-18.66-18.659s8.371-18.659,18.66-18.659s18.66,8.371,18.66,18.659 S92.774,101.144,82.485,101.144z M109.144,37.175c1.038-6.138,1.934-11.439,5.938-16.276c4.861-5.845,11.371-6.121,14.818-5.764 c6.335,0.655,12.48,4.356,14.945,9.005c5.692,10.717,6.125,21.858,1.218,31.371c-5.252,10.183-15.963,17.376-30.521,20.663 c-1.572-8.248-6.161-15.437-12.575-20.372C107.048,49.493,108.184,42.849,109.144,37.175z M20.87,49.82 c-5.845-4.86-6.12-11.369-5.763-14.816c0.655-6.337,4.356-12.482,9.005-14.947c10.719-5.691,21.861-6.123,31.372-1.217 c10.199,5.261,17.4,15.997,20.679,30.59c-8.23,1.571-15.403,6.147-20.335,12.54c-6.324-4.108-12.99-5.249-18.68-6.211 C31.009,54.721,25.707,53.825,20.87,49.82z M55.827,127.795c-1.038,6.138-1.935,11.439-5.94,16.277 c-4.86,5.845-11.373,6.12-14.815,5.763c-6.336-0.655-12.482-4.356-14.947-9.004c-5.692-10.718-6.124-21.859-1.217-31.372 c5.252-10.182,15.962-17.376,30.52-20.661c1.572,8.248,6.162,15.436,12.576,20.371C57.923,115.477,56.786,122.122,55.827,127.795z  M149.864,129.966c-0.655,6.337-4.356,12.482-9.004,14.947c-10.719,5.692-21.859,6.124-31.373,1.217 c-10.199-5.261-17.4-15.997-20.678-30.591c8.23-1.571,15.403-6.146,20.335-12.541c6.324,4.109,12.99,5.25,18.679,6.212 c6.138,1.038,11.439,1.934,16.277,5.939C149.945,120.01,150.22,126.519,149.864,129.966z" />    
                            </svg>
                        </div>
                        <div class="heat ${heatOn}" id="pin_${allPins[elm[0]][0]}">
                            <svg version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 511.998 511.998" style="enable-background:new 0 0 511.998 511.998;" xml:space="preserve">
                                <path d="M501.34,117.336c-5.906,0-10.688,4.773-10.688,10.664v85.335H469.34v-21.336c0-23.523-19.156-42.663-42.688-42.663c-12.718,0-24.155,5.609-31.999,14.491c-7.812-8.882-19.25-14.491-32-14.491c-12.718,0-24.155,5.609-31.999,14.491c-7.812-8.882-19.25-14.491-32-14.491c-12.719,0-24.156,5.609-31.992,14.491c-7.82-8.882-19.258-14.491-32-14.491c-12.726,0-24.171,5.609-32,14.491c-7.82-8.882-19.258-14.491-32-14.491c-12.726,0-24.171,5.609-32,14.491c-7.82-8.882-19.258-14.491-32-14.491c-19.835,0-36.57,13.616-41.32,31.999H32c-5.891,0-10.664,4.773-10.664,10.664v21.336H10.664C4.773,213.335,0,218.108,0,223.999s4.773,10.664,10.664,10.664h10.672v21.336c0,5.891,4.773,10.664,10.664,10.664h32v202.678c0,23.516,19.141,42.656,42.664,42.656c12.742,0,24.18-5.625,32-14.5c7.828,8.875,19.273,14.5,32,14.5c12.742,0,24.18-5.625,32-14.5c7.828,8.875,19.273,14.5,32,14.5c12.742,0,24.18-5.625,32-14.5c7.836,8.875,19.273,14.5,31.992,14.5c12.75,0,24.188-5.625,32-14.5c7.844,8.875,19.281,14.5,31.999,14.5c12.75,0,24.188-5.625,32-14.5c7.844,8.875,19.281,14.5,31.999,14.5c23.531,0,42.688-19.141,42.688-42.656V234.663h32c5.875,0,10.656-4.773,10.656-10.664V128C511.996,122.109,507.215,117.336,501.34,117.336z M64,245.335H42.664v-42.672H64V245.335z M127.999,469.341c0,11.75-9.563,21.312-21.336,21.312c-11.757,0-21.328-9.562-21.328-21.312V255.999v-64c0-11.758,9.57-21.336,21.328-21.336c11.773,0,21.336,9.578,21.336,21.336V469.341z M191.999,469.341c-0.001,11.75-9.563,21.312-21.336,21.312c-11.757,0-21.328-9.562-21.328-21.312V191.999c0-11.758,9.57-21.336,21.328-21.336c11.773,0,21.336,9.578,21.336,21.336V469.341z M255.998,469.341c0,11.75-9.562,21.312-21.336,21.312c-11.757,0-21.328-9.562-21.328-21.312V191.999c0-11.758,9.57-21.336,21.328-21.336c11.773,0,21.336,9.578,21.336,21.336V469.341zM319.998,469.341c0,11.75-9.562,21.312-21.344,21.312c-11.75,0-21.312-9.562-21.312-21.312V191.999c0-11.758,9.562-21.336,21.312-21.336c11.781,0,21.344,9.578,21.344,21.336V469.341z M383.997,469.341c0,11.75-9.562,21.312-21.344,21.312c-11.749,0-21.312-9.562-21.312-21.312V191.999c0-11.758,9.562-21.336,21.312-21.336c11.781,0,21.344,9.578,21.344,21.336V469.341z M447.996,469.341c0,11.75-9.562,21.312-21.344,21.312c-11.749,0-21.312-9.562-21.312-21.312V191.999c0-11.758,9.562-21.336,21.312-21.336c11.781,0,21.344,9.578,21.344,21.336V469.341z" />
                                <path d="M356.871,103.672c-1.312-4.539-0.5-7.844,2.688-11.055c8.656-8.695,11.531-19.984,8.062-31.812c-2.312-7.938-6.812-13.547-7.375-14.25c-0.031,0-0.031-0.008-0.031-0.008v-0.015c-1.969-2.359-4.906-3.867-8.219-3.867c-5.89,0-10.655,4.781-10.655,10.672c0,2.586,0.906,4.961,2.438,6.805c0.031,0.023,2.281,2.938,3.375,6.656c1.266,4.367,0.469,7.586-2.719,10.766c-8.625,8.678-11.515,20.045-8.078,31.998c2.328,8.102,6.828,13.836,7.328,14.469v-0.008c1.969,2.422,4.953,3.977,8.312,3.977c5.891,0,10.656-4.773,10.656-10.664c0-2.539-0.875-4.867-2.344-6.695C360.278,110.61,357.965,107.555,356.871,103.672z" />
                                <path d="M268.656,96.61c-2.461-9.836-0.492-18.195,6-25.547c11.406-12.891,15.031-28.695,10.5-45.726c-3.188-11.938-9.312-20.227-10-21.133V4.22c-1.969-2.555-5.023-4.219-8.492-4.219c-5.893,0-10.666,4.773-10.666,10.664c0,2.414,0.805,4.633,2.156,6.414h-0.016c0.047,0.062,4.492,6.109,6.547,14.32c2.453,9.827,0.484,18.187-6.016,25.538c-11.39,12.891-15.015,28.696-10.483,45.727c3.172,11.938,9.305,20.227,9.984,21.133l0.016-0.016c1.945,2.57,5.016,4.219,8.477,4.219c5.898,0,10.68-4.773,10.68-10.664c0-2.414-0.812-4.633-2.156-6.414C275.14,110.86,270.695,104.813,268.656,96.61z" />
                                <path d="M189.64,110.641c-0.023-0.031-2.328-3.086-3.445-6.969c-1.305-4.539-0.5-7.844,2.695-11.055c8.664-8.695,11.523-19.984,8.062-31.812c-2.32-7.938-6.789-13.531-7.391-14.25c0,0,0-0.008-0.008-0.008l-0.008-0.023v0.008c-1.961-2.359-4.906-3.867-8.211-3.867c-5.891,0-10.672,4.781-10.672,10.672c0,2.586,0.922,4.961,2.453,6.805c0.023,0.023,2.273,2.938,3.359,6.656c1.281,4.367,0.469,7.586-2.703,10.766c-8.64,8.678-11.515,20.045-8.077,31.998c2.328,8.102,6.828,13.836,7.336,14.469v-0.008c1.961,2.422,4.945,3.977,8.305,3.977c5.891,0,10.664-4.773,10.664-10.664c0-2.539-0.891-4.867-2.367-6.695H189.64z" />
                            </svg>
                        </div>
                    </div>
                    <div class="setTemperature">
                        <span>Utrzymywana temperatura</span>
                        <span>${elm[1]}</span>
                        <span>&#x2103;</span>
                    </div>
                </div>`;

        if (firstElem) firstElem = false;
    }
    $('.temperature .selectPins').html(htmlMenu);
    $('.temperature .containerPins').html(htmlElem);


    $('.pinsMenu').click(function () {
        $('.temperature .selectPins div').removeClass('select');
        $(this).addClass('select');

        $(".containerPins > div").css('display', 'none');
        $(".containerPins #" + $(this).attr("id")).css('display', 'flex');
    })

    $(".temperature .containerPins .fan svg").click(fanClick)
    $(".temperature .containerPins .heat svg").click(heatClick)

    $(".temperature .setTemperature").hover(function () {
        $(this).append('<div class="setTemperature-tooltip tooltip-edit" tooltip-edit> <i class="fas fa-pen"></i></div>');
    }, function () {
        $('.temperature .setTemperature .setTemperature-tooltip').remove();
    })

    $('body').delegate(".setTemperature-window .close-wind", "click", function () {
        $('.setTemperature-window').remove();
        $('.blure').remove();
    })

    $('body').delegate('#inpTemperatureSet', 'keydown', function (event) {
        if (!(event.which >= 48) || !(event.which <= 57) && event.which != 189 && event.which != 190) event.preventDefault();
    })

    $('.setTemperature').delegate('.setTemperature-tooltip', 'click', function (e) {
        $('body').prepend(`<div class="blure">&nbsp;</div>`);
        $('body').prepend(`<div class="setTemperature-window">
        <div class="closes-window"><i class="close-wind fas fa-times"></i></div>
        <form>
            <input id="inpTemperatureSet" type="text" name="temperature" placeholder="Podaj temperature"
                autocomplete="off">
        </form>
        <div class="submit-button">
            <button id="setTemperature-submit" class="button-sbm delaySubmit">Zatwierdź</button>
        </div>
        <div class="info">
            <div>
                <i class="fas fa-info-circle"></i>
                <span>Znak myślnika(-) - umożliwia podanie zakresu temperatur np: 20.2-24</span>
            </div>
        </div>
    </div>`);
    })

    $('body').delegate('#setTemperature-submit', 'click', setTemerature)
})