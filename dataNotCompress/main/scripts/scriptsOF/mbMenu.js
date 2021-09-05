var MainMenu = (function () {
    var MainMenu = function (config) {
        config = config || {};
        this.toggleBtn = $(config.toggleBtn);
        this.menu = $(config.menu);
        this.close = $(config.close);
        this.menuForm = $(config.menuForm);
        this.menuContent = $(config.menuContent);

        this.init();
        config = null;
    };

    var documentClick;
    // public interface
    MainMenu.prototype = {
        constructor: MainMenu,
        init: function () {
            this.eventManager();
        },
        eventManager: function () {
            this.toggleBtn.on('click.openMenu', onButtonClickHandler.bind(this));
            this.close.on('click.closeMenu', onCloseClickHandler.bind(this));
        }
    };
    // private interface
    function onButtonClickHandler(menu, evt) {
        if (!this.menu.hasClass('open')) {
            $(".main-content").removeClass("top");
            this.menu.addClass('open');
            $(document).click(onDocumentClickHandler.bind(this));
            $(".main-mb-menu-form").addClass('seeBackground');
        };

    }

    function onCloseClickHandler(evt) {
        setTimeout(function () {
            $(".main-content").addClass("top");
        }, 500);
        $(".main-mb-menu").removeClass('open')
        $(document).off("click");
        $(".main-mb-menu-form").removeClass('seeBackground');
    }

    function onDocumentClickHandler(evt) {
        var $target = $(evt.target);

        if (!$target.closest(this.menuForm).length && !$target.closest(this.menuContent).length && this.menu.hasClass('open')) {
            onCloseClickHandler();
        }
    }

    return MainMenu;
})();


$(document).ready(function () {
    var mainMenu = new MainMenu({
        menu: '.main-mb-menu',
        toggleBtn: '.main-mb-menu-btn',
        close: '.main-mb-menu-close',
        menuForm: '.main-mb-menu-form',
        menuContent: '.main-mb-menu-content'
    });
});

// Obsługa mobilnego menu
$(".whatClick").click(function () {
    var colorStl = ["#D35400", "#F5AB35", "#00B16A", "#9A12B3", "#F64747", "#2cbaf8"];
    var nbEle = ["orange", "yellow", "green", "purple", "red", "blue"];

    var getId = $(this).prop("id");
    var getClass = $(this).prop("class").substring(0, $(this).prop("class").indexOf(" "));

    $(".whatClick").removeAttr("style");
    $(".whatClick i").removeAttr("style");

    var inArray = nbEle.indexOf(getClass);

    $(this).attr("style", "color: " + colorStl[inArray] + ";")
    $("." + getClass + " i").attr("style", "color: " + colorStl[inArray] + ";")

    $(".main-content > div").css("display", "none");
    $("." + getId).css("display", "flex");

    if ($(this).parent().attr('desktop')) {
        $('.navigation .menu-bar li').removeClass('select')
        $(this).addClass('select')
    }
})