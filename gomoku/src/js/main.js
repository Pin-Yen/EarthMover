var timer = { 'black': null, 'white': null };

var player = { 'black': 'human', 'white': 'computer'};

var game = {black : "N/A", white : "N/A", rule : "N/A", startTime : -1,
 earthmover : {level : -1, version : 0}};
var gameID;

var board = new Board();

var dialog = new Dialog();

function notifyWinner(winnerColor) {
  // TODO: display game status
  setTimeout(function() { alert(winnerColor + " wins !"); }, 0);
  board.enable = false;
  board.gameStarted = false;
  $('.ctrl-replay input').prop('disabled', false);
  $('.ctrl-game input').prop('disabled', true);

  // write winner to database
  firebase.database().ref(gameID).child('result').set(winnerColor);
}

// post request, params should be json type
function post(params, path) {
  var http = new XMLHttpRequest();
  http.open('POST', path);

  var changeTimer = function() {
    timer[board.whoTurn()].start();
  }

  var playAiPoint = function(response) {
    // play at ai's respond point
    board.play([response.col, response.row]);

    if (response.col == board.mousePos[0] && response.row == board.mousePos[1])
      board.mousePos = [-1, -1];
  }

  // returns true if someone wins
  var checkWinner = function(response) {
    if (response.winner != -1) {
      notifyWinner(response.winner == 0 ? 'Black' : 'White');
      return true;
    }
    return false;
  }

  var checkNextPlayer = function() {
    if (humanTurn()) {
      board.draw(board.mousePos);
      board.enable = true;
    } else {
      post(null, 'think');
    }
  }

  http.onreadystatechange = function() {
    if (!(http.readyState == 4 && (http.status == 200 || http.status == 204))) return;

    switch (path) {
      case 'start':
        checkNextPlayer();
        changeTimer();
        break;
      case 'think' :
        var response = JSON.parse(http.responseText);
        playAiPoint(response);
        if (checkWinner(response))
          return;
        checkNextPlayer();
        changeTimer();
        break;
      case 'play':
        var response = JSON.parse(http.responseText);
        if (checkWinner(response))
          return;
        checkNextPlayer();
        changeTimer();
        break;
      case 'resign':
        notifyWinner(board.whoTurn(board.playNo + 1));
        break;
    }
  };


  http.send(JSON.stringify(params));
}

function humanTurn() {
  return player[board.whoTurn()] == 'human';
}

function btnPlayNumberClick() {
  board.changePlayNumber();

  $('#play-number-check').toggle();
}

function btnCoordinateClick() {
  board.changeCoordinate();

  $('#coordinate-check').toggle();
}

function btnWatchGameClick() {
  var gameIdPrompt = prompt("Please paste the game ID here");
  if (gameIdPrompt != null) {
    gameID = gameIdPrompt.trim();
    alert("STILL UNDER CONSTRUCTION");
  }
}

function changeDisplayNo(changeAmount) {
  board.changeDisplayNo(changeAmount);
}

function resign() {
  timer[board.whoTurn()].stop();
  post(null, 'resign');
}