var game = { black: 'human', white: 'computer'};
var boardEnable = false;
var gameStarted = false;

var mousePos = [-1, -1];
var lastPlay = [-1, -1];

var whoTurn = 0;

// image array
var chessImage = new Array(6);
for (var i = chessImage.length - 1; i >= 0; i--) {
  chessImage[i] = new Image();
}
chessImage[0].src = "gomoku/src/chess_black_transparent.png";
chessImage[1].src = "gomoku/src/chess_black.png";
chessImage[2].src = "gomoku/src/chess_black_marked.png";
chessImage[3].src = "gomoku/src/chess_white_transparent.png";
chessImage[4].src = "gomoku/src/chess_white.png";
chessImage[5].src = "gomoku/src/chess_white_marked.png";

var canvas = document.getElementById('cvs-board');
canvas.setAttribute('width', 525);
canvas.setAttribute('height', 525);

var playNo = 0;

// board status array
var boardStatus = new Array(15);
for (var i = boardStatus.length - 1; i >= 0; i--)
  boardStatus[i] = new Array(15).fill(0);

canvas.onmousemove = function(event) {
  if (!boardEnable) return;

  // get the coordinate
  var position = getPosition(event);

  // if the position is empty
  if (!boardStatus[position[0]][position[1]]) {
    // draw only if mouse at the new coordinate
    if (mousePos !== position) {
      clear(mousePos);
      draw(position, chessImage[whoTurn * 3]);

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
  if (mousePos === [-1, -1]) return;

  // clear the previous image
  clear(mousePos);

  // set mousePos to -1, -1
  mousePos = [-1, -1];
}

canvas.onclick = function(event) {
  if (!gameStarted) {
    // show dialog
    document.getElementById('dialog-new-game').style.display = "block";
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
  var context = canvas.getContext("2d");
  context.drawImage(image, position[0] * 35 + 1, position[1] * 35 + 1, 33, 33);
}

// clear the position
function clear(position) {
  var context = canvas.getContext("2d");
  context.clearRect(position[0] * 35 + 1, position[1] * 35 + 1, 33, 33);
}

// play at position, we should make sure that position is empty
function play(position) {
  ++playNo;
  boardStatus[position[0]][position[1]] = playNo;
  mousePos = [-1, -1];

  draw(position, chessImage[whoTurn * 3 + 2]);

  whoTurn = whoTurn * -1 + 1;

  clear(lastPlay);
  draw(lastPlay, chessImage[whoTurn * 3 + 1])

  lastPlay = position.slice();

  if ((whoTurn == 0 && game.black == 'computer') ||
      (whoTurn == 1 && game.white == 'computer'))
    boardEnable = false;
  else
    boardEnable = true;
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

$('#btn-new-game').click(function() {
  // set dialog visibility
  $('#dialog-new-game').css('display', 'block');
});

$('#btn-dialog-cancel').click(function() {
  // close dialog
  $('#dialog-new-game').css('display', 'none');
});

$('#btn-dialog-ok').click(function() {
  // get the black/white player's ratio buttons' result
  var black = document.querySelector('input[name="black"]:checked').value,
      white = document.querySelector('input[name="white"]:checked').value;

  game = { black: black, white: white};

  post(game, 'start');

  if (game.black == 'human') {
    document.getElementById('pi-chess-sel').src = 'gomoku/src/chess_black.png';
    document.getElementById('pi-chess-opp').src = 'gomoku/src/chess_white.png';
    document.getElementById('pi-background-sel').className = 'pi-background sel black';
    document.getElementById('pi-background-opp').className = 'pi-background opp white';
    document.getElementById('pi-timer-sel').className = 'pi-timer sel black';
    document.getElementById('pi-timer-opp').className = 'pi-timer opp white';
    document.getElementById('pi-icon-sel').src = 'gomoku/src/human.png';
    document.getElementById('pi-icon-opp').src =
      (game.white == 'human' ? 'gomoku/src/human.png' : 'gomoku/src/icon.png');
    document.getElementById('pi-name-sel').innerHTML =
      document.getElementById('dl-name-black').value;
    document.getElementById('pi-name-opp').innerHTML =
      (game.white == 'human' ? document.getElementById('dl-name-white').value : 'EarthMover');

    boardEnable = true;
  } else {
    document.getElementById('pi-icon-opp').src = 'gomoku/src/icon.png';
    document.getElementById('pi-name-opp').innerHTML = 'EarthMover';
    if (game.white == 'human') {
      document.getElementById('pi-chess-sel').src = 'gomoku/src/chess_white.png';
      document.getElementById('pi-chess-opp').src = 'gomoku/src/chess_black.png';
      document.getElementById('pi-background-sel').className = 'pi-background sel white';
      document.getElementById('pi-background-opp').className = 'pi-background opp black';
      document.getElementById('pi-timer-sel').className = 'pi-timer sel white';
      document.getElementById('pi-timer-opp').className = 'pi-timer opp black';
      document.getElementById('pi-icon-sel').src = 'gomoku/src/human.png';
      document.getElementById('pi-name-sel').innerHTML =
        document.getElementById('dl-name-white').value;
    } else {
      document.getElementById('pi-chess-sel').src = 'gomoku/src/chess_black.png';
      document.getElementById('pi-chess-opp').src = 'gomoku/src/chess_white.png';
      document.getElementById('pi-background-sel').className = 'pi-background sel black';
      document.getElementById('pi-background-opp').className = 'pi-background opp white';
      document.getElementById('pi-timer-sel').className = 'pi-timer sel black';
      document.getElementById('pi-timer-opp').className = 'pi-timer opp white';
      document.getElementById('pi-icon-sel').src = 'gomoku/src/icon.png';
      document.getElementById('pi-name-sel').innerHTML = 'EarthMover';
    }
    boardEnable = false;
  }

  gameStarted = true;

  // close dialog
  $('#dialog-new-game').css('display', 'none');
});