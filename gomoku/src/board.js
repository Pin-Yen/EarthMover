var game = { black: 'human', white: 'computer'};

var boardEnable = false, gameStarted = false;
var playNumber = false;
var coordinate = false;

var mousePos = [-1, -1], lastPlay = [-1, -1];

// image array
var chessImage = new Array(6);
for (var i = chessImage.length - 1; i >= 0; i--)
  chessImage[i] = new Image();

chessImage[0].src = "gomoku/src/chess_black_transparent.png";
chessImage[1].src = "gomoku/src/chess_black.png";
chessImage[2].src = "gomoku/src/chess_black_marked.png";
chessImage[3].src = "gomoku/src/chess_white_transparent.png";
chessImage[4].src = "gomoku/src/chess_white.png";
chessImage[5].src = "gomoku/src/chess_white_marked.png";

var canvas = document.getElementById('cvs');
var context = canvas.getContext("2d");
$('#cvs').attr({ 'width': 565, 'height': 565 });

var playNo = 0;

// board status array
var boardStatus = new Array(15);
for (var i = boardStatus.length - 1; i >= 0; i--)
  boardStatus[i] = new Array(15);

// initialize board
function initBoard() {
  playNo = 0;

  for (var row = boardStatus.length - 1; row >= 0; row--)
    for (var col = boardStatus[row].length - 1; col >= 0; col--)
      boardStatus[row][col] = 0;

  mousePos = [-1, -1];
  lastPlay = [-1, -1];

  context.clearRect(20, 20, 525, 525);
}

function cvsMouseMoveOrOver(event) {
  // get the coordinate
  var position = getPosition(event);

  // if at new position
  if (mousePos === position) return;

  // check if not out of bound
  if (position[0] != -1) {
    // check if the position is empty
    if (!boardStatus[position[0]][position[1]]) {
      if (boardEnable) {
        clear(mousePos);
        draw(position);
      }
      // update mousePos
      mousePos = position.slice();

      return;
    }
  }

  // out of bound or at an occupyed position, clear and set position to (-1, -1)
  if (boardEnable) clear(mousePos);
  mousePos = [-1, -1];
}

function cvsMouseOut() {
  if (!boardEnable) return;

  // if mouse's previous position not has chess
  if (mousePos[0] != -1) {
    // clear the previous image
    clear(mousePos);

    // set mousePos to -1, -1
    mousePos = [-1, -1];
  }
}

function cvsClick() {
  // if the game does not started, show new game dialog
  if (!gameStarted) {
    $('#dialog-new-game').show();;
    return;
  }

  if (!boardEnable) return;

  if (mousePos[0] == -1) return;

  // if the position is empty, play and post
  if (!boardStatus[mousePos[0]][mousePos[1]]) {
    play(mousePos);
    post({ row: mousePos[1], col: mousePos[0], think: !humanTurn() }, 'play');
    mousePos = [-1, -1];
  }
}

// get position array [x, y]
function getPosition(event) {
  var rect = canvas.getBoundingClientRect();
  var scaling = canvas.scrollWidth / 565;

  var x = Math.floor((event.clientX - rect.left - 20)  / (35 * scaling)),
      y = Math.floor((event.clientY - rect.top - 20) / (35 * scaling));

  if (x < 0 || x > 14 || y < 0 || y > 14) {
    x = -1; y = -1;
  }

  return [x, y];
}

// draw iamge at position
function draw(pos) {
  if (pos[0] == -1) return;

  var status = boardStatus[pos[0]][pos[1]];
  var image;
  if (status == 0) {
    image = chessImage[(playNo & 1) * 3];
  } else if (status == playNo) {
    image = playNumber ? chessImage[((status - 1) & 1) * 3 + 1] :
                         chessImage[((status - 1) & 1) * 3 + 2];
  } else {
    image = chessImage[((status - 1) & 1) * 3 + 1];
  }

  context.drawImage(image, pos[0] * 35 + 21, pos[1] * 35 + 21, 33, 33);

  if (playNumber && status > 0) {
    if (status == playNo)
      context.fillStyle = 'red';
    else {
      context.fillStyle = ((status - 1) & 1) ? 'black' : 'white';
    }
    context.font = '29px Ubuntu';
    context.textAlign = 'center';

    context.fillText(status, pos[0] * 35 + 37, pos[1] * 35 + 47, 27);
  }
}

// clear the position
function clear(pos) {
  if (pos[0] == -1) return;

  context.clearRect(pos[0] * 35 + 21, pos[1] * 35 + 21, 33, 33);
}

// play at position, we should make sure that position is empty
function play(position) {
  ++playNo;
  boardStatus[position[0]][position[1]] = playNo;

  // draw a marked chess
  draw(position);

  // redraw last play's image (remove the mark)
  clear(lastPlay);
  draw(lastPlay);

  // update last play
  lastPlay = position.slice(0);

  // lock board
  boardEnable = false;
}

function notifyWinner(winnerColor) {
  // TODO: display game status
  boardEnable = false;
  alert(winnerColor + " wins !");
}

// post request, params should be json type
function post(params, path) {
  var http = new XMLHttpRequest();
  http.open('POST', path);

  http.onreadystatechange = function() {
    if (http.readyState == 4 && (http.status == 200 || http.status == 204)) {

      if (http.status == 200){
        var response = JSON.parse(http.responseText);

        if (!humanTurn() && response.row != -1 && response.col != -1) {
          // change JSON to array, and play
          play([response.col, response.row]);
        }

        if (response.winner != 'none') {
          notifyWinner(response.winner);
          return;
        }
      }


      if (humanTurn()) {
        if (http.status == 200 && response.col == mousePos[0] && response.row == mousePos[1]) {
          mousePos = [-1, -1];
        }
       else {
          draw(mousePos);
        }
        boardEnable = true;
      } else {
        post({ row: -1, col: -1, think: true}, 'play');
      }
    }
  };


  http.send(JSON.stringify(params));
}

function humanTurn() {
  return ((playNo & 1) == 0 && game.black == 'human') ||
          ((playNo & 1) == 1 && game.white == 'human');
}

// initialize player
// NOTE: player should be 'sel' or 'opp', color should be 'black' or 'white'
// human is bool
function initPlayer(player, color, human) {
  var oppColor = (color == 'black' ? 'white' : 'black');

  // set png
  $('#pi-chess-' + player).attr('src', 'gomoku/src/chess_' + color + '.png');
  $('#pi-icon-' + player).
    attr('src', 'gomoku/src/' + (human ? 'human' : 'icon') + '.png');

  // set background
  $('#pi-background-' + player).removeClass(oppColor);
  $('#pi-background-' + player).addClass(color);

  // set timer
  $('#pi-timer-' + player).removeClass(oppColor);
  $('#pi-timer-' + player).addClass(color);

  // set name
  $('#pi-name-' + player).html((human ? $('#dl-name-' + color).val() : 'EarthMover'));
}

$('#new-game').click(function() {
  $('#dialog-new-game').show();
});

$('#btn-dialog-cancel').click(function() {
  $('#dialog-new-game').hide();
});

$('#btn-dialog-ok').click(function() {
  // get the black/white player's radio buttons' result
  var black = $('input[name="black"]:checked').val(),
      white = $('input[name="white"]:checked').val();

  if ($('#dl-name-black').val().trim() == '')
    $('#dl-name-black').val('you');
  if ($('#dl-name-white').val().trim() == '')
    $('#dl-name-white').val('you');

  game = {black: black, white: white};

  var level = $('#dl-select').val();

  post({level: level}, 'start');

  // deside player's place
  if (game.black == 'human') {
    // black == human, white == human -> sel: black, human, opp: white, human
    // black == human, white == ai -> sel: black, human, opp: white, ai
    initPlayer('sel', 'black', true);
    initPlayer('opp', 'white', (game.white == 'human'));
  } else {
    if (game.white == 'human') {
      // black == ai, white == human -> sel: white, human, opp: black, ai
      initPlayer('sel', 'white', true);
      initPlayer('opp', 'black', false);
    } else {
      // black == ai, white == ai -> sel: black, ai, opp: white, ai
      initPlayer('sel', 'black', false);
      initPlayer('opp', 'white', false);
    }
    boardEnable = false;
  }

  gameStarted = true;

  // initialize board
  initBoard();

  $('#dialog-new-game').hide();
});

$('#play-number').click(function() {
  playNumber = !playNumber;

  if (playNumber) {
    $('#play-number-check').show();
  } else {
    $('#play-number-check').hide();
  }

  for (var row = boardStatus.length - 1; row >= 0; row--)
    for (var col = boardStatus[row].length - 1; col >= 0; col--)
      if (boardStatus[row][col] > 0) {
        clear([row, col]);
        draw([row, col]);
      }
});

$('#coordinate').click(function() {
  coordinate = !coordinate;

  if (coordinate) {
    $('#coordinate-check').show();
    context.fillStyle = '#444';
    context.font = '12px Ubuntu';
    context.textAlign = 'center';
    for (var i = 1; i <= 15; ++i) {
      var text = String.fromCharCode(64 + i);
      context.fillText(text, i * 35 + 2, 15);
      context.fillText(text, i * 35 + 2, 558);
      context.fillText(i, 10, i * 35 + 7);
      context.fillText(i, 552, i * 35 + 7);
    }
  } else {
    $('#coordinate-check').hide();
    context.clearRect(0, 20, 20, 525);
    context.clearRect(545, 20, 20, 525);
    context.clearRect(20, 0, 525, 20);
    context.clearRect(20, 545, 525, 20);
  }
});