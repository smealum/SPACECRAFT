var Module = {
  preRun: [],
  postRun: [],

  print: (function() {
    var output = document.getElementById('output');
    if (output)
      output.value = ''; // clear browser cache

    return function(text) {
      if (arguments.length > 1)
        text = Array.prototype.slice.call(arguments).join(' ');

      // Log output in the console and in the <textarea id="output"> element.
      console.log(text);
      if (output) {
        output.value += text + "\n";
        output.scrollTop = output.scrollHeight; // focus on bottom
      }
    };
  })(),

  printErr: function(text) {
    if (arguments.length > 1)
      text = Array.prototype.slice.call(arguments).join(' ');
    console.error(text);
  },

  canvas: (function() {
    var canvas = document.getElementById('canvas');
    var context_lost_handler = function(e) {
      alert('WebGL context lost. You will need to reload the page.');
      e.preventDefault();
    }
    canvas.addEventListener("webglcontextlost", e, false);

    return canvas;
  })(),
};
