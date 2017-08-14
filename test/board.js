var Board = function() {
  this.lastPlay = [-1, -1];

  this.canvas = document.getElementById('cvs');
  this.context = this.canvas.getContext("2d");
  var cvs = $(this.canvas);
  cvs.attr({ 'width': 565, 'height': 565 });

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
      'normal': createImage('black'),
      'marked' : createImage('black_marked')
    },
    'white': {
      'normal': createImage('white'),
      'marked' : createImage('white_marked')
    }
  };
};

Board.prototype.draw = function(color, status, x, y) {
  var image = this.image[color][status];

  this.context.drawImage(image, x * 35 + 21, y * 35 + 21, 33, 33);
};

// clear
Board.prototype.clear = function() {
  this.context.clearRect(20, 20, 525, 525);;
}

// draw all games
// params: [{'row': 7, 'col': 7}...]
Board.prototype.drawAll = function(params) {
  for (var i = 0; i < params.length; ++i) {
    var color = (i & 1 ? 'white' : 'black');
    var status = (i == params.length - 1 ? 'marekd' : 'normal');
    this.draw(color, status, params[i].row, params[i].col);
  }
};