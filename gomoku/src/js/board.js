var Board = function() {
  this.enable = false;
  this.gameStarted = false;

  this.display = { playNumber: false, coordinate: false };

  this.mousePos = [-1, -1];
  this.lastPlay = [-1, -1];

  this.canvas = document.getElementById('cvs');
  this.context = this.canvas.getContext("2d");
  var cvs = $(this.canvas);
  cvs.attr({ 'width': 565, 'height': 565 });

  cvs.click(this.click.bind(this));

  cvs.mousemove(this.mouseMoveOrOver.bind(this));
  cvs.mouseover(this.mouseMoveOrOver.bind(this));

  cvs.mouseout(this.mouseOut.bind(this));

  this.playNo = 0;

  // board status array
  this.status = new Array(15);
  for (var i = this.status.length - 1; i >= 0; i--)
    this.status[i] = new Array(15);

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
};

Board.prototype.init = function() {
  this.playNo = 0;

  for (var row = this.status.length - 1; row >= 0; row--)
    for (var col = this.status[row].length - 1; col >= 0; col--)
      this.status[row][col] = 0;

  this.mousePos = [-1, -1];
  this.lastPlay = [-1, -1];

  this.context.clearRect(20, 20, 525, 525);
};

Board.prototype.mouseMoveOrOver = function(event) {
  // get position
  var rect = this.canvas.getBoundingClientRect();
  var scaling = this.canvas.scrollWidth / 565;

  var x = Math.floor((event.clientX - rect.left - 20 * scaling)  / (35 * scaling)),
      y = Math.floor((event.clientY - rect.top - 20 * scaling) / (35 * scaling));

  if (x < 0 || x > 14 || y < 0 || y > 14) {
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
    $('#dialog-new-game').show();;
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
  } else if (status == this.playNo) {
    image = this.display.playNumber ? this.image[this.whoTurn(status - 1)].normal :
                              this.image[this.whoTurn(status - 1)].marked;
  } else {
    image = this.image[this.whoTurn(status - 1)].normal;
  }

  this.context.drawImage(image, pos[0] * 35 + 21, pos[1] * 35 + 21, 33, 33);

  if (this.display.playNumber && status > 0) {
    if (status == this.playNo)
      this.context.fillStyle = 'red';
    else {
      this.context.fillStyle = ((status - 1) & 1) ? 'black' : 'white';
    }
    this.context.font = '29px Ubuntu';
    this.context.textAlign = 'center';

    this.context.fillText(status, pos[0] * 35 + 37, pos[1] * 35 + 47, 27);
  }
}

// clear the position
Board.prototype.clear = function(pos) {
  if (pos[0] == -1) return;

  this.context.clearRect(pos[0] * 35 + 21, pos[1] * 35 + 21, 33, 33);
}

// play at position, we should make sure that position is empty
Board.prototype.play = function(pos) {
  ++this.playNo;
  this.status[pos[0]][pos[1]] = this.playNo;

  // draw a marked chess
  this.draw(pos);

  // redraw last play's image (remove the mark)
  this.clear(this.lastPlay);
  this.draw(this.lastPlay);

  // update last play
  this.lastPlay = pos.slice(0);

  // lock board
  this.enable = false;

  // stop timer
  timer[this.whoTurn(this.playNo - 1)].stop();
}

Board.prototype.whoTurn = function(param) {
  var num = param === undefined ? this.playNo : param;
  return (num & 1) ? 'white' : 'black';
}

Board.prototype.changePlayNumber = function() {
  this.display.playNumber = !this.display.playNumber;

  for (var row = this.status.length - 1; row >= 0; row--)
    for (var col = this.status[row].length - 1; col >= 0; col--)
      if (this.status[row][col] > 0) {
        this.clear([row, col]);
        this.draw([row, col]);
      }

  return this.display.playNumber;
}

Board.prototype.changeCoordinate = function() {
  this.display.coordinate = !this.display.coordinate;

  if (this.display.coordinate) {
    this.context.fillStyle = '#444';
    this.context.font = '12px Ubuntu';
    this.context.textAlign = 'center';
    for (var i = 1; i <= 15; ++i) {
      var text = String.fromCharCode(64 + i);
      this.context.fillText(text, i * 35 + 2, 15);
      this.context.fillText(text, i * 35 + 2, 558);
      this.context.fillText(i, 10, i * 35 + 7);
      this.context.fillText(i, 552, i * 35 + 7);
    }
  } else {
    this.context.clearRect(0, 20, 20, 525);
    this.context.clearRect(545, 20, 20, 525);
    this.context.clearRect(20, 0, 525, 20);
    this.context.clearRect(20, 545, 525, 20);
  }

  return this.display.coordinate;
};