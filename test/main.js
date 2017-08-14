var board = new Board();

function post(params, path) {
  var http = new XMLHttpRequest();
  http.open('POST', path);

  http.onreadystatechange = function() {
    if (!(http.readyState == 4 && (http.status == 200 || http.status == 204))) return;

    switch (path) {
      case 'start':
        board.init();
        break;
      case 'fetch':
        board.drawAll(JSON.parse(http.responseText));
        break;
    }
  };


  http.send(JSON.stringify(params));
}

function start() {
  post(null, 'start');
}
