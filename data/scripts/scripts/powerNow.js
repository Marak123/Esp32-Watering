var em = "";
var inf = {
    turn: "",
    pin: "",
    pinId: "",
    textTurn: "",
    namePin: "",
    check: "",
    classTurn: ""
}
for (var el of elementsPins) {
    inf.pin = allPins[el[0]][0];
    inf.pinId = el[0]
    inf.namePin = allPins[el[0]][1];

    if (el[1]) { inf.turn = "on"; inf.textTurn = "WŁĄCZONE"; inf.check = "checked"; inf.classTurn = "power-on"; }
    else { inf.turn = "off"; inf.textTurn = "WYŁĄCZONE"; inf.check = ""; inf.classTurn = ""; }

    em += `<div class="pin_${inf.pin} ${inf.turn}" id="pinId_${inf.pinId}">
    <span class="switch-ct">
        <label class="switch">
            <input type="checkbox" class="checkbox" name="pin_${inf.pin}" value="pin_${inf.pin}" ${inf.check}>
            <div class="slider">
                <svg viewBox="144.142 128.056 268.269 169.133" width="80%">
                    <path
                            d="M 162.624 242.721 C 159.204 242.721 156.567 241.824 154.714 240.031 C 152.854 238.244 151.924 235.381 151.924 231.441 C 151.924 227.501 152.854 224.634 154.714 222.841 C 156.567 221.048 159.204 220.151 162.624 220.151 C 166.037 220.151 168.674 221.048 170.534 222.841 C 172.387 224.634 173.314 227.501 173.314 231.441 C 173.314 235.381 172.387 238.244 170.534 240.031 C 168.674 241.824 166.037 242.721 162.624 242.721 Z M 162.624 237.741 C 163.257 237.741 163.734 237.571 164.054 237.231 C 164.367 236.898 164.524 236.378 164.524 235.671 L 164.524 227.211 C 164.524 226.504 164.367 225.981 164.054 225.641 C 163.734 225.308 163.257 225.141 162.624 225.141 C 161.991 225.141 161.514 225.308 161.194 225.641 C 160.874 225.981 160.714 226.504 160.714 227.211 L 160.714 235.671 C 160.714 236.398 160.874 236.924 161.194 237.251 C 161.514 237.578 161.991 237.741 162.624 237.741 ZM 188.179 242.331 L 175.269 242.331 L 175.269 237.741 L 176.669 237.741 L 176.669 225.141 L 175.269 225.141 L 175.269 220.551 L 192.009 220.551 L 192.009 227.801 L 187.059 227.801 L 187.059 225.141 L 184.509 225.141 L 184.509 229.791 L 188.739 229.791 L 188.739 232.981 L 184.509 232.981 L 184.509 237.741 L 188.179 237.741 L 188.179 242.331 ZM 206.875 242.331 L 193.965 242.331 L 193.965 237.741 L 195.365 237.741 L 195.365 225.141 L 193.965 225.141 L 193.965 220.551 L 210.705 220.551 L 210.705 227.801 L 205.755 227.801 L 205.755 225.141 L 203.205 225.141 L 203.205 229.791 L 207.435 229.791 L 207.435 232.981 L 203.205 232.981 L 203.205 237.741 L 206.875 237.741 L 206.875 242.331 Z"
                            transform="matrix(4.460696220397949, 0, 0, 4.4127678871154785, -530.53857421875, -813.1041870117188)"
                            fill="#fff" />
                </svg>
                <svg viewBox="98.062 117.952 264.418 196.833" width="80%">
                    <path
                            d="M 155.262 195.482 C 151.842 195.482 149.205 194.585 147.352 192.792 C 145.492 191.005 144.562 188.142 144.562 184.202 C 144.562 180.262 145.492 177.395 147.352 175.602 C 149.205 173.809 151.842 172.912 155.262 172.912 C 158.675 172.912 161.312 173.809 163.172 175.602 C 165.025 177.395 165.952 180.262 165.952 184.202 C 165.952 188.142 165.025 191.005 163.172 192.792 C 161.312 194.585 158.675 195.482 155.262 195.482 Z M 155.262 190.502 C 155.895 190.502 156.372 190.332 156.692 189.992 C 157.005 189.659 157.162 189.139 157.162 188.432 L 157.162 179.972 C 157.162 179.265 157.005 178.742 156.692 178.402 C 156.372 178.069 155.895 177.902 155.262 177.902 C 154.629 177.902 154.152 178.069 153.832 178.402 C 153.512 178.742 153.352 179.265 153.352 179.972 L 153.352 188.432 C 153.352 189.159 153.512 189.685 153.832 190.012 C 154.152 190.339 154.629 190.502 155.262 190.502 ZM 184.037 177.902 L 184.037 173.312 L 193.217 173.312 L 193.217 177.902 L 191.817 177.902 L 191.817 195.092 L 183.947 195.092 L 175.747 182.462 L 175.747 190.502 L 177.087 190.502 L 177.087 195.092 L 167.907 195.092 L 167.907 190.502 L 169.307 190.502 L 169.307 177.902 L 167.907 177.902 L 167.907 173.312 L 178.017 173.312 L 185.377 184.532 L 185.377 177.902 L 184.037 177.902 Z"
                            transform="matrix(5.298288822174072, 0, 0, 5.131278991699219, -664.4776611328125, -734.0007934570312)"
                            fill="#fff" />
                </svg>
            </div>
        </label>
    </span>
    <span class="text-info">
        <p class="info-power ${inf.classTurn}">${inf.textTurn}</p>
        &nbsp;
        <p class="info-pin">${inf.namePin.toUpperCase()}</p>
    </span>
</div>`
}

$(".power-now").html(em);

$(".switch").click(function (event) {
    if ($(event.target).is("input")){
        if (!($(event.target).is(":checked"))) $(event.target).parent().parent().next().children(".info-power").removeClass("power-on").html("WYŁĄCZONE");
        else $(event.target).parent().parent().next().children(".info-power").addClass("power-on").html("WŁĄCZONE");
    }
})