var mousePosX = -1, mousePosY;
var lastPlayX = -1, lastPlayY;

var whoTurn = 0;

// image array
var chessImage = new Array(6);
for (var i = chessImage.length - 1; i >= 0; i--) {
  chessImage[i] = new Image();
}

const CHESS_DIMEN = 33;

var canvas = document.getElementById('cvs_board');
canvas.setAttribute('width', 525);
canvas.setAttribute('height', 525);

var boardStatus = new Array(225);
for (var i = boardStatus.length - 1; i >= 0; i--) {
  boardStatus[i] = false;
}

chessImage[0].src = "gomoku/src/chess_black_transparent.png";
chessImage[1].src = "gomoku/src/chess_black.png";
chessImage[2].src = "gomoku/src/chess_black_marked.png";
chessImage[3].src = "gomoku/src/chess_white_transparent.png";
chessImage[4].src = "gomoku/src/chess_white.png";
chessImage[5].src = "gomoku/src/chess_white_marked.png";

canvas.onmousemove = function(event) {
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

  // get the coordinate (0 ~ 14)
  var rect = this.getBoundingClientRect();
  var scaling = this.scrollWidth / 525;

  var x = Math.min(Math.floor((event.clientX - rect.left)  / (35 * scaling)), 14),
      y = Math.min(Math.floor((event.clientY - rect.top) / (35 * scaling)), 14);

  // if the position is empty
  if (!boardStatus[x * 15 + y]) {
    play(x, y);
    post(x, y);
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

function post(row, col) {
  var http = XMLHttpRequest();
  http.open('POST', '/play');
  var params = { row: row, col: col };

  http.onreadystatechange = function() {
    if (http.readyState == 4 && http.status == 200) {
      alert(http.responseText);
    }
  }

  http.send(params);
}


document.getElementById('btn_new_game').onclick = function() {
  document.getElementById('dialog').style.display = "block";
}


document.getElementById('btn_dialog_ok').onclick = function() {
  document.getElementById('dialog').style.display = "none";
}