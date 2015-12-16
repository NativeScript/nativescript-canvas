function getRandomHex() {
    return [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 'A', 'B', 'C', 'D', 'E', 'F'][0|(Math.random() * 15)].toString();
}


var vmModule = require("./main-view-model");
function pageLoaded(args) {
    var page = args.object;
    page.bindingContext = vmModule.mainViewModel;

    var canvas = page.content.getViewById('canvas');

    function test2d() {
        var context = canvas.getContext('2d');
        setInterval(function () {
            var fillStyle = '#' + getRandomHex() + getRandomHex() + getRandomHex();
            console.log('Fillstyle is ' + fillStyle);
            context.fillStyle = fillStyle;
            for (var i = 0; i < 10000; ++i) {
                context.fillRect(0 | (Math.random() * 200), Math.random() * 200, 5, 5);
            }
            if (canvas.ios) {
                canvas.ios.setNeedsDisplay();
            }
        }, 0);
    }

    function testGL() {
        var gl = canvas.getContext('webgl');
        function draw() {

            gl.viewport(0, 0, 200, 200);
            gl.clearColor(0.4, 0.4, 0.4, 0.4, 1.0);
            gl.enable(gl.DEPTH_TEST);
            gl.depthFunc(gl.LEQUAL);
            gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

            canvas.requestAnimationFrame(draw);
        };
        canvas.requestAnimationFrame(draw);
    }
    testGL();
}
exports.pageLoaded = pageLoaded;
