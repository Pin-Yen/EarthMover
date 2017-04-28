var gameStarted = false;
var youTurn = false;

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
  if (!youTurn) return;
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
  if (!youTurn) return;
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
    document.getElementById('dialog').style.display = "block";
    return;
  }

  if (!youTurn) return;

  // get the coordinate (0 ~ 14)
  var rect = this.getBoundingClientRect();
  var scaling = this.scrollWidth / 525;

  var x = Math.min(Math.floor((event.clientX - rect.left)  / (35 * scaling)), 14),
      y = Math.min(Math.floor((event.clientY - rect.top) / (35 * scaling)), 14);

  // if the position is empty
  if (!boardStatus[x * 15 + y]) {
    play(x, y);
    var params = { row: x, col: y };
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
}

// post request, pareams should be json type
function post(params, path) {
  var http = new XMLHttpRequest();
  http.open('POST', path);

  http.onreadystatechange = function() {
    if (http.readyState == 4 && http.status == 200) {
      alert(http.responseText);
    }
  };

  http.send(JSON.stringify(params));
}


document.getElementById('btn-new-game').onclick = function() {
  // set dialog visibility
  document.getElementById('dialog').style.display = "block";
}


document.getElementById('btn-dialog-cancel').onclick = function() {
  // close dialog
  document.getElementById('dialog').style.display = "none";
}

document.getElementById('btn-dialog-ok').onclick = function() {
  // get the black/white player's ratio buttons' result
  var black = document.querySelector('input[name="black"]:checked').value,
      white = document.querySelector('input[name="white"]:checked').value;

  var result = { black: black, white: white};

  post(result, 'start');

  if (result.black == 'human') {
    document.getElementById('pi-chess-sel').src = 'gomoku/src/chess_black.png';
    document.getElementById('pi-chess-opp').src = 'gomoku/src/chess_white.png';
    document.getElementById('pi-background-sel').className = 'pi-background sel black';
    document.getElementById('pi-background-opp').className = 'pi-background opp white';
    document.getElementById('pi-timer-sel').className = 'pi-timer sel black';
    document.getElementById('pi-timer-opp').className = 'pi-timer opp white';
    document.getElementById('pi-icon-sel').src = 'gomoku/src/human.png';
    document.getElementById('pi-icon-opp').src =
      (result.white == 'human' ? 'gomoku/src/human.png' : 'gomoku/src/icon.png');

    youTurn = true;
  } else {
    document.getElementById('pi-icon-opp').src = 'gomoku/src/icon.png';
    if (result.white == 'human') {
      document.getElementById('pi-chess-sel').src = 'gomoku/src/chess_white.png';
      document.getElementById('pi-chess-opp').src = 'gomoku/src/chess_black.png';
      document.getElementById('pi-background-sel').className = 'pi-background sel white';
      document.getElementById('pi-background-opp').className = 'pi-background opp black';
      document.getElementById('pi-timer-sel').className = 'pi-timer sel white';
      document.getElementById('pi-timer-opp').className = 'pi-timer opp black';
      document.getElementById('pi-icon-sel').src = 'gomoku/src/human.png';
    } else {
      document.getElementById('pi-chess-sel').src = 'gomoku/src/chess_black.png';
      document.getElementById('pi-chess-opp').src = 'gomoku/src/chess_white.png';
      document.getElementById('pi-background-sel').className = 'pi-background sel black';
      document.getElementById('pi-background-opp').className = 'pi-background opp white';
      document.getElementById('pi-timer-sel').className = 'pi-timer sel black';
      document.getElementById('pi-timer-opp').className = 'pi-timer opp white';
      document.getElementById('pi-icon-sel').src = 'gomoku/src/icon.png';
    }
    youTurn = false;
  }

  gameStarted = true;

  // close dialog
  document.getElementById('dialog').style.display = "none";
}