$('.plan .dataPicker .weekLine').weekLine();


$(function(){
    const Data = new Date();

    let dataFormat = (Data.getMonth() + 1) + "/" + Data.getDate() + "/" + Data.getFullYear();
    if(Data.getMonth() < 9) dataFormat = "0" + dataFormat
    if(Data.getDate() < 9)  dataFormat = dataFormat.substring(0, 3) + "0" + dataFormat.substring(3, dataFormat.length)
    $("#id_4 input").val(dataFormat);
})

$(".plan-menu .date").click(function(){
    $(this).children().addClass('selected');
    $(this).next().children().removeClass('selected');
    $(".dataPicker .date").removeClass('hidden');
    $(".dataPicker .weekLine").addClass('hidden');
})
$(".plan-menu .weekDay").click(function(){
    $(this).children().addClass('selected');
    $(this).prev().children().removeClass('selected');
    $(".dataPicker .date").addClass('hidden');
    $(".dataPicker .weekLine").removeClass('hidden');
})