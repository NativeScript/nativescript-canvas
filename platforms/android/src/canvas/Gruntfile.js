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

  grunt.registerTask('libs', ['unzip:arm', 'unzip:x86']);
  grunt.registerTask('default', ['libs']);
};
