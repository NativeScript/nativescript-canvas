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
        var M = require('./sylvester');
        var gl = canvas.getContext('webgl');
        var initialized = false;
        var shaderProgram;
        var vertexPositionAttribute;
        var mvMatrix, perspectiveMatrix;
        var squareVerticesBuffer;

        function initBuffers() {
            // Create a buffer for the square's vertices.
            console.log('creating buffers...')

            squareVerticesBuffer = gl.createBuffer();

            // Select the squareVerticesBuffer as the one to apply vertex
            // operations to from here out.

            gl.bindBuffer(gl.ARRAY_BUFFER, squareVerticesBuffer);
            gl.disable(gl.CULL_FACE);

            // Now create an array of vertices for the square. Note that the Z
            // coordinate is always 0 here.

            var vertices = [
                1.0, 1.0, 0.0,
                -1.0, 1.0, 0.0,
                1.0, -1.0, 0.0,
                -1.0, -1.0, 0.0
            ];

            // Now pass the list of vertices into WebGL to build the shape. We
            // do this by creating a Float32Array from the JavaScript array,
            // then use it to fill the current vertex buffer.

            console.log('filling buffer data...')
            gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
            console.log('buffers are done!')
        }

        function loadIdentity() {
            mvMatrix = M.Matrix.I(4);
        }

        function multMatrix(m) {
            mvMatrix = mvMatrix.x(m);
        }

        function mvTranslate(v) {
            multMatrix(M.Matrix.Translation(M.$V([v[0], v[1], v[2]])).ensure4x4());
        }

        function setMatrixUniforms() {
            var pUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
            gl.uniformMatrix4fv(pUniform, false, new Float32Array(perspectiveMatrix.flatten()));

            var mvUniform = gl.getUniformLocation(shaderProgram, "uMVMatrix");
            gl.uniformMatrix4fv(mvUniform, false, new Float32Array(mvMatrix.flatten()));
        }

        var shaderSources = {
            "shader-fs": {
                kind: gl.FRAGMENT_SHADER,
                source: "void main(void) {gl_FragColor = vec4(1.0, 0.4, 0.2, 1.0);}"
            },
            "shader-vs": {
                kind: gl.VERTEX_SHADER,
                source: "attribute vec3 aVertexPosition;\r\n      uniform mat4 uMVMatrix;\r\n      uniform mat4 uPMatrix;\r\n      void main(void) { \r\n        gl_Position = /*uPMatrix * uMVMatrix * */vec4(aVertexPosition, 1.0);\r\n      }"
            }
        };

        function getShader(name) {
            console.log('loading shader ' + name);
            var shaderData = shaderSources[name];
            var shader = gl.createShader(shaderData.kind);
            console.log('loading shader source');
            gl.shaderSource(shader, shaderData.source);
            console.log('compiling shader');
            gl.compileShader(shader);
            if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
                console.log("An error occurred compiling the shaders: " + gl.getShaderInfoLog(shader));
                return null;
            }
            console.log('shader is done');
            return shader;
        }

        function initShaders() {
            var fragmentShader = getShader("shader-fs");
            var vertexShader = getShader("shader-vs");

            // Create the shader program
            console.log('creating program');
            shaderProgram = gl.createProgram();
            gl.attachShader(shaderProgram, vertexShader);
            gl.attachShader(shaderProgram, fragmentShader);
            gl.linkProgram(shaderProgram);

            // If creating the shader program failed, alert

            if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
                console.log("Unable to initialize the shader program.");
                return;
            }
            console.log('program is complete');
            gl.useProgram(shaderProgram);

            vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
            gl.enableVertexAttribArray(vertexPositionAttribute);
        }

        function draw() {
            // if (!initialized) {
            //     console.log('initializing first time')
            //     initialized = true;
            //     initShaders();
            //     initBuffers();
            //     console.log('initialization finished, proceeding to draw!')
            // }

            //gl.enable(gl.DEPTH_TEST);
            // gl.depthFunc(gl.LEQUAL);
            // gl.viewport(0, 0, 400, 400);
            android.opengl.GLES20.glClearColor(0.5, 0.5, 0.5, 1.0);
            android.opengl.GLES20.glClear(android.opengl.GLES20.GL_COLOR_BUFFER_BIT);
            // gl.clearColor(0.5, 0.5, 0.5, 1.0);
            // gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

            // perspectiveMatrix = M.makePerspective(45, 640.0/480.0, 0.1, 100.0);

            // loadIdentity();
            // mvTranslate([-0.0, 0.0, -6.0]);

            // gl.bindBuffer(gl.ARRAY_BUFFER, squareVerticesBuffer);
            // gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);
            // //setMatrixUniforms();

            // gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);

            canvas.requestAnimationFrame(draw);
        };
        canvas.requestAnimationFrame(draw);
    }
    testGL();
}
exports.pageLoaded = pageLoaded;
