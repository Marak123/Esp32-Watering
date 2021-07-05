$(function () {
    let htmlStatusList = "";
    let posi;
    for (let em of allPins) {
        if (em[2] == "power" || em[2] == "air" || em[2] == "heat") {
            posi = -1;
            if(em[2] == "power")
                for(let lm of elementsPins){
                    if (lm[0] == allPins.indexOf(em)) posi = lm[1];
                }
            else if(em[2] == "air")
                for (let lm of tmpEm) {
                    if (lm[1] == allPins.indexOf(em)) {
                        for (let elm of temperatureEle) {
                            if (lm[0] == elm[0]) {
                                posi = elm[2];
                                break;
                            }
                        }
                        break;
                    }
                }
            else if(em[2] == "heat")
                for (let lm of tmpEm) {
                    if (lm[2] == allPins.indexOf(em)){
                        for(let elm of temperatureEle){
                            if(lm[0] == elm[0]){
                                posi = elm[3];
                                break;
                            }
                        }
                        break;
                    }
                }
            htmlStatusList += `<div><div><span class="${ posi ? 'on' : 'off'}"></span>
                                                    <p>${posi ? 'Włączone' : 'Wyłączone'}</p>
                                                </div>
                                                <div>
                                                    <p>${ em[1] }</p>
                                                </div>
                                            </div>`
        }
    }
    $('main .status .sts-list').html(htmlStatusList);
})