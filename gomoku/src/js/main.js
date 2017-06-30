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
  $('.ctrl-analyze input').prop('disabled', true);

  // write winner to database
  firebase.database().ref(gameID).child('result').set(winnerColor);
}

// post request, params should be json type
function post(params, path) {
  var http = new XMLHttpRequest();
  http.open('POST', path);

  var playAiPoint = function(response) {
    if (response.row == -1) {
      board.pass();
      alert('computer pass'); //computer pass
    } else {
      board.play([response.col, response.row]); // play at ai's respond point
    }

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
      $('.ctrl-game input').prop('disabled', false);
      $('.ctrl-analyze input').prop('disabled', false);
      if (board.playNo == 0 || (board.playNo == 1 && player['black'] == 'computer'))
        $('#ctrl-undo').prop('disabled', true);
    } else {
      post(null, 'think');
    }

    timer[board.whoTurn()].start();
  }

  http.onreadystatechange = function() {
    if (!(http.readyState == 4 && (http.status == 200 || http.status == 204))) return;

    switch (path) {
      case 'start':
      case 'pass':
        checkNextPlayer();
        break;
      case 'think':
        var response = JSON.parse(http.responseText);
        playAiPoint(response);
        if (checkWinner(response))
          return;
        checkNextPlayer();
        break;
      case 'play':
        var response = JSON.parse(http.responseText);
        if (checkWinner(response))
          return;
        checkNextPlayer();
        break;
      case 'resign':
        notifyWinner(board.whoTurn(board.playNo + 1));
        break;
      case 'undo':
        checkNextPlayer();
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

    // clear the board
    board.init();

    // check if game id exists. If existing, populate player data.
    var gameRef = firebase.database().ref(gameID);
    gameRef.once('value').then(function(snapshot) {

      if (snapshot.val() == null) {
        alert("Sorry, the game ID you entered is invalid");
        return;
      }

      // deside player's place
      if (snapshot.child('black').val() == 'human') {
        // black == human, white == human -> sel: black, human, opp: white, human
        // black == human, white == ai -> sel: black, human, opp: white, ai
        dialog.initPlayer('sel', 'black', true);
        dialog.initPlayer('opp', 'white', (snapshot.child('white').val() == 'human'));
      } else {
        if (snapshot.child('white').val() == 'human') {
        // black == ai, white == human -> sel: white, human, opp: black, ai
        dialog.initPlayer('sel', 'white', true);
        dialog.initPlayer('opp', 'black', false);
        } else {
          // black == ai, white == ai -> sel: black, ai, opp: white, ai
          dialog.initPlayer('sel', 'black', false);
          dialog.initPlayer('opp', 'white', false);
        }
      }
    });


    gameRef.child('record').on('child_added', function(data) {
      console.log('key=' + data.key +",row=" + data.val().r + ",col=" + data.val().c);
      board.put([data.val().r, data.val().c], data.key);
    });
  }
}

function changeDisplayNo(changeAmount) {
  board.changeDisplayNo(changeAmount);
}

function undo() {
  $('.ctrl-game input').prop('disabled', true);
  $('.ctrl-analyze input').prop('disabled', true);
  var times = (player[board.whoTurn(board.playNo + 1)] === 'human' ? 1 : 2);
  board.undo(times);
  post({ times: times }, 'undo');
}

function pass() {
  $('.ctrl-game input').prop('disabled', true);
  $('.ctrl-analyze input').prop('disabled', true);
  board.pass();
  post(null, 'pass');
}

function resign() {
  $('.ctrl-game input').prop('disabled', true);
  $('.ctrl-analyze input').prop('disabled', true);
  timer[board.whoTurn()].stop();
  post(null, 'resign');
}

function hint() {
  $('.ctrl-game input').prop('disabled', true);
  $('.ctrl-analyze input').prop('disabled', true);
  board.enable = false;
  post(null, 'think');
}