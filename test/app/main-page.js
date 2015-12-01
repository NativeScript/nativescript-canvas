var vmModule = require("./main-view-model");
function pageLoaded(args) {
    var page = args.object;
    page.bindingContext = vmModule.mainViewModel;

    // var canvas = page.content.getViewById('canvas');
    // canvas.onLoaded = function () {
    //     console.log('loaded canvas');
    //     var context = canvas.getContext('2d');
    //     context.fillRect(20, 20, 300, 300);
    // };
}
exports.pageLoaded = pageLoaded;
