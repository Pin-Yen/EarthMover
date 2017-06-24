var timer = { 'black': null, 'white': null };

var game = { 'black': 'human', 'white': 'computer'};

var board = new Board();

var dialog = new Dialog();

function notifyWinner(winnerColor) {
  // TODO: display game status
  setTimeout(function() { alert(winnerColor + " wins !"); }, 0);
}

// post request, params should be json type
function post(params, path) {
  var http = new XMLHttpRequest();
  http.open('POST', path);

  http.onreadystatechange = function() {
    if (!(http.readyState == 4 && (http.status == 200 || http.status == 204))) return;

    if (path != 'start') {
      var response = JSON.parse(http.responseText);

      if (path == 'think') {
        // play at ai's respond point
        board.play([response.col, response.row]);

        if (response.col == board.mousePos[0] && response.row == board.mousePos[1])
          board.mousePos = [-1, -1];
      }

      // check if someone win
      if (response.winner != -1) {
        notifyWinner(response.winner == 0 ? 'Black' : 'White');
        board.enable = false;
        board.gameStarted = false;
        $('.ctrl-replay input').prop('disabled', false);
        return;
      }
    }

    if (humanTurn()) {
      board.draw(board.mousePos);
      board.enable = true;
    } else {
      post(null, 'think');
    }

    timer[board.whoTurn()].start();
  };


  http.send(JSON.stringify(params));
}

function humanTurn() {
  return game[board.whoTurn()] == 'human';
}

function btnPlayNumberClick() {
  board.changePlayNumber();

  $('#play-number-check').toggle();
}

function btnCoordinateClick() {
  board.changeCoordinate();

  $('#coordinate-check').toggle();
}

function changeDisplayNo(changeAmount) {
  board.changeDisplayNo(changeAmount);
}