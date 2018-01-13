window.Module = (function() {

  var canvas = document.getElementById('canvas');
  var output = document.getElementById('output');

  // clear the output.
  output.value = '';

  function print(text) {
    if (arguments.length > 1)
      text = Array.prototype.slice.call(arguments).join(' ');
    output.value += text + "\n";
    output.scrollTop = output.scrollHeight; // focus on bottom
  };

  function printErr(text) {
    if (arguments.length > 1)
      text = Array.prototype.slice.call(arguments).join(' ');
    console.error(text);
  };

  // Alert the user if we have lost the WebGL context.
  canvas.addEventListener("webglcontextlost", function(e) {
    alert('WebGL context lost. You will need to reload the page.');
    e.preventDefault();
  }, false);

  return {
    preRun: [],
    postRun: [],
    print: print,
    printErr: printErr,
    canvas: canvas
  };

})();
