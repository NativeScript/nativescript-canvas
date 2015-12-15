module.exports = function(grunt) {

  grunt.initConfig({
    unzip: {
      arm: {
        src: 'jni/libs/arm/libskia_android.zip',
        dest: 'jni/libs/arm/'
      },
      x86: {
        src: 'jni/libs/x86/libskia_android.zip',
        dest: 'jni/libs/x86/'
      },
    }
  });

  grunt.loadNpmTasks('grunt-zip');

  var unzipTasks = [];
  var fs = require('fs');
  if (!fs.existsSync('jni/libs/arm/libskia_android.a')) {
    unzipTasks.push('unzip:arm');
  }
  if (!fs.existsSync('jni/libs/x86/libskia_android.a')) {
    unzipTasks.push('unzip:x86');
  }

  grunt.registerTask('libs', unzipTasks);
  grunt.registerTask('default', ['libs']);
};
