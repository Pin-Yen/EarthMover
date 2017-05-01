var game = { black: 'human', white: 'computer'};

var boardEnable = false, gameStarted = false;
var playNumber = false;

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

var canvas = document.getElementById('cvs-board');
var context = canvas.getContext("2d");
$('#cvs-board').attr({ 'width': 525, 'height': 525 });

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

  context.clearRect(0, 0, 525, 525);
}

canvas.onmousemove = function(event) {
  if (!boardEnable) return;

  // get the coordinate
  var position = getPosition(event);

  // if the position is empty
  if (!boardStatus[position[0]][position[1]]) {
    // draw only if mouse at the new coordinate
    if (mousePos !== position) {
      clear(mousePos);
      draw(position);

      // update mousePos
      mousePos = position.slice();
    }
  } else {
    // if mouse at the exist chess, clear mose position
    if (mousePos[0] != -1) {
      clear(mousePos);
      mousePos = [-1, -1];
    }
  }
}

canvas.onmouseout = function() {
  if (!boardEnable) return;

  // if mouse's previous position not has chess
  if (mousePos[0] != -1) {
    // clear the previous image
    clear(mousePos);

    // set mousePos to -1, -1
    mousePos = [-1, -1];
  }
}

canvas.onclick = function(event) {
  // if the game does not started, show new game dialog
  if (!gameStarted) {
    $('#dialog-new-game').show();;
    return;
  }

  if (!boardEnable) return;

  var position = getPosition(event);

  // if the position is empty, play and post
  if (!boardStatus[position[0]][position[1]]) {
    play(position);
    post({ row: position[1], col: position[0] }, 'play');
  }
}

// get position array [x, y]
function getPosition(event) {
  var rect = canvas.getBoundingClientRect();
  var scaling = canvas.scrollWidth / 525;

  var x = Math.min(Math.floor((event.clientX - rect.left)  / (35 * scaling)), 14),
      y = Math.min(Math.floor((event.clientY - rect.top) / (35 * scaling)), 14);

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

  context.drawImage(image, pos[0] * 35 + 1, pos[1] * 35 + 1, 33, 33);

  if (playNumber && status > 0) {
    if (status == playNo)
      context.fillStyle = 'red';
    else {
      context.fillStyle = ((status - 1) & 1) ? 'black' : 'white';
    }
    context.font = '29px Ubuntu';
    context.textAlign = 'center';

    context.fillText(status, pos[0] * 35 + 17, pos[1] * 35 + 27, 27);
  }
}

// clear the position
function clear(pos) {
  if (pos[0] == -1) return;

  context.clearRect(pos[0] * 35 + 1, pos[1] * 35 + 1, 33, 33);
}

// play at position, we should make sure that position is empty
function play(position) {
  ++playNo;
  boardStatus[position[0]][position[1]] = playNo;

  mousePos = [-1, -1];

  // draw a marked chess
  draw(position);

  // redraw last play's image (remove the mark)
  clear(lastPlay);
  draw(lastPlay);

  // update last play
  lastPlay = position.slice(0);

  // check board's enable
  boardEnable = (((playNo & 1)== 0 && game.black == 'human') ||
                 ((playNo & 1) == 1 && game.white == 'human'));
}

function notifyWinner(winnerColor) {
  // TODO: display game status
  alert(winnerColor + " wins !");
}

// post request, params should be json type
function post(params, path) {
  var http = new XMLHttpRequest();
  http.open('POST', path);

  http.onreadystatechange = function() {
    if (http.readyState == 4 && http.status == 200) {
      var response = JSON.parse(http.responseText);

      // play EM's move only if user's previous move is not winning.
      if (response.winner == 'none' ||
          (response.winner == 'black' && game.black == 'computer') ||
          (response.winner == 'white' && game.white == 'computer')) {
        // change JSON to array, and play
        play([response.col, response.row]);
      }

      if (response.winner != 'none')
        notifyWinner(response.winner);
    }
  };


  http.send(JSON.stringify(params));
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

  game = { black: black, white: white};

  post(game, 'start');

  // deside player's place
  if (game.black == 'human') {
    // black == human, white == human -> sel: black, human, opp: white, human
    // black == human, white == ai -> sel: black, human, opp: white, ai
    initPlayer('sel', 'black', true);
    initPlayer('opp', 'white', (game.white == 'human'));
    boardEnable = true;
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