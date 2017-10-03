var Board = function() {
  this.enable = true;
  this.gameStarted = true;

  this.display = { playNumber: false, coordinate: false };

  this.mousePos = [-1, -1];
  this.lastPlay = [-1, -1];

  this.canvas = document.getElementById('cvs');
  this.context = this.canvas.getContext("2d");
  var cvs = $(this.canvas);
  cvs.attr({ 'width': 730, 'height': 730 });

  cvs.click(this.click.bind(this));

  cvs.mousemove(this.mouseMoveOrOver.bind(this));
  cvs.mouseover(this.mouseMoveOrOver.bind(this));

  cvs.mouseout(this.mouseOut.bind(this));

  this.playNo = 0;
  this.displayNo = 0;

  // board status array
  this.status = new Array(19);
  for (var i = this.status.length - 1; i >= 0; i--)
    this.status[i] = new Array(19);

  function createImage(src) {
    var image = new Image();
    image.src = 'gomoku/src/png/chess_' + src + '.png';
    return image;
  }

  this.image = {
    'black': {
      normal: createImage('black'),
      transparent: createImage('black_transparent'),
      marked : createImage('black_marked')
    },
    'white': {
      normal: createImage('white'),
      transparent: createImage('white_transparent'),
      marked : createImage('white_marked')
    }
  };

  this.init();
};

Board.prototype.init = function() {
  this.playNo = 0;
  this.displayNo = 0;

  for (var row = this.status.length - 1; row >= 0; row--)
    for (var col = this.status[row].length - 1; col >= 0; col--)
      this.status[row][col] = 0;

  this.mousePos = [-1, -1];
  this.lastPlay = [-1, -1];

  this.context.clearRect(20, 20, 525, 525);

  //this.enable = false;
  this.gameStarted = true;
};

Board.prototype.mouseMoveOrOver = function(event) {
  // get position
  var rect = this.canvas.getBoundingClientRect();
  var scaling = this.canvas.scrollWidth / 730;

  var x = Math.floor((event.clientX - rect.left - 34 * scaling)  / (35 * scaling)),
      y = Math.floor((event.clientY - rect.top - 34 * scaling) / (35 * scaling));

  if (x < 0 || x > 18 || y < 0 || y > 18) {
    x = -1; y = -1;
  }

  var pos = [x, y];

  // if at new position
  if (this.mousePos === pos) return;

  // check if not out of bound and position is empty
  if (pos[0] != -1 && !this.status[pos[0]][pos[1]]) {
    if (this.enable) {
      this.clear(this.mousePos);
      this.draw(pos);
    }
    // update mousePos
    this.mousePos = pos.slice();
  } else {
    // out of bound or at an occupyed position, clear and set position to (-1, -1)
    if (this.enable) this.clear(this.mousePos);

    this.mousePos = [-1, -1];
  }
}

Board.prototype.mouseOut = function() {
  // if mouse's previous position not has chess
  if (this.mousePos[0] != -1) {
    // clear the previous image
    this.clear(this.mousePos);

    // set mousePos to -1, -1
    this.mousePos = [-1, -1];
  }
}

Board.prototype.click = function() {
  // if the game does not started, show new game dialog
  if (!this.gameStarted) {
    dialog.toggle();
    return;
  }

  if (!this.enable) return;

  if (this.mousePos[0] == -1) return;

  // if the position is empty, play and post
  if (!this.status[this.mousePos[0]][this.mousePos[1]]) {
    this.play(this.mousePos);
    post({ row: this.mousePos[1], col: this.mousePos[0] }, 'play');
    this.mousePos = [-1, -1];
  }
}

// draw iamge at position
Board.prototype.draw = function(pos) {
  if (pos[0] == -1) return;

  var status = this.status[pos[0]][pos[1]];
  var image;
  if (status == 0) {
    image = this.image[this.whoTurn()].transparent;
  } else if (status == this.displayNo) {
    image = this.display.playNumber ? this.image[this.whoTurn(status - 1)].normal :
                              this.image[this.whoTurn(status - 1)].marked;
  } else {
    image = this.image[this.whoTurn(status - 1)].normal;
  }

  this.drawChess(image, pos[0], pos[1]);

  if (this.display.playNumber && status > 0) {
    var color = status == this.displayNo ? 'red' : ((status - 1) & 1) ? 'black' : 'white';

    this.drawNumber(status, color, pos[0], pos[1]);
  }
}

Board.prototype.drawChess = function(image, x, y) {
  this.context.drawImage(image, x * 35 + 34, y * 35 + 34, 33, 33);
};

Board.prototype.drawNumber = function(number, color, x, y) {
  this.context.fillStyle = color;
  this.context.font = '29px Ubuntu';
  this.context.textAlign = 'center';
  this.context.fillText(number, x * 35 + 37, y * 35 + 47, 27);
};

// clear the position
Board.prototype.clear = function(pos) {
  if (pos[0] == -1) return;

  this.context.clearRect(pos[0] * 35 + 34, pos[1] * 35 + 34, 33, 33);
}

// play at position, we should make sure that position is empty
Board.prototype.play = function(pos) {
  // lock board
  //this.enable = false;

  // lock game and analyze bar
  //$('.ctrl-game input').prop('disabled', true);
  //$('.ctrl-analyze input').prop('disabled', true);

  // stop timer
  //timer[this.whoTurn()].stop();

  ++this.playNo;
  ++this.displayNo;
  this.status[pos[0]][pos[1]] = this.playNo;

  // draw a marked chess
  this.draw(pos);

  // redraw last play's image (remove the mark)
  this.clear(this.lastPlay);
  this.draw(this.lastPlay);

  // update last play
  this.lastPlay = pos.slice(0);

  // write to firebase
  //firebase.database().ref(gameID).child('record').child(this.playNo).set({r : pos[0], c : pos[1]});
}

// pass
Board.prototype.pass = function() {
  // lock board
  this.enable = false;

  // stop timer
  //timer[this.whoTurn()].stop();

  ++this.playNo;
  ++this.displayNo;

  // write '{r: -1, c: -1} to database
  //firebase.database().ref(gameID).child('record').child(this.playNo).set({r : -1, c : -1});
};

// undo 'times' move
Board.prototype.undo = function(times) {
  // lock board
  this.enable = false;

  // stop timer
  timer[this.whoTurn()].stop();

  this.playNo -= times;
  this.displayNo = this.playNo;

  // remove chess
  for (var row = this.status.length - 1; row >= 0; row--)
    for (var col = this.status[row].length - 1; col >= 0; col--) {
      if (this.status[row][col] > this.playNo) {
        this.status[row][col] = 0;
        this.clear([row, col]);
      } else if (this.playNo > 0 && this.status[row][col] == this.playNo) {
        this.lastPlay = [row, col];
        this.clear(this.lastPlay);
        this.draw(this.lastPlay);
      }
    }
};

// Puts a chess at the specific position, with the specific playNo.
// In contrast to play(), put() does not check if it is legal nor trigger the timer.
// All it does is put a chess and update total play/display numbers (for loading a game from the database).
Board.prototype.put = function(pos, playNo) {
  ++this.playNo;
  ++this.displayNo;

  if (pos != [-1, -1])
    this.status[pos[0]][pos[1]] = playNo;

  this.drawAll();
}

Board.prototype.whoTurn = function(param) {
  var num = param === undefined ? this.playNo : param;
  return (num & 1) ? 'white' : 'black';
}

Board.prototype.changePlayNumber = function() {
  this.display.playNumber = !this.display.playNumber;

  this.drawAll();
}

Board.prototype.changeCoordinate = function() {
  this.display.coordinate = !this.display.coordinate;

  if (this.display.coordinate) {
    this.context.fillStyle = '#444';
    this.context.font = '16px Ubuntu';
    this.context.textAlign = 'center';
    for (var i = 1; i <= 19; ++i) {
      this.context.fillText(i, 13, i * 35 + 20);
      this.context.fillText(i, 714, i * 35 + 20);
      var textNum = i < 9 ? i : i + 1;
      var text = String.fromCharCode(64 + textNum);
      this.context.fillText(text, i * 35 + 16, 20);
      this.context.fillText(text, i * 35 + 16, 720);
    }
  } else {
    this.context.clearRect(0, 34, 34, 630);
    this.context.clearRect(630, 34, 34, 630);
    this.context.clearRect(34, 0, 630, 34);
    this.context.clearRect(34, 630, 630, 34);
  }
};

Board.prototype.changeDisplayNo = function(changeAmount) {
  this.displayNo += changeAmount;
  if (this.displayNo < 0) this.displayNo = 0;
  if (this.displayNo > this.playNo) this.displayNo = this.playNo;

  this.drawAll();
};

Board.prototype.drawAll = function() {
  for (var row = this.status.length - 1; row >= 0; row--)
    for (var col = this.status[row].length - 1; col >= 0; col--) {
      this.clear([row, col]);
      if (this.status[row][col] > 0 && this.status[row][col] <= this.displayNo)
        this.draw([row, col]);
    }
};

Board.prototype.drawEvolve = function(evolve) {
  var playNumber = this.display.playNumber;
  if (playNumber) {
    this.display.playNumber = false;
    this.drawAll();
  }

  for (var i = evolve.length - 1; i >= 0; i--) {
    this.drawChess(this.image[evolve[i].color].normal, evolve[i].col, evolve[i].row);
    this.drawNumber(i + 1, evolve[i].color == 'black' ? 'white' : 'black', evolve[i].col, evolve[i].row);
  }

  if (playNumber) this.display.playNumber = true;
};