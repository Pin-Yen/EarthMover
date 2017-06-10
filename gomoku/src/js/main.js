var timer = { 'black': null, 'white': null };

var game = { 'black': 'human', 'white': 'computer'};

var board = new Board();

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

// initialize player
// NOTE: player should be 'sel' or 'opp', color should be 'black' or 'white'
// human is bool
function initPlayer(player, color, human) {
  var oppColor = (color == 'black' ? 'white' : 'black');

  // set color
  $('.player-information.' + player).removeClass(oppColor);
  $('.player-information.' + player).addClass(color);

  // set chess
  $('#pi-chess-' + player).attr('src', 'gomoku/src/png/chess_' + color + '.png');
  $('#pi-icon-' + player).attr('src', 'gomoku/src/png/' + (human ? 'human' : 'icon') + '.png');

  // set name
  $('#pi-name-' + player).html((human ? $('#dl-name-' + color).val() : 'EarthMover'));
}

function btnNewGameClick() {
  $('#dialog-new-game').show();
}

function btnDialogCancelClick() {
  $('#dialog-new-game').hide();
}

function btnDialogOKClick() {
  // get the black/white player's radio buttons' result
  var black = $('input[name="black"]:checked').val(),
      white = $('input[name="white"]:checked').val();

  if ($('#dl-name-black').val().trim() == '')
    $('#dl-name-black').val('you');
  if ($('#dl-name-white').val().trim() == '')
    $('#dl-name-white').val('you');

  game = {'black': black, 'white': white};

  var rule = parseInt($('input[name="rule"]:checked').val(), 10), level = parseInt($('#dl-select').val(), 10);

  post({rule: rule, level: level}, 'start');

  // deside player's place
  if (black == 'human') {
    // black == human, white == human -> sel: black, human, opp: white, human
    // black == human, white == ai -> sel: black, human, opp: white, ai
    initPlayer('sel', 'black', true);
    initPlayer('opp', 'white', (white == 'human'));
  } else {
    if (white == 'human') {
      // black == ai, white == human -> sel: white, human, opp: black, ai
      initPlayer('sel', 'white', true);
      initPlayer('opp', 'black', false);
    } else {
      // black == ai, white == ai -> sel: black, ai, opp: white, ai
      initPlayer('sel', 'black', false);
      initPlayer('opp', 'white', false);
    }
  }

  board.enable = false;
  board.gameStarted = true;

  // initialize board
  board.init();

  // initialize timer
  if (timer.black != null) timer.black.stop();
  if (timer.white != null) timer.white.stop();
  timer = {black: new Timer($('.black .pi-timer')), white: new Timer($('.white .pi-timer'))};

  $('#dialog-new-game').hide();
}

function btnPlayNumberClick() {
  if (board.changePlayNumber()) {
    $('#play-number-check').show();
  } else {
    $('#play-number-check').hide();
  }
}

function btnCoordinateClick() {
  if (board.changeCoordinate()) {
    $('#coordinate-check').show();
  } else {
    $('#coordinate-check').hide();
  }
}

function changeDisplayNo(argument) {
  if (board.gameStarted) return;
  board.changeDisplayNo(argument);
}