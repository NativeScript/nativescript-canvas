function getRandomHex() {
    return [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 'A', 'B', 'C', 'D', 'E', 'F'][0|(Math.random() * 15)].toString();
}


var vmModule = require("./main-view-model");
function pageLoaded(args) {
    var page = args.object;
    page.bindingContext = vmModule.mainViewModel;

    var canvas = page.content.getViewById('canvas');
    var context = canvas.getContext('2d');
    setInterval(function () {
        var fillStyle = '#' + getRandomHex() + getRandomHex() + getRandomHex();
        console.log('Fillstyle is ' + fillStyle);
        context.fillStyle = fillStyle;
        for (var i = 0; i < 10000; ++i) {
            context.fillRect(0|(Math.random() * 200), Math.random() * 200, 5, 5);
        }
        if (canvas.ios) {
            canvas.ios.setNeedsDisplay();
        }
    }, 0);
}
exports.pageLoaded = pageLoaded;
