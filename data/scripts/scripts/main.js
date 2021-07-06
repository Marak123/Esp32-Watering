const allPins = [[13, "Pin 13", "power"], [14, "Pin 14", "power"], [15, "Pin 15", "power"], [16, "Pin 16", "power"], [17, "Pin 17", "power"], [18, "Pin 18", "power"], [19, "Pin 19", "power"], [20, "Pin 20", "power"], [21, "Pin 21", "temperature"], [22, "Pin 22", "air"], [23, "Pin 23", "heat"], [24, "Pin 24", "temperature"], [25, "Pin 25", "air"], [26, "Pin 26", "heat"]];

const elementsPins = [[0, false], [1, true], [2, false], [3, true], [4, true], [5, false], [6, false], [7, false]];

const tmpEm = [[8, 9, 10], [11, 12, 13]];

const temperatureEle = [[8, "21", true, false, "22.6"], [11, "23.2", true, false, "25.9"]];

const listActionElm = [[[11, 1, 2022], [20, 49], false, 4], [[19, 4, 2021], [8, 9], true, 7], [[18, 9, 2020], [5, 24], true, 0], [[18, 9, 2020], [7, 24], false, 0], [[17, 5, 2021], [19, 29], true, 6]];

function messageSee(text, action) {
    if (!($('div.messageAlert').length)) {
        $('body').prepend(`<div class="messageAlert ${action}"><p>${text}</p></div>`);
        $('.messageAlert').css({ 'right': '-300%' }).animate({ "right": "10" }, "slow")
        setTimeout(function () {
            $('.messageAlert').css({ 'right': '10px' }).animate({ "right": "-300%" }, "slow")
            setTimeout(function () {
                $('.messageAlert').remove();
            }, 1000)
        }, 5000)
    }
}