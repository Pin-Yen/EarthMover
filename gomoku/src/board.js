var game = { black: 'human', white: 'computer'};

var boardEnable = false, gameStarted = false;

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

  canvas.getContext("2d").clearRect(0, 0, 525, 525);
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
      draw(position, chessImage[(playNo & 1) * 3]);

      // update mousePos
      mousePos = position.slice();
    }
  } else {
    // if mouse at the exist chess, clear mose position
    if (mousePos !== [-1, -1]) {
      clear(mousePos);
      mousePos = [-1, -1];
    }
  }
}

canvas.onmouseout = function() {
  if (!boardEnable) return;

  // if mouse's previous position not has chess
  if (mousePos !== [-1, -1]) {
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
function draw(position, image) {
  canvas.getContext("2d").drawImage
    (image, position[0] * 35 + 1, position[1] * 35 + 1, 33, 33);
}

// clear the position
function clear(position) {
  canvas.getContext("2d").
    clearRect(position[0] * 35 + 1, position[1] * 35 + 1, 33, 33);
}

// play at position, we should make sure that position is empty
function play(position) {
  ++playNo;
  boardStatus[position[0]][position[1]] = playNo;

  mousePos = [-1, -1];

  // draw a marked chess
  draw(position, chessImage[((playNo - 1) & 1) * 3 + 2]);

  // redraw last play's image (remove the mark)
  clear(lastPlay);
  draw(lastPlay, chessImage[(playNo & 1) * 3 + 1])

  // update last play
  lastPlay = position.slice();

  // check board's enable
  boardEnable = (((playNo & 1)== 0 && game.black == 'human') ||
                 ((playNo & 1) == 1 && game.white == 'human'));
}

// post request, pareams should be json type
function post(params, path) {
  var http = new XMLHttpRequest();
  http.open('POST', path);

  http.onreadystatechange = function() {
    if (http.readyState == 4 && http.status == 200) {
      var response = JSON.parse(http.responseText);
      // change JSON to array
      play([response.col, response.row]);
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

