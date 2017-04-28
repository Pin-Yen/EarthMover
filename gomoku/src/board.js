var game = { black: 'human', white: 'computer'};
var boardEnable = false;
var gameStarted = false;

var mousePosX = -1, mousePosY;
var lastPlayX = -1, lastPlayY;

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

const CHESS_DIMEN = 33;

var canvas = document.getElementById('cvs-board');
canvas.setAttribute('width', 525);
canvas.setAttribute('height', 525);

// board status array
var boardStatus = new Array(225);
for (var i = boardStatus.length - 1; i >= 0; i--) {
  boardStatus[i] = false;
}

canvas.onmousemove = function(event) {
  if (!boardEnable) return;
  // get the coordinate (0 ~ 14)
  var rect = this.getBoundingClientRect();
  var scaling = this.scrollWidth / 525;

  var x = Math.min(Math.floor((event.clientX - rect.left)  / (35 * scaling)), 14),
      y = Math.min(Math.floor((event.clientY - rect.top) / (35 * scaling)), 14);

  // if the position is empty
  if (!boardStatus[x * 15 + y]) {
    // draw only if mouse at the new coordinate
    if (mousePosX != x || mousePosY != y) {
      var context = this.getContext("2d");
      // clear the previous image
      context.clearRect((mousePosX * 35 + 1),
                        (mousePosY * 35 + 1),
                        CHESS_DIMEN, CHESS_DIMEN);
          // draw image in new position
      context.drawImage(chessImage[whoTurn * 3],
                        (x * 35 + 1),
                        (y * 35 + 1),
                        CHESS_DIMEN, CHESS_DIMEN);

      // update mousePos
      mousePosX = x; mousePosY = y;
    }
  } else {
    if (mousePosX != -1) {
      // clear the previous image
      this.getContext("2d").clearRect(mousePosX * 35 + 1,
                                      mousePosY * 35 + 1,
                                      CHESS_DIMEN, CHESS_DIMEN);
      // set mousePos to -1
      mousePosX = -1;
    }
  }
}

canvas.onmouseout = function() {
  if (!boardEnable) return;
  if (mousePosX != -1) {
    // clear the previous image
    this.getContext("2d").clearRect(mousePosX * 35 + 1,
                                    mousePosY * 35 + 1,
                                    CHESS_DIMEN, CHESS_DIMEN);
    // set mousePos to -1, -1
    mousePosX = -1;
  }
}

canvas.onclick = function(event) {
  if (!gameStarted) {
    // set dialog visibility
    document.getElementById('dialog-new-game').style.display = "block";
    return;
  }

  if (!boardEnable) return;

  // get the coordinate (0 ~ 14)
  var rect = this.getBoundingClientRect();
  var scaling = this.scrollWidth / 525;

  var x = Math.min(Math.floor((event.clientX - rect.left)  / (35 * scaling)), 14),
      y = Math.min(Math.floor((event.clientY - rect.top) / (35 * scaling)), 14);

  // if the position is empty
  if (!boardStatus[x * 15 + y]) {
    play(x, y);
    var params = { row: y, col: x };
    post(params, 'play');
  }
}

// play at (x, y), we should make sure that (x, y) is empty
function play(x, y) {
  boardStatus[x * 15 + y] = true;
  mousePosX = -1;

  var context = canvas.getContext("2d");
  context.drawImage(chessImage[whoTurn * 3 + 2],
                    x * 35 + 1,
                    y * 35 + 1,
                    CHESS_DIMEN, CHESS_DIMEN);

  whoTurn = whoTurn * -1 + 1;

  if (lastPlayX != -1) {
    context.clearRect(lastPlayX * 35 + 1,
                      lastPlayY * 35 + 1,
                      CHESS_DIMEN, CHESS_DIMEN);
    context.drawImage(chessImage[whoTurn * 3 + 1],
                      lastPlayX * 35 + 1,
                      lastPlayY * 35 + 1,
                      CHESS_DIMEN, CHESS_DIMEN);
  }

  lastPlayX = x; lastPlayY = y;

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
      play(response.col, response.row);
    }
  };

  http.send(JSON.stringify(params));
}


document.getElementById('btn-new-game').onclick = function() {
  // set dialog visibility
  document.getElementById('dialog-new-game').style.display = "block";
}


document.getElementById('btn-dialog-cancel').onclick = function() {
  // close dialog
  document.getElementById('dialog-new-game').style.display = "none";
}

document.getElementById('btn-dialog-ok').onclick = function() {
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
  document.getElementById('dialog-new-game').style.display = "none";
}