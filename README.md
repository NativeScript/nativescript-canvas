About
============
This is a proof-of-concept plugin that enables HTML5 like Canvas API for NativeScript. The code is not production ready and a lot of features are currently missing. You can check the overall progress in [this](https://github.com/NativeScript/nativescript-canvas/issues/1) issue.

Contributing
============

The project encourages hacking, so hack away. Clone the repo and follow the steps to set yourself up for development.

Prerequisites:
* `npm i -g grunt-cli`
* iOS: XCode 
* Android: Android NDK, SDK and Ant

```shell
npm install # install canvas library dependencies
cd test
npm install # install test project dependencies
tns run android # fire up the test project on android
tns run ios # fire up the test project on ios
```

After making modifications to the canvas library, regardless whether it's the iOS, Android or JS part, you can re-launch with your changes like so:

```shell
npm i .. && rm -rf platforms && tns run <platform>
```

You can skip the `rm -rf platforms` part when you don't change the public interface of the native bits.
