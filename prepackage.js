var fs = require('fs');
var path = require('path');
var mkdirp = require('mkdirp');

var filesToCopy = [
	['platforms/android/jniLibs/x86/libcanvas.so', 'platforms/android/src/canvas/libs/x86/libcanvas.so'],
	['platforms/android/jniLibs/armeabi-v7a/libcanvas.so', 'platforms/android/src/canvas/libs/armeabi-v7a/libcanvas.so'],
	['platforms/android/canvas.jar', 'platforms/android/src/canvas/bin/classes.jar'],
];

filesToCopy.forEach(function (pair) {
	mkdirp(path.dirname(pair[0]), function () {
		fs.writeFileSync(pair[0], fs.readFileSync(pair[1]));
	});
});
